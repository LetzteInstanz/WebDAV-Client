#pragma once

#include "../FileSystem/FileSystemModel.h"
#include "AbstractFileSystemModel.h"

namespace Qml {
    class MainFileSystemModel : public AbstractFileSystemModel {
        Q_OBJECT

    public:
        explicit MainFileSystemModel(std::shared_ptr<FileSystemModel> model, std::function<void ()>&& on_destroy_func);
        ~MainFileSystemModel() override;

        Q_INVOKABLE QString getCurrentPath() const;

        const FileSystemObject& get_curr_dir_object() const;
        const FileSystemObject& get_object(std::size_t index) const;
        std::size_t get_count() const noexcept;

    private:
        const FileSystemModel::ObjectPair& get_pair() const noexcept;

    private:
        const std::function<void ()> _on_destroy_func;
    };
}
