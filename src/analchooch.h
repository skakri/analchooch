#ifndef SRC_ANALCHOOCH_H_
#define SRC_ANALCHOOCH_H_

#include <vector>
#include <algorithm>

#ifndef WIFI_SSID
#define WIFI_SSID "ssid"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "pass"
#endif

#ifndef STEAM_KEY
#define STEAM_KEY ""
#endif

#ifndef STEAM_USER_ID
#define STEAM_USER_ID ""
#endif

template<typename T>
std::vector<std::vector<T>> SplitVector(const std::vector<T>& vec, size_t n) {
    std::vector<std::vector<T>> outVec;

    size_t length = vec.size() / n;
    size_t remain = vec.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < std::min(n, vec.size()); ++i) {
        end += (remain > 0) ? (length + !!(remain--)) : length;

        outVec.push_back(
            std::vector<T>(vec.begin() + begin, vec.begin() + end));

        begin = end;
    }

    return outVec;
}

#endif  // SRC_ANALCHOOCH_H_
