#include "FileSystemStatsInfo.hpp"

#include <cstddef>
#include <format>
#include <stdexcept>

#include <iostream>

std::optional<FwdRef<FileEntity>> FileSystemStatsInfo::lookup_file(std::string_view path) {
  auto it = files_map.find(path);
  if (it == files_map.end()) {
    return {};
  }
  return it->second;
}

std::optional<std::reference_wrapper<FileSystemStatsInfo::DirInfo>> FileSystemStatsInfo::lookup_dir(std::string_view path) {
  auto it = dir_info_map.find(path);
  if (it == dir_info_map.end()) {
    return {};
  }
  return it->second;
}

std::optional<std::reference_wrapper<const FileSystemStatsInfo::DirInfo>> FileSystemStatsInfo::lookup_dir(std::string_view path) const {
  auto it = dir_info_map.find(path);
  if (it == dir_info_map.cend()) {
    return {};
  }
  return it->second;
}

void FileSystemStatsInfo::add_file(const std::filesystem::path& path, FwdRef<FileEntity> file) {
  namespace fs = std::filesystem;

  if (path.empty() or not path.has_filename()) {
    throw std::runtime_error(std::format("empty file path or no filename"));
  }

  fs::path normalized = path.lexically_normal();
  auto [_, newly_created] = files_map.try_emplace(file->get_name(), file);
  if (not newly_created) {
    throw std::runtime_error(std::format("file '{}' was already added", normalized.native()));
  }

  auto get_or_create_path = [&](std::string_view path) {
    auto it = dir_info_map.find(path);
    if (it == dir_info_map.end()) {
      auto [pit, _] = dir_info_map.try_emplace(std::string(path));
      it = pit;
    }
    return it;
  };

  fs::path parent = normalized.parent_path();
  std::string_view parent_view = not parent.empty() ? parent.native() : DEF_ROOT_PATH;
  auto it = get_or_create_path(parent_view);
  it->second.files.insert(file);
  if (parent.empty()) {
    return;
  }

  parent = parent.parent_path();
  while(not parent.empty()) {
    auto pit = get_or_create_path(parent.native());
    pit->second.subdirs.insert(it->first);
    it = pit;
    parent = parent.parent_path();
  }

  auto pit = get_or_create_path(DEF_ROOT_PATH);
  pit->second.subdirs.insert(it->first);
}

void FileSystemStatsInfo::explore() const {
  std::cout << "DIRS\n";
  for(const auto& [k, v]: dir_info_map) {
    std::cout << "----- " << k << ":\n";
    const auto& stats = lookup_dir(k)->get().stats;
    std::cout << "    - covered_lines " << stats.covered_lines << std::endl;
    std::cout << "    - included_lines " << stats.included_lines << std::endl;
    for(const auto& f: v.subdirs) {
      std::cout << "  dir: " << f << "\n";
      const auto& stats = lookup_dir(f)->get().stats;
      std::cout << "    - covered_lines " << stats.covered_lines << std::endl;
      std::cout << "    - included_lines " << stats.included_lines << std::endl;
    }
    for(const auto& f: v.files) {
      std::cout << "  file: " << f->get_name() << "\n";
      std::cout << "    - covered_lines " << f->get_covered_lines() << std::endl;
      std::cout << "    - included_lines " << f->get_included_lines() << std::endl;
    }
    std::cout << "\n";
  }
  std::cout << "FILES\n";
  for(const auto& [k, _]: files_map) {
    std::cout << "----- " << k << "\n";
  }
}

void FileSystemStatsInfo::remove_file(const std::filesystem::path& filepath) {
  namespace fs = std::filesystem;
  fs::path normalized = filepath.lexically_normal();
  std::string_view norm_view = normalized.native();
  auto file_it = files_map.find(normalized.native());
  if (file_it == files_map.end()) {
    throw std::runtime_error(std::format("cannot find file name \"{}\"", filepath.native()));
  }
  FwdRef<FileEntity> fe = file_it->second;
  files_map.erase(file_it);

  auto get_opt_path = [&](std::string_view path) {
    auto it = dir_info_map.find(path);
    if (it == dir_info_map.end()) {
      return std::optional<decltype(it)>();
    }
    return std::optional(it);
  };

  fs::path parent = normalized.parent_path();
  bool is_root = parent.empty();
  std::string_view parent_view = not is_root ? parent.native() : DEF_ROOT_PATH;
  auto parent_it = get_opt_path(parent_view);
  if (parent_it) {
    (*parent_it)->second.files.erase(fe);
  }
  while(parent_it and (*parent_it)->second.files.empty() and (*parent_it)->second.subdirs.empty() and not is_root) {
    std::size_t last_sep = (*parent_it)->first.find_last_of('/');
    is_root = last_sep == 0 or last_sep == std::string_view::npos;
    std::string_view parent_view = is_root ? DEF_ROOT_PATH : norm_view.substr(0, last_sep);
    auto current_it = *parent_it;
    parent_it = get_opt_path(parent_view);
    if (parent_it) {
      (*parent_it)->second.subdirs.erase(current_it->first);
    }
    dir_info_map.erase(current_it);
  }
}
