#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <chrono>

#ifndef NOMINMAX
#define NOMINMAX
#endif

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

enum class PatreonTiers_t : uint64_t {
	Rich = 0,
	RichAF,

	Size,
};

enum class WebAPIResult : uint64_t {
	Success = 0,
	Awaiting,
	Timeout,
	CURLNotInitialized,
	UnknownError,
};

struct Patron_t {
	std::string UserName;
	std::string TierName;
	uint64_t UserID;
	uint64_t TierID;
	PatreonTiers_t Tier;
};

class WebAPICalls {
public:
	static WebAPICalls& GetInstance() {
		static WebAPICalls instance;
		return instance;
	}

	void SetVersionCallback(std::function<void(WebAPIResult res, Version_t latestVersion)> callback) {
		m_VersionCallback = callback;
	}

	void SetPatronsCallback(std::function<void(WebAPIResult res, std::vector<Patron_t> patrons)> callback) {
		m_PatronsCallback = callback;
	}

	void QueueLatestRelease(size_t timeOutMS = 0);
	void QueuePatrons(size_t timeOutMS = 0);

private:
	WebAPICalls();
	virtual ~WebAPICalls();

private:
	std::function<void(WebAPIResult res, Version_t latestVersion)> m_VersionCallback{};
	std::function<void(WebAPIResult res, std::vector<Patron_t> patrons)> m_PatronsCallback{};
};