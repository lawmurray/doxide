#include "Driver.hpp"
#include "Version.h"

void write_file(const std::string& contents,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  std::fstream out(dst, std::ios::out);
  if (!out.is_open()) {
    throw std::runtime_error("could not write file " + dst.string());
  }
  out << contents;
}

void write_file_prompt(const std::string& contents,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  if (std::filesystem::exists(dst)) {
    std::cout << dst.string() << " already exists, overwrite? [y/N] ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans.length() > 0 && (ans[0] == 'y' || ans[0] == 'Y')) {
      write_file(contents, dst);
    }
  } else {
    write_file(contents, dst);
  }
}

std::string gulp(const std::filesystem::path& src) {
  std::string contents;
  std::ifstream in(src);
  if (!in.is_open()) {
    throw std::runtime_error("could not read file " + src.string());
  }
  char buffer[8192];
  while (in.read(buffer, sizeof(buffer))) {
    contents.append(buffer, sizeof(buffer));
  }
  contents.append(buffer, in.gcount());
  return contents;
}

int main(int argc, char** argv) {
  Driver driver;
  CLI::App app{"Modern documentation for modern C++.\n"};
  app.get_formatter()->column_width(30);
  app.add_option("--title",
      driver.title,
      "Main page title.");
  app.add_option("--description",
      driver.description,
      "Main page description.");
  app.add_option("--output", driver.output,
      "Output directory.");
  app.add_option("--coverage", driver.coverage,
      "Code coverage file (.gcov or .json).");
  app.set_version_flag("--version,-v", DOXIDE_VERSION, "Doxide version.");
  app.add_subcommand("init",
      "Initialize configuration files.")->
      fallthrough()->
      callback([&]() { driver.init(); });
  app.add_subcommand("build",
      "Build documentation in output directory.")->
      fallthrough()->
      callback([&]() { driver.build(); });
  app.add_subcommand("watch",
      "Watch the documentation's source files and rebuild it on changes.")->
      fallthrough()->
      callback([&]() { driver.watch(); });
  app.add_subcommand("clean",
      "Clean output directory.")->
      fallthrough()->
      callback([&]() { driver.clean(); });
  app.add_subcommand("cover",
      "Output code coverage data to stdout in JSON format.")->
      fallthrough()->
      callback([&]() { driver.cover(); });
  app.require_subcommand(1);
  CLI11_PARSE(app, argc, argv);
}
