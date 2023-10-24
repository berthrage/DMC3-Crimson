#pragma once

#include <mutex>
#include <cstdint>
#include <functional>

#include "Address.hpp"

namespace Utility {
	class Detour_t {
	public:
		Detour_t(Address target, Address destination, int64_t return_offset = 0, std::string_view name = "Unnamed");

		~Detour_t();

		/*
		* Returns the name of the detour
		*/
		const auto& GetName() const {
			return m_Name;
		}

		/*
		* Returns the address of where the detour should be placed
		* Template takes the type for the target which is the original type of the function we hooked
		*/
		template <typename T>
		const T& GetTarget() const {
			if constexpr (std::is_same_v<T, Address>) {
				return m_Target;
			}
			else {
				return m_Target.As<T>();
			}
		}

		/*
		* Returns the address of the code the detour wouldl jump to
		* Template takes the type for the destination which is the original type of the function we hooked
		*/
		template <typename T>
		const T& GetDestination() const {
			if constexpr (std::is_same_v<T, Address>) {
				return m_Destination;
			}
			else {
				return m_Destination.As<T>();
			}
		}

		/*
		* Returns the address of the trampoline so we can call the original code
		* Template takes the type for the trampoline which is the original type of the function we hooked
		*/
		template <typename T = Address>
		const T& GetTrampoline() const {
			if constexpr (std::is_same_v<T, Address>) {
				return m_Trampoline;
			}
			else {
				return m_Trampoline.As<T>();
			}
		}

		/*
		* Returns the return address which the detour is supposed to jump to after execution
		*/
		Address GetReturnAddress() const { return m_ReturnOffset == 0 ? Address(m_Trampoline) : Address((uintptr_t)(m_Target.As<uintptr_t>() + m_ReturnOffset)); }

		/*
		* Returns the return address offset from the original code
		*/
		const auto& GetReturnOffset() const { return m_ReturnOffset; }

		/*
		* Changes the name of the detour
		*/
		void SetName(const std::string& newName) {
			m_Name = newName;
		}

		/*
		* Sets the return offset used to calculate the return address
		*/
		void SetReturnOffset(int64_t offset) { m_ReturnOffset = offset; }

		/*
		* Returns a bool that indicates the state of the detour whether if the target and destination are set (true) or not (false)
		*/
		auto IsValid() const { return m_Trampoline != nullptr; }

		/*
		* Returns a bool that indicates the state of the detour whether if the detour is in place (true) or not (false)
		*/
		const auto& IsEnabled() const { return m_IsEnabled; }

		/*
		* Places the detour in place, returns true if succeeded and fale if failed
		*/
		bool Hook();

		/*
		* Adds the detour to the list but doesn't place the detour in place, returns true if succeeded and false if failed
		*/
		bool Create();

		/*
		* Disables the detour but doesn't remove it from minhook's table, returns true if succeeded and false if failed
		*/
		bool Unhook();

		/*
		* Calls Create() if the detour is not in place (Is disabled) or
		* calls Remove() if the detour is already in place (Is enabled)
		* returns the current state of the hook after enabling (true) or disabling (false) it
		*/
		bool Toggle();

		/*
		* Takes a bool and enables (true) or disables (false) the detour based on that
		* returns the current state of the hook after enabling (true) or disabling (false) it
		*/
		bool Toggle(bool state);

		/*
		* Set a callback that will be called before the detour is placed
		* The argument of the callback is the current Detour_t object pointer
		*/
		void SetPreHookEnableCallback(std::function<void(Detour_t*)> cb) { m_PreHookEnableCallback = cb; }

		/*
		* Set a callback that will be called after the detour is placed
		* The argument of the callback is the current Detour_t object pointer
		*/
		void SetPostHookEnableCallback(std::function<void(Detour_t*)> cb) { m_PostHookEnableCallback = cb; }

		/*
		* Set a callback that will be called before the detour is removed
		* The argument of the callback is the current Detour_t object pointer
		*/
		void SetPreHookDisableCallback(std::function<void(Detour_t*)> cb) { m_PreHookDisableCallback = cb; }

		/*
		* Set a callback that will be called after the detour is removed
		* The argument of the callback is the current Detour_t object pointer
		*/
		void SetPostHookDisableCallback(std::function<void(Detour_t*)> cb) { m_PostHookDisableCallback = cb; }

	private:
		std::recursive_mutex m_HookMutex{};
		bool m_IsEnabled{ false };

		std::function<void(Detour_t*)> m_PreHookEnableCallback{}, m_PostHookEnableCallback{},
			m_PreHookDisableCallback{}, m_PostHookDisableCallback{};

		int64_t m_ReturnOffset = 0; // Offset from the the original address to return to
		Address m_Target{ nullptr }; // Address of the target code
		Address m_Destination{ nullptr }; // Address of the code to detour to
		Address m_Trampoline{ nullptr }; // Address of the trampoline

		std::string m_Name{};
	};
}