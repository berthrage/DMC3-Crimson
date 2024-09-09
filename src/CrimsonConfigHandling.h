#ifndef CONFIG_CRIMSON_HANDLER_H
#define CONFIG_CRIMSON_HANDLER_H
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/prettywriter.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include <tuple>
#include <string>
#include <cstring>
#include <utility>

using namespace rapidjson;

namespace JSON {

// General function to serialize the config
template <typename ConfigT>
void SerializeConfig(Value& obj, const ConfigT& config, Document::AllocatorType& allocator);

// General function to serialize the config
template <typename ConfigT>
void SerializeConfig(Value& obj, const ConfigT& config, Document::AllocatorType& allocator);

// Helper function to handle individual types
template <typename T>
void SerializeField(Value& obj, Document::AllocatorType& allocator, const char* name, const T& value) {
	// Create or update the member in the JSON object
	Value& member = obj.HasMember(name) ? obj[name] : (obj.AddMember(StringRef(name), Value(), allocator), obj[name]);

	// Function to handle serialization of array elements
	auto serializeArrayElement = [&allocator](Value& arr, const auto& elem) {
		using ElementType = std::decay_t<decltype(elem)>;
		if constexpr (std::is_same_v<ElementType, bool>) {
			arr.PushBack(elem, allocator);
		}
		else if constexpr (std::is_same_v<ElementType, int>) {
			arr.PushBack(static_cast<int>(elem), allocator);  // int is treated as int
		}
		else if constexpr (std::is_same_v<ElementType, int8>) {
			arr.PushBack(static_cast<int>(elem), allocator);  // int8 is treated as int
		}
		else if constexpr (std::is_same_v<ElementType, int16>) {
			arr.PushBack(static_cast<int>(elem), allocator);  // int16 is treated as int
		}
		else if constexpr (std::is_same_v<ElementType, int32>) {
			arr.PushBack(static_cast<int>(elem), allocator);  // int32 is treated as int
		}
		else if constexpr (std::is_same_v<ElementType, int64>) {
			arr.PushBack(static_cast<int64_t>(elem), allocator);  // int64 is treated as int64
		}
		else if constexpr (std::is_same_v<ElementType, uint8>) {
			arr.PushBack(static_cast<unsigned int>(elem), allocator);  // uint8 is treated as unsigned int
		}
		else if constexpr (std::is_same_v<ElementType, uint16>) {
			arr.PushBack(static_cast<unsigned int>(elem), allocator);  // uint16 is treated as unsigned int
		}
		else if constexpr (std::is_same_v<ElementType, uint32>) {
			arr.PushBack(static_cast<unsigned int>(elem), allocator);  // uint32 is treated as unsigned int
		}
		else if constexpr (std::is_same_v<ElementType, uint64>) {
			arr.PushBack(static_cast<uint64_t>(elem), allocator);  // uint64 is treated as uint64
		}
		else if constexpr (std::is_floating_point_v<ElementType>) {
			arr.PushBack(static_cast<double>(elem), allocator);
		}
		else if constexpr (std::is_same_v<ElementType, std::string> || std::is_same_v<ElementType, const char*>) {
			arr.PushBack(Value().SetString(StringRef(elem.c_str()), allocator), allocator);
		}
		else {
			Value nestedObj(kObjectType);
			SerializeConfig(nestedObj, elem, allocator);
			arr.PushBack(nestedObj, allocator);
		}
		};

	if constexpr (std::is_same_v<T, bool>) {
		member.SetBool(value);
	}
	else if constexpr (std::is_same_v<T, int>) {
		member.SetInt(static_cast<int>(value));  // int is treated as int
	}
	else if constexpr (std::is_same_v<T, int8>) {
		member.SetInt(static_cast<int>(value));  // int8 is treated as int
	}
	else if constexpr (std::is_same_v<T, int16>) {
		member.SetInt(static_cast<int>(value));  // int16 is treated as int
	}
	else if constexpr (std::is_same_v<T, int32>) {
		member.SetInt(static_cast<int>(value));  // int32 is treated as int
	}
	else if constexpr (std::is_same_v<T, int64>) {
		member.SetInt64(static_cast<int64_t>(value));  // int64 is treated as int64
	}
	else if constexpr (std::is_same_v<T, uint8>) {
		member.SetUint(static_cast<unsigned int>(value));  // uint8 is treated as unsigned int
	}
	else if constexpr (std::is_same_v<T, uint16>) {
		member.SetUint(static_cast<unsigned int>(value));  // uint16 is treated as unsigned int
	}
	else if constexpr (std::is_same_v<T, uint32>) {
		member.SetUint(static_cast<unsigned int>(value));  // uint32 is treated as unsigned int
	}
	else if constexpr (std::is_same_v<T, uint64>) {
		member.SetUint64(static_cast<uint64_t>(value));  // uint64 is treated as uint64
	}
	else if constexpr (std::is_floating_point_v<T>) {
		member.SetDouble(static_cast<double>(value));
	}
	else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>) {
		member.SetString(StringRef(value.c_str()), allocator);
	}
	else if constexpr (std::is_array_v<T>) {
		Value arr(kArrayType);
		if constexpr (std::rank_v<T> == 1) {
			// 1D array
			for (const auto& elem : value) {
				serializeArrayElement(arr, elem);
			}
		}
		else if constexpr (std::rank_v<T> == 2) {
			// 2D array
			for (const auto& subArray : value) {
				Value subArr(kArrayType);
				for (const auto& elem : subArray) {
					serializeArrayElement(subArr, elem);
				}
				arr.PushBack(subArr, allocator);
			}
		}
		else if constexpr (std::rank_v<T> == 3) {
			// 3D array
			for (const auto& subSubArray : value) {
				Value subArr(kArrayType);
				for (const auto& subArray : subSubArray) {
					Value innerArr(kArrayType);
					for (const auto& elem : subArray) {
						serializeArrayElement(innerArr, elem);
					}
					subArr.PushBack(innerArr, allocator);
				}
				arr.PushBack(subArr, allocator);
			}
		}
		else {
			// Handle higher-dimensional arrays if necessary
			// Logic for arrays of higher dimensions
		}
		member = arr;
	}
	else {
		// Handle nested structs by recursively serializing
		Value nestedObj(kObjectType);
		SerializeConfig(nestedObj, value, allocator);
		member = nestedObj;
	}
}


// General function to serialize the config
template <typename ConfigT>
void SerializeConfig(Value& obj, const ConfigT& config, Document::AllocatorType& allocator) {
	std::apply([&](const auto&... members) {
		(..., SerializeField(obj, allocator, members.first, config.*(members.second)));
		}, ConfigT::Metadata());
}

template <typename ConfigT>
void ParseConfig(const Value& obj, ConfigT& config);

// Helper function to check if a value is an array
bool IsArray(const Value& value) {
	return value.IsArray();
}

template <typename T>
void ParseField(const Value& obj, const char* name, T& value) {
	if (!obj.HasMember(name)) return;  // Early exit if the member does not exist

	const auto& memberValue = obj[name];

	// Handle non-array types
	if constexpr (TypeMatch<T, bool>::value) {
		if (memberValue.IsBool()) {
			value = memberValue.GetBool();
		}
	}
	else if constexpr (TypeMatch<T, int8>::value || TypeMatch<T, int16>::value || TypeMatch<T, int32>::value) {
		if (memberValue.IsInt()) {
			value = static_cast<T>(memberValue.GetInt());
		}
	}
	else if constexpr (TypeMatch<T, int64>::value) {
		if (memberValue.IsInt64()) {
			value = static_cast<T>(memberValue.GetInt64());
		}
	}
	else if constexpr (TypeMatch<T, uint8>::value || TypeMatch<T, uint16>::value || TypeMatch<T, uint32>::value) {
		if (memberValue.IsUint()) {
			value = static_cast<T>(memberValue.GetUint());
		}
	}
	else if constexpr (TypeMatch<T, uint64>::value) {
		if (memberValue.IsUint64()) {
			value = static_cast<T>(memberValue.GetUint64());
		}
	}
	else if constexpr (TypeMatch<T, int>::value) {
		if (memberValue.IsInt()) {
			value = static_cast<T>(memberValue.GetInt());
		}
	}
	else if constexpr (std::is_floating_point_v<T>) {
		if (memberValue.IsFloat()) {
			value = memberValue.GetFloat();
		}
	}
	else if constexpr (std::is_same_v<T, std::string>) {
		if (memberValue.IsString()) {
			value = memberValue.GetString();
		}
	}
	else if constexpr (std::is_array_v<T>) {
		// Handle arrays
		if (memberValue.IsArray()) {
			const auto& arr = memberValue.GetArray();

			// 1D arrays
			if constexpr (std::rank_v<T> == 1) {
				size_t index = 0;
				for (const auto& elem : arr) {
					if constexpr (TypeMatch<std::remove_extent_t<T>, uint8>::value) {
						value[index++] = static_cast<uint8>(elem.GetUint());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, int8>::value) {
						value[index++] = static_cast<int8>(elem.GetInt());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, int16>::value) {
						value[index++] = static_cast<int16>(elem.GetInt());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, int32>::value) {
						value[index++] = static_cast<int32>(elem.GetInt());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, int64>::value) {
						value[index++] = static_cast<int64>(elem.GetInt64());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, uint16>::value) {
						value[index++] = static_cast<uint16>(elem.GetUint());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, uint32>::value) {
						value[index++] = static_cast<uint32>(elem.GetUint());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, uint64>::value) {
						value[index++] = static_cast<uint64>(elem.GetUint64());
					}
					else if constexpr (TypeMatch<std::remove_extent_t<T>, int>::value) {
						value[index++] = static_cast<int>(elem.GetInt());
					}
					else if constexpr (std::is_floating_point_v<std::remove_extent_t<T>>) {
						value[index++] = elem.GetFloat();
					}
				}
			}
			// 2D arrays
			else if constexpr (std::rank_v<T> == 2) {
				size_t row = 0;
				for (const auto& subArray : arr) {
					const auto& innerArr = subArray.GetArray();
					size_t col = 0;
					for (const auto& elem : innerArr) {
						if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint8>::value) {
							value[row][col++] = static_cast<uint8>(elem.GetUint());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int8>::value) {
							value[row][col++] = static_cast<int8>(elem.GetInt());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int16>::value) {
							value[row][col++] = static_cast<int16>(elem.GetInt());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int32>::value) {
							value[row][col++] = static_cast<int32>(elem.GetInt());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int64>::value) {
							value[row][col++] = static_cast<int64>(elem.GetInt64());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint16>::value) {
							value[row][col++] = static_cast<uint16>(elem.GetUint());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint32>::value) {
							value[row][col++] = static_cast<uint32>(elem.GetUint());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint64>::value) {
							value[row][col++] = static_cast<uint64>(elem.GetUint64());
						}
						else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int>::value) {
							value[row][col++] = static_cast<int>(elem.GetInt());
						}
						else if constexpr (std::is_floating_point_v<std::remove_all_extents_t<T>>) {
							value[row][col++] = elem.GetFloat();
						}
					}
					++row;
				}
			}
			// 3D arrays
			else if constexpr (std::rank_v<T> == 3) {
				size_t layer = 0;
				for (const auto& subSubArray : arr) {
					const auto& innerArr = subSubArray.GetArray();
					size_t row = 0;
					for (const auto& subArray : innerArr) {
						const auto& innerInnerArr = subArray.GetArray();
						size_t col = 0;
						for (const auto& elem : innerInnerArr) {
							if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint8>::value) {
								value[layer][row][col++] = static_cast<uint8>(elem.GetUint());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int8>::value) {
								value[layer][row][col++] = static_cast<int8>(elem.GetInt());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int16>::value) {
								value[layer][row][col++] = static_cast<int16>(elem.GetInt());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int32>::value) {
								value[layer][row][col++] = static_cast<int32>(elem.GetInt());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int64>::value) {
								value[layer][row][col++] = static_cast<int64>(elem.GetInt64());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint16>::value) {
								value[layer][row][col++] = static_cast<uint16>(elem.GetUint());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint32>::value) {
								value[layer][row][col++] = static_cast<uint32>(elem.GetUint());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, uint64>::value) {
								value[layer][row][col++] = static_cast<uint64>(elem.GetUint64());
							}
							else if constexpr (TypeMatch<std::remove_all_extents_t<T>, int>::value) {
								value[layer][row][col++] = static_cast<int>(elem.GetInt());
							}
							else if constexpr (std::is_floating_point_v<std::remove_all_extents_t<T>>) {
								value[layer][row][col++] = elem.GetFloat();
							}
						}
						++row;
					}
					++layer;
				}
			}
		}
	}
	else {
		// Handle nested objects (e.g., structs)
		if (memberValue.IsObject()) {
			ParseConfig(memberValue, value);
		}
	}
}


// General function to parse the config
template <typename ConfigT>
void ParseConfig(const Value& obj, ConfigT& config) {
	if constexpr (requires { ConfigT::Metadata(); }) {
		// Assuming Metadata returns a tuple of member names and pointers
		std::apply([&](const auto&... members) {
			(..., ParseField(obj, members.first, config.*(members.second)));
			}, ConfigT::Metadata());
	}
}
// Function to write the serialized config to a file
bool WriteConfig(const char* locationConfig, const Document& root) {
	StringBuffer stringBuffer;
	PrettyWriter<StringBuffer> prettyWriter(stringBuffer);

	root.Accept(prettyWriter);

	auto name = stringBuffer.GetString();
	auto size = strlen(name);

	return SaveFile(locationConfig, name, size);
}

} // namespace JSON

#endif // CONFIG_CRIMSON_HANDLER_H