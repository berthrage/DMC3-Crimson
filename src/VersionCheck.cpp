#include "VersionCheck.hpp"

#include <chrono>
#include <string.h>

#include "Core/RapidJSON.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

class Timer_t {
public:
	static void Reset() {
		auto now = std::chrono::high_resolution_clock::now();
		m_StartingPoint = now;
		m_LastRecord = now;
	}

	static double GetStep() {
		auto now = std::chrono::high_resolution_clock::now();

		return std::chrono::duration<float, std::milli>(now - m_LastRecord).count();

		m_LastRecord = now;
	}

	static double GetTotalTime() {
		auto now = std::chrono::high_resolution_clock::now();

		return std::chrono::duration<double, std::milli>(now - m_StartingPoint).count();
	}

private:
	inline static std::chrono::steady_clock::time_point m_StartingPoint{};
	inline static std::chrono::steady_clock::time_point m_LastRecord{};
} g_Timer{};

struct ClinetData_t {
	size_t MaxTimeMX;
	Timer_t* pTimer;
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

int VersionTracker::CurlProgressCallback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	auto transferInfo = (ClinetData_t*)clientp;

	if (transferInfo->pTimer->GetTotalTime() <= (double)transferInfo->MaxTimeMX)
		return CURL_PROGRESSFUNC_CONTINUE;
	
	return 1;
}

VersionTracker::VersionTracker()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	m_CURLInstance = curl_easy_init();
}

VersionTracker::~VersionTracker()
{
	curl_easy_cleanup(m_CURLInstance);

	curl_global_cleanup();
}

void VersionTracker::QueueLatestRelease(size_t timeOutMS /*= 0*/)
{
	if (m_CURLInstance == nullptr) {
		if (m_Callback)
			m_Callback(VersionCheckResult::CURLNotInitialized, {});

		return;
	}

	constexpr char url[] = "https://api.github.com/repos/amir-120/Blender-RE-Engine-Model/releases/latest";

	curl_easy_setopt(m_CURLInstance, CURLOPT_URL, url);
	curl_easy_setopt(m_CURLInstance, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);

	auto customData = ClinetData_t{ timeOutMS, &g_Timer };

	if (timeOutMS > 0) {
		curl_easy_setopt(m_CURLInstance, CURLOPT_XFERINFODATA, &customData);
		curl_easy_setopt(m_CURLInstance, CURLOPT_XFERINFOFUNCTION, CurlProgressCallback);
	}

	struct curl_slist* headers = NULL;

	headers = curl_slist_append(headers, "Accept: application/vnd.github+json");
	headers = curl_slist_append(headers, "Authorization: Bearer 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");
	headers = curl_slist_append(headers, "X-GitHub-Api-Version: 2022-11-28");
	headers = curl_slist_append(headers, "User-Agent: curl");

	curl_easy_setopt(m_CURLInstance, CURLOPT_HTTPHEADER, headers);

	std::string responseJSON;

	curl_easy_setopt(m_CURLInstance, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
	curl_easy_setopt(m_CURLInstance, CURLOPT_WRITEDATA, &responseJSON);

	CURLcode res = curl_easy_perform(m_CURLInstance);

	if (res == CURLE_OK) {
		if (m_Callback) {
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

				m_Callback(VersionCheckResult::Success, queuedVersion);
			}
		}
	}
	else {
		if (m_Callback)
			m_Callback(VersionCheckResult::UnknownError, {});
	}

	curl_slist_free_all(headers);
}
