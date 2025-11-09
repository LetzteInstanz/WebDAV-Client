#pragma once

class FileSystemModel;
class FileSystemObject;

namespace Qml {
    class FileItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit FileItemModel(const std::filesystem::path& root_path, std::shared_ptr<::FileSystemModel> model, QObject* parent = nullptr);
        ~FileItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE int getCheckedToDownloadItemCount() const;
        Q_INVOKABLE QVariantList getCheckedToDownloadIndexes() const;

    private:
        std::size_t get_shift() const noexcept;
        FileSystemObject get_object(int row) const;
        std::string get_icon_name(const FileSystemObject& obj, int row) const;
        void update();

    private:
        static const std::unordered_map<std::string, std::string> _icon_name_by_extension_map;
        static const std::unordered_set<std::string> _special_icon_name_set;

        const std::filesystem::path _root_path;
        std::shared_ptr<::FileSystemModel> _fs_model;
        bool _root;
        std::unordered_set<int> _ready_to_download_indexes;
    };
}
