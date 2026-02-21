#include "../include/FileSystem/FileSystemObject.h"

FileSystemObject::FileSystemObject(std::filesystem::path&& path, std::optional<std::chrono::sys_seconds>&& creation_time, std::optional<std::chrono::sys_seconds>&& modification_time, std::optional<std::uint64_t>&& size) noexcept
    : _path(std::move(path)), _creation_time(std::move(creation_time)), _modification_time(std::move(modification_time)), _size(std::move(size))
{}

const std::filesystem::path& FileSystemObject::get_path() const { return _path; }

std::string FileSystemObject::get_name() const {
    const auto final_path = _path.has_filename() ? _path : _path.parent_path();
    return final_path.filename().generic_string();
}

std::string FileSystemObject::get_extension() const {
    const auto extension = _path.extension().generic_string();
    return extension.empty() ? extension : extension.substr(1);
}

FileSystemObject::Type FileSystemObject::get_type() const noexcept(noexcept(_path.has_filename())) { return _path.has_filename() ? Type::File : Type::Directory; }

const std::optional<std::chrono::sys_seconds>& FileSystemObject::get_creation_time() const noexcept { return _creation_time; }

const std::optional<std::chrono::sys_seconds>& FileSystemObject::get_modification_time() const noexcept { return _modification_time; }

const std::optional<std::uint64_t>& FileSystemObject::get_size() const noexcept { return _size; }
