#pragma once

#include "CodeEntity.hpp"
#include "ContainerBases.hpp"

#include <cstdint>
#include <string>
#include <string_view>

class NamespaceEntity: public CodeEntity, public TypeContainer, public VariableContainer, public FunctionContainer, public EnumContainer, public NamespaceContainer, public ConceptContainer, public OperatorContainer, public TypedefContainer, public MacroContainer {
public:

  constexpr NamespaceEntity(std::string_view decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, const Doc& docs, bool visible, std::string_view full_name):
    CodeEntity(decl, name_coords, file, start_line, end_line, docs, visible),
    _full_name(full_name)
    {}

  constexpr NamespaceEntity(std::string_view decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, Doc&& docs, bool visible, std::string_view full_name):
    CodeEntity(decl, name_coords, file, start_line, end_line, std::move(docs), visible),
    _full_name(full_name)
    {}

    constexpr NamespaceEntity(std::string_view decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, Doc&& docs, bool visible, std::string&& full_name):
      CodeEntity(decl, name_coords, file, start_line, end_line, std::move(docs), visible),
      _full_name(std::move(full_name))
      {}

  constexpr NamespaceEntity(std::string&& decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, const Doc& docs, bool visible, std::string_view full_name):
    CodeEntity(std::move(decl), name_coords, file, start_line, end_line, docs, visible),
    _full_name(full_name)
    {}

  constexpr NamespaceEntity(std::string&& decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, Doc&& docs, bool visible, std::string_view full_name):
    CodeEntity(std::move(decl), name_coords, file, start_line, end_line, std::move(docs), visible),
    _full_name(full_name)
    {}

  constexpr std::string_view get_full_name() const noexcept { return _full_name; }

private:
  std::string _full_name;
};
