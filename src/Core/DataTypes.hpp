#pragma once

#include "Macros.h"

typedef char int8;
typedef short int16;
typedef long int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef uint8 bool8;
typedef uint16 bool16;
typedef uint32 bool32;
typedef uint64 bool64;

typedef uint8 byte8;
typedef uint16 byte16;
typedef uint32 byte32;
typedef uint64 byte64;

typedef float float32;
typedef double float64;

// @Research: Signed and unsigned versions of these should be useful.
// For example for string functions like LastIndexOf to return -1.

#ifdef _WIN64

typedef uint64 offset_t;
typedef uint64 new_size_t;

#else

typedef uint32 offset_t;
typedef uint32 new_size_t;

#endif

struct vec2 {
    float x;
    float y;
};

struct vec3 {
    float x;
    float y;
    float z;
};

struct vec4 {
    float x;
    float y;
    float z;
    float a;
};

struct struct_t {};
struct array_t {};
