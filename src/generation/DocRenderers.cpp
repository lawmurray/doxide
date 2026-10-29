#include "DocRenderers.hpp"

#include <Regex.hpp>
#include <Sink.hpp>
#include "entities/ConceptEntity.hpp"
#include <entities/Entities.hpp>
#include <entities/EntityRegistry.hpp>
#include "entities/EnumEntity.hpp"
#include "entities/FunctionEntity.hpp"
#include <entities/FwdRef.hpp>
#include "entities/GroupEntity.hpp"
#include "entities/MacroEntity.hpp"
#include <entities/NamespaceEntity.hpp>
#include "entities/OperatorEntity.hpp"
#include <entities/RefVariant.hpp>
#include "entities/TypedefEntity.hpp"
#include <entities/TypeEntity.hpp>
#include "entities/VariableEntity.hpp"
#include "generation/DocRenderer.hpp"

#include "Sanitize.hpp"

#include <format>
#include <optional>
#include <regex>
#include <vector>

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

  void print_indent_line() {
    sink.println("    ");
  }

  void print_indent_line(std::string_view s) {
    static const std::regex start("\\n", REGEX_FLAGS);
    sink.print("    ");
    std::regex_replace(sink.iter(), s.begin(), s.end(), start, "\n    ");
  }

  template<entity U> void print_detail(const FwdRef<U>& e, std::string_view name) {
    sink.println("!!! {} \"{}\"\n", name, htmlize(line(e->get_decl())));
  }

  void print_detail(const FwdRef<TypedefEntity>& e) {
    print_detail(e, "typedef");
  }

  void print_paragraph_end() {}

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

  void print_indent_line() {
    sink.println("> ");
  }

  void print_indent_line(std::string_view s) {
    sink.print("{}", s);
  }

  template<entity U> void print_detail(const FwdRef<U>& e, std::string_view name) {
    sink.println("::: {}\n**`{}`**\n", name, line(e->get_decl()));
  }

  void print_detail(const FwdRef<TypedefEntity>& e) {
    print_detail(e, "typedef");
  }

  void print_paragraph_end() {
    sink.println(":::");
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

  void print_indent_line() {
    sink.println("> ");
  }

  void print_indent_line(std::string_view s) {
    static const std::regex start("\\n", REGEX_FLAGS);
    sink.print("> ");
    std::regex_replace(sink.iter(), s.begin(), s.end(), start, "\n> ");
  }

  template<entity U> void _print_detail(const FwdRef<U>& e) {
    sink.println("> **`{}`**", line(e->get_decl()));
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
    void print_md_section_link(const std::vector<FwdRef<E>>& vec) {
    for (const auto&  e: vec) { printer.print(e); }
  }

  void render_groups(const std::vector<FwdRef<GroupEntity>>& groups) override {
    print_md_section_link(groups);
  }

  void render_ns(const std::vector<FwdRef<NamespaceEntity>>& ns) override {
    print_md_section_link(ns);
  }

  void print_coverage(std::string_view dir) override {
    printer.print_coverage(dir);
  }

  void render_enums(const std::vector<FwdRef<EnumEntity>>& ns) override {
    print_md_section_link(ns);
  }

  template<entity E>
    void print_html_details(const std::vector<FwdRef<E>>& vec, std::string_view caption) {
    sink.println("## {} Details\n", caption);
    for (const auto& e: vec) {
      sink.println("### {}<a name=\"{}\"></a>\n", e->get_name(), sanitize(e->get_name()));
      printer.print_detail(e);
      printer.print_indent_line();
      printer.print_indent_line(e->get_docs());
      printer.print_paragraph_end();
      sink.println();
    }
  }

  void render_details(const std::vector<FwdRef<TypedefEntity>>& td) override {
    print_html_details(td, "Type Alias");
  }

  void render_details(const std::vector<FwdRef<ConceptEntity>>& td) override {
    print_html_details(td, "Concept");
  }

  void render_details(const std::vector<FwdRef<MacroEntity>>& td) override {
    print_html_details(td, "Macro");
  }

  void render_details(const std::vector<FwdRef<VariableEntity>>& td) override {
    print_html_details(td, "Variable");
  }

  template<entity E>
    void print_html_details_first_only(const std::vector<FwdRef<E>>& vec, std::string_view caption) {
    sink.println("## {} Details\n", caption);
    std::string_view prev = "";
    for (const auto& e: vec) {
      std::string_view name = e->get_name();
      if (name != prev) {
        sink.println("### {}<a name=\"{}\"></a>\n", e->get_name(), sanitize(e->get_name()));
      }
      printer.print_detail(e);
      printer.print_indent_line();
      printer.print_indent_line(e->get_docs());
      printer.print_paragraph_end();
      sink.println("\n");
      prev = name;
    }
  }

  void render_details(const std::vector<FwdRef<OperatorEntity>>& td) override {
    print_html_details_first_only(td, "Operator");
  }

  void render_details(const std::vector<FwdRef<FunctionEntity>>& td) override {
    print_html_details_first_only(td, "Function");
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
