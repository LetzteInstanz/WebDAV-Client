#pragma once

#include <FileSystem/FileSystemModel.h>

namespace Qml {
    class AbstractFileSystemModel : public QObject {
        Q_OBJECT

    public:
        explicit AbstractFileSystemModel(std::shared_ptr<FileSystemModel> model, QString&& log_prefix, FileSystemModel::DataSet data_set, bool overwrite_result);
        ~AbstractFileSystemModel() override;

        Q_INVOKABLE void requestData(const QString& absolute_path, bool recursive);
        Q_INVOKABLE void abortRequests();

    signals:
        void maxProgressEnabled(bool enabled);
        void progressChanged(float value);
        void maxProgressChanged(float max);
        void progressTextChanged(const QString& text);
        void errorOccurred(FileSystemModel::Id id, const QString& text, bool is_critical);
        void ready(FileSystemModel::Id id);

    private:
        void process_error(FileSystemModel::Id id, FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error, bool is_critical);
        void process_reply(FileSystemModel::FsPair&& id_fs_pair);
        bool drop_ignore(FileSystemModel::Id id);

    protected:
        std::unordered_map<FileSystemModel::Id, FileSystemModel::ObjectPair> _fs_by_id_map;

    private:
        std::shared_ptr<FileSystemModel> _fs_model;
        const QString _log_prefix;
        const FileSystemModel::DataSet _data_set;
        const bool _overwrite_result;
        std::unordered_set<FileSystemModel::Id> _uncompl_req_ids;
        std::unordered_set<FileSystemModel::Id> _ignored_req_ids;
    };
}
