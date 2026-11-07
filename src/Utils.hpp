#pragma once

#include <algorithm>
#include <coroutine>
#include <cctype>
#include <filesystem>
#include <format>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <utility>

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

/**
 * Convert a string to lowercase with uppercase first character.
 *
 * @ingroup developer
 */
std::string capitalize(const std::string_view s);

bool can_write(const std::filesystem::path& path);

struct UpperStr {
    std::string_view view;
};

template <> struct std::formatter<UpperStr> : std::formatter<std::string_view> {
    auto format(UpperStr wrapper, format_context& ctx) const {
        // Convert to uppercase on the fly into a temporary buffer
        std::string upper;
        upper.reserve(wrapper.view.size());
        for (char c : wrapper.view) {
            upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
        }
        // Forward the temporary string to the base string_view formatter
        return std::formatter<std::string_view>::format(upper, ctx);
    }
};

struct RepeatChar {
    int count;
    char ch = ' ';
};

template <> struct std::formatter<RepeatChar> {

    // Parses format specifiers if needed (e.g., if you wrote {:x}).
    // We don't need specifiers here, so we just return the iterator.
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    // This writes the output to the formatting buffer
    auto format(const RepeatChar& rc, std::format_context& ctx) const {
        // Efficiently write the character 'rc.count' times directly into the output
        return std::fill_n(ctx.out(), rc.count, rc.ch);
    }
};

struct het_string_hash {
    using is_transparent = void; // Enables heterogeneous lookup

    constexpr std::size_t operator()(const std::string_view& sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }

    constexpr std::size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string>{}(s);
    }
};

template<typename RetType> struct CoroIterGenerator {
    struct promise_type {
        std::optional<RetType> current_value;

        CoroIterGenerator get_return_object() {
            return CoroIterGenerator{handle_type::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { throw; }
        std::suspend_always yield_value(const RetType& value) noexcept {
            current_value.emplace(value);
            return {};
        }
        std::suspend_always yield_value(RetType&& value) noexcept {
            current_value.emplace(std::move(value));
            return {};
        }
        void return_void() {}
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type h;

    CoroIterGenerator(handle_type h) : h(h) {}
    ~CoroIterGenerator() { if (h) h.destroy(); }

    // Iterator interface to support range-based for-loops over the generator
    struct iterator {
        handle_type h;
        bool done;
        void operator++() { h.resume(); done = h.done(); }
        RetType& operator*() { return h.promise().current_value.value(); }
        bool operator!=(const iterator& other) const { return done != other.done; }
    };

    iterator begin() { h.resume(); return iterator{h, h.done()}; }
    iterator end() { return iterator{h, true}; }
};

template<typename T> concept has_pre_yield = requires (T& c) {
  c.make_yield_pre(std::declval<const typename T::stack_value&>());
  requires std::same_as<typename T::yield_value, std::decay_t<decltype(
    c.make_yield_pre(std::declval<const typename T::stack_value&>()))>>;
};

template<typename T> concept has_post_yield = requires (T& c) {
  c.make_yield_post(std::declval<const typename T::stack_value&>());
  requires std::same_as<typename T::yield_value, std::decay_t<decltype(
    c.make_yield_post(std::declval<const typename T::stack_value&>()))>>;
};

enum class DfsTraversalType {
  PRE,
  POST,
  BOTH
};

template<typename T, DfsTraversalType TRAVERSAL_TYPE> concept dfs_stack_controller = requires (T& c) {
  typename T::yield_value;
  typename T::stack_value;
  c.make_init();
  requires std::same_as<typename T::stack_value, std::decay_t<decltype(c.make_init())>>;

  requires not (TRAVERSAL_TYPE == DfsTraversalType::PRE or TRAVERSAL_TYPE == DfsTraversalType::BOTH) or has_pre_yield<T>;

  requires not (TRAVERSAL_TYPE == DfsTraversalType::POST or TRAVERSAL_TYPE == DfsTraversalType::BOTH) or has_post_yield<T>;

  c.make_next(std::declval<typename T::stack_value&>());
  requires std::same_as<typename T::stack_value, std::decay_t<decltype(c.make_next(std::declval<typename T::stack_value&>()))>>;

  { c.has_next(std::declval<const typename T::stack_value&>()) } -> std::convertible_to<bool>;

  c.after_pop(std::declval<const typename T::stack_value&>());
};

// stack_controller_t handles potential global state to be modified during stack manipulation;
// if there is no global state, it can just re-wire the calls to the object that is on the stack, like
//
// bool has_next(const stack_t& top) { return top.hash_next(); }
//
// stack_t make_next(stack_t& top) { return top.make_next(); }
//
// void after_pop(stack_t&) {} // do nothing
//
// and so on.
//
// Instead, make_yield_pre() transforms the stack top into something else, in order to co_yield to the user
// a "view" of state of the stack (and, if needed, of the controller), e.g., to expose only certain info.
// If this is not needed, simply re-wire like:
//
// const stack_t& make_yield_pre(const stack_t& top) { return top; }
//
template<DfsTraversalType TRAVERSAL_TYPE, typename stack_controller_t>
requires dfs_stack_controller<stack_controller_t, TRAVERSAL_TYPE>
CoroIterGenerator<typename stack_controller_t::yield_value> dag_dfs_traverse(
  stack_controller_t& controller) {
  using stack_t = typename stack_controller_t::stack_value;
  constexpr bool pre_traversal = TRAVERSAL_TYPE == DfsTraversalType::PRE
    or TRAVERSAL_TYPE == DfsTraversalType::BOTH;
  constexpr bool post_traversal = TRAVERSAL_TYPE == DfsTraversalType::POST
    or TRAVERSAL_TYPE == DfsTraversalType::BOTH;

  std::stack<stack_t> stack;

  stack.push(controller.make_init());
  if constexpr (pre_traversal) {
    // pre-order traversal: push and visit immediately
    co_yield controller.make_yield_pre(stack.top());
  }

  while (not stack.empty()) {
    stack_t& current_frame = stack.top();
    // query if the current node has unvisited children left
    if (controller.has_next(current_frame)) {
      // push the new child to the top of the stack to process it on the next loop iteration (DFS)
      stack.push(controller.make_next(current_frame));
      if constexpr (pre_traversal) {
        // pre-order traversal: push and visit immediately
        co_yield controller.make_yield_pre(stack.top());
      }
    } else {
      if constexpr (post_traversal) {
        // post-order traversal: visit and then pop
        co_yield controller.make_yield_post(stack.top());
      }
      // node processing finished, safely clear it out of the stack
      stack.pop();
      if (not stack.empty()) {
        controller.after_pop(stack.top());
      }
    }
  }
}

template<typename stack_controller_t>
decltype(auto) dag_dfs_traverse(stack_controller_t& controller) {
  return dag_dfs_traverse<DfsTraversalType::PRE, stack_controller_t>(controller);
}
