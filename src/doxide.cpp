#include "Driver.hpp"

void write_file(const std::string& contents,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  std::fstream stream(dst, std::ios::out);
  stream << contents << '\n';
  stream.close();
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

void copy_file_prompt(const std::filesystem::path& src,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  if (std::filesystem::exists(dst)) {
    std::cout << dst.string() << " already exists, overwrite? [y/N] ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans.length() > 0 && (ans[0] == 'y' || ans[0] == 'Y')) {
      std::filesystem::copy_file(src, dst,
          std::filesystem::copy_options::overwrite_existing);
    }
  } else {
    std::filesystem::copy_file(src, dst,
        std::filesystem::copy_options::overwrite_existing);
  }
}

std::string gulp(const std::filesystem::path& src) {
  std::string contents;
  std::ifstream in(src);
  char buffer[4096];
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
  app.add_subcommand("init",
      "Initialize configuration files.")->
      fallthrough()->
      callback([&]() { driver.init(); });
  app.add_subcommand("build",
      "Build documentation in output directory.")->
      fallthrough()->
      callback([&]() { driver.build(); });
  app.add_subcommand("clean",
      "Clean output directory.")->
      fallthrough()->
      callback([&]() { driver.clean(); });
  app.add_subcommand("cover",
      "Output (on stdout) zero-count line data for mixing with code coverage reports.")->
      fallthrough()->
      callback([&]() { driver.cover(); });
  app.require_subcommand(1);
  CLI11_PARSE(app, argc, argv);
}
