#include "QmlLogger.h"

#include "../Logger.h"

using namespace Qml;

QmlLogger::QmlLogger(std::shared_ptr<Logger> logger, QObject* parent) : QObject(parent), _logger(logger) {
    const std::function<void(const QString&)> func = std::bind(&QmlLogger::emit_signal, this, std::placeholders::_1);
    _logger->set_notification_func(func);
}

QmlLogger::~QmlLogger() { _logger->set_notification_func(nullptr); }

QString QmlLogger::getLog() const { return _logger->get_log(); }

void QmlLogger::emit_signal(const QString& msg) { QTimer::singleShot(0, this, [this, msg]() { msgReceived(msg); }); }
