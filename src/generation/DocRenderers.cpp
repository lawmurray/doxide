#include "DocRenderers.hpp"

#include "Sanitize.hpp"
#include <Regex.hpp>
#include <Sink.hpp>
#include "entities/ConceptEntity.hpp"
#include <entities/Entities.hpp>
#include "entities/EnumEntity.hpp"
#include <entities/FunctionEntity.hpp>
#include <entities/FwdRef.hpp>
#include <entities/GroupEntity.hpp>
#include <entities/MacroEntity.hpp>
#include <entities/NamespaceEntity.hpp>
#include <entities/OperatorEntity.hpp>
#include <entities/TypedefEntity.hpp>
#include <entities/VariableEntity.hpp>
#include <generation/DocRenderer.hpp>

#include <optional>
#include <regex>
#include <span>
#include <string_view>

struct MkdDocRender {
  sink_t& sink;

  template<entity U> void print_link_section(const FwdRef<U>& e, std::string_view section) {
    sink.println(":{}: [{}]({}/)\n:   {}\n", section, get_title(e),
      sanitize(e->get_name()), line(brief(e)));
  }

  void print(const FwdRef<GroupEntity>& e) {
    print_link_section(e, "material-format-section");
  }

  void print(const FwdRef<NamespaceEntity>& e) {
    print_link_section(e, "material-package");
  }

  void print_coverage(std::string_view dir) {
    sink.println(":material-chart-pie: [Code Coverage]({}/)\n:    \n", dir);
  }

  void print(const FwdRef<EnumEntity>& e) {
    sink.println("`{}`\n:   {}\n", e->get_decl(), e->get_docs());
  }

  void print_indent_line(std::string_view s) {
    static const std::regex start("\\n", REGEX_FLAGS);
    sink.print("    ");
    std::regex_replace(sink.iter(), s.begin(), s.end(), start, "\n    ");
    sink.println();
  }

  static constexpr std::string_view NEWLINE_INDENT = "\n    ";
  static constexpr std::string_view INDENT = NEWLINE_INDENT.substr(1);
  template<entity U> void print_detail(const FwdRef<U>& e, std::string_view name) {
    static const std::regex start("\\n", REGEX_FLAGS);
    sink.print("!!! {1} \"{2}\"\n{0}\n{0}",
      INDENT, name, htmlize(line(e->get_decl())));
    std::string_view docs = e->get_docs();
    std::regex_replace(sink.iter(), docs.begin(), docs.end(), start, NEWLINE_INDENT.cbegin());
    sink.println("\n{}\n", INDENT);
  }

  void print_detail(const FwdRef<TypedefEntity>& e) {
    print_detail(e, "typedef");
  }

  void print_detail(const FwdRef<ConceptEntity>& e) {
    print_detail(e, "concept");
  }

  void print_detail(const FwdRef<MacroEntity>& e) {
    print_detail(e, "macro");
  }

  void print_detail(const FwdRef<VariableEntity>& e) {
    print_detail(e, "variable");
  }

  void print_detail(const FwdRef<OperatorEntity>& e) {
    print_detail(e, "function");
  }

  void print_detail(const FwdRef<FunctionEntity>& e) {
    print_detail(e, "function");
  }
};

struct PandocRender {
  sink_t& sink;

  template<entity U> void print_link_section(const FwdRef<U>& e) {
    sink.println("[{}]({}/)\n:   {}", get_title(e),
      sanitize(e->get_name()), line(brief(e)));
  }

  void print(const FwdRef<GroupEntity>& e) {
    print_link_section(e);
  }

  void print(const FwdRef<NamespaceEntity>& e) {
    print_link_section(e);
  }

  void print_coverage(std::string_view dir) {
    sink.println("[Code Coverage]({}/)\n:    \n", dir);
  }

  void print(const FwdRef<EnumEntity>& e) {
    sink.println("`{}`\n:   {}\n", e->get_decl(), e->get_docs());
  }

  static constexpr std::string_view INDENT = "> ";
  static constexpr std::string_view PAR_DEL = ":::";
  template<entity U> void print_detail(const FwdRef<U>& e, std::string_view name) {
    sink.println("{0} {2}\n**`{3}`**\n{1}\n{1}{4}\n{1}\n{0}\n",
      PAR_DEL, INDENT, name, line(e->get_decl()), e->get_docs());
  }

  void print_detail(const FwdRef<TypedefEntity>& e) {
    print_detail(e, "typedef");
  }

  void print_detail(const FwdRef<ConceptEntity>& e) {
    print_detail(e, "concept");
  }

  void print_detail(const FwdRef<MacroEntity>& e) {
    print_detail(e, "macro");
  }

  void print_detail(const FwdRef<VariableEntity>& e) {
    print_detail(e, "variable");
  }

  void print_detail(const FwdRef<OperatorEntity>& e) {
    print_detail(e, "function");
  }

  void print_detail(const FwdRef<FunctionEntity>& e) {
    print_detail(e, "function");
  }
};

struct PlainRender {
  sink_t& sink;

  void print(const FwdRef<GroupEntity>& e) {
    sink.println("  - [{}]({}/)\n    {}\n", get_title(e),
      sanitize(e->get_name()), line(brief(e)));
  }

  void print(const FwdRef<NamespaceEntity>& e) {
    sink.println("  - [{}]({}/)\n    {}\n", e->get_name(),
      sanitize(e->get_name()), line(brief(e)));
  }

  void print_coverage(std::string_view dir) {
    sink.println("  - [Code Coverage]({}/)\n    \n", dir);
  }

  void print(const FwdRef<EnumEntity>& e) {
    sink.println("  - `{}`\n    {}\n", e->get_decl(), e->get_docs());
  }

  static constexpr std::string_view NEWLINE_INDENT = "\n> ";
  static constexpr std::string_view INDENT = NEWLINE_INDENT.substr(1);
  template<entity U> void _print_detail(const FwdRef<U>& e) {
    static const std::regex start("\\n", REGEX_FLAGS);
    sink.println("{0}**`{1}`**\n{0}\n{0}", INDENT, line(e->get_decl()));
    std::string_view docs = e->get_docs();
    std::regex_replace(sink.iter(), docs.begin(), docs.end(), start, NEWLINE_INDENT.cbegin());
    sink.println("\n{0}\n", INDENT);
  }

  void print_detail(const FwdRef<TypedefEntity>& e) {
    _print_detail(e);
  }

  void print_paragraph_end() {
  }

  void print_detail(const FwdRef<ConceptEntity>& e) {
    _print_detail(e);
  }

  void print_detail(const FwdRef<MacroEntity>& e) {
    _print_detail(e);
  }

  void print_detail(const FwdRef<VariableEntity>& e) {
    _print_detail(e);
  }

  void print_detail(const FwdRef<OperatorEntity>& e) {
    _print_detail(e);
  }

  void print_detail(const FwdRef<FunctionEntity>& e) {
    _print_detail(e);
  }
};

template<typename Printer> class Renderer: public DocRenderer {
public:

  Renderer(sink_t& _sink): sink{_sink}, printer{_sink} {}

  void set_sink(sink_t& _sink) {
    sink = _sink;
    printer.sink = _sink;
  }

  template<entity E>
    void print_md_section_link(const std::span<const FwdRef<E>>& vec) {
    for (const auto&  e: vec) { printer.print(e); }
  }

  void render_groups(std::span<const FwdRef<GroupEntity>> groups) override {
    print_md_section_link(groups);
  }

  void render_ns(std::span<const FwdRef<NamespaceEntity>> ns) override {
    print_md_section_link(ns);
  }

  void print_coverage(std::string_view dir) override {
    printer.print_coverage(dir);
  }

  void render_enums(std::span<const FwdRef<EnumEntity>> ns) override {
    print_md_section_link(ns);
  }

  template<bool FIRST_ONLY, entity E>
    void print_html_details(const std::span<const FwdRef<E>>& vec, std::string_view caption) {
    sink.println("## {} Details\n", caption);
    std::string_view prev = "";
    for (const auto& e: vec) {
      std::string_view name = e->get_name();
      if constexpr (FIRST_ONLY) {
        if (name != prev) {
          sink.println("### {}<a name=\"{}\"></a>\n", name, sanitize(name));
        }
      } else {
        sink.println("### {}<a name=\"{}\"></a>\n", name, sanitize(name));
      }
      printer.print_detail(e);
      prev = name;
    }
  }

  void render_details(std::span<const FwdRef<TypedefEntity>> td) override {
    print_html_details<false>(td, "Type Alias");
  }

  void render_details(std::span<const FwdRef<ConceptEntity>> td) override {
    print_html_details<false>(td, "Concept");
  }

  void render_details(std::span<const FwdRef<MacroEntity>> td) override {
    print_html_details<true>(td, "Macro");
  }

  void render_details(std::span<const FwdRef<VariableEntity>> td) override {
    print_html_details<true>(td, "Variable");
  }

  void render_details(std::span<const FwdRef<OperatorEntity>> td) override {
    print_html_details<true>(td, "Operator");
  }

  void render_details(std::span<const FwdRef<FunctionEntity>> td) override {
    print_html_details<true>(td, "Function");
  }

private:
  sink_t& sink;
  Printer printer;
};

template<typename Printer> class FactoryImpl: public DocRendererFactory {
public:

  virtual DocRenderer& get_renderer(sink_t& sink) override {
    if (renderer) {
      renderer->set_sink(sink);
    } else {
      renderer.emplace(sink);
    }
    return *renderer;
  }

private:
  std::optional<Renderer<Printer>> renderer;
};


std::unique_ptr<DocRendererFactory> make_doc_renderer(StyleOption style) {
  switch (style) {
    case StyleOption::MKDOCS: {
      return std::unique_ptr<DocRendererFactory>(new FactoryImpl<MkdDocRender>());
    }
    case StyleOption::PANDOCS: {
      return std::unique_ptr<DocRendererFactory>(new FactoryImpl<PandocRender>());
    }
    case StyleOption::PLAIN: {
      return std::unique_ptr<DocRendererFactory>(new FactoryImpl<PlainRender>());
    }
  }
}
