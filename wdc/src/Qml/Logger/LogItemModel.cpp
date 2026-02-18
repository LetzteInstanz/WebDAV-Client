#include "LogItemModel.h"

#include "../../Util.h"

using namespace Qml;

namespace {
    QString getTimeFormat() {
        const QLocale locale;
        return locale.dateFormat(QLocale::ShortFormat) + " hh:mm:ss" + locale.decimalPoint() + "zzz";
    }

    QDateTime cast_to_qdatetime(const Logger::TimePoint& time) {
#ifdef ANDROID
        return QDateTime::fromStdTimePoint(time).toTimeZone(QTimeZone(QTimeZone::LocalTime));
#else
        try {
            const std::chrono::time_zone* zone = std::chrono::current_zone();
            return QDateTime::fromStdLocalTime(zone->to_local(time));
        } catch (const std::runtime_error& e) {
            qCritical(qUtf8Printable(QObject::tr("LogItemModel: the time zone database cannot be initialized")), e.what());
            return QDateTime::fromStdTimePoint(time);
        }
#endif
    }

    constexpr QColor to_color(QtMsgType type) {
        switch (type) {
            case QtDebugMsg:
                return QColor(0x8A, 0x8A, 0x89);

            case QtInfoMsg:
                break;

            case QtWarningMsg:
                return QColor(0xB1, 0xB1, 0x2B);

            case QtCriticalMsg:
            case QtFatalMsg:
                return QColor(0xBC, 0x1C, 0x28);
        }
        return QColor(0x00, 0x00, 0x00);
    }
}

LogItemModel::LogItemModel(std::shared_ptr<Logger> logger, QObject* parent) : QAbstractTableModel(parent), _logger(std::move(logger)), _time_format(getTimeFormat()) {
    qDebug().noquote().nospace() << QObject::tr("The log item model is being created");

    _update_timer.setInterval(400);
    connect(&_update_timer, &QTimer::timeout, this, &LogItemModel::update);
    _update_timer.start();

    const auto update = [this](Logger::Message&& message) {
        const std::lock_guard<std::mutex> locker(_mutex);
        _sync_log.emplace_back(std::move(message));
        _needs_to_update.store(true, std::memory_order::relaxed);
    };
    _logger->set_notification_func(update);
    Logger::Log log = _logger->get_log();
    std::ranges::transform(std::ranges::views::as_rvalue(log), std::back_inserter(_log), [](Logger::Message&& message) { return std::make_tuple(to_color(std::get<QtMsgType>(message)), cast_to_qdatetime(std::get<Logger::TimePoint>(message)), std::move(std::get<QString>(message))); });
}

LogItemModel::~LogItemModel() {
    qDebug().noquote().nospace() << QObject::tr("The log item model is being destroyed");
    _logger->set_notification_func(nullptr);
}

int LogItemModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : _log.size(); }

int LogItemModel::columnCount(const QModelIndex& parent) const { return 2; }

QVariant LogItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Colour) || role >= to_int(Role::EnumSize))
        return QVariant();

    const Message& msg = _log[index.row()];
    switch (to_type<Role>(role)) {
        case Role::Colour: {
            return std::get<QColor>(msg);
        }

        case Role::TimeStr: {
            const auto& time = std::get<QDateTime>(msg);
            QLocale locale;
            return locale.toString(time, _time_format);
        }

        case Role::Text: {
            return std::get<QString>(msg);
        }

        default:
            break;
    }
    return QVariant();
}

QHash<int, QByteArray> LogItemModel::roleNames() const {
    auto names = QAbstractTableModel::roleNames();
    names.emplace(to_int(Role::Colour), "colour");
    names.emplace(to_int(Role::TimeStr), "time");
    names.emplace(to_int(Role::Text), "text");
    return names;
}

void LogItemModel::copyToClipboard(int index) const {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const Message& msg = _log[index];
    const auto& time = std::get<QDateTime>(msg);
    QLocale locale;
    clipboard->setText(QString("%1 %2").arg(locale.toString(time, _time_format), std::get<QString>(msg)));
}

void LogItemModel::copyAllToClipboard() const {
    QString text;
    text.reserve(_log.size() * std::pow(2, 15));
    QLocale locale;
    std::ranges::for_each(_log, [this, &text, &locale](const Message& msg) { text += QString("%1 %2").arg(locale.toString(std::get<QDateTime>(msg), _time_format), std::get<QString>(msg)) + '\n'; });
    if (!text.isEmpty())
        text.resize(text.size() - 1);

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

void LogItemModel::update() {
    if (!_needs_to_update.load(std::memory_order::relaxed))
        return;

    const std::size_t sz = _log.size();
    {
        const std::lock_guard<std::mutex> locker(_mutex);
        beginInsertRows(QModelIndex(), sz, sz + _sync_log.size() - 1);
        std::ranges::transform(std::ranges::views::as_rvalue(_sync_log), std::back_inserter(_log), [](Logger::Message&& message) { return std::make_tuple(to_color(std::get<QtMsgType>(message)), cast_to_qdatetime(std::get<Logger::TimePoint>(message)), std::move(std::get<QString>(message))); });
        _sync_log.clear();
    }
    endInsertRows();
}
