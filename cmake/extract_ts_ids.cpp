#include "CppQueries.hpp"

#include <tree_sitter/api.h>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <format>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

extern "C" const TSLanguage* tree_sitter_cuda();

static void print_name_printer(string_view name, const vector<string>& values) {
  ostream_iterator<char> sink{cout};
  format_to(sink, "constexpr std::string_view to_string({} id) {{\n", name);
  format_to(sink, "  switch (id) {{\n");
  for(auto& n : values) {
    format_to(sink, "    case {}::{}: {{ return std::string_view{{\"{}\", {}}}; }}\n", name, n, n, n.size());
  }
  format_to(sink, "  }}\n}}");
}

static void print_enum(string_view name, const vector<string>& values) {
  ostream_iterator<char> sink{cout};
  format_to(sink, "enum {}: uint32_t {{\n", name);
  unsigned count = 0;
  for(auto& n : values) {
    const string_view comma = (count < values.size() - 1) ? "," : "";
    format_to(sink, "  {} = {}{}\n", n, count, comma);
    count++;
  }
  format_to(sink, "}};");
}

static void extract_from_query(string_view enum_name, string_view query_str) {
  uint32_t error_offset;
  TSQueryError error_type;
  TSQuery* query = ts_query_new(tree_sitter_cuda(), query_str.data(),
      uint32_t(query_str.length()), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    string_view from(query_str.data() + error_offset,
        min(size_t(40), query_str.length() - error_offset));
    cerr << "problems with query '" << from << "'..." << endl;
    exit(1);
  }

  uint32_t count = ts_query_capture_count(query);
  vector<string> labels;
  labels.reserve(count);
  auto upper_case = std::views::transform([](char c) {
      return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    });

  for (uint32_t i = 0; i < count; i++) {
    uint32_t name_len = 0;
    const char *name = ts_query_capture_name_for_id(query, i, &name_len);
    string_view name_view{name, name_len};
    auto upper = name_view | upper_case;
    labels.emplace_back(ranges::begin(upper), ranges::end(upper));
  }

  print_enum(enum_name, labels);
  cout << "\n\n";
  print_name_printer(enum_name, labels);
}

constexpr initializer_list<pair<string_view, string_view>> QUERIES = {
  make_pair("QueryCppExcludeNodes", QUERY_CPP_EXCLUDE),
  make_pair("QueryCppIncludeNodes", QUERY_CPP_INCLUDE),
  make_pair("QueryCppNodes", QUERY_CPP)
};

static void print_queries() {
  auto strs = QUERIES | views::keys;
  bool first = true;
  for (const auto& key : strs) {
    if (!first) {
          cerr << "|";
      }
      cerr << key;
      first = false;
  }
}

int main(int argc, const char** argv) {
  if (argc == 2 and std::strncmp(argv[1], "none", 4) == 0) {
    return 0;
  }
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " [";
    print_queries();
    cerr <<  "]" << endl << "but " << argc << " arguments given" << endl;
    exit(1);
  }
  string_view arg{ argv[1]};
  auto it = QUERIES.begin();
  for(; (it != QUERIES.end()) and (arg != it->first); ++it);
  if (it == QUERIES.end()) {
    cerr << "Argument: '" << arg << "' is not [";
    print_queries();
    cerr << "]" << endl;
    exit(1);
  }
  extract_from_query(it->first, it->second);
  return 0;
}
