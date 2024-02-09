#include "Logger.h"

namespace {
    bool operator>(QtMsgType lhs, QtMsgType rhs) {
        if (lhs != QtInfoMsg && rhs != QtInfoMsg)
            return static_cast<int>(lhs) > static_cast<int>(rhs);

        return lhs == QtInfoMsg && rhs == QtDebugMsg;
    }

    bool operator>=(QtMsgType lhs, QtMsgType rhs) { return lhs == rhs || lhs > rhs; }

    bool operator<(QtMsgType lhs, QtMsgType rhs) { return !(lhs >= rhs); }
}

QtMessageHandler Logger::_default_handler = nullptr;

Logger::Logger() { _default_handler = qInstallMessageHandler(&Logger::message_handler); }

void Logger::message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    _default_handler(type, context, msg);
    std::shared_ptr<Logger> logger = Logger::get_instance();
    if (type >= logger->get_max_level())
        logger->append_msg(type, msg);
}

std::shared_ptr<Logger> Logger::get_instance() {
#ifdef __cpp_lib_atomic_shared_ptr
    static std::atomic<std::shared_ptr<Logger>> logger(std::shared_ptr<Logger>(new Logger()));
    return logger.load(std::memory_order::relaxed);
#else
    static std::shared_ptr<Logger> logger(new Logger());
    return std::atomic_load_explicit(&logger, std::memory_order::relaxed);
#endif
}

void Logger::set_max_level(QtMsgType level) {
    const auto old = _max_level.load(std::memory_order::relaxed);
    if (old == level)
        return;

    _max_level.store(level, std::memory_order::relaxed);
    if (_filtered || old > level)
        return;

    {
        const std::lock_guard<std::mutex> locker(_mutex);
        const auto end = std::end(_log);
        const auto it = std::remove_if(std::begin(_log), end, [level](const std::pair<QtMsgType, QString>& pair) { return pair.first < level; });
        _log.erase(it, end);
    }
    _filtered = true;
}

QString Logger::get_log() const {
    QString log;
    QTextStream stream(&log);
    {
        const std::lock_guard<std::mutex> locker(_mutex);
        for (const std::pair<QtMsgType, QString>& pair : _log)
            stream << pair.second << Qt::endl;

        _enable_signal = true;
    }
    return log;
}

void Logger::append_msg(QtMsgType type, const QString& msg) {
    const std::lock_guard<std::mutex> locker(_mutex);
    if (type < get_max_level()) // note: The value of _max_level may change in the main thread between calls get_max_level() and append_msg() in message_handler() in another thread
        return;

    _log.emplace_back(std::make_pair(type, msg));
    if (_enable_signal && _notification_func) // note: A message was passed from the QML engine 2 times into the TextArea in QML: signal call _notification_func was posted into the event loop, then get_log() was called.
        _notification_func(msg);              // _enable_signal enables signal sending only after get_log() call.
}

void Logger::set_notification_func(std::function<void (const QString&)>&& func) {
    const std::lock_guard<std::mutex> locker(_mutex);
    _notification_func = func;
}
