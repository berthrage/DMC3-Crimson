#include "MusicSwitcherLoudness.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>

namespace MusicSwitcherLoudness {

namespace {

constexpr double kAbsoluteGate = -70.0;
constexpr double kRelativeGate = -10.0;

// BS.1770 K-weighting, taken from its analog prototype so any sample rate works.
// At 48 kHz this reproduces the coefficient table printed in the standard.
struct Coefficients {
    double b0 = 0.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double a1 = 0.0;
    double a2 = 0.0;
};

Coefficients ShelfCoefficients(double rate) {
    constexpr double kFrequency = 1681.974450955533;
    constexpr double kGainDb    = 3.999843853973347;
    constexpr double kQ         = 0.7071752369554196;

    const double k  = std::tan(3.14159265358979323846 * kFrequency / rate);
    const double vh = std::pow(10.0, kGainDb / 20.0);
    const double vb = std::pow(vh, 0.4996667741545416);
    const double a0 = 1.0 + k / kQ + k * k;

    Coefficients c;
    c.b0 = (vh + vb * k / kQ + k * k) / a0;
    c.b1 = 2.0 * (k * k - vh) / a0;
    c.b2 = (vh - vb * k / kQ + k * k) / a0;
    c.a1 = 2.0 * (k * k - 1.0) / a0;
    c.a2 = (1.0 - k / kQ + k * k) / a0;

    return c;
}

Coefficients HighPassCoefficients(double rate) {
    constexpr double kFrequency = 38.13547087602444;
    constexpr double kQ         = 0.5003270373238773;

    const double k  = std::tan(3.14159265358979323846 * kFrequency / rate);
    const double a0 = 1.0 + k / kQ + k * k;

    Coefficients c;
    // BS.1770 leaves the stage 2 numerator unscaled, which is why the published
    // table reads {1, -2, 1} rather than {1, -2, 1} / a0.
    c.b0 = 1.0;
    c.b1 = -2.0;
    c.b2 = 1.0;
    c.a1 = 2.0 * (k * k - 1.0) / a0;
    c.a2 = (1.0 - k / kQ + k * k) / a0;

    return c;
}

double LoudnessOf(double power) {
    return -0.691 + 10.0 * std::log10(power);
}

// --- WAV ----------------------------------------------------------------

struct WavFormat {
    uint16_t tag        = 0; // 1 PCM, 3 IEEE float
    uint16_t channels   = 0;
    uint16_t bits       = 0;
    uint32_t rate       = 0;
    uint32_t dataOffset = 0;
    uint32_t dataSize   = 0;
};

bool ReadWavFormat(std::ifstream& file, WavFormat& out) {
    char riff[4];
    char wave[4];
    uint32_t riffSize = 0;

    file.read(riff, 4);
    file.read(reinterpret_cast<char*>(&riffSize), 4);
    file.read(wave, 4);

    if (!file || std::memcmp(riff, "RIFF", 4) != 0 || std::memcmp(wave, "WAVE", 4) != 0) {
        return false;
    }

    bool haveFormat = false;

    while (file) {
        char id[4];
        uint32_t size = 0;

        file.read(id, 4);
        file.read(reinterpret_cast<char*>(&size), 4);
        if (!file) {
            break;
        }

        const std::streampos chunkStart = file.tellg();

        if (std::memcmp(id, "fmt ", 4) == 0 && size >= 16) {
            uint16_t tag      = 0;
            uint16_t channels = 0;
            uint16_t bits     = 0;
            uint32_t rate     = 0;

            file.read(reinterpret_cast<char*>(&tag), 2);
            file.read(reinterpret_cast<char*>(&channels), 2);
            file.read(reinterpret_cast<char*>(&rate), 4);
            file.seekg(4, std::ios::cur); // average bytes per second
            file.seekg(2, std::ios::cur); // block align
            file.read(reinterpret_cast<char*>(&bits), 2);

            // WAVE_FORMAT_EXTENSIBLE: the real tag is the head of the sub format.
            if (tag == 0xFFFE && size >= 40) {
                file.seekg(8, std::ios::cur); // cbSize + valid bits
                file.read(reinterpret_cast<char*>(&tag), 2);
            }

            out.tag      = tag;
            out.channels = channels;
            out.bits     = bits;
            out.rate     = rate;
            haveFormat   = true;
        } else if (std::memcmp(id, "data", 4) == 0) {
            out.dataOffset = static_cast<uint32_t>(chunkStart);
            out.dataSize   = size;
        }

        if (!file) {
            break;
        }

        // RIFF chunks are word aligned.
        file.seekg(chunkStart + static_cast<std::streamoff>(size) + static_cast<std::streamoff>(size & 1u));

        if (haveFormat && out.dataSize != 0) {
            return true;
        }
    }

    return false;
}

float ReadSample(const unsigned char* sample, const WavFormat& format) {
    if (format.tag == 3) {
        if (format.bits == 32) {
            float value = 0.0f;
            std::memcpy(&value, sample, 4);

            return value;
        }

        double value = 0.0;
        std::memcpy(&value, sample, 8);

        return static_cast<float>(value);
    }

    switch (format.bits) {
    case 8: {
        return (static_cast<int>(sample[0]) - 128) / 128.0f;
    }
    case 16: {
        int16_t value = 0;
        std::memcpy(&value, sample, 2);

        return value / 32768.0f;
    }
    case 24: {
        int32_t value = static_cast<int32_t>(sample[0]) | (static_cast<int32_t>(sample[1]) << 8) |
                        (static_cast<int32_t>(sample[2]) << 16);

        if (value & 0x800000) {
            value |= ~0xFFFFFF;
        }

        return value / 8388608.0f;
    }
    case 32: {
        int32_t value = 0;
        std::memcpy(&value, sample, 4);

        return static_cast<float>(value / 2147483648.0);
    }
    default: {
        return 0.0f;
    }
    }
}

} // namespace

void Analyzer::Biquad::Set(double nb0, double nb1, double nb2, double na1, double na2) {
    b0 = nb0;
    b1 = nb1;
    b2 = nb2;
    a1 = na1;
    a2 = na2;
}

double Analyzer::Biquad::Process(double x) {
    const double y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

    x2 = x1;
    x1 = x;
    y2 = y1;
    y1 = y;

    return y;
}

Analyzer::Analyzer(uint32_t sampleRate) {
    double rate = (sampleRate >= 8000 && sampleRate <= 192000) ? sampleRate : 48000.0;

    m_blockFrames = static_cast<size_t>(rate * 0.4);  // 400 ms
    m_hopFrames   = static_cast<size_t>(rate * 0.1);  // 75% overlap

    const Coefficients shelf    = ShelfCoefficients(rate);
    const Coefficients highPass = HighPassCoefficients(rate);

    for (int channel = 0; channel < 2; ++channel) {
        m_shelf[channel].Set(shelf.b0, shelf.b1, shelf.b2, shelf.a1, shelf.a2);
        m_highPass[channel].Set(highPass.b0, highPass.b1, highPass.b2, highPass.a1, highPass.a2);
        m_window[channel].assign(m_blockFrames, 0.0f);
    }

    m_blocks.reserve(4096);
}

Analyzer::~Analyzer() {}

void Analyzer::Feed(const float* interleavedStereo, size_t frames) {
    if (interleavedStereo == NULL) {
        return;
    }

    for (size_t i = 0; i < frames; ++i) {
        const float left  = interleavedStereo[i * 2];
        const float right = interleavedStereo[i * 2 + 1];

        const float magnitude = std::max(std::fabs(left), std::fabs(right));
        if (magnitude > m_peak) {
            m_peak = magnitude;
        }

        m_window[0][m_filled] = static_cast<float>(m_highPass[0].Process(m_shelf[0].Process(left)));
        m_window[1][m_filled] = static_cast<float>(m_highPass[1].Process(m_shelf[1].Process(right)));

        if (++m_filled == m_blockFrames) {
            FlushBlock();

            // Slide one hop forward: the next block shares 75% of this one.
            for (int channel = 0; channel < 2; ++channel) {
                std::memmove(m_window[channel].data(), m_window[channel].data() + m_hopFrames,
                    (m_blockFrames - m_hopFrames) * sizeof(float));
            }

            m_filled = m_blockFrames - m_hopFrames;
        }
    }
}

void Analyzer::FlushBlock() {
    double sum = 0.0;

    for (int channel = 0; channel < 2; ++channel) {
        const float* samples = m_window[channel].data();
        double squares       = 0.0;

        for (size_t i = 0; i < m_blockFrames; ++i) {
            squares += static_cast<double>(samples[i]) * samples[i];
        }

        sum += squares / static_cast<double>(m_blockFrames);
    }

    m_blocks.push_back(sum);
}

Result Analyzer::Finish() {
    Result result;

    if (m_blocks.empty() && m_filled > 0) {
        // Shorter than one block: measure what there is instead of nothing.
        for (int channel = 0; channel < 2; ++channel) {
            std::fill(m_window[channel].begin() + m_filled, m_window[channel].end(), 0.0f);
        }

        FlushBlock();
    }

    double sum    = 0.0;
    size_t counted = 0;

    for (double power : m_blocks) {
        if (LoudnessOf(power) > kAbsoluteGate) {
            sum += power;
            ++counted;
        }
    }

    if (counted == 0) {
        return result;
    }

    const double relativeThreshold = LoudnessOf(sum / static_cast<double>(counted)) + kRelativeGate;

    sum     = 0.0;
    counted = 0;

    for (double power : m_blocks) {
        const double loudness = LoudnessOf(power);

        if (loudness > kAbsoluteGate && loudness > relativeThreshold) {
            sum += power;
            ++counted;
        }
    }

    if (counted == 0) {
        return result;
    }

    result.lufs     = static_cast<float>(LoudnessOf(sum / static_cast<double>(counted)));
    result.peakDbfs = static_cast<float>(20.0 * std::log10(std::max(static_cast<double>(m_peak), 1e-9)));
    result.valid    = true;

    return result;
}

float NormalizationGain(float lufs, float peakDbfs, float targetLufs) {
    // Unmeasured, silent or nonsensical: leave the file alone.
    if (!(lufs < 0.0f) || peakDbfs < -119.0f) {
        return 1.0f;
    }

    float gainDb = (targetLufs + kEngineTrimDb) - lufs;
    gainDb       = std::clamp(gainDb, -40.0f, 6.0f);

    if (peakDbfs + gainDb > -1.0f) {
        gainDb = -1.0f - peakDbfs;
    }

    return std::pow(10.0f, gainDb / 20.0f);
}

bool MeasureWavFile(const char* path, Result& out) {
    out = Result();

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    WavFormat format;
    if (!ReadWavFormat(file, format)) {
        return false;
    }

    const bool isFloat = (format.tag == 3);
    const bool isPcm   = (format.tag == 1);

    if ((!isPcm && !isFloat) || format.channels == 0 || format.channels > 8 || format.rate == 0) {
        return false;
    }
    if (isPcm && format.bits != 8 && format.bits != 16 && format.bits != 24 && format.bits != 32) {
        return false;
    }
    if (isFloat && format.bits != 32 && format.bits != 64) {
        return false;
    }

    const size_t bytesPerSample = format.bits / 8;
    const size_t frameBytes     = bytesPerSample * format.channels;

    size_t dataSize = format.dataSize;

    // A streamed WAV can leave the data size unfilled.
    if (dataSize == 0) {
        file.seekg(0, std::ios::end);

        const std::streamoff end = file.tellg();
        if (end <= static_cast<std::streamoff>(format.dataOffset)) {
            return true;
        }

        dataSize = static_cast<size_t>(end - static_cast<std::streamoff>(format.dataOffset));
    }

    file.seekg(static_cast<std::streamoff>(format.dataOffset), std::ios::beg);
    if (!file) {
        return true;
    }

    Analyzer analyzer(format.rate);

    constexpr size_t kFramesPerChunk = 8192;
    std::vector<unsigned char> raw(kFramesPerChunk * frameBytes);
    std::vector<float> stereo(kFramesPerChunk * 2);

    size_t remaining = dataSize;

    while (remaining >= frameBytes && file) {
        const size_t frames = std::min(remaining / frameBytes, kFramesPerChunk);
        const size_t bytes  = frames * frameBytes;

        file.read(reinterpret_cast<char*>(raw.data()), static_cast<std::streamsize>(bytes));
        if (file.gcount() < static_cast<std::streamsize>(bytes)) {
            break;
        }

        for (size_t i = 0; i < frames; ++i) {
            const unsigned char* frame = raw.data() + i * frameBytes;

            const float left  = ReadSample(frame, format);
            const float right = (format.channels > 1) ? ReadSample(frame + bytesPerSample, format) : left;

            stereo[i * 2]     = left;
            stereo[i * 2 + 1] = right;
        }

        analyzer.Feed(stereo.data(), frames);

        remaining -= bytes;
    }

    out = analyzer.Finish();

    return true;
}

} // namespace MusicSwitcherLoudness
