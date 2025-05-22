#include "WebAPICalls.hpp"

#include <string.h>

#define CURL_STATICLIB
#include <curl/curl.h>

#include "Core/RapidJSON.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

struct PatronTier_t
{
	std::string Name;
	PatreonTiers_t ID;
};

static size_t CurlWriteCallback(void* data, size_t size, size_t nmemb, void* clientp)
{
	std::string* responseJSON = (std::string*)clientp;

	size_t realsize = size * nmemb;

	char* buffer = new char[realsize + 1];

	if (buffer == nullptr)
		return 0;

	buffer[realsize] = 0;

	memcpy(buffer, data, realsize);

	responseJSON->append(buffer);

	delete[] buffer;

	return realsize;
}

static std::chrono::system_clock::time_point ParseISO8601(std::string iso8601)
{
	std::chrono::system_clock::time_point result;

	std::istringstream dateStrStream{ iso8601 };

	dateStrStream >> std::chrono::parse("%FT%TZ", result);

	return result;
}

WebAPICalls::WebAPICalls()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

WebAPICalls::~WebAPICalls()
{
	curl_global_cleanup();
}

void WebAPICalls::QueueLatestRelease(size_t timeOutMS /*= 0*/)
{
	CURL* curlInstance = curl_easy_init();

	if (curlInstance == nullptr) {
		if (m_VersionCallback)
			m_VersionCallback(WebAPIResult::CURLNotInitialized, {});

		return;
	}

	constexpr char url[] = "https://crimson-api-redirect.vercel.app/latest-release";

	curl_easy_setopt(curlInstance, CURLOPT_URL, url);
	curl_easy_setopt(curlInstance, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);

	if (timeOutMS > 0) {
		curl_easy_setopt(curlInstance, CURLOPT_CONNECTTIMEOUT_MS, timeOutMS);
	}

	struct curl_slist* headers = NULL;

	headers = curl_slist_append(headers, "Accept: application/vnd.github+json");
	headers = curl_slist_append(headers, "X-GitHub-Api-Version: 2022-11-28");
	headers = curl_slist_append(headers, "User-Agent: Crimson");

	curl_easy_setopt(curlInstance, CURLOPT_HTTPHEADER, headers);

	std::string responseJSON;

	curl_easy_setopt(curlInstance, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
	curl_easy_setopt(curlInstance, CURLOPT_WRITEDATA, &responseJSON);

	CURLcode res = curl_easy_perform(curlInstance);

	if (res == CURLE_OK) {
		if (m_VersionCallback) {
			rapidjson::Document d;
			d.Parse(responseJSON.c_str());

			if (!(d.HasMember("githubResponseStatus") && d["githubResponseStatus"].GetUint64() == 200 && d.HasMember("latestReleaseInfo") && !d["latestReleaseInfo"].IsNull())) // Patron results are valid
			{
				m_PatronsCallback(WebAPIResult::PatreonError, {});

				return;
			}

			rapidjson::Value &info = d["latestReleaseInfo"];

			rapidjson::Value& tagValue = info["tag_name"];
		
			// The format is always ( <Major>.<Minor>.<Patch><HotfixLetter> ) the hotfix letter is optional
			if (auto tag = tagValue.GetString(); (char)tolower(tag[0]) == 'v') {
				std::string verStr{ &tag[1] };
		
				Version_t queuedVersion{ 0, 0, 0, {0, 0}, {0, 0} };
				
				// Version number
				{
					auto dotPos = std::find(verStr.begin(), verStr.end(), '.');

					queuedVersion.Major = std::stoi(verStr.substr(0, dotPos - verStr.begin()));

					verStr.erase(verStr.begin(), dotPos + 1);

					char finalChar = verStr.back();

					// If the final letter of the string is not a digit
					if (!(finalChar >= '0' && finalChar <= '9')) {
						queuedVersion.Minor = std::stoi(verStr.substr(0, verStr.end() - verStr.begin() - 1));

						verStr.erase(verStr.begin(), verStr.end() - 1);

						queuedVersion.PatchLetter = verStr.c_str()[0];
					}
					else {
						queuedVersion.Minor = std::stoi(verStr);
					}
				}

				// Date
				{
					{
						rapidjson::Value& createdAtValue = info["created_at"];

						auto tp = ParseISO8601(createdAtValue.GetString());

						time_t tt = std::chrono::system_clock::to_time_t(tp);

						localtime_s(&queuedVersion.CreateTime.Local, &tt);
						gmtime_s(&queuedVersion.CreateTime.UTC, &tt);
					}

					{
						rapidjson::Value& createdAtValue = info["published_at"];

						auto tp = ParseISO8601(createdAtValue.GetString());

						time_t tt = std::chrono::system_clock::to_time_t(tp);

						localtime_s(&queuedVersion.PublishTime.Local, &tt);
						gmtime_s(&queuedVersion.PublishTime.UTC, &tt);
					}
				}

				// URL
				{
					rapidjson::Value& downloadUrl = info["download_url"];
				
					queuedVersion.DownloadURL = downloadUrl.GetString();
				}

				m_VersionCallback(WebAPIResult::Success, queuedVersion);
			}
		}
	}
	else {
		if (m_VersionCallback) {
			switch (res) {
			case CURLE_OPERATION_TIMEDOUT:
				m_VersionCallback(WebAPIResult::Timeout, {});
				break;

			default:
				m_VersionCallback(WebAPIResult::UnknownError, {});
				break;
			}
		}
	}

	curl_slist_free_all(headers);

	curl_easy_cleanup(curlInstance);
}

PatronTier_t GetMostValuableTier(const std::vector<std::string>& tierNames)
{	
	int64_t maxTierScore = -1;
	PatreonTiers_t mvtID = PatreonTiers_t::Invalid;

	for (const std::string tier : tierNames)
	{
		for (size_t i = 0; i < sizeof(PATRON_TIER_NAMES) / sizeof(decltype(*PATRON_TIER_NAMES)); i++)
		{
			std::string toCompare = std::string(PATRON_TIER_NAMES[i]);
			if (tier == toCompare && (int64_t)i > maxTierScore)
			{
				maxTierScore = i;
				mvtID = PATRON_TIER_IDS[i];
			}
		}
	}

	if (maxTierScore != -1)
	{
		return {
			.Name = std::string(PATRON_TIER_NAMES[maxTierScore]),
			.ID = mvtID
		};
	}

	return { .Name = "", .ID = PatreonTiers_t::Invalid };
}

void WebAPICalls::QueuePatrons(size_t timeOutMS /*= 0*/)
{
	CURL* curlInstance = curl_easy_init();

	if (curlInstance == nullptr) {
		if (m_PatronsCallback)
			m_PatronsCallback(WebAPIResult::CURLNotInitialized, {});

		return;
	}

	constexpr char url[] = "https://crimson-api-redirect.vercel.app/patrons";

	curl_easy_setopt(curlInstance, CURLOPT_URL, url);
	curl_easy_setopt(curlInstance, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);

	if (timeOutMS > 0) {
		curl_easy_setopt(curlInstance, CURLOPT_CONNECTTIMEOUT_MS, timeOutMS);
	}

	std::string responseJSON;

	curl_easy_setopt(curlInstance, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
	curl_easy_setopt(curlInstance, CURLOPT_WRITEDATA, &responseJSON);

	CURLcode res = curl_easy_perform(curlInstance);

	if (res == CURLE_OK) {
		if (m_PatronsCallback) {
			rapidjson::Document d;
			d.Parse(responseJSON.c_str());

			if (!(d.HasMember("patreonResponseStatus") && d["patreonResponseStatus"].GetUint64() == 200 && d.HasMember("patronList") && !d["patronList"].IsNull())) // Patron results are valid
			{
				m_PatronsCallback(WebAPIResult::PatreonError, {});

				return;
			}

			rapidjson::Value& patrons = d["patronList"];

			std::vector<Patron_t> patronsVec{};

			for (size_t i = 0; i < patrons.Size(); i++) {
				rapidjson::Value& patron = patrons[i];

				PatreonTiers_t tier;

				rapidjson::Value &patronTiers = patron["tiers"];

				std::vector<std::string> tierNames;
				tierNames.reserve(patronTiers.Size());
				for (size_t j = 0; j < patronTiers.Size(); j++)
				{
					tierNames.push_back(patronTiers[j].GetString());
				}

				const auto mostValuableTier = GetMostValuableTier(tierNames);

				const auto fullName = patron["fullName"].IsNull() ? "" : std::string_view(patron["fullName"].GetString());
				const auto vanity = patron["vanity"].IsNull() ? "" : std::string_view(patron["vanity"].GetString());

				const std::string shownName = std::string(vanity.empty() ? fullName : vanity);
				
				if (!shownName.empty() && mostValuableTier.ID != PatreonTiers_t::Invalid && mostValuableTier.ID != PatreonTiers_t::Free)
					patronsVec.emplace_back(shownName, mostValuableTier.Name, mostValuableTier.ID);
			}

			if (m_PatronsCallback)
				m_PatronsCallback(WebAPIResult::Success, patronsVec);
		}
	}
	else {
		if (m_PatronsCallback) {
			switch (res) {
			case CURLE_OPERATION_TIMEDOUT:
				m_PatronsCallback(WebAPIResult::Timeout, {});
				break;

			default:
				m_PatronsCallback(WebAPIResult::UnknownError, {});
				break;
			}
		}
	}

	curl_easy_cleanup(curlInstance);
}
