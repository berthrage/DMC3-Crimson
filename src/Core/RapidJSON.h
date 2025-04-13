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


// #define RAPIDJSON_PARSE_DEFAULT_FLAGS (kParseCommentsFlag | kParseTrailingCommasFlag)


#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/prettywriter.h"
