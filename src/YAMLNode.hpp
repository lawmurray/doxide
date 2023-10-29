#pragma once

#include "doxide.hpp"

/**
 * Node in a YAML document.
 */
class YAMLNode {
public:
  using key_type = std::string;
  using value_type = std::string;
  using sequence_type = std::list<std::shared_ptr<YAMLNode>>;
  using mapping_type = std::unordered_map<key_type,std::shared_ptr<YAMLNode>>;
  using variant_type = std::variant<value_type,sequence_type,mapping_type>;

  /**
   * Does this have a given key?
   */
  bool has(const key_type& key) const;

  /**
   * Is this a value?
   */
  bool isValue() const;

  /**
   * Is there a value at a given path?
   * 
   * @param key Key.
   * @param keys... Keys.
   */
  template<class... Args>
  bool isValue(const key_type& key, Args&&... keys) const {
    return has(key) &&
        mapping().at(key)->isValue(std::forward<Args>(keys)...);
  }

  /**
   * Is this a sequence?
   */
  bool isSequence() const;

  /**
   * Is there a sequence at a given path?
   * 
   * @param key Key.
   * @param keys... Keys.
   */
  template<class... Args>
  bool isSequence(const key_type& key, Args&&... keys) const {
    return has(key) &&
        mapping().at(key)->isSequence(std::forward<Args>(keys)...);
  }

  /**
   * Is this a mapping?
   */
  bool isMapping() const;

  /**
   * Is there a mapping at a given path?
   * 
   * @param key Key.
   * @param keys... Keys.
   */
  template<class... Args>
  bool isMapping(const key_type& key, Args&&... keys) const {
    return has(key) &&
        mapping().at(key)->isMapping(std::forward<Args>(keys)...);
  }

  /**
   * Get this as a value.
   */
  value_type& value();

  /**
   * Get this as a value.
   */
  const value_type& value() const;

  /**
   * Get the value at a given path.
   * 
   * @param key Key.
   * @param keys... Keys.
   * 
   * @return The value.
   */
  template<class... Args>
  value_type& value(const key_type& key, Args&&... keys) {
    return mapping().at(key)->value(std::forward<Args>(keys)...);
  }

  /**
   * Get the value at a given path.
   * 
   * @param key Key.
   * @param keys... Keys.
   * 
   * @return The value.
   */
  template<class... Args>
  const value_type& value(const key_type& key, Args&&... keys) const {
    return mapping().at(key)->value(std::forward<Args>(keys)...);
  }

  /**
   * Get this as a sequence.
   */
  sequence_type& sequence();

  /**
   * Get this as a sequence.
   */
  const sequence_type& sequence() const;

  /**
   * Get the sequence at a given path.
   * 
   * @param key Key.
   * @param keys... Keys.
   * 
   * @return The sequence.
   */
  template<class... Args>
  sequence_type& sequence(const key_type& key, Args&&... keys) {
    return mapping().at(key)->sequence(std::forward<Args>(keys)...);
  }

  /**
   * Get the sequence at a given path.
   * 
   * @param key Key.
   * @param keys... Keys.
   * 
   * @return The sequence.
   */
  template<class... Args>
  const sequence_type& sequence(const key_type& key, Args&&... keys) const {
    return mapping().at(key)->sequence(std::forward<Args>(keys)...);
  }

  /**
   * Get this as a mapping.
   */
  mapping_type& mapping();

  /**
   * Get this as a mapping.
   */
  const mapping_type& mapping() const;

  /**
   * Get the mapping at a given path.
   * 
   * @param key Key.
   * @param keys... Keys.
   * 
   * @return The mapping.
   */
  template<class... Args>
  mapping_type& mapping(const key_type& key, Args&&... keys) {
    return mapping().at(key)->mapping(std::forward<Args>(keys)...);
  }

  /**
   * Get the mapping at a given path.
   * 
   * @param key Key.
   * @param keys... Keys.
   * 
   * @return The mapping.
   */
  template<class... Args>
  const mapping_type& mapping(const key_type& key, Args&&... keys) const {
    return mapping().at(key)->mapping(std::forward<Args>(keys)...);
  }

  /**
   * Set the value.
   */
  void set(const variant_type& contents);

  /**
   * Set as a sequence.
   */
  void setSequence();

  /**
   * Set as a mapping.
   */
  void setMapping();

  /**
   * For a sequence, push an uninitialized node onto the end and return it.
   */
  YAMLNode& push();

  /**
   * For a mapping, insert a key with an uninitialized value and return the
   * value.
   */
  YAMLNode& insert(const key_type& key);

private:
  /**
   * Contents.
   */
  variant_type contents;
};
