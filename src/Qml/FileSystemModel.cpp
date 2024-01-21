#include "FileSystemModel.h"

#include "../FileSystem/FileSystemModel.h"

Qml::FileSystemModel::FileSystemModel(std::shared_ptr<::FileSystemModel> model) : _fs_model(model) {}

void Qml::FileSystemModel::setServerInfo(const QString& addr, const uint16_t port, const QString& path) { _fs_model->set_server_info(addr, port, path); }

void Qml::FileSystemModel::requestFileList() {
    maxProgressEnabled(false);
    progressTextChanged(tr("Getting the list of files…"));
    _fs_model->request_file_list();
}

void Qml::FileSystemModel::stop() { _fs_model->stop(); }
