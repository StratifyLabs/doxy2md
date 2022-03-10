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
  explicit DoxyJson(const Options &options) : m_options(&options) {}

  DoxyJson &process_file(var::StringView path);

  const var::String output() const { return m_printer.output(); }

private:
  const Options *m_options = nullptr;

  enum class ProcessMode { summary, details };

  class StringPrinter : public printer::MarkdownPrinter {
  public:
    void print(var::StringView value) { interface_print_final(value); }

  private:
    API_AC(Printer, var::String, output);
    void interface_print_final(var::StringView value) override {
      m_output += value;
    }
  };

  StringPrinter m_printer;
  ProcessMode m_process_mode = ProcessMode::summary;

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

  void print(json::JsonValue input) {
    if (input.is_null()) {
      return;
    }
    if (input.is_string()) {
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

  var::GeneralString get_link_url(var::StringView id) {
    return "#" | id | config().general().get_link_suffix();
  }

  var::String get_id(json::JsonArray input) {
    return String(get_value_as_string_view(input, "@id"))
      .replace(String::Replace{.new_string = "-", .old_string = "_"});
  }

  var::String get_ref_id(json::JsonArray input) {
    return String(get_value_as_string_view(input, "@refid"))
      .replace(String::Replace{.new_string = "-", .old_string = "_"});
  }

  void print(var::StringView value) { m_printer.print(value); }

  void print_hyperlink(var::StringView text, var::StringView url){
    print("[" | text | "](" | url | ")");
  }

  void print_anchor(var::StringView name) { print("###### " | name); }

  json::JsonValue get_value(json::JsonArray input, var::StringView key) {
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
  get_value_as_string_view(json::JsonArray input, var::StringView key) {
    const auto json_value = get_value(input, key);
    if( json_value.is_string() ){
      return json_value.to_string_view();
    }
    return "";
  }

  void newline() { m_printer.newline(); }

  void process_summary(var::StringView name, json::JsonValue input);
  void process_details(var::StringView name, json::JsonValue input);

  bool is_summary() const { return m_process_mode == ProcessMode::summary; }

  const Configuration &config() { return m_options->configuration(); }
};

#endif // DOXY2MD_DOXYJSON_HPP
