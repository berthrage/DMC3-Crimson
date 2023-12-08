#pragma once

#include <functional>
#include <memory>
#include <chrono>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define CURL_STATICLIB
#include <curl/curl.h>

struct Version_t {
	uint8_t Major;
	uint8_t Minor;
	char	PatchLetter;

	struct {
		tm Local;
		tm UTC;
	} CreateTime, PublishTime;

	std::string DirectURL;
};

enum class VersionCheckResult : uint64_t {
	Success = 0,
	IsChecking,
	Timeout,
	CURLNotInitialized,
	UnknownError,
};

class VersionTracker {
public:
	static VersionTracker& GetInstance() {
		static VersionTracker instance;
		return instance;
	}

	void SetCallback(std::function<void(VersionCheckResult res, Version_t latestVersion)> callback) {
		m_Callback = callback;
	}

	void QueueLatestRelease(size_t timeOutMS = 0);

private:
	VersionTracker();
	virtual ~VersionTracker();

	static int CurlProgressCallback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);

private:
	std::function<void(VersionCheckResult res, Version_t latestVersion)> m_Callback{};
	CURL* m_CURLInstance{ nullptr };
};