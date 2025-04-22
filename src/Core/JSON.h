rapidjson::Document crimsonConfigRoot = {};
rapidjson::Document crimsonConfigGameplayRoot = {};

rapidjson::Value::AllocatorType* g_allocator = 0;
rapidjson::Document::AllocatorType* g_gameplay_allocator = 0;

bool IsString(rapidjson::Value& member) {
	return member.IsString();
}

void GetString(std::string buffer, new_size_t bufferSize, rapidjson::Value& member) {
	auto name = member.GetString();

	char* bufferArray = new char[buffer.size() + 1];

	std::strcpy(bufferArray, buffer.c_str());

	snprintf(bufferArray, bufferSize, "%s", name);
}


void SetString(rapidjson::Value& member, const char* value) {
	member.SetString(rapidjson::StringRef(value, strlen(value)));
}


template <typename T, new_size_t length> rapidjson::Value& CreateString(rapidjson::Value& member, T(&name)[length], const char* value) {
	auto& allocator = *g_allocator;

	if (!member.HasMember(name)) {
		rapidjson::Value newMember = {};

		member.AddMember(name, newMember, allocator);
	}


	auto& member2 = member[name];

	if (!IsString(member2)) {
		Log("value !IsString %s", name);

		SetString(member2, value);
	}

	return member2;
}


template <typename T> bool Is(rapidjson::Value& member) {
	if constexpr (TypeMatch<T, bool>::value) {
		return member.IsBool();
	}
	else if constexpr (TypeMatch<T, int8>::value) {
		return member.IsInt();
	}
	else if constexpr (TypeMatch<T, int16>::value) {
		return member.IsInt();
	}
	else if constexpr (TypeMatch<T, int32>::value) {
		return member.IsInt();
	}
	else if constexpr (TypeMatch<T, int64>::value) {
		return member.IsInt64();
	}
	else if constexpr (TypeMatch<T, uint8>::value) {
		return member.IsUint();
	}
	else if constexpr (TypeMatch<T, uint16>::value) {
		return member.IsUint();
	}
	else if constexpr (TypeMatch<T, uint32>::value) {
		return member.IsUint();
	}
	else if constexpr (TypeMatch<T, uint64>::value) {
		return member.IsUint64();
	}
	else if constexpr (TypeMatch<T, float>::value) {
		return member.IsFloat();
	}
	else if constexpr (TypeMatch<T, double>::value) {
		return member.IsDouble();
	}
	else if constexpr (TypeMatch<T, const char*>::value) {
		return member.IsString();
	}
	else if constexpr (TypeMatch<T, struct_t>::value) {
		return member.IsObject();
	}
	else if constexpr (TypeMatch<T, array_t>::value) {
		return member.IsArray();
	}
}


#pragma region Get

template <typename T> T Get(rapidjson::Value& member) {
	if constexpr (TypeMatch<T, bool>::value) {
		return member.GetBool();
	}
	else if constexpr (TypeMatch<T, int8>::value) {
		return static_cast<T>(member.GetInt());
	}
	else if constexpr (TypeMatch<T, int16>::value) {
		return static_cast<T>(member.GetInt());
	}
	else if constexpr (TypeMatch<T, int32>::value) {
		return static_cast<T>(member.GetInt());
	}
	else if constexpr (TypeMatch<T, int64>::value) {
		return static_cast<T>(member.GetInt64());
	}
	else if constexpr (TypeMatch<T, uint8>::value) {
		return static_cast<T>(member.GetUint());
	}
	else if constexpr (TypeMatch<T, uint16>::value) {
		return static_cast<T>(member.GetUint());
	}
	else if constexpr (TypeMatch<T, uint32>::value) {
		return static_cast<T>(member.GetUint());
	}
	else if constexpr (TypeMatch<T, uint64>::value) {
		return static_cast<T>(member.GetUint64());
	}
	else if constexpr (TypeMatch<T, float>::value) {
		return member.GetFloat();
	}
	else if constexpr (TypeMatch<T, double>::value) {
		return member.GetDouble();
	}
	else if constexpr (TypeMatch<T, const char*>::value) {
		return member.GetString();
	}
	else if constexpr (TypeMatch<T, std::string>::value) {
		return member.GetString();
	}
}

// Function to get a value from JSON, handling various types
template <typename T>
T GetCrimson(rapidjson::Value& member) {
	if constexpr (TypeMatch<T, bool>::value) {
		return member.GetBool();
	}
	else if constexpr (TypeMatch<T, int8>::value) {
		return static_cast<T>(member.GetInt());
	}
	else if constexpr (TypeMatch<T, int16>::value) {
		return static_cast<T>(member.GetInt());
	}
	else if constexpr (TypeMatch<T, int32>::value) {
		return static_cast<T>(member.GetInt());
	}
	else if constexpr (TypeMatch<T, int64>::value) {
		return static_cast<T>(member.GetInt64());
	}
	else if constexpr (TypeMatch<T, uint8>::value) {
		return static_cast<T>(member.GetUint());
	}
	else if constexpr (TypeMatch<T, uint16>::value) {
		return static_cast<T>(member.GetUint());
	}
	else if constexpr (TypeMatch<T, uint32>::value) {
		return static_cast<T>(member.GetUint());
	}
	else if constexpr (TypeMatch<T, uint64>::value) {
		return static_cast<T>(member.GetUint64());
	}
	else if constexpr (TypeMatch<T, float>::value) {
		return member.GetFloat();
	}
	else if constexpr (TypeMatch<T, double>::value) {
		return member.GetDouble();
	}
	else if constexpr (TypeMatch<T, const char*>::value) {
		return member.GetString();
	}
	else if constexpr (TypeMatch<T, std::string>::value) {
		return member.GetString();
	}
}

// Function to get a 1D array from JSON
template <typename T, std::size_t count>
void GetCrimsonArray(T(&values)[count], rapidjson::Value& member) {
	for (std::size_t index = 0; index < count; ++index) {
		auto& member2 = member[index];
		auto& value = values[index];

		value = GetCrimson<std::remove_extent_t<T>>(member2);
	}
}

// Function to get a 2D array from JSON
template <typename T, std::size_t count, std::size_t count2>
void GetCrimsonArray2(T(&values)[count][count2], rapidjson::Value& member) {
	for (std::size_t index = 0; index < count; ++index) {
		for (std::size_t index2 = 0; index2 < count2; ++index2) {
			auto& member2 = member[index][index2];
			auto& value = values[index][index2];

			value = GetCrimson<std::remove_all_extents_t<T>>(member2);
		}
	}
}

// Function to get a 3D array from JSON
template <typename T, std::size_t count, std::size_t count2, std::size_t count3>
void GetCrimsonArray3(T(&values)[count][count2][count3], rapidjson::Value& member) {
	for (std::size_t index = 0; index < count; ++index) {
		for (std::size_t index2 = 0; index2 < count2; ++index2) {
			for (std::size_t index3 = 0; index3 < count3; ++index3) {
				auto& member2 = member[index][index2][index3];
				auto& value = values[index][index2][index3];

				value = GetCrimson<std::remove_all_extents_t<T>>(member2);
			}
		}
	}
}

template <typename T, new_size_t count> void GetArray(T(&values)[count], rapidjson::Value& member) {
	for_all(index, count) {
		auto& member2 = member[index];
		auto& value = values[index];

		value = Get<T>(member2);
	}
}


template <typename T, new_size_t count, new_size_t count2> void GetArray2(T(&values)[count][count2], rapidjson::Value& member) {
	for_all(index, count) for_all(index2, count2) {
		auto& member2 = member[index][index2];
		auto& value = values[index][index2];

		value = Get<T>(member2);
	}
}


template <typename T, new_size_t count, new_size_t count2, new_size_t count3>
void GetArray3(T(&values)[count][count2][count3], rapidjson::Value& member) {
	for_all(index, count) for_all(index2, count2) for_all(index3, count3) {
		auto& member2 = member[index][index2][index3];
		auto& value = values[index][index2][index3];

		value = Get<T>(member2);
	}
}

#pragma endregion


#pragma region Set

template <typename T> void Set(rapidjson::Value& member) = delete;


template <> void Set<array_t>(rapidjson::Value& member) {
	member.SetArray();
}

template <> void Set<struct_t>(rapidjson::Value& member) {
	member.SetObject();
}


template <typename T> void Set(rapidjson::Value& member, T value) {
	if constexpr (TypeMatch<T, bool>::value) {
		member.SetBool(value);
	}
	else if constexpr (TypeMatch<T, int8>::value) {
		member.SetInt(value);
	}
	else if constexpr (TypeMatch<T, int16>::value) {
		member.SetInt(value);
	}
	else if constexpr (TypeMatch<T, int32>::value) {
		member.SetInt(value);
	}
	else if constexpr (TypeMatch<T, int64>::value) {
		member.SetInt64(value);
	}
	else if constexpr (TypeMatch<T, uint8>::value) {
		member.SetUint(value);
	}
	else if constexpr (TypeMatch<T, uint16>::value) {
		member.SetUint(value);
	}
	else if constexpr (TypeMatch<T, uint32>::value) {
		member.SetUint(value);
	}
	else if constexpr (TypeMatch<T, uint64>::value) {
		member.SetUint64(value);
	}
	else if constexpr (TypeMatch<T, float>::value) {
		member.SetFloat(value);
	}
	else if constexpr (TypeMatch<T, double>::value) {
		member.SetDouble(value);
	}
	else if constexpr (TypeMatch<T, const char*>::value) {
		member.SetString(value);
	}
}


template <typename T, new_size_t count> void SetArray(rapidjson::Value& member, T(&values)[count]) {
	for_all(index, count) {
		auto& member2 = member[index];
		auto& value = values[index];

		Set<T>(member2, value);
	}
}


template <typename T, new_size_t count, new_size_t count2> void SetArray2(rapidjson::Value& member, T(&values)[count][count2]) {
	for_all(index, count) for_all(index2, count2) {
		auto& member2 = member[index][index2];
		auto& value = values[index][index2];

		Set<T>(member2, value);
	}
}


template <typename T, new_size_t count, new_size_t count2, new_size_t count3>
void SetArray3(rapidjson::Value& member, T(&values)[count][count2][count3]) {
	for_all(index, count) for_all(index2, count2) for_all(index3, count3) {
		auto& member2 = member[index][index2][index3];
		auto& value = values[index][index2][index3];

		Set<T>(member2, value);
	}
}

#pragma endregion


#pragma region Create


// The 2 separate versions are required for proper compile-time type checking.

template <typename T, typename T2, new_size_t length> rapidjson::Value& Create(rapidjson::Value& member, T2(&name)[length]) {
	auto& allocator = *g_allocator;

	if (!member.HasMember(name)) {
		rapidjson::Value newMember = {};

		member.AddMember(name, newMember, allocator);
	}


	auto& member2 = member[name];

	if (!Is<T>(member2)) {
		Log("value-less !Is %s", name);

		Set<T>(member2);
	}

	return member2;
}


template <typename T, typename T2, new_size_t length> rapidjson::Value& Create(rapidjson::Value& member, T2(&name)[length], T& value) {
	auto& allocator = *g_allocator;

	if (!member.HasMember(name)) {
		rapidjson::Value newMember = {};

		member.AddMember(name, newMember, allocator);
	}


	auto& member2 = member[name];

	if (!Is<T>(member2)) {
		Log("value !Is %s", name);

		Set<T>(member2, value);
	}

	return member2;
}

// Trait to check if a type is a multi-dimensional array
template<typename T>
struct is_multi_dimensional_array : std::false_type {};

template<typename T, std::size_t N>
struct is_multi_dimensional_array<T[N]> : is_multi_dimensional_array<T> {};

template<typename T>
struct is_multi_dimensional_array<T[]> : std::true_type {};

template<typename T>
inline constexpr bool is_multi_dimensional_array_v = is_multi_dimensional_array<T>::value;

template <typename T>
void CreateCrimson(rapidjson::Value& root, const char* fieldName, T& fieldValue) {
	auto& allocator = *g_allocator;

	// Handle scalar types
	if constexpr (std::is_same_v<T, bool>) {
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), rapidjson::Value().SetBool(fieldValue), allocator);
	}
	else if constexpr (std::is_same_v<T, uint8_t>) {
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), rapidjson::Value().SetUint(fieldValue), allocator);
	}
	else if constexpr (std::is_same_v<T, int>) {
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), rapidjson::Value().SetInt(fieldValue), allocator);
	}
	else if constexpr (std::is_same_v<T, float>) {
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), rapidjson::Value().SetFloat(fieldValue), allocator);
	}
	else if constexpr (std::is_same_v<T, double>) {
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), rapidjson::Value().SetDouble(fieldValue), allocator);
	}
	// Handle 1D arrays
	else if constexpr (std::is_array_v<T> && std::rank_v<T> == 1) {
		rapidjson::Value array(rapidjson::kArrayType);
		for (size_t i = 0; i < std::extent_v<T>; ++i) {
			if constexpr (std::is_same_v<std::remove_extent_t<T>, bool>) {
				array.PushBack(rapidjson::Value().SetBool(fieldValue[i]), allocator);
			}
			else if constexpr (std::is_same_v<std::remove_extent_t<T>, uint8_t>) {
				array.PushBack(rapidjson::Value().SetUint(fieldValue[i]), allocator);
			}
			else if constexpr (std::is_same_v<std::remove_extent_t<T>, int>) {
				array.PushBack(rapidjson::Value().SetInt(fieldValue[i]), allocator);
			}
			else if constexpr (std::is_same_v<std::remove_extent_t<T>, float>) {
				array.PushBack(rapidjson::Value().SetFloat(fieldValue[i]), allocator);
			}
			else if constexpr (std::is_same_v<std::remove_extent_t<T>, double>) {
				array.PushBack(rapidjson::Value().SetDouble(fieldValue[i]), allocator);
			}
		}
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), array, allocator);
	}
	// Handle 2D arrays
	else if constexpr (std::is_array_v<T> && std::rank_v<T> == 2) {
		rapidjson::Value array(rapidjson::kArrayType);
		for (size_t i = 0; i < std::extent_v<T, 0>; ++i) {
			rapidjson::Value subArray(rapidjson::kArrayType);
			for (size_t j = 0; j < std::extent_v<T, 1>; ++j) {
				if constexpr (std::is_same_v<std::remove_all_extents_t<T>, bool>) {
					subArray.PushBack(rapidjson::Value().SetBool(fieldValue[i][j]), allocator);
				}
				else if constexpr (std::is_same_v<std::remove_all_extents_t<T>, uint8_t>) {
					subArray.PushBack(rapidjson::Value().SetUint(fieldValue[i][j]), allocator);
				}
				else if constexpr (std::is_same_v<std::remove_all_extents_t<T>, int>) {
					subArray.PushBack(rapidjson::Value().SetInt(fieldValue[i][j]), allocator);
				}
				else if constexpr (std::is_same_v<std::remove_all_extents_t<T>, float>) {
					subArray.PushBack(rapidjson::Value().SetFloat(fieldValue[i][j]), allocator);
				}
				else if constexpr (std::is_same_v<std::remove_all_extents_t<T>, double>) {
					subArray.PushBack(rapidjson::Value().SetDouble(fieldValue[i][j]), allocator);
				}
			}
			array.PushBack(subArray, allocator);
		}
		root.AddMember(rapidjson::Value().SetString(fieldName, allocator), array, allocator);
	}
	// Handle unsupported types
	else {
		static_assert(!std::is_same_v<T, T>, "Unhandled type in CreateCrimson");
	}
}



template <typename T, std::size_t N>
rapidjson::Value& CreateCrimsonArray(rapidjson::Value& member, const char* name, T(&array)[N]) {
	auto& allocator = *g_allocator; // Access the global allocator

	// Check if the member exists, if not, create it as an array
	if (!member.HasMember(name)) {
		rapidjson::Value newMember(rapidjson::kArrayType);
		member.AddMember(rapidjson::Value(name, allocator), newMember, allocator);
	}

	auto& memberArray = member[name];
	memberArray.SetArray();

	// Populate the array in JSON
	for (std::size_t i = 0; i < N; ++i) {
		rapidjson::Value arrayElement;
		Set<T>(arrayElement, array[i]);
		memberArray.PushBack(arrayElement, allocator);
	}

	return memberArray;
}

template <typename T, size_t N, size_t M>
void CreateCrimsonArray2(rapidjson::Value& root, const char* fieldName, T(&array)[N][M]) {
    using namespace rapidjson;

    // Create an array in the JSON
    Value arrayValue(kArrayType);

    // Iterate over the 1st dimension (rows)
    for (size_t i = 0; i < N; ++i) {
        Value rowValue(kArrayType);

        // Iterate over the 2nd dimension (columns)
        for (size_t j = 0; j < M; ++j) {
            // Add each element to the row array
            Value element;
            Set<T>(element, array[i][j]);
            rowValue.PushBack(element, *g_allocator);
        }

        // Push the row array into the outer array
        arrayValue.PushBack(rowValue, *g_allocator);
    }

    // Add the array to the root object
    root.AddMember(Value().SetString(StringRef(fieldName), *g_allocator), arrayValue, *g_allocator);
}

template <typename T, std::size_t N1, std::size_t N2, std::size_t N3>
rapidjson::Value& CreateCrimsonArray3(rapidjson::Value& member, const char* name, T(&array)[N1][N2][N3]) {
	auto& allocator = *g_allocator; // Access the global allocator

	// Check if the member exists, if not, create it as an array
	if (!member.HasMember(name)) {
		rapidjson::Value newMember(rapidjson::kArrayType);
		member.AddMember(rapidjson::Value(name, allocator), newMember, allocator);
	}

	auto& memberArray = member[name];
	memberArray.SetArray();

	// Populate the 3D array in JSON
	for (std::size_t i = 0; i < N1; ++i) {
		rapidjson::Value plane(rapidjson::kArrayType);
		for (std::size_t j = 0; j < N2; ++j) {
			rapidjson::Value row(rapidjson::kArrayType);
			for (std::size_t k = 0; k < N3; ++k) {
				rapidjson::Value arrayElement;
				Set<T>(arrayElement, array[i][j][k]);
				row.PushBack(arrayElement, allocator);
			}
			plane.PushBack(row, allocator);
		}
		memberArray.PushBack(plane, allocator);
	}

	return memberArray;
}


// General template for non-array types
template <typename T>
void FromCrimson(const rapidjson::Value& jsonField, T& value) {
	std::cout << "Extracting value from JSON field" << std::endl;
	if (jsonField.Is<T>()) {
		value = jsonField.Get<T>();
	}
}

// Specialization for 1D arrays
template <typename T, size_t N>
void FromCrimson(const rapidjson::Value& jsonField, T(&array)[N]) {
	std::cout << "Extracting 1D array from JSON field" << std::endl;
	assert(jsonField.IsArray());
	for (size_t i = 0; i < N; ++i) {
		array[i] = jsonField[i].Get<T>();
	}
}

// Specialization for 2D arrays
template <typename T, size_t N, size_t M>
void FromCrimson(const rapidjson::Value& jsonField, T(&array)[N][M]) {
	std::cout << "Extracting 2D array from JSON field" << std::endl;
	assert(jsonField.IsArray());
	for (size_t i = 0; i < N; ++i) {
		assert(jsonField[i].IsArray());
		for (size_t j = 0; j < M; ++j) {
			array[i][j] = jsonField[i][j].Get<T>();
		}
	}
}

// Specialization for 3D arrays
template <typename T, size_t N, size_t M, size_t L>
void FromCrimson(const rapidjson::Value& jsonField, T(&array)[N][M][L]) {
	std::cout << "Extracting 3D array from JSON field" << std::endl;
	assert(jsonField.IsArray());
	for (size_t i = 0; i < N; ++i) {
		assert(jsonField[i].IsArray());
		for (size_t j = 0; j < M; ++j) {
			assert(jsonField[i][j].IsArray());
			for (size_t k = 0; k < L; ++k) {
				array[i][j][k] = jsonField[i][j][k].Get<T>();
			}
		}
	}
}

// template
// <
// 	typename T,
// 	typename T2,
// 	new_size_t length,
// 	typename... Args
// >
// rapidjson::Value & Create
// (
// 	rapidjson::Value & member,
// 	T2 (&name)[length],
// 	Args... args
// )
// {
// 	auto & allocator = *g_allocator;

// 	if (!member.HasMember(name))
// 	{
// 		rapidjson::Value newMember = {};

// 		member.AddMember
// 		(
// 			name,
// 			newMember,
// 			allocator
// 		);
// 	}


// 	auto & member2 = member[name];

// 	if (!Is<T>(member2))
// 	{
// 		constexpr new_size_t argCount = sizeof...(args);

// 		if constexpr (argCount == 0)
// 		{
// 			Log("value-less !Is %s", name);
// 		}
// 		else
// 		{
// 			Log("value !Is %s", name);
// 		}


// 		Set<T>(member2, args...);
// 	}

// 	return member2;
// }


void UpdateCount(rapidjson::Value& member, new_size_t count) {
	if (!member.IsArray()) {
		return;
	}

	auto& allocator = *g_allocator;

	auto count2 = member.Size();


	if (count2 < count) {
		auto diff = (count - count2);

		for_all(index, diff) {
			rapidjson::Value newMember = {};

			member.PushBack(newMember, allocator);
		}
	}


	if (count2 > count) {
		auto diff = (count2 - count);

		for_all(index, diff) {
			member.PopBack();
		}
	}
}


/*
Successful calls to CreateArray guarantee that:

* The array exists.
* The count is correct.
* The type of every single item is correct.
*/


// value-less

template <typename T, new_size_t count, typename T2, new_size_t length>
rapidjson::Value& CreateArray(rapidjson::Value& member, T2(&name)[length]) {
	auto& newMember = Create<array_t>(member, name);

	UpdateCount(newMember, count);

	for_all(index, count) {
		auto& member2 = newMember[index];

		if (!Is<T>(member2)) {
			Set<T>(member2);
		}
	}

	return newMember;
}


template <typename T, new_size_t count, new_size_t count2, typename T2, new_size_t length>
rapidjson::Value& CreateArray2(rapidjson::Value& member, T2(&name)[length]) {
	auto& newMember = CreateArray<array_t, count>(member, name);

	for_all(index, count) {
		auto& member2 = newMember[index];

		UpdateCount(member2, count2);
	}

	for_all(index, count) {
		for_all(index2, count2) {
			auto& member2 = newMember[index][index2];

			if (!Is<T>(member2)) {
				Set<T>(member2);
			}
		}
	}

	return newMember;
}


template <typename T, new_size_t count, new_size_t count2, new_size_t count3, typename T2, new_size_t length>
rapidjson::Value& CreateArray3(rapidjson::Value& member, T2(&name)[length]) {
	auto& newMember = CreateArray2<array_t, count, count2>(member, name);

	for_all(index, count) {
		for_all(index2, count2) {
			auto& member2 = newMember[index][index2];

			UpdateCount(member2, count3);
		}
	}

	for_all(index, count) {
		for_all(index2, count2) {
			for_all(index3, count3) {
				auto& member2 = newMember[index][index2][index3];

				if (!Is<T>(member2)) {
					Set<T>(member2);
				}
			}
		}
	}

	return newMember;
}


// value

template <typename T, new_size_t count, typename T2, new_size_t length>
rapidjson::Value& CreateArray(rapidjson::Value& member, T2(&name)[length], T(&values)[count]) {
	auto& newMember = Create<array_t>(member, name);

	UpdateCount(newMember, count);

	for_all(index, count) {
		auto& member2 = newMember[index];
		auto& value = values[index];

		if (!Is<T>(member2)) {
			Set<T>(member2, value);
		}
	}

	return newMember;
}


template <typename T, new_size_t count, new_size_t count2, typename T2, new_size_t length>
rapidjson::Value& CreateArray2(rapidjson::Value& member, T2(&name)[length], T(&values)[count][count2]) {
	auto& newMember = CreateArray<array_t, count>(member, name);

	for_all(index, count) {
		auto& member2 = newMember[index];

		UpdateCount(member2, count2);
	}

	for_all(index, count) {
		for_all(index2, count2) {
			auto& member2 = newMember[index][index2];
			auto& value = values[index][index2];

			if (!Is<T>(member2)) {
				Set<T>(member2, value);
			}
		}
	}

	return newMember;
}


template <typename T, new_size_t count, new_size_t count2, new_size_t count3, typename T2, new_size_t length>
rapidjson::Value& CreateArray3(rapidjson::Value& member, T2(&name)[length], T(&values)[count][count2][count3]) {
	auto& newMember = CreateArray2<array_t, count, count2>(member, name);

	for_all(index, count) {
		for_all(index2, count2) {
			auto& member2 = newMember[index][index2];

			UpdateCount(member2, count3);
		}
	}

	for_all(index, count) {
		for_all(index2, count2) {
			for_all(index3, count3) {
				auto& member2 = newMember[index][index2][index3];
				auto& value = values[index][index2][index3];

				if (!Is<T>(member2)) {
					Set<T>(member2, value);
				}
			}
		}
	}

	return newMember;
}


#pragma endregion
