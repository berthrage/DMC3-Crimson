/* Compatibility shim: SDL2's SDL_rwops.h for SDL3.
   SDL3 renamed SDL_RWops to SDL_IOStream in SDL_iostream.h.
   This provides an opaque forward declaration so SDL2_mixer.h compiles
   against SDL3 headers without pulling in conflicting SDL2 core types. */

#ifndef SDL_rwops_h_
#define SDL_rwops_h_

/* Opaque type - SDL_mixer.h only uses SDL_RWops* as a pointer */
typedef struct SDL_IOStream SDL_RWops;

/* SDL3 removed SDL_bool; SDL2_mixer.h still uses it. Map to C bool. */
#ifndef SDL_bool
#define SDL_bool bool
#endif

/* SDL_MIX_MAXVOLUME used by SDL_mixer.h, not defined in SDL3 */
#ifndef SDL_MIX_MAXVOLUME
#define SDL_MIX_MAXVOLUME 128
#endif

#endif /* SDL_rwops_h_ */
