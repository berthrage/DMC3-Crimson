#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// Integrated loudness of a music file, so every song in the switcher lands on
// the level of the game's battle themes whatever its mastering is.
namespace MusicSwitcherLoudness {

struct Result {
    float lufs     = 0.0f;
    float peakDbfs = -120.0f;
    bool valid     = false;
};

// Default level every song is measured onto. Median of Battle_00..Battle_0a as
// shipped with the HD Collection is -14.9 LUFS by this analyzer; this sits 62%
// below it (0.38 amplitude, -8.40 dB) because matching the median and every step
// down from it still played above the game's own mix in a fight.
constexpr float kDefaultTargetLufs = -22.7f;

// Absorbs the playback gain the engine itself applies to its cues, which cannot
// be read from inside the process. Zero until an A/B in game says otherwise.
constexpr float kEngineTrimDb = 0.0f;

// ITU-R BS.1770-4: K-weighting, 400 ms blocks at 75% overlap, absolute and
// relative gating. The weighting and the block length follow the sample rate,
// so a file can be measured at its own rate instead of being resampled. A long
// file costs one block of memory.
class Analyzer {
public:
    explicit Analyzer(uint32_t sampleRate = 48000);
    ~Analyzer();

    void Feed(const float* interleavedStereo, size_t frames);
    Result Finish();

private:
    struct Biquad {
        double b0 = 0.0;
        double b1 = 0.0;
        double b2 = 0.0;
        double a1 = 0.0;
        double a2 = 0.0;
        double x1 = 0.0;
        double x2 = 0.0;
        double y1 = 0.0;
        double y2 = 0.0;

        void Set(double nb0, double nb1, double nb2, double na1, double na2);
        double Process(double x);
    };

    void FlushBlock();

    Biquad m_shelf[2];
    Biquad m_highPass[2];
    std::vector<float> m_window[2];
    std::vector<double> m_blocks;
    float m_peak         = 0.0f;
    size_t m_blockFrames = 0;
    size_t m_hopFrames   = 0;
    size_t m_filled      = 0;
};

// Linear gain that puts `lufs` on `targetLufs`. Clamped so a bad measurement
// cannot ask for +40 dB, and reduced so the file cannot start clipping.
float NormalizationGain(float lufs, float peakDbfs, float targetLufs);

// Measures a PCM or float WAV straight from the file, at its own sample rate.
// The mixer's decoder stops after half of a WAV data chunk, so WAVs are never
// sent through it. True means the file was handled here, whatever the result;
// false means it is not a WAV this reader understands and the caller should
// decode it itself.
bool MeasureWavFile(const char* path, Result& out);

} // namespace MusicSwitcherLoudness
