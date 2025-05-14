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

enum class PatreonTiers_t : int64_t
{
	Invalid = 0,
	Free,
	DT,
	SDT,
	LDK,

	Size,
};

enum class WebAPIResult : uint64_t {
	Success = 0,
	CURLNotInitialized,
	Awaiting,
	Timeout,
	PatreonError,
	GithubError,
	UnknownError,
};

struct Patron_t {
	std::string ShownName;
	std::string TierName;
	PatreonTiers_t Tier;
};

constexpr std::string_view PATRON_TIER_NAMES[] = { "Free", "Devil Trigger Tier", "Sin Devil Trigger Tier", "Legendary Dark Knight Tier" };
constexpr PatreonTiers_t PATRON_TIER_IDS[] = { PatreonTiers_t::Free, PatreonTiers_t::DT, PatreonTiers_t::SDT, PatreonTiers_t::LDK };

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