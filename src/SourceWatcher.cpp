#include "SourceWatcher.hpp"

SourceWatcher::SourceWatcher(std::list<std::string> patterns): patterns{patterns}{
  for (auto pattern : patterns){
    auto paths = glob::rglob(pattern);
    for (auto& file : paths) {
      tracked_files[file.string()] = std::filesystem::last_write_time(file);
    }
  }
}

SourceWatcher::SourceWatcher(std::string pattern): SourceWatcher(std::list<std::string>{pattern}){
  //
}

std::tuple<std::unordered_set<std::filesystem::path>,
           std::unordered_set<std::filesystem::path>,
           std::unordered_set<std::filesystem::path>> SourceWatcher::diff(){
  std::unordered_set<std::filesystem::path> added_files;
  std::unordered_set<std::filesystem::path> modified_files;
  std::unordered_set<std::filesystem::path> deleted_files = filenames();

  for (auto pattern : patterns){
    auto paths = glob::rglob(pattern);
    for(auto& file : paths) {
      auto last_write_time = std::filesystem::last_write_time(file);

      /* Remove this file from the deleted files set as it still exists*/
      deleted_files.erase(file.string());

      /* Check file creation and modification */
      if(! tracked_files.contains(file.string())) {
        added_files.insert(file.string());
      } else if(tracked_files[file.string()] != last_write_time) {
        modified_files.insert(file.string());
      }

      /* Update the timestamp */
      tracked_files[file.string()] = last_write_time;
    }
  }

  /* Remove deleted files from the tracked_files map */
  for (auto deleted_file : deleted_files){
      tracked_files.erase(deleted_file);
  }

  return {added_files, modified_files, deleted_files};
}

bool SourceWatcher::changed(){
  auto [added_files, modified_files, deleted_files] = diff();
  return !added_files.empty() || !modified_files.empty() || !deleted_files.empty();
}

std::unordered_set<std::filesystem::path> SourceWatcher::filenames(){
  std::unordered_set<std::filesystem::path> filenames;
  for (auto pairs : tracked_files) {
    filenames.insert(pairs.first);
  }
  return filenames;
}
