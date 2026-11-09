// Tag, artwork and playlist readers for the music switcher.
//
// Only the head of each file is read: tag frames carry their own lengths, so a
// 200 MB FLAC costs the same as a 3 MB MP3. The embedded picture itself is never
// loaded here — the song only records where it is.

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>

#include "MusicSwitcherTags.hpp"

namespace MusicSwitcherTags {

namespace {

// Enough for ID3 text frames and a full FLAC/OGG comment block in practice.
constexpr size_t kHeadBytes = 1024 * 1024;

constexpr size_t kMaxArtworkBytes = 8 * 1024 * 1024;

using Bytes = std::vector<uint8>;

bool ReadHead(const std::string& path, Bytes& out, uint64& outFileSize) {
    out.clear();
    outFileSize = 0;

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    file.seekg(0, std::ios::end);
    const std::streamoff size = file.tellg();
    if (size <= 0) {
        return false;
    }
    outFileSize = static_cast<uint64>(size);

    const size_t want = (static_cast<uint64>(size) < kHeadBytes) ? static_cast<size_t>(size) : kHeadBytes;
    out.resize(want);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(want));

    const std::streamsize got = file.gcount();
    if (got <= 0) {
        out.clear();
        return false;
    }
    out.resize(static_cast<size_t>(got));

    return true;
}

bool ReadRange(const std::string& path, uint64 offset, uint32 size, Bytes& out) {
    out.clear();
    if (size == 0 || size > kMaxArtworkBytes) {
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
    if (!file) {
        return false;
    }

    out.resize(size);
    file.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));

    const std::streamsize got = file.gcount();
    if (got <= 0) {
        out.clear();
        return false;
    }
    out.resize(static_cast<size_t>(got));

    return true;
}

// --- byte readers ---------------------------------------------------------

uint32 Be24(const Bytes& b, size_t pos) {
    return (static_cast<uint32>(b[pos]) << 16) | (static_cast<uint32>(b[pos + 1]) << 8) |
        static_cast<uint32>(b[pos + 2]);
}

uint32 Be32(const Bytes& b, size_t pos) {
    return (static_cast<uint32>(b[pos]) << 24) | (static_cast<uint32>(b[pos + 1]) << 16) |
        (static_cast<uint32>(b[pos + 2]) << 8) | static_cast<uint32>(b[pos + 3]);
}

uint32 Le32(const Bytes& b, size_t pos) {
    return static_cast<uint32>(b[pos]) | (static_cast<uint32>(b[pos + 1]) << 8) |
        (static_cast<uint32>(b[pos + 2]) << 16) | (static_cast<uint32>(b[pos + 3]) << 24);
}

// ID3 uses 7 bits per byte so the size never looks like a frame sync.
uint32 SyncSafe32(const Bytes& b, size_t pos) {
    return (static_cast<uint32>(b[pos] & 0x7F) << 21) | (static_cast<uint32>(b[pos + 1] & 0x7F) << 14) |
        (static_cast<uint32>(b[pos + 2] & 0x7F) << 7) | static_cast<uint32>(b[pos + 3] & 0x7F);
}

bool MatchAt(const Bytes& b, size_t pos, const char* text, size_t length) {
    return pos + length <= b.size() && memcmp(b.data() + pos, text, length) == 0;
}

// --- text ----------------------------------------------------------------

void AppendUtf8(std::string& out, uint32 codePoint) {
    if (codePoint < 0x80) {
        out.push_back(static_cast<char>(codePoint));
    } else if (codePoint < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    } else if (codePoint < 0x10000) {
        out.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    }
}

std::string Latin1ToUtf8(const uint8* data, size_t length) {
    std::string out;
    out.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        AppendUtf8(out, data[i]);
    }

    return out;
}

std::string Utf16ToUtf8(const uint8* data, size_t length, bool bigEndianHint) {
    bool bigEndian = bigEndianHint;

    // A BOM overrides the encoding byte, which is what most taggers rely on.
    if (length >= 2 && data[0] == 0xFF && data[1] == 0xFE) {
        bigEndian = false;
        data += 2;
        length -= 2;
    } else if (length >= 2 && data[0] == 0xFE && data[1] == 0xFF) {
        bigEndian = true;
        data += 2;
        length -= 2;
    }

    std::string out;
    out.reserve(length / 2);

    for (size_t i = 0; i + 1 < length; i += 2) {
        uint32 unit = bigEndian ? ((static_cast<uint32>(data[i]) << 8) | data[i + 1])
                                : ((static_cast<uint32>(data[i + 1]) << 8) | data[i]);

        if (unit >= 0xD800 && unit <= 0xDBFF && i + 3 < length) {
            const uint32 low = bigEndian ? ((static_cast<uint32>(data[i + 2]) << 8) | data[i + 3])
                                         : ((static_cast<uint32>(data[i + 3]) << 8) | data[i + 2]);
            if (low >= 0xDC00 && low <= 0xDFFF) {
                unit = 0x10000 + ((unit - 0xD800) << 10) + (low - 0xDC00);
                i += 2;
            }
        }

        AppendUtf8(out, unit);
    }

    return out;
}

// encoding: 0 latin-1, 1 utf-16 with BOM, 2 utf-16be, 3 utf-8
std::string DecodeTagText(uint8 encoding, const uint8* data, size_t length) {
    // Trim the padding in whole units — cutting a UTF-16 string by single bytes
    // would drop the last character's high half and lose it entirely.
    if (encoding == 1 || encoding == 2) {
        while (length >= 2 && data[length - 1] == 0 && data[length - 2] == 0) {
            length -= 2;
        }
    } else {
        while (length > 0 && data[length - 1] == 0) {
            --length;
        }
    }

    if (length == 0) {
        return std::string();
    }

    switch (encoding) {
    case 1: {
        return Utf16ToUtf8(data, length, false);
    }
    case 2: {
        return Utf16ToUtf8(data, length, true);
    }
    case 3: {
        return std::string(reinterpret_cast<const char*>(data), length);
    }
    default: {
        return Latin1ToUtf8(data, length);
    }
    }
}

size_t TextTerminatorLength(uint8 encoding) {
    return (encoding == 1 || encoding == 2) ? 2 : 1;
}

// Walks the terminator of a null-terminated tag string and returns the offset
// just past it, or `length` when there is none.
size_t SkipTerminated(const Bytes& b, size_t pos, size_t limit, uint8 encoding) {
    const size_t step = TextTerminatorLength(encoding);

    while (pos + step <= limit) {
        if (b[pos] == 0 && (step == 1 || b[pos + 1] == 0)) {
            return pos + step;
        }
        pos += step;
    }

    return limit;
}

// --- base64 --------------------------------------------------------------

bool Base64Decode(const std::string& in, Bytes& out) {
    out.clear();
    out.reserve(in.size() * 3 / 4);

    int accumulator = 0;
    int bits        = 0;

    for (char c : in) {
        int value = -1;

        if (c >= 'A' && c <= 'Z') {
            value = c - 'A';
        } else if (c >= 'a' && c <= 'z') {
            value = c - 'a' + 26;
        } else if (c >= '0' && c <= '9') {
            value = c - '0' + 52;
        } else if (c == '+') {
            value = 62;
        } else if (c == '/') {
            value = 63;
        } else if (c == '=') {
            break;
        } else {
            continue;
        }

        accumulator = (accumulator << 6) | value;
        bits += 6;

        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<uint8>((accumulator >> bits) & 0xFF));
        }
    }

    return !out.empty();
}

// --- ID3v2 ---------------------------------------------------------------

// `frameSize` is the size the frame declares, which may reach past the head we
// read. The picture length comes from it rather than from what is buffered, so
// a 2 MB cover in a 1 MB head still resolves to a usable offset.
void ParseApicFrame(MusicSwitcherSong& song, const Bytes& head, uint64 fileSize, size_t frameStart,
    size_t frameSize, bool offsetReliable) {
    const size_t end = head.size();
    if (frameStart >= end || frameSize == 0) {
        return;
    }

    size_t pos = frameStart;

    const uint8 encoding = head[pos++];

    // mime type is always latin-1, whatever the text encoding says
    const size_t mimeStart = pos;
    while (pos < end && head[pos] != 0) {
        ++pos;
    }
    const std::string mime = Latin1ToUtf8(head.data() + mimeStart, pos - mimeStart);
    ++pos;

    // picture type
    if (pos >= end) {
        return;
    }
    ++pos;

    // description, in the frame's own encoding
    pos = SkipTerminated(head, pos, end, encoding);
    if (pos >= end) {
        return;
    }

    const size_t consumed = pos - frameStart;
    if (consumed >= frameSize) {
        return;
    }

    const uint64 dataSize = frameSize - consumed;
    if (dataSize > kMaxArtworkBytes) {
        return;
    }

    // De-unsynchronised tags shift every byte, so their offset would point at
    // the wrong place; those skip the art instead.
    const uint64 relative = static_cast<uint64>(pos);
    if (!offsetReliable || relative + dataSize > fileSize) {
        return;
    }

    song.artwork.offset = relative;
    song.artwork.size   = static_cast<uint32>(dataSize);
    song.artwork.mimeType = mime.empty() ? std::string("image/jpeg") : mime;
    song.artwork.insideComment = false;
}

void ParseId3v2(MusicSwitcherSong& song, const Bytes& head, uint64 fileSize) {
    if (head.size() < 10 || !MatchAt(head, 0, "ID3", 3)) {
        return;
    }

    const uint8 major = head[3];
    const uint8 flags = head[5];
    uint32 tagSize    = SyncSafe32(head, 6);

    size_t body = 10;
    if (body + tagSize > head.size()) {
        tagSize = static_cast<uint32>(head.size() - body);
    }
    size_t end = body + tagSize;

    // v2.3 applies unsynchronisation to the whole tag; v2.4 does it per frame.
    Bytes unsynchronised;
    if (major < 4 && (flags & 0x80)) {
        unsynchronised.reserve(tagSize);
        for (size_t i = body; i < end; ++i) {
            unsynchronised.push_back(head[i]);
            if (head[i] == 0xFF && i + 1 < end && head[i + 1] == 0x00) {
                ++i;
            }
        }
        body = 0;
        end  = unsynchronised.size();
    }
    const Bytes& tag = unsynchronised.empty() ? head : unsynchronised;

    if (flags & 0x40) {
        if (major >= 4) {
            if (body + 4 > end) {
                return;
            }
            body += SyncSafe32(tag, body);
        } else {
            if (body + 4 > end) {
                return;
            }
            body += Be32(tag, body) + 4;
        }
    }

    const bool v22 = (major <= 2);

    while (body + (v22 ? 6u : 10u) <= end) {
        if (tag[body] == 0) {
            break; // padding
        }

        char id[5] = {};
        memcpy(id, tag.data() + body, v22 ? 3 : 4);

        uint32 frameSize = 0;
        size_t frameHeader = 0;

        if (v22) {
            frameSize   = Be24(tag, body + 3);
            frameHeader = 6;
        } else if (major >= 4) {
            frameSize   = SyncSafe32(tag, body + 4);
            frameHeader = 10;
        } else {
            frameSize   = Be32(tag, body + 4);
            frameHeader = 10;
        }

        const size_t frameStart = body + frameHeader;
        if (frameSize == 0 || frameStart >= end) {
            break;
        }

        // A picture can reach past the head we read; its declared size is what
        // matters, so it is the one frame allowed to overflow.
        const bool isPicture = !strcmp(id, "APIC") || !strcmp(id, "PIC");
        const size_t frameEnd = frameStart + frameSize;
        if (!isPicture && frameEnd > end) {
            break;
        }

        const size_t dataPos = frameStart + 1; // skip the text-encoding byte

        if (dataPos <= frameEnd) {
            const uint8 encoding = tag[frameStart];
            const size_t length  = frameEnd - dataPos;

            if (!strcmp(id, "TIT2") || !strcmp(id, "TT2")) {
                song.title = DecodeTagText(encoding, tag.data() + dataPos, length);
            } else if (!strcmp(id, "TPE1") || !strcmp(id, "TP1")) {
                song.artist = DecodeTagText(encoding, tag.data() + dataPos, length);
            } else if (!strcmp(id, "TALB") || !strcmp(id, "TAL")) {
                song.album = DecodeTagText(encoding, tag.data() + dataPos, length);
            } else if (isPicture) {
                if (v22) {
                    // v2.2: encoding, 3-char image format, picture type, description
                    if (frameStart + 5 > end) {
                        body = frameEnd;
                        continue;
                    }
                    const uint8 picEncoding = tag[frameStart];
                    std::string format(reinterpret_cast<const char*>(tag.data() + frameStart + 1), 3);
                    for (auto& ch : format) {
                        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
                    }
                    size_t pos = frameStart + 5;
                    pos        = SkipTerminated(tag, pos, end, picEncoding);
                    if (pos < end) {
                        const size_t consumed = pos - frameStart;
                        const uint64 dataSize = (consumed < frameSize) ? (frameSize - consumed) : 0;
                        if (dataSize > 0 && dataSize <= kMaxArtworkBytes && unsynchronised.empty() &&
                            static_cast<uint64>(pos) + dataSize <= fileSize) {
                            song.artwork.offset = pos;
                            song.artwork.size   = static_cast<uint32>(dataSize);
                            song.artwork.mimeType =
                                (format == "PNG") ? "image/png" : "image/jpeg";
                            song.artwork.insideComment = false;
                        }
                    }
                } else if (!song.HasArtwork()) {
                    ParseApicFrame(song, tag, fileSize, frameStart, frameSize, unsynchronised.empty());
                }
            }
        }

        body = frameEnd;
    }
}

// --- Vorbis comments -----------------------------------------------------

// `pos` points at the comment list. Returns the offset after it.
// Fills title/artist/album and, for METADATA_BLOCK_PICTURE, the artwork.
void ParseVorbisComments(MusicSwitcherSong& song, const Bytes& b, size_t pos, size_t end) {
    if (pos + 4 > end) {
        return;
    }

    const uint32 vendorLength = Le32(b, pos);
    pos += 4;
    if (pos + vendorLength > end) {
        return;
    }
    pos += vendorLength;

    if (pos + 4 > end) {
        return;
    }
    const uint32 count = Le32(b, pos);
    pos += 4;

    for (uint32 i = 0; i < count && pos + 4 <= end; ++i) {
        const uint32 length = Le32(b, pos);
        pos += 4;

        if (pos + length > end) {
            return;
        }

        const char* text = reinterpret_cast<const char*>(b.data() + pos);
        const char* equals = static_cast<const char*>(memchr(text, '=', length));

        if (equals != NULL) {
            std::string key(text, equals - text);
            for (auto& ch : key) {
                ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
            }

            const std::string value(equals + 1, length - (equals + 1 - text));

            if (key == "TITLE") {
                song.title = value;
            } else if (key == "ARTIST") {
                song.artist = value;
            } else if (key == "ALBUM") {
                song.album = value;
            } else if (key == "METADATA_BLOCK_PICTURE" && !song.HasArtwork()) {
                Bytes picture;
                if (Base64Decode(value, picture) && picture.size() > 32) {
                    // Same layout as a FLAC PICTURE block, starting at its type field.
                    const uint32 mimeLength = Be32(picture, 4);
                    if (8 + mimeLength + 4 <= picture.size()) {
                        song.artwork.mimeType.assign(
                            reinterpret_cast<const char*>(picture.data() + 8), mimeLength);
                        if (song.artwork.mimeType.empty()) {
                            song.artwork.mimeType = "image/jpeg";
                        }
                        // The bytes stay base64 in the file, so they are re-decoded
                        // when the banner asks for them.
                        song.artwork.insideComment = true;
                        song.artwork.size          = static_cast<uint32>(value.size());
                        song.artwork.offset        = 0;
                    }
                }
            }
        }

        pos += length;
    }
}

// --- FLAC ----------------------------------------------------------------

void ParseFlac(MusicSwitcherSong& song, const Bytes& head, uint64 fileSize) {
    if (head.size() < 4 || !MatchAt(head, 0, "fLaC", 4)) {
        return;
    }

    size_t pos = 4;
    bool last  = false;

    while (!last && pos + 4 <= head.size()) {
        last = (head[pos] & 0x80) != 0;
        const uint8 type = head[pos] & 0x7F;
        const uint32 length = Be24(head, pos + 1);
        pos += 4;

        if (length == 0) {
            break;
        }

        // Only the fields we read have to be buffered: a picture can reach past
        // the head, and the walk still advances by the declared block length.
        const size_t blockEnd = pos + length;

        if (type == 4) {
            if (blockEnd <= head.size()) {
                ParseVorbisComments(song, head, pos, blockEnd);
            }
        } else if (type == 6 && !song.HasArtwork()) {
            // type, mimeLen, mime, descLen, desc, width, height, depth, colors, dataLen, data
            if (pos + 8 <= head.size() && pos + 8 <= blockEnd) {
                const uint32 mimeLength = Be32(head, pos + 4);
                const size_t mimeStart  = pos + 8;

                if (mimeLength <= blockEnd - mimeStart && mimeStart + mimeLength <= head.size()) {
                    song.artwork.mimeType.assign(reinterpret_cast<const char*>(head.data() + mimeStart),
                        mimeLength);
                    if (song.artwork.mimeType.empty()) {
                        song.artwork.mimeType = "image/jpeg";
                    }

                    size_t p = mimeStart + mimeLength;
                    if (p + 4 <= head.size()) {
                        const uint32 descriptionLength = Be32(head, p);
                        p += 4 + descriptionLength + 16; // description + w/h/depth/colors

                        if (p + 4 <= head.size() && p + 4 <= blockEnd) {
                            const uint64 dataSize = Be32(head, p);
                            p += 4;

                            if (dataSize > 0 && dataSize <= kMaxArtworkBytes && p + dataSize <= fileSize) {
                                song.artwork.offset = p;
                                song.artwork.size   = static_cast<uint32>(dataSize);
                                song.artwork.insideComment = false;
                            }
                        }
                    }
                }
            }
        }

        pos = blockEnd;
    }
}

// --- Ogg --------------------------------------------------------------

// Rebuilds the first two packets of the stream (identification and comment
// header) from the page sequence.
bool ReadOggCommentPacket(const Bytes& head, Bytes& outPacket) {
    outPacket.clear();

    if (head.size() < 27 || !MatchAt(head, 0, "OggS", 4)) {
        return false;
    }

    size_t pos = 0;
    int packets = 0;
    Bytes current;

    while (pos + 27 <= head.size()) {
        if (!MatchAt(head, pos, "OggS", 4)) {
            break;
        }

        const uint8 segmentCount = head[pos + 26];
        const size_t lacingStart = pos + 27;

        if (lacingStart + segmentCount > head.size()) {
            break;
        }

        size_t bodySize = 0;
        for (uint8 i = 0; i < segmentCount; ++i) {
            bodySize += head[lacingStart + i];
        }

        const size_t bodyStart = lacingStart + segmentCount;
        if (bodyStart + bodySize > head.size()) {
            break;
        }

        size_t cursor = bodyStart;
        for (uint8 i = 0; i < segmentCount; ++i) {
            const uint8 lacing = head[lacingStart + i];
            current.insert(current.end(), head.begin() + cursor, head.begin() + cursor + lacing);
            cursor += lacing;

            if (lacing < 255) {
                ++packets;
                if (packets == 2) {
                    outPacket = current;
                    return !outPacket.empty();
                }
                current.clear();
            }
        }

        pos = bodyStart + bodySize;
    }

    return false;
}

void ParseOgg(MusicSwitcherSong& song, const Bytes& head) {
    Bytes comment;
    if (!ReadOggCommentPacket(head, comment)) {
        return;
    }

    // Vorbis: \x03vorbis, Opus: OpusTags, both followed by the comment list.
    if (comment.size() > 7 && comment[0] == 0x03 && memcmp(comment.data() + 1, "vorbis", 6) == 0) {
        ParseVorbisComments(song, comment, 7, comment.size());
    } else if (comment.size() > 8 && memcmp(comment.data(), "OpusTags", 8) == 0) {
        ParseVorbisComments(song, comment, 8, comment.size());
    }
}

std::string ExtensionOf(const std::string& path) {
    const size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) {
        return std::string();
    }

    std::string extension = path.substr(dot);
    for (auto& ch : extension) {
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    }

    return extension;
}

} // namespace

void ReadSongTags(MusicSwitcherSong& song, const std::string& path) {
    Bytes head;
    uint64 fileSize = 0;

    if (!ReadHead(path, head, fileSize)) {
        return;
    }

    const std::string extension = ExtensionOf(path);

    if (extension == ".mp3") {
        ParseId3v2(song, head, fileSize);
    } else if (extension == ".flac") {
        ParseFlac(song, head, fileSize);
    } else if (extension == ".ogg") {
        ParseOgg(song, head);
    }
    // WAV carries no usable tags; the file name stands in for the title.
}

bool ReadSongArtwork(const MusicSwitcherSong& song, const std::string& path, std::vector<uint8>& outBytes,
    std::string& outMimeType) {
    outBytes.clear();
    outMimeType.clear();

    if (!song.HasArtwork()) {
        return false;
    }

    outMimeType = song.artwork.mimeType;

    if (!song.artwork.insideComment) {
        return ReadRange(path, song.artwork.offset, song.artwork.size, outBytes);
    }

    // Ogg: the picture lives base64-encoded in the comment header, so re-read it.
    Bytes head;
    uint64 fileSize = 0;
    if (!ReadHead(path, head, fileSize)) {
        return false;
    }

    Bytes comment;
    if (!ReadOggCommentPacket(head, comment)) {
        return false;
    }

    size_t start = 0;
    if (comment.size() > 7 && comment[0] == 0x03) {
        start = 7;
    } else if (comment.size() > 8) {
        start = 8;
    } else {
        return false;
    }

    // Walk the comment list once more, this time decoding the picture.
    if (start + 4 > comment.size()) {
        return false;
    }
    size_t pos = start + 4 + Le32(comment, start);
    if (pos + 4 > comment.size()) {
        return false;
    }

    const uint32 count = Le32(comment, pos);
    pos += 4;

    for (uint32 i = 0; i < count && pos + 4 <= comment.size(); ++i) {
        const uint32 length = Le32(comment, pos);
        pos += 4;
        if (pos + length > comment.size()) {
            return false;
        }

        const char* text   = reinterpret_cast<const char*>(comment.data() + pos);
        const char* equals = static_cast<const char*>(memchr(text, '=', length));
        if (equals != NULL && static_cast<size_t>(equals - text) == 22 &&
            memcmp(text, "METADATA_BLOCK_PICTURE", 22) == 0) {
            Bytes picture;
            if (!Base64Decode(std::string(equals + 1, length - (equals + 1 - text)), picture)) {
                return false;
            }
            if (picture.size() < 32) {
                return false;
            }

            const uint32 mimeLength = Be32(picture, 4);
            if (8 + mimeLength + 4 > picture.size()) {
                return false;
            }

            outMimeType.assign(reinterpret_cast<const char*>(picture.data() + 8), mimeLength);
            if (outMimeType.empty()) {
                outMimeType = "image/jpeg";
            }

            size_t p = 8 + mimeLength;
            p += 4 + Be32(picture, p) + 16;

            if (p + 4 > picture.size()) {
                return false;
            }
            const uint32 dataSize = Be32(picture, p);
            p += 4;

            if (dataSize == 0 || p + dataSize > picture.size()) {
                return false;
            }

            outBytes.assign(picture.begin() + p, picture.begin() + p + dataSize);
            return true;
        }

        pos += length;
    }

    return false;
}

} // namespace MusicSwitcherTags
