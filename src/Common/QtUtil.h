#pragma once

#include <chrono>

#include <QMetaObject>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <Qt>

QString to_qstring(QNetworkReply::NetworkError error);

template <typename Func, typename... Args>
bool invoke_method(QObject& object, Func&& func, Args&&... args) { return QMetaObject::invokeMethod(&object, std::forward<Func>(func), Qt::QueuedConnection, std::forward<Args>(args)...); }

template <typename Func, typename... Args>
bool invoke_method_sync(QObject& object, Func&& func, Args&&... args) { return QMetaObject::invokeMethod(&object, std::forward<Func>(func), Qt::BlockingQueuedConnection, std::forward<Args>(args)...); }

void log_duration(const QString& prefix, const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end);
