#include "Detour.hpp"
#include <windows.h>
#include <detours.h>

#define LOG_INFO(...) // TODO: Log
#define LOG_WARN(...) // TODO: Log
#define LOG_ERROR(...) // Todo: Log

bool g_IsMSDetoursInitialized;

namespace Utility {
	Detour_t::Detour_t(Address target, Address destination, int64_t return_offset /*= 0*/, std::string_view name /*= "Unnamed"*/)
		: m_Target(target), m_Destination(destination), m_ReturnOffset(return_offset), m_Name(name)
	{
		std::scoped_lock _{ m_HookMutex };

		if (Create()) {
			Hook();
		}

		LOG_INFO("[DETOUR] \"{:s}\" Setting up a hook on {:p}->{:p}", m_Name.c_str(), target.Ptr(), destination.Ptr());
	}

	Detour_t::~Detour_t()
	{
		Unhook();
	}

	bool Detour_t::Hook()
	{
		if (m_Trampoline.Ptr() != nullptr) {
			LOG_WARN("[DETOUR] \"{:s}\" .Hook() called while the detour is already in place!", m_Name);
			return true;
		}

		std::scoped_lock _{ m_HookMutex };

		if (m_PreHookEnableCallback)
			m_PreHookEnableCallback(this);

		if (auto status = DetourTransactionBegin(); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to hook {:p}: 0x{:x} on DetourTransactionBegin()", m_Name, m_Target.Ptr(), status);

			m_Destination.Set(nullptr);
			m_Target.Set(nullptr);
			m_Trampoline.Set(nullptr);
			return false;
		}

		if (auto status = DetourUpdateThread(GetCurrentThread()); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to hook {:p}: 0x{:x} on DetourUpdateThread()", m_Name, m_Target.Ptr(), status);

			m_Destination.Set(nullptr);
			m_Target.Set(nullptr);
			m_Trampoline.Set(nullptr);

			if (auto status = DetourTransactionCommit(); status != NO_ERROR) {
				LOG_ERROR("[DETOUR] \"{:s}\" Failed to hook {:p}: 0x{:x} on DetourTransactionCommit()", m_Name, m_Target.Ptr(), status);
			}

			return false;
		}

		m_Trampoline = m_Target;

		if (auto status = DetourAttach(&m_Trampoline.As<LPVOID>(), m_Destination.As<LPVOID>()); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to hook {:p}: 0x{:x} on DetourAttach()", m_Name, m_Target.Ptr(), status);

			m_Destination.Set(nullptr);
			m_Target.Set(nullptr);
			m_Trampoline.Set(nullptr);

			if (auto status = DetourTransactionCommit(); status != NO_ERROR) {
				LOG_ERROR("[DETOUR] \"{:s}\" Failed to hook {:p}: 0x{:x} on DetourTransactionCommit()", m_Name, m_Target.Ptr(), status);
			}

			return false;
		}

		if (auto status = DetourTransactionCommit(); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to hook {:p}: 0x{:x} on DetourTransactionCommit()", m_Name, m_Target.Ptr(), status);

			m_Destination.Set(nullptr);
			m_Target.Set(nullptr);
			m_Trampoline.Set(nullptr);
			return false;
		}

		LOG_INFO("[DETOUR] \"{:s}\" Hooked {:p}->{:p}", m_Name, m_Target.Ptr(), m_Destination.Ptr());

		m_IsEnabled = true;

		if (m_PostHookEnableCallback)
			m_PostHookEnableCallback(this);

		return true;
	}

	bool Detour_t::Create()
	{
		std::scoped_lock _{ m_HookMutex };

		if (!g_IsMSDetoursInitialized) {
			if (DetourIsHelperProcess() == TRUE) {
				if (DetourRestoreAfterWith() == TRUE) {
					LOG_INFO("[DETOUR] MS Detours initialized successfully.");
				}
				else {
					LOG_ERROR("[DETOUR] MS Detours failed to initiliaze.");

					return false;
				}
			}

			g_IsMSDetoursInitialized = true;
			LOG_INFO("[DETOUR] MS Detours initialized successfully.");
		}

		if (m_Target == nullptr || m_Destination == nullptr || m_Trampoline != nullptr) {
			LOG_ERROR("[DETOUR] \"{:s}\" Detour not initialized", m_Name.c_str());
			return false;
		}

		LOG_INFO("[DETOUR] \"{:s}\" Detour added {:p}->{:p}", m_Name.c_str(), m_Target.Ptr(), m_Destination.Ptr());

		return true;
	}

	bool Detour_t::Unhook()
	{
		if (m_Trampoline.Ptr() == nullptr) {
			LOG_WARN("[DETOUR] \"{:s}\" .Hook() called while the detour is not placed!", m_Name);
			return true;
		}

		std::scoped_lock _{ m_HookMutex };

		if (m_PreHookDisableCallback)
			m_PreHookDisableCallback(this);

		// Don't try to remove invalid hooks.
		if (m_Target == nullptr || m_Destination == nullptr || m_Target == nullptr) {
			return true;
		}

		if (auto status = DetourTransactionBegin(); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to unhook {:p}: 0x{:x} on DetourTransactionBegin()", m_Name, m_Target.Ptr(), status);

			return false;
		}

		if (auto status = DetourUpdateThread(GetCurrentThread()); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to unhook {:p}: 0x{:x} on DetourUpdateThread()", m_Name, m_Target.Ptr(), status);

			if (auto status = DetourTransactionCommit(); status != NO_ERROR) {
				LOG_ERROR("[DETOUR] \"{:s}\" Failed to unhook {:p}: 0x{:x} on DetourTransactionCommit()", m_Name, m_Target.Ptr(), status);
			}

			return false;
		}

		if (auto status = DetourDetach(&m_Trampoline.As<LPVOID>(), m_Destination.As<LPVOID>()); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to unhook {:p}: 0x{:x} on DetourDetach()", m_Name, m_Target.Ptr(), status);

			if (auto status = DetourTransactionCommit(); status != NO_ERROR) {
				LOG_ERROR("[DETOUR] \"{:s}\" Failed to unhook {:p}: 0x{:x} on DetourTransactionCommit()", m_Name, m_Target.Ptr(), status);
			}

			return false;
		}

		if (auto status = DetourTransactionCommit(); status != NO_ERROR) {
			LOG_ERROR("[DETOUR] \"{:s}\" Failed to unhook {:p}: 0x{:x} on DetourTransactionCommit()", m_Name, m_Target.Ptr(), status);

			return false;
		}

		m_Trampoline.Set(nullptr);

		LOG_INFO("[DETOUR] \"{:s}\" Unhooked {:p}, Previous detour: {:p}", m_Name, m_Target.Ptr(), m_Destination.Ptr());

		m_IsEnabled = false;

		if (m_PostHookDisableCallback)
			m_PostHookDisableCallback(this);

		return true;
	}

	bool Detour_t::Toggle()
	{
		return Toggle(!m_IsEnabled);
	}

	bool Detour_t::Toggle(bool state)
	{
		if (state)
			Hook();
		else
			Unhook();

		return m_IsEnabled;
	}
}