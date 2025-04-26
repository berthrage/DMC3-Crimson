#pragma once
#include <type_traits>
#include <cmath>

namespace CrimsonGameModes {
void SetGameMode(uint8 mode);
void TrackGameMode();
void TrackCheats();
void TrackMissionResultGameMode();

// Tolerance constant for floating point comparisons
constexpr float FLOAT_EPSILON = 0.01f;

// Helper trait to detect if a type is an array
template<typename T>
struct is_array_type : std::is_array<T> {};

// Helper trait to detect if a type is floating point
template<typename T>
struct is_floating_point_type : std::is_floating_point<T> {};

// Base comparison for non-array, non-floating point types
template<typename T>
typename std::enable_if<!is_array_type<T>::value && !is_floating_point_type<T>::value, bool>::type
CompareValue(const T& val1, const T& val2) {
	return val1 == val2;
}

// Specialized comparison for floating point types with tolerance
template<typename T>
typename std::enable_if<is_floating_point_type<T>::value, bool>::type
CompareValue(const T& val1, const T& val2) {
	// Check if the difference between values is within tolerance
	return std::abs(val1 - val2) <= FLOAT_EPSILON;
}

// Helper function to compare arrays
template<typename T, size_t N>
bool CompareArrays(const T(&arr1)[N], const T(&arr2)[N]) {
	for (size_t i = 0; i < N; ++i) {
		if (!CompareValue(arr1[i], arr2[i])) {
			return false;
		}
	}
	return true;
}

// Specialized comparison for array types
template<typename T, size_t N>
bool CompareValue(const T(&arr1)[N], const T(&arr2)[N]) {
	return CompareArrays(arr1, arr2);
}

template<typename T>
bool CompareMembers(const T& obj1, const T& obj2) {
	return std::apply([&](auto&&... args) {
		return (... && CompareValue(obj1.*(args.second), obj2.*(args.second)));
		}, T::Metadata());
}

template<typename T>
bool CompareStructs(const T& obj1, const T& obj2) {
	return std::apply([&](auto&&... args) {
		return (... && CompareMembers(obj1.*(args.second), obj2.*(args.second)));
		}, T::Metadata());
}

// For bool mask members (leaf)
template<typename T, typename MaskT>
typename std::enable_if<std::is_same<MaskT, bool>::value, bool>::type
CompareMemberMasked(const T& val1, const T& val2, const MaskT& mask) {
	return mask ? CompareValue(val1, val2) : true;
}

// Forward declaration for struct mask members (recurse)
template<typename T, typename MaskT>
bool CompareMembersMasked(const T& obj1, const T& obj2, const MaskT& mask);

// For struct mask members (recurse)
template<typename T, typename MaskT>
typename std::enable_if<!std::is_same<MaskT, bool>::value, bool>::type
CompareMemberMasked(const T& val1, const T& val2, const MaskT& mask) {
	return CompareMembersMasked(val1, val2, mask);
}

// Helper for masked member comparison
template<typename T, typename MaskT, typename Tuple1, typename Tuple2, std::size_t... I>
bool CompareMembersMaskedImpl(const T& obj1, const T& obj2, const MaskT& mask,
	const Tuple1& configMeta, const Tuple2& maskMeta, std::index_sequence<I...>) {
	return (... && (
		CompareMemberMasked(
			obj1.*(std::get<I>(configMeta).second),
			obj2.*(std::get<I>(configMeta).second),
			mask.*(std::get<I>(maskMeta).second)
		)
		));
}

// Helper for masked struct comparison
template<typename T, typename MaskT, typename Tuple1, typename Tuple2, std::size_t... I>
bool CompareStructsMaskedImpl(const T& obj1, const T& obj2, const MaskT& mask,
	const Tuple1& configMeta, const Tuple2& maskMeta, std::index_sequence<I...>) {
	return (... && CompareMembersMasked(
		obj1.*(std::get<I>(configMeta).second),
		obj2.*(std::get<I>(configMeta).second),
		mask.*(std::get<I>(maskMeta).second)
	));
}

// Masked member comparison
template<typename T, typename MaskT>
bool CompareMembersMasked(const T& obj1, const T& obj2, const MaskT& mask) {
	constexpr auto configMeta = T::Metadata();
	constexpr auto maskMeta = MaskT::Metadata();
	return CompareMembersMaskedImpl(obj1, obj2, mask, configMeta, maskMeta,
		std::make_index_sequence<std::tuple_size<decltype(configMeta)>::value>{});
}

// Masked struct comparison
template<typename T, typename MaskT>
bool CompareStructsMasked(const T& obj1, const T& obj2, const MaskT& mask) {
	constexpr auto configMeta = T::Metadata();
	constexpr auto maskMeta = MaskT::Metadata();
	return CompareStructsMaskedImpl(obj1, obj2, mask, configMeta, maskMeta,
		std::make_index_sequence<std::tuple_size<decltype(configMeta)>::value>{});
}

// For array types (leaf)
template<typename T, size_t N>
void AssignMemberMasked(T(&dst)[N], const T(&src)[N], bool mask) {
	if (mask) {
		for (size_t i = 0; i < N; ++i) {
			dst[i] = src[i];
		}
	}
}

// For bool mask members (leaf)
template<typename T, typename MaskT>
typename std::enable_if<std::is_same<MaskT, bool>::value && !std::is_array<T>::value, void>::type
AssignMemberMasked(T& dst, const T& src, const MaskT& mask) {
	if (mask) dst = src;
}

// Forward declaration for struct mask members (recurse)
template<typename T, typename MaskT>
void AssignMembersMasked(T& dst, const T& src, const MaskT& mask);

// For struct mask members (recurse)
template<typename T, typename MaskT>
typename std::enable_if<!std::is_same<MaskT, bool>::value, void>::type
AssignMemberMasked(T& dst, const T& src, const MaskT& mask) {
	AssignMembersMasked(dst, src, mask);
}

// Helper for masked member assignment
template<typename T, typename MaskT, typename Tuple1, typename Tuple2, std::size_t... I>
void AssignMembersMaskedImpl(T& dst, const T& src, const MaskT& mask,
	const Tuple1& configMeta, const Tuple2& maskMeta, std::index_sequence<I...>) {
	(AssignMemberMasked(
		dst.*(std::get<I>(configMeta).second),
		src.*(std::get<I>(configMeta).second),
		mask.*(std::get<I>(maskMeta).second)
	), ...);
}

// Masked member assignment
template<typename T, typename MaskT>
void AssignMembersMasked(T& dst, const T& src, const MaskT& mask) {
	constexpr auto configMeta = T::Metadata();
	constexpr auto maskMeta = MaskT::Metadata();
	AssignMembersMaskedImpl(dst, src, mask, configMeta, maskMeta,
		std::make_index_sequence<std::tuple_size<decltype(configMeta)>::value>{});
}
}