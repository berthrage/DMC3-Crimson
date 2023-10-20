# Simple SDL2 Audio

## About

* A simple native SDL2 Audio library that has 2 files, and an easy to use interface.
* This library works without SDL2 Mixer, and plays a single music file at a time, and unlimited sounds (Mixes audio natively without Mixer)

## Install

* Include `src/audio.c` and `src/audio.h` in your project

## Examples

* `src/test.c` shows all the functionality possible:

Basic use case:

```c
// Initialize SDL2 Audio only
SDL_Init(SDL_INIT_AUDIO);

// Initialize Simple-SDL2-Audio
initAudio();

// Play music and a sound
playMusic("music/highlands.wav", SDL_MIX_MAXVOLUME);
playSound("sounds/door1.wav", SDL_MIX_MAXVOLUME / 2);

// Let play for 1 second
SDL_Delay(1000);

// End Simple-SDL2-Audio
endAudio();

// End SDL2
SDL_Quit();
```

## API Functions:

```c
// Initialize Simple-SDL2-Audio on default audio device
void initAudio(void);

// Play many Sounds or single Musics
void playSound(const char * filename, int volume);
void playMusic(const char * filename, int volume);

// Clean up Simple-SDL2-Audio
void endAudio(void);

// Pause or Unpause running audio
void pauseAudio(void);
void unpauseAudio(void);

// Advanced functions used for caching WAV files in memory, create, play many times, free
Audio * createAudio(const char * filename, uint8_t loop, int volume);
void playSoundFromMemory(Audio * audio, int volume);
void playMusicFromMemory(Audio * audio, int volume);
void freeAudio(Audio * audio);
```

## Difference between Music vs Sound

* Only one music can play at a time, and it loops (to close music you can just run `endAudio()`, or use `pauseAudio()` and `unpauseAudio()`).
   * If you add another music when one is playing, the first one fades out before ending, and then playing the second.
   * If you play more than 2 music at once, the first fades as expected, only the last music queued before the first fade out is used

* Any number of sounds can be played at once, but obviously the more, can become distorted
   * Can change `AUDIO_MAX_SOUNDS` in `src/audio.c` to limit how many sounds can be played at once to reduce distortion from too many playing

## Caveats

* This implementation uses SDL_MixAudioFormat for mixing for simplicity. It's noted "Do not use this function for mixing together more than two streams of sample data". While only playing 1 music removes a lot of these issues, if you need something more powerful you should write your own mixing function.
* This implementation ONLY plays WAV files, and they should all be the same format, but can have differing formats if you play around with `SDL_AUDIO_ALLOW_CHANGES` in `src/audio.c`, see the top of `src/audio.c` to set the format, stereo vs mono etc... No conversion
* Caching: Using the standard `playMusic()` functions makes a disk read each call. To only make one disk read, cache, and play the audio from memory, use the `createAudio(); playSoundFromMemory(); freeAudio();` functions (recommend storing the Audio* object in a dictionary / hashmap)

## Features to add

* Pause / unpause only music, only sound or ~~both~~
* Current implementation uses callback method, however in SDL 2.0.4 there exists `SDL_QueueAudio()` (no callback)
* For gSoundCount check for duplicate sounds

## Windows 7 Compatability

* [Github Issue - Solution](https://github.com/jakebesworth/Simple-SDL2-Audio/issues/3)

SDL2.0.6 updated how audio was handled, for Windows 7 using a later release of SDL2, you need to set `#define SDL_AUDIO_ALLOW_CHANGES SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE` which is a flag near the top of the file.

## Emscripten Compatibility

* [Github Issue - Solution](https://github.com/jakebesworth/Simple-SDL2-Audio/issues/2)

## Resources

* I made this project as a more modern version of https://gist.github.com/armornick/3447121
* https://davidgow.net/handmadepenguin/ch7.html
* http://rerwarwar.weebly.com/sdl2-audio.html

## Contributors

* [Jake Besworth](https://github.com/jakebesworth)
* [Lorenzo Mancini](https://github.com/lmancini)
* [Ted](https://github.com/claimred)
* [Eric Boez](https://github.com/ericb59)
* [Ivan Karlović](https://github.com/Karlovsky120)
