#include "LogItemModel.h"

#include "../../Logger.h"
#include "../../pch.h"
#include "../Util.h"

using namespace Qml;

namespace {
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

    QString&& insert_indentation(QString&& str) { return std::move(str.replace('\n', QStringLiteral("\n    "))); }
}

LogItemModel::LogItemModel(std::shared_ptr<Logger> logger, QObject* parent) : QAbstractListModel(parent), _logger(std::move(logger)) {
    qDebug().noquote() << QObject::tr("The log item model is being created");

    _update_timer.setInterval(400);
    connect(&_update_timer, &QTimer::timeout, this, &LogItemModel::update);
    _update_timer.start();

    const auto update = [this](Logger::Message&& message) {
        const std::lock_guard<std::mutex> locker(_mutex);
        _sync_log.emplace_back(std::make_pair(to_color(message.first), message.second));
        _needs_to_update.store(true, std::memory_order::relaxed);
    };
    _logger->set_notification_func(update);
    Logger::Log log = _logger->get_log();
    std::transform(std::make_move_iterator(std::begin(log)), std::make_move_iterator(std::end(log)), std::back_inserter(_log), [](Logger::Message&& message) { return std::make_pair(to_color(message.first), insert_indentation(std::move(message.second))); });
}

LogItemModel::~LogItemModel() {
    qDebug().noquote() << QObject::tr("The log item model is being destroyed");
    _logger->set_notification_func(nullptr);
}

int LogItemModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : _log.size(); }

QVariant LogItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Colour) || role >= to_int(Role::EnumSize))
        return QVariant();

    const Message& msg = _log[index.row()];
    switch (to_type<Role>(role)) {
        case Role::Colour:
            return msg.first;

        case Role::Text:
            return msg.second;

        default:
            break;
    }
    return QVariant();
}

QHash<int, QByteArray> LogItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Colour), "colour");
    names.emplace(to_int(Role::Text), "text");
    return names;
}

void LogItemModel::copyToClipboard(int index) {
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(_log[index].second);
}

void LogItemModel::copyAllToClipboard() {
    QString text;
    text.reserve(_log.size() * std::pow(2, 15));
    std::for_each(std::cbegin(_log), std::cend(_log), [&text](const Message& msg) { text += msg.second + '\n'; });
    if (!text.isEmpty())
        text.resize(text.size() - 1);

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

void LogItemModel::update() {
    if (!_needs_to_update.load(std::memory_order::relaxed))
        return;

    const size_t sz = _log.size();
    {
        const std::lock_guard<std::mutex> locker(_mutex);
        beginInsertRows(QModelIndex(), sz, sz + _sync_log.size() - 1);
        std::transform(std::make_move_iterator(std::begin(_sync_log)), std::make_move_iterator(std::end(_sync_log)), std::back_inserter(_log), [](Message&& message) { return std::make_pair(message.first, insert_indentation(std::move(message.second))); });
        _sync_log.clear();
    }
    endInsertRows();
}
