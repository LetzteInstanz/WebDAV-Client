#pragma once

class FileSystemModel;
class FileSystemObject;

namespace Qml {
    class MainFileSystemModel;

    class FileItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit FileItemModel(std::filesystem::path&& root_path, MainFileSystemModel& model, QObject* parent = nullptr);
        ~FileItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE int getCheckedToDownloadItemCount() const;
        Q_INVOKABLE QVariantList getCheckedToDownloadIndexes() const;

    private:
        std::size_t get_shift() const noexcept;
        bool is_up_dir_row(int row) const noexcept;
        const FileSystemObject& get_object(int row) const;
        std::string get_icon_name(const FileSystemObject& obj, int row) const;
        void update();

    private:
        static const std::unordered_map<std::string, std::string> _icon_name_by_extension_map;
        static const std::unordered_set<std::string> _special_icon_name_set;

        const std::filesystem::path _root_path;
        MainFileSystemModel& _fs_model;
        bool _root;
        std::unordered_set<int> _ready_to_download_indexes;
    };
}
