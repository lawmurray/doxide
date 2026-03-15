#pragma once

#include <regex>


/**
 * Regular expression configuration.
 *
 * @ingroup developer
 */
constexpr std::regex_constants::syntax_option_type REGEX_FLAGS =
    std::regex_constants::ECMAScript|std::regex_constants::optimize;
