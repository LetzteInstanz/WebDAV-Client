#include "Logger.h"

#include "../Logger.h"

Qml::Logger::Logger(std::shared_ptr<::Logger> logger, QObject* parent) : QObject(parent), _logger(std::move(logger)) {
    _logger->set_notification_func(std::bind(&Logger::emit_signal, this, std::placeholders::_1, std::placeholders::_2));
}

Qml::Logger::~Logger() { _logger->set_notification_func(nullptr); }

QString Qml::Logger::getLog() const {
    QString html;
    QTextStream stream(&html);
    const ::Logger::Log log = _logger->get_log();
    for (const ::Logger::Message& pair : log)
        stream << wrap(pair.first, pair.second.toHtmlEscaped());

    return html;
}

QString Qml::Logger::wrap(QtMsgType type, QString&& msg) {
    msg += QStringLiteral("<br/>");
    QString colour;
    switch (type) {
        case QtDebugMsg:
            colour = QStringLiteral("#8A8A89");
            break;

        case QtInfoMsg:
            break;

        case QtWarningMsg:
            colour = QStringLiteral("#B1B12B");
            break;

        case QtCriticalMsg:
        case QtFatalMsg:
            colour = QStringLiteral("#BC1C28");
            break;
    }
    if (!colour.isEmpty())
        msg = QStringLiteral("<font color=\"") + colour + QStringLiteral("\">") + msg + QStringLiteral("</font>");

    return std::move(msg);
}

void Qml::Logger::emit_signal(QtMsgType type, const QString& msg) { QTimer::singleShot(0, this, [this, type, msg]() { msgReceived(wrap(type, msg.toHtmlEscaped())); }); }
