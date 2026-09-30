#pragma once

#include <string>
#include <filesystem>

/**
 * Write a whole file, prompting to confirm overwrite if it already exists.
 *
 * @ingroup developer
 */
void write_file_prompt(const std::string& contents, const std::filesystem::path& dst);

/**
 * Read in a whole file.
 *
 * @ingroup developer
 */
std::string gulp(const std::filesystem::path& src);
