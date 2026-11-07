#pragma once

#include "Utils.hpp"
#include "entities/FileEntity.hpp"
#include "entities/FwdRef.hpp"
#include "entities/LineStats.hpp"

#include <cstddef>
#include <filesystem>
#include <format>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <tuple>
#include <type_traits>

class FileSystemStatsInfo {
public:

  FileSystemStatsInfo() = default;

  FileSystemStatsInfo(const FileSystemStatsInfo&) = delete;

  FileSystemStatsInfo& operator=(const FileSystemStatsInfo&) = delete;

  static inline constexpr std::string_view DEF_ROOT_PATH = "./";

  struct DirInfo {
    std::unordered_set<std::string_view> subdirs;
    std::unordered_set<FwdRef<FileEntity>> files;
    LineStats stats;

    DirInfo() = default;
  };

  std::optional<FwdRef<FileEntity>> lookup_file(std::string_view path);

  std::optional<FwdRef<FileEntity>> lookup_file(const std::filesystem::path& path) {
    std::string_view p = path.native();
    return lookup_file(p);
  }

  std::optional<std::reference_wrapper<DirInfo>> lookup_dir(std::string_view path);

  std::optional<std::reference_wrapper<const DirInfo>> lookup_dir(std::string_view path) const;

  void add_file(const std::filesystem::path& path, FwdRef<FileEntity> file);

  void explore() const;

  bool contains(const std::filesystem::path& p) const {
    return files_map.contains(p.native());
  }

  void remove_file(const std::filesystem::path& filepath);

  void clear() {
    dir_info_map.clear();
    files_map.clear();
  }

private:
  std::unordered_map<std::string, DirInfo, het_string_hash, std::equal_to<>> dir_info_map;
  std::unordered_map<std::string_view, FwdRef<FileEntity>> files_map;
};

template<bool IS_CONST> struct _DirStackController {

  using _dirinfo_t = std::conditional_t<IS_CONST,
    const FileSystemStatsInfo::DirInfo, FileSystemStatsInfo::DirInfo>;
  using _fsinfo_t = std::conditional_t<IS_CONST,
    const FileSystemStatsInfo, FileSystemStatsInfo>;

  using iter_t = std::conditional_t<IS_CONST,
    std::unordered_set<std::string_view>::const_iterator,
    std::unordered_set<std::string_view>::iterator>;
  using stack_value = std::tuple<std::string_view, _dirinfo_t&, iter_t>;
  using yield_value = std::tuple<std::string_view, _dirinfo_t&>;

  _fsinfo_t& fsinfo;
  std::string_view root_dir = FileSystemStatsInfo::DEF_ROOT_PATH;

  stack_value make_init() {
    auto opt = fsinfo.lookup_dir(root_dir);
    if (not opt) {
      throw std::runtime_error(std::format("cannot initialize from {}", root_dir));
    }
    _dirinfo_t& d = opt->get();
    return stack_value{root_dir, d, d.subdirs.begin()};
  }

  yield_value make_yield_pre(const stack_value& top) {
    return yield_value{std::get<0>(top), std::get<1>(top)};
  }

  yield_value make_yield_post(const stack_value& top) {
    return make_yield_pre(top);
  }

  bool has_next(const stack_value& top) const {
    return std::get<2>(top) != std::get<1>(top).subdirs.end();
  }

  stack_value make_next(stack_value& top) {
    iter_t& it = std::get<2>(top);
    std::string_view next_path = *it;
    ++it;
    auto opt = fsinfo.lookup_dir(next_path);
    if (not opt) {
      throw std::runtime_error(std::format("cannot make next from {}", root_dir));
    }
    _dirinfo_t& d = opt->get();
    return stack_value{next_path, d, d.subdirs.begin()};
  }

  void after_pop(const stack_value&) {}
};

using DirStackController = _DirStackController<false>;
using ConstDirStackController = _DirStackController<true>;
