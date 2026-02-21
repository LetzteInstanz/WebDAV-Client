#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

class FileSystemObject {
public:
    enum class Type {Directory, File};

    FileSystemObject(std::filesystem::path&& path, std::optional<std::chrono::sys_seconds>&& creation_time, std::optional<std::chrono::sys_seconds>&& modification_time, std::optional<std::uint64_t>&& size) noexcept;

    const std::filesystem::path& get_path() const;
    std::string get_name() const;
    std::string get_extension() const;
    Type get_type() const noexcept(noexcept(_path.has_filename()));
    const std::optional<std::chrono::sys_seconds>& get_creation_time() const noexcept;
    const std::optional<std::chrono::sys_seconds>& get_modification_time() const noexcept;
    const std::optional<std::uint64_t>& get_size() const noexcept;

private:
    std::filesystem::path _path;
    std::optional<std::chrono::sys_seconds> _creation_time;
    std::optional<std::chrono::sys_seconds> _modification_time;
    std::optional<std::uint64_t> _size;
};
