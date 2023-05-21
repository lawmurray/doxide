#include "Driver.hpp"

int main(int argc, char** argv) {
  /* first option (should be a command name) */
  std::string prog = argc > 1 ? argv[1]: "help";

  Driver driver(argc - 1, argv + 1);
  if (prog.compare("init") == 0) {
    driver.init();
  } else if (prog.compare("docs") == 0) {
    driver.docs();
  } else if (prog.compare("clean") == 0) {
    driver.clean();
  } else if (prog.compare("help") == 0 || prog.compare("--help") == 0) {
    driver.help();
  } else {
    std::cerr << "unrecognized command '" << prog << "', see 'doxide help' for usage." << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
