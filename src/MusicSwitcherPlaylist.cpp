// Playlist JSON reader and writer.
//
// The file format is the shareable part of the feature: it names songs, not
// paths, so a playlist keeps working when the folder moves.

#include "MusicSwitcherPlaylist.hpp"

#include <cstring>
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "Core/Core.hpp"

namespace MusicSwitcherPlaylistFile {

namespace {

using rapidjson::Document;
using rapidjson::PrettyWriter;
using rapidjson::StringBuffer;
using rapidjson::Value;

const char* JsonString(const Value& object, const char* name) {
    auto it = object.FindMember(name);

    return (it != object.MemberEnd() && it->value.IsString()) ? it->value.GetString() : NULL;
}

uint8 ClampPercent(int value, uint8 fallback) {
    if (value < 0 || value > 100) {
        return fallback;
    }

    return static_cast<uint8>(value);
}

const char* ModeName(uint8 mode) {
    switch (mode) {
    case MUSICPLAYLISTMODE::RANDOM: {
        return "random";
    }
    case MUSICPLAYLISTMODE::PER_MISSION: {
        return "perMission";
    }
    default: {
        return "sequential";
    }
    }
}

uint8 ModeFromName(const char* name) {
    if (name == NULL) {
        return MUSICPLAYLISTMODE::SEQUENTIAL;
    }

    if (!_stricmp(name, "random")) {
        return MUSICPLAYLISTMODE::RANDOM;
    }
    if (!_stricmp(name, "perMission") || !_stricmp(name, "per_mission") || !_stricmp(name, "mission")) {
        return MUSICPLAYLISTMODE::PER_MISSION;
    }

    return MUSICPLAYLISTMODE::SEQUENTIAL;
}

} // namespace

bool Load(const std::string& path, MusicSwitcherPlaylist& playlist) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (text.empty()) {
        return false;
    }

    Document document;
    if (document.Parse(text.c_str()).HasParseError() || !document.IsObject()) {
        Log("CrimsonMusicSwitcher: %s is not valid JSON.", path.c_str());

        return false;
    }

    if (const char* name = JsonString(document, "name"); name != NULL && name[0] != '\0') {
        playlist.name = name;
    }

    playlist.mode = ModeFromName(JsonString(document, "mode"));

    auto songs = document.FindMember("songs");
    if (songs == document.MemberEnd() || !songs->value.IsArray()) {
        return true;
    }

    for (const auto& item : songs->value.GetArray()) {
        if (!item.IsObject()) {
            continue;
        }

        const char* fileName = JsonString(item, "file");
        if (fileName == NULL || fileName[0] == '\0') {
            continue;
        }

        MusicSwitcherSongEntry entry;
        entry.file = fileName;

        if (auto it = item.FindMember("startMinPercent"); it != item.MemberEnd() && it->value.IsInt()) {
            entry.startMinPercent = ClampPercent(it->value.GetInt(), entry.startMinPercent);
        }

        if (auto it = item.FindMember("startMaxPercent"); it != item.MemberEnd() && it->value.IsInt()) {
            entry.startMaxPercent = ClampPercent(it->value.GetInt(), entry.startMaxPercent);
        }

        if (auto it = item.FindMember("randomStart"); it != item.MemberEnd() && it->value.IsBool()) {
            entry.randomStart = it->value.GetBool();
        }

        playlist.songs.push_back(entry);
    }

    return true;
}

bool Save(const std::string& path, const MusicSwitcherPlaylist& playlist) {
    Document document;
    document.SetObject();

    auto& allocator = document.GetAllocator();

    document.AddMember("name", Value().SetString(playlist.name.c_str(), allocator), allocator);
    document.AddMember("version", 2, allocator);
    document.AddMember("mode", Value().SetString(ModeName(playlist.mode), allocator), allocator);

    Value songs(rapidjson::kArrayType);

    for (const auto& entry : playlist.songs) {
        Value song(rapidjson::kObjectType);

        song.AddMember("file", Value().SetString(entry.file.c_str(), allocator), allocator);
        song.AddMember("startMinPercent", entry.startMinPercent, allocator);
        song.AddMember("startMaxPercent", entry.startMaxPercent, allocator);
        song.AddMember("randomStart", entry.randomStart, allocator);

        songs.PushBack(song, allocator);
    }

    document.AddMember("songs", songs, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        Log("CrimsonMusicSwitcher: cannot write %s.", path.c_str());

        return false;
    }

    file.write(buffer.GetString(), static_cast<std::streamsize>(buffer.GetSize()));

    return file.good();
}

} // namespace MusicSwitcherPlaylistFile
