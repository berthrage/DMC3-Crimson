#include "WebAPICalls.hpp"

#include <string.h>

#define CURL_STATICLIB
#include <curl/curl.h>

#include "Core/RapidJSON.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

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

	constexpr char url[] = "https://api.github.com/repos/amir-120/Blender-RE-Engine-Model/releases/latest";

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
		
			rapidjson::Value& tagValue = d["tag_name"];
		
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
						rapidjson::Value& createdAtValue = d["created_at"];

						auto tp = ParseISO8601(createdAtValue.GetString());

						time_t tt = std::chrono::system_clock::to_time_t(tp);

						localtime_s(&queuedVersion.CreateTime.Local, &tt);
						gmtime_s(&queuedVersion.CreateTime.UTC, &tt);
					}

					{
						rapidjson::Value& createdAtValue = d["published_at"];

						auto tp = ParseISO8601(createdAtValue.GetString());

						time_t tt = std::chrono::system_clock::to_time_t(tp);

						localtime_s(&queuedVersion.PublishTime.Local, &tt);
						gmtime_s(&queuedVersion.PublishTime.UTC, &tt);
					}
				}

				// URL
				{
					rapidjson::Value& createdAtValue = d["assets"][0]["browser_download_url"];
				
					queuedVersion.DirectURL = createdAtValue.GetString();
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

void WebAPICalls::QueuePatrons(size_t timeOutMS /*= 0*/)
{
	// Todo: fix the online api for patrons, then remove the return
	return;

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

			rapidjson::Value& patrons = d["patrons"];

			std::vector<Patron_t> patronsVec{};

			for (size_t i = 0; i < patrons.Size(); i++) {
				rapidjson::Value& patron = patrons[i];

				PatreonTiers_t tier;

				switch (patron["tier_id"].GetUint64()) {
				// Rich
				case 9668149:
					tier = PatreonTiers_t::Rich;
					break;

				// Rich AF
				case 9668242:
					tier = PatreonTiers_t::RichAF;
					break;

				default:
					break;
				}

				patronsVec.emplace_back(patron["user_name"].GetString(), patron["tier_name"].GetString(), 
										patron["user_id"].GetUint64(), patron["tier_id"].GetUint64(), tier);
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
