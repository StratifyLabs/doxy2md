//
// Created by Tyler Gilbert on 3/8/22.
//

#ifndef DOXY2MD_DOXYJSON_HPP
#define DOXY2MD_DOXYJSON_HPP

#include <json/Json.hpp>
#include <printer/MarkdownPrinter.hpp>

#include "IO.hpp"
#include "Options.hpp"

class DoxyJson : public IOAccess {
public:
  using LinkContainer = var::Queue<var::PathString>;

  struct LinkTag {
    LinkContainer link_container;
    var::PathString path;
  };

  using LinkTagContainer = var::Queue<LinkTag>;

  DoxyJson(
    const Options &options,
    const LinkTagContainer &external_link_container)
    : m_options(&options), m_external_link_container(&external_link_container) {
  }

  DoxyJson &process_file(var::StringView path);

  const var::String output() const { return m_printer.output(); }
  const LinkContainer link_container() const { return m_link_container; }

private:
  enum class ProcessMode { add_links, summary, details };

  class StringPrinter : public printer::MarkdownPrinter {
  public:
    void print(var::StringView value) { interface_print_final(value); }
    void clear_output() { m_output = ""; }

  private:
    API_AC(Printer, var::String, output);
    void interface_print_final(var::StringView value) override {
      m_output += value;
    }
  };

  struct ExternalLink {
    var::StringView id;
    var::StringView path;
  };

  static constexpr auto example_hash = "1a834750161cc33b57f6cfa5733fc762b1";
  const Options *m_options = nullptr;
  StringPrinter m_printer;
  ProcessMode m_process_mode = ProcessMode::summary;
  var::PathString m_compound_def_id;
  LinkContainer m_link_container;
  const LinkTagContainer *m_external_link_container;
  API_AC(DoxyJson, var::PathString, relative_output_folder);
  var::PathString m_external_prefix;

  void print(json::JsonValue input) {
    if (input.is_null()) {
      return;
    }
    if (input.is_string()) {
      if (is_add_links()) {
        return;
      }
      print(input.to_string_view());
      return;
    }

    if (input.is_object()) {
      const auto input_object = input.to_object();
      const auto key_container = input_object.get_key_list();
      for (const auto &key : key_container) {
        handle(key, input_object.at(key));
      }
      return;
    }

    if (input.is_array()) {
      const auto input_array = input.to_array();
      for (const auto value : input_array) {
        const auto value_object = value.to_object();
        const auto key_container = value_object.get_key_list();
        for (const auto &key : key_container) {
          handle(key, value_object.at(key));
        }
      }
      return;
    }
    printf("Unhandled Json type %d\n", input.type());
  }

  bool is_internal_link(var::StringView item) const {
    if(is_link_with_hash(item) ){
      const auto link_without_hash = get_link_without_hash(item);
      return link_without_hash == m_compound_def_id.string_view();
    }
    return false;
  }

  static bool is_link_with_hash(var::StringView item) {
    const var::StringView hash = example_hash;
    if (item.length() < hash.length() + 1) {
      return false;
    }
    const auto item_hash = StringView(item).pop_front(item.length() - hash.length());
    if( item_hash.find("_") != StringView::npos ){
      return false;
    }
    return item.pop_back(hash.length()).back() == '_';
  }

  static var::StringView get_link_without_hash(var::StringView item) {
    const var::StringView hash = example_hash;
    API_ASSERT(item.length() > hash.length() + 1);
    return item.pop_back(hash.length() + 1);
  }

  var::StringView get_external_prefix(var::StringView id) const {
    for (const auto &link_tag : *m_external_link_container) {
      for (const auto &link : link_tag.link_container) {
        if (link == id) {
          return link_tag.path;
        }
      }
    }
    return "";
  }

  var::PathString get_link_url(var::StringView id) const {

    const auto is_internal = is_internal_link(id);
    const auto is_external_with_hash = !is_internal && is_link_with_hash(id);

    if (!is_internal) {
      const auto id_without_hash
        = is_external_with_hash ? get_link_without_hash(id) : id;

      const auto external_prefix = get_external_prefix(id);


      if (external_prefix.is_empty()) {
        return id_without_hash & "#" & id;
      }
      // is there a prefix path
      if( id_without_hash == id ){
        return m_external_prefix & external_prefix / id_without_hash;
      }

      return m_external_prefix & external_prefix / id_without_hash & "#" & id;
    }

    return (is_internal_link(id) ? "#" : "") & id;
  }


  var::String get_id(json::JsonArray input) const {
    return String(get_value_as_string_view(input, "@id"));
  }

  var::String get_ref_id(json::JsonArray input) const {
    return String(get_value_as_string_view(input, "@refid"));
  }

  void print(var::StringView value) { m_printer.print(value); }
  void print_inline_code(var::StringView value) {
    if (!value.is_empty()) {
      print("`" | value | "`");
    }
  }

  void print_hyperlink(var::StringView text, var::StringView url) {
    print("[" | text | "](" | url | ")");
  }

  void print_anchor(var::StringView name) {
    if (config().general().is_generate_anchor_headers()) {
      print("###### " | name);
      newline();
      newline();
    }
    if (config().general().is_generate_anchor_html()) {
      print("<a id=\"" | name | "\" />");
      newline();
      newline();
    }
  }

  size_t count_children(json::JsonArray input, var::StringView key) {
    const auto value = get_value(input, key);
    if (value.is_array()) {
      return value.to_array().count();
    }
    return 0;
  }

  json::JsonValue get_value(json::JsonArray input, var::StringView key) const {
    for (const auto &value : input) {
      const auto object = value.to_object();
      const auto key_container = object.get_key_list();
      for (const auto &object_key : key_container) {
        if (object_key == key) {
          return object.at(object_key);
        }
      }
    }
    return json::JsonNull();
  }

  var::StringView
  get_value_as_string_view(json::JsonArray input, var::StringView key) const {
    const auto json_value = get_value(input, key);
    if (json_value.is_string()) {
      return json_value.to_string_view();
    }
    return "";
  }

  void newline() { m_printer.newline(); }

  void process_links(var::StringView name, json::JsonValue input);
  void process_summary(var::StringView name, json::JsonValue input);
  void process_details(var::StringView name, json::JsonValue input);

  bool is_summary() const { return m_process_mode == ProcessMode::summary; }
  bool is_add_links() const { return m_process_mode == ProcessMode::add_links; }

  bool add_links(json::JsonValue input);

  const Configuration &config() const { return m_options->configuration(); }

  void handle(var::StringView name, json::JsonValue input);

  void handle_argsstring(json::JsonValue input);
  void handle_basecompoundref(json::JsonValue input);
  void handle_briefdescription(json::JsonValue input);
  void handle_codeline(json::JsonValue input);
  void handle_compounddef(json::JsonValue input);
  void handle_compoundname(json::JsonValue input);
  void handle_declname(json::JsonValue input);
  void handle_definition(json::JsonValue input);
  void handle_derivedcompoundref(json::JsonValue input);
  void handle_detaileddescription(json::JsonValue input);
  void handle_doxygen(json::JsonValue input);
  void handle_emphasis(json::JsonValue input);
  void handle_enumvalue(json::JsonValue input);
  void handle_highlight(json::JsonValue input);
  void handle_inbodydescription(json::JsonValue input);
  void handle_innerclass(json::JsonValue input);
  void handle_innerfile(json::JsonValue input);
  void handle_itemizedlist(json::JsonValue input);
  void handle_listitem(json::JsonValue input);
  void handle_listofallmembers(json::JsonValue input);
  void handle_location(json::JsonValue input);
  void handle_member(json::JsonValue input);
  void handle_memberdef(json::JsonValue input);
  void handle_name(json::JsonValue input);
  void handle_para(json::JsonValue input);
  void handle_param(json::JsonValue input);
  void handle_programlisting(json::JsonValue input);
  void handle_ref(json::JsonValue input);
  void handle_sectiondef(json::JsonValue input);
  void handle_simplesect(json::JsonValue input);
  void handle_sp(json::JsonValue input);
  void handle_title(json::JsonValue input);
  void handle_type(json::JsonValue input);

  void handle_amp_refid(json::JsonValue input);

  void handle_at_bodyend(json::JsonValue input);
  void handle_at_bodyfile(json::JsonValue input);
  void handle_at_bodystart(json::JsonValue input);
  void handle_at_class(json::JsonValue input);
  void handle_at_column(json::JsonValue input);
  void handle_at_const(json::JsonValue input);
  void handle_at_explicit(json::JsonValue input);
  void handle_at_file(json::JsonValue input);
  void handle_at_id(json::JsonValue input);
  void handle_at_inline(json::JsonValue input);
  void handle_at_kind(json::JsonValue input);
  void handle_at_line(json::JsonValue input);
  void handle_at_prot(json::JsonValue input);
  void handle_at_refid(json::JsonValue input);
  void handle_at_static(json::JsonValue input);
  void handle_at_virt(json::JsonValue input);

  void handle_hashtag_text(json::JsonValue input);
};

#endif // DOXY2MD_DOXYJSON_HPP
