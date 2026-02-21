#include "../include/FileSystem/FileSystemModel.h"

#include "ThreadData.h"

FileSystemModel::FileSystemModel(QStringView addr, std::uint16_t port) {
    qDebug().noquote().nospace() << QObject::tr("FileSystemModel: created");
    std::mutex mutex;
    std::condition_variable cv;
    auto thread_func = [this, &mutex, &cv, addr = addr.toString(), port]() mutable {
        qDebug().noquote().nospace() << QObject::tr("FileSystemModel: thread started");
        FileSystemModel::ThreadData data(std::move(addr), port);
        {
            const std::lock_guard lock(mutex);
            _thread_data = &data;
        }
        cv.notify_one();
        return data.loop.exec();
    };
    _future = std::async(std::launch::async, std::move(thread_func));
    std::unique_lock lock(mutex);
    cv.wait(lock, [this]() { return _thread_data != nullptr; });
}

FileSystemModel::~FileSystemModel() {
    _thread_data->stop();
    _thread_data = nullptr;
    qDebug().noquote().nospace() << QObject::tr("FileSystemModel: destroyed, thread exit code: ") << _future.get();
}

FileSystemModel::Id FileSystemModel::request_data(RequestTuple&& request) {
    assert(std::get<std::filesystem::path>(request).root_path() == "/" && std::get<std::filesystem::path>(request).root_directory() == "/");
    _thread_data->request_data(_next_id, std::move(request));
    return _next_id++;
}

void FileSystemModel::abort_request(IdVector&& ids, std::function<void (IdVector&&)>&& notif_func) { _thread_data->abort_request(std::move(ids), std::move(notif_func)); }

void FileSystemModel::abort_request_sync(IdVector&& ids) { _thread_data->abort_request_sync(std::move(ids)); }
