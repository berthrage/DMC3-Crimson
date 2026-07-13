// RapidJSON headers and configuration are now provided by the precompiled
// header (pch.hpp). This file only defines the SizeType specialization
// that must appear before any RapidJSON header is included.

#define RAPIDJSON_NO_SIZETYPEDEFINE
#define RAPIDJSON_HAS_STDSTRING 1
#include <string>


namespace rapidjson {

#ifdef _WIN64
typedef unsigned long long SizeType;
#else
typedef unsigned long SizeType;
#endif

} // namespace rapidjson
