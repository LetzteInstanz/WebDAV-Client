#include "Util.h"

void log_duration(const QString& prefix, const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end) {
    const auto duration = end - start;
    const auto seconds = std::chrono::floor<std::chrono::seconds>(duration);
    const auto ms = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(duration - seconds);
    qDebug().noquote().nospace() << prefix << std::format(std::locale(), "{}", seconds) << ' ' << std::format(std::locale(), "{}", ms);
}
