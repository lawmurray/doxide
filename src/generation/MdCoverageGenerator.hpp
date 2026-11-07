#pragma once

#include <FileSystemStatsInfo.hpp>

#include <filesystem>

class FileSystemDeleter;
class sink_t;

class MdCoverageGenerator {
public:
  /**
    * Constructor.
    *
    * @param output Output directory.
    * @param style Markdown style.
    */
  MdCoverageGenerator(const std::filesystem::path& output, FileSystemDeleter& _fscleaner):
    output_root(output),
    fscleaner(_fscleaner)
    {}

  /**
    * Generate coverage information.
    *
    * @param root Root entity.
    * @param cov Include code coverage report?
    */
  void generate(const FileSystemStatsInfo& fsinfo);

private:

  /**
   * Output directory.
   */
  std::filesystem::path output_root;

  FileSystemDeleter& fscleaner;
};
