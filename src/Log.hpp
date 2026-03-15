#pragma once


#include <string>
#include <iostream>
#include <cstdlib>

/**
 * Print a warning message.
 *
 * @ingroup developer
 */
#define warn(msg) \
    std::cerr << "warning: " << msg << std::endl;

/**
 * Print an error message and exit.
 *
 * @ingroup developer
 */
#define error(msg) \
    std::cerr << "error: " << msg << std::endl; \
    std::exit(EXIT_FAILURE);
