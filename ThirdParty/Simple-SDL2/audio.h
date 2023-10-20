/*
 * Simple-SDL2-Audio
 *
 * Copyright 2016 Jake Besworth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 * audio.h
 *
 * All audio related functions go here
 *
 */
#ifndef SIMPLE_AUDIO_
#define SIMPLE_AUDIO_

#ifdef __cplusplus
extern "C"
{
#endif

#include "../ThirdParty/SDL2/SDL.h"

/*
 * Queue structure for all loaded sounds
 *
 */
typedef struct sound
{
    uint32_t length;
    uint32_t lengthTrue;
    uint8_t * bufferTrue;
    uint8_t * buffer;
    uint8_t loop;
    uint8_t fade;
    uint8_t free;
    uint8_t volume;

    SDL_AudioSpec audio;

    struct sound * next;
} Audio;

/*
 * Create a Audio object
 *
 * @param filename      Filename for the WAVE file to load
 * @param loop          0 ends after playing once (sound), 1 repeats and fades when other music added (music)
 * @param volume        Volume, read playSound()
 *
 * @return returns a new Audio or NULL on failure, you must call freeAudio() on return Audio
 *
 */
Audio * createAudio(const char * filename, uint8_t loop, int volume);

/*
 * Frees as many chained Audios as given
 *
 * @param audio     Chain of sounds to free
 *
 */
void freeAudio(Audio * audio);

/*
 * Play a wave file currently must be S16LE format 2 channel stereo
 *
 * @param filename      Filename to open, use getAbsolutePath
 * @param volume        Volume 0 - 128. SDL_MIX_MAXVOLUME constant for max volume
 *
 */
void playSound(const char * filename, int volume);

/*
 * Plays a new music, only 1 at a time plays
 *
 * @param filename      Filename of the WAVE file to load
 * @param volume        Volume read playSound for moree
 *
 */
void playMusic(const char * filename, int volume);

/*
 * Plays a sound from a createAudio object (clones), only 1 at a time plays
 * Advantage to this method is no more disk reads, only once, data is stored and constantly reused
 *
 * @param audio         Audio object to clone and use
 * @param volume        Volume read playSound for moree
 *
 */
int playSoundFromMemory(Audio * audio, int volume);

/*
 * Plays a music from a createAudio object (clones), only 1 at a time plays
 * Advantage to this method is no more disk reads, only once, data is stored and constantly reused
 *
 * @param audio         Audio object to clone and use
 * @param volume        Volume read playSound for moree
 *
 */
void playMusicFromMemory(Audio * audio, int volume);

/*
 * Free all audio related variables
 * Note, this needs to be run even if initAudio fails, because it frees the global audio device
 *
 */
void endAudio(void);

/*
 * Initialize Audio Variable
 *
 */
int initAudio(void);

/*
 * Pause audio from playing
 *
 */
void pauseAudio(void);

/*
 * Unpause audio from playing
 *
 */
void unpauseAudio(void);

#ifdef __cplusplus
}
#endif

#endif
