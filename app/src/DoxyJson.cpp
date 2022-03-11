//
// Created by Tyler Gilbert on 3/8/22.
//

#include <fs/DataFile.hpp>
#include <fs/File.hpp>
#include <fs/FileSystem.hpp>
#include <json/JsonDocument.hpp>

#include "DoxyJson.hpp"
#include "macros.hpp"

using namespace json;
using namespace var;
using namespace printer;
using namespace fs;

DoxyJson &DoxyJson::process_file(var::StringView path) {

  if (!FileSystem().exists(path)) {
    API_RETURN_VALUE_ASSIGN_ERROR(*this, path | " does not exist", EEXIST);
  }

  {

    if (relative_output_folder() != ".") {
      const auto path_container
        = relative_output_folder().string_view().split("/");
      for (const auto &path : path_container) {
        m_external_prefix &= "../";
      }
    }
  }

  JsonDocument json_document;
  const auto object = [&]() {
    const auto suffix = Path::suffix(path);
    if (suffix == "json") {
      return json_document.load(File(path)).to_object();
    }

    if (suffix == "xml") {
      DataFile data_file;
      data_file.write(File(path)).seek(0);
      return json_document
        .from_xml_string(
          data_file.data().add_null_terminator(),
          JsonDocument::IsXmlFlat::no)
        .to_object();
    }
    return JsonObject();
  }();

  API_RETURN_VALUE_IF_ERROR(*this);

  printer().object("input", object, Printer::Level::trace);
  if (printer().verbose_level() == Printer::Level::trace) {
    const auto json_path = Path::no_suffix(path) & ".json";
    JsonDocument().save(object, File(File::IsOverwrite::yes, json_path));
  }

  const auto doxygen_object = object.at("doxygen");
  const auto doxygen_name = "doxygen";

  process_links(doxygen_name, doxygen_object);
  {
    Printer::Array pa(printer(), "links", Printer::Level::debug);
    for (const auto &link : m_link_container) {
      printer().info(link);
    }
  }

  if (config().general().is_generate_summary()) {
    Printer::Object po(printer(), "summary");
    process_summary(doxygen_name, doxygen_object);
  }
  API_RETURN_VALUE_IF_ERROR(*this);

  {
    Printer::Object po(printer(), "details");
    process_details(doxygen_name, doxygen_object);
  }

  m_link_container.push(m_compound_def_id);
  return *this;
}

void DoxyJson::process_links(var::StringView name, json::JsonValue input) {
  API_RETURN_IF_ERROR();
  m_process_mode = ProcessMode::add_links;
  handle(name, input);
}

void DoxyJson::process_summary(var::StringView name, json::JsonValue input) {
  API_RETURN_IF_ERROR();
  m_process_mode = ProcessMode::summary;
  handle(name, input);
}

void DoxyJson::process_details(var::StringView name, json::JsonValue input) {
  API_RETURN_IF_ERROR();
  m_process_mode = ProcessMode::details;
  handle(name, input);
}

bool DoxyJson::add_links(json::JsonValue input) {
  if (is_add_links()) {
    const auto id = get_value(input, "@id");
    if (id.is_string()) {
      m_link_container.push(id.to_string_view());
    }
    print(input);
    return true;
  }
  return false;
}

void DoxyJson::handle(var::StringView name, json::JsonValue input) {

  HANDLE(argsstring, input)
  HANDLE(basecompoundref, input)
  HANDLE(briefdescription, input)
  HANDLE(codeline, input)
  HANDLE(compounddef, input)
  HANDLE(compoundname, input)
  HANDLE(declname, input)
  HANDLE(definition, input)
  HANDLE(detaileddescription, input)
  HANDLE(doxygen, input)
  HANDLE(emphasis, input)
  HANDLE(enumvalue, input)
  HANDLE(highlight, input)
  HANDLE(inbodydescription, input)
  HANDLE(innerclass, input)
  HANDLE(innerfile, input)
  HANDLE(itemizedlist, input)
  HANDLE(listitem, input)
  HANDLE(listofallmembers, input)
  HANDLE(location, input)
  HANDLE(member, input)
  HANDLE(memberdef, input)
  HANDLE(name, input)
  HANDLE(para, input)
  HANDLE(param, input)
  HANDLE(programlisting, input)
  HANDLE(ref, input)
  HANDLE(sectiondef, input)
  HANDLE(simplesect, input)
  HANDLE(sp, input)
  HANDLE(title, input)
  HANDLE(type, input)

  HANDLE_AMP(refid, input)

#if 0
  HANDLE_AT(bodyend, input)
  HANDLE_AT(bodyfile, input)
  HANDLE_AT(bodystart, input)
  HANDLE_AT(class, input)
  HANDLE_AT(column, input)
  HANDLE_AT(const, input)
  HANDLE_AT(explicit, input)
  HANDLE_AT(file, input)
  HANDLE_AT(id, input)
  HANDLE_AT(inline, input)
  HANDLE_AT(kind, input)
  HANDLE_AT(line, input)
  HANDLE_AT(prot, input)
  HANDLE_AT(refid, input)
  HANDLE_AT(static, input)
  HANDLE_AT(virt, input)
#endif

  HANDLE_HASHTAG(text, input)

  // printf("Did not handle %s\n", String(name).cstring());
}

void DoxyJson::handle_argsstring(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}
void DoxyJson::handle_basecompoundref(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_briefdescription(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    return;
  }
  if (input.is_null()) {
    return;
  }
  printer().message("insert brief description");
  print(input);
}

void DoxyJson::handle_codeline(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    return;
  }

  printer().message("insert codeline");
  print(input);
  newline();
}

void DoxyJson::handle_compounddef(json::JsonValue input) {
  if (is_add_links()) {
    // don't add this link because it is the current file
    const auto id = get_value(input, "@id");
    if (id.is_string()) {
      m_compound_def_id = id.to_string_view();
    }
    print(input);
    return;
  }

  const auto title = get_value_as_string_view(input, "title");
  const auto kind = get_value_as_string_view(input, "@kind");
  const auto id = get_id(input);

  const auto effective_title = [&]() {
    if (title.is_empty()) {
      const auto compound_name
        = get_value_as_string_view(input, "compoundname");
      return "`" | kind | "` " | compound_name;
    }
    return GeneralString(title);
  }();

  if (is_summary()) {
    printer().message("compounddef summary" | effective_title);
    MarkdownPrinter::Header header(m_printer, effective_title);
    newline();
    newline();
    MarkdownPrinter::Header md_po(m_printer, "Summary");
    newline();
    print(input);
    return;
  }

  if (config().general().is_generate_summary() == false) {
    newline();
    newline();
    printer().message("compounddef " | effective_title);
    MarkdownPrinter::Header header(m_printer, effective_title);
    newline();
    print(input);
  } else {
    MarkdownPrinter::Header md_po(m_printer, "Details");
    newline();
    print(input);
  }
}

void DoxyJson::handle_compoundname(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}
void DoxyJson::handle_declname(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}
void DoxyJson::handle_definition(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);


}
void DoxyJson::handle_derivedcompoundref(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_detaileddescription(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    return;
  }

  if (input.is_null()) {
    return;
  }

  newline();
  const auto title = config().detaileddescription().get_title();
  if (!title.is_empty()) {
    newline();
    MarkdownPrinter::Header header(m_printer, title);
    printer().debug("detaileddescription ");
    newline();
  }
  print(input);
}

void DoxyJson::handle_doxygen(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print(input);
}

void DoxyJson::handle_emphasis(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  printer().debug("emphasis");
  print("*");
  print(input);
  print("*");
}

void DoxyJson::handle_enumvalue(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  const auto name = get_value_as_string_view(input, "name");
  const auto id = get_id(input);

  if (is_summary()) {
    m_printer.hyperlink(name, get_link_url(id));
    return;
  }

  const auto brief = get_value(input, "briefdescription");
  const auto detailed = get_value(input, "detaileddescription");

  if (brief.is_null() && detailed.is_null()) {
    print("- " | name);
    newline();
  } else {
    newline();
    newline();
    MarkdownPrinter::Header header(m_printer, name);
    newline();
    print(input);
  }
}

void DoxyJson::handle_highlight(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  printer().debug("highlight");
  print(input);
}

void DoxyJson::handle_inbodydescription(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print(input);
}
void DoxyJson::handle_innerclass(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);

  if (is_summary() || !config().general().is_generate_summary()) {
    const auto prot = get_value_as_string_view(input, "@prot");
    if (prot == "private" && !config().general().is_show_private()) {
      return;
    }

    if (prot == "protected" && !config().general().is_show_protected()) {
      return;
    }

    const auto refid = get_ref_id(input);
    const auto text = get_value_as_string_view(input, "#text");
    print(config().innerclass().get_label());
    print_hyperlink(text, get_link_url(refid));
    newline();
  }
}
void DoxyJson::handle_innerfile(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}
void DoxyJson::handle_itemizedlist(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    return;
  }
  printer().debug("itemizedlist");
  newline();
  newline();
  MarkdownPrinter::List list(m_printer, MarkdownPrinter::ListType::unordered);
  print(input);
}
void DoxyJson::handle_listitem(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  printer().debug("listitem");
  print("- ");
  print(input);
  newline();
}

void DoxyJson::handle_listofallmembers(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    newline();
    newline();
    if (const auto title = config().listofallmembers().get_title();
        !title.is_empty()) {
      MarkdownPrinter::Header header(m_printer, title);
      newline();
      newline();
      print(input);
    } else {
      print(input);
    }
  }
}

void DoxyJson::handle_location(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print(input);
}

void DoxyJson::handle_member(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  const auto scope = get_value_as_string_view(input, "scope");
  const auto name = get_value_as_string_view(input, "name");
  const auto prot = get_value_as_string_view(input, "@prot");

  if (prot == "private" && !config().general().is_show_private()) {
    return;
  }

  print("- ");
  const auto id = get_ref_id(input);
  print_hyperlink(scope | "::" | name, get_link_url(id));
  newline();
}

void DoxyJson::handle_memberdef(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);

  const auto kind = get_value_as_string_view(input, "@kind");
  const auto id = get_id(input);
  const auto name = get_value_as_string_view(input, "name");
  const auto definition = get_value_as_string_view(input, "definition");
  const auto argsstring = get_value_as_string_view(input, "argsstring");

  const auto show_name = name | (kind == "function" ? "()" : "");

  if (is_summary()) {
    printer().message("memberdef summary " | name);
    m_printer.hyperlink(show_name, get_link_url(id));
    if (kind == "enum") {
      MarkdownPrinter::List list(
        m_printer,
        MarkdownPrinter::ListType::unordered);
      print(input);
    } else {
      print(input);
    }
    return;
  }
  newline();
  newline();
  printer().message("memberdef " | show_name);

  print_anchor(id);
  MarkdownPrinter::Header header(m_printer, show_name);
  newline();

  if (!definition.is_empty()) {
    {
      MarkdownPrinter::Code code(
        m_printer,
        config().programlisting().get_language());
      print(definition | " " | argsstring);
    }
    newline();

    const auto type = get_value(input, "type");

    if (type.is_null()) {
      return;
    }

    print("***Type:*** ");
    if (type.is_string()) {
      print("`" | type.to_string_view() | "`");
    } else {
      print(type);
    }

    const auto parameter_count = count_children(input, "param");
    if (parameter_count > 0) {
      newline();
      newline();
      print("***Parameters:*** ");
    }
    newline();
  }
  print(input);
}

void DoxyJson::handle_name(json::JsonValue input) { HANDLE_ADD_LINKS(input); }

void DoxyJson::handle_para(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print(input);
}

void DoxyJson::handle_param(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  const auto type = get_value(input, "type");
  const auto declname = get_value_as_string_view(input, "declname");

  if (is_summary()) {
    return;
  }

  print("- ");
  if (type.is_string()) {
    print_inline_code(type.to_string_view());
  } else {
    print(type);
  }
  print(" ");
  print_inline_code(declname);

  const auto brief_description = get_value(input, "briefdescription");
  if (!brief_description.is_null()) {
    print(": ");
    print(input);
  }
  newline();
}

void DoxyJson::handle_programlisting(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    return;
  }
  newline();
  newline();
  print("```");
  print(m_options->configuration().programlisting().get_language());
  newline();
  print(input);
  print("```");
  newline();
}

void DoxyJson::handle_ref(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print_hyperlink(
    get_value_as_string_view(input.to_array(), "#text"),
    get_link_url(get_value_as_string_view(input.to_array(), "@refid")));
}

void DoxyJson::handle_sectiondef(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  const auto kind_string = get_value_as_string_view(input.to_array(), "@kind");
  if (
    Configuration::SectionDef::is_private(kind_string)
    && !config().general().is_show_private()) {
    printer().debug("skip private section " | kind_string);
    return;
  }

  const auto kind = [&]() {
    const auto result = config().sectiondef().to_object().at(kind_string);
    if (result.is_valid()) {
      return GeneralString(result.to_cstring());
    }
    return "Unknown SectionDef:" | kind_string;
  }();

  newline();
  newline();
  MarkdownPrinter::Header section_header(m_printer, kind);
  newline();
  if (is_summary()) {
    MarkdownPrinter::List list(m_printer, MarkdownPrinter::ListType::unordered);
    print(input);
  } else {
    print(input);
  }
}

void DoxyJson::handle_simplesect(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  if (is_summary()) {
    return;
  }
  newline();
  newline();
  print("***");
  const auto label_value = config().simplesect().to_object().at(
    get_value_as_string_view(input.to_array(), "@kind"));
  if (label_value.is_string()) {
    print(label_value.to_string_view());
  }
  print("***");
  newline();
  newline();
  print(input);
  newline();
}

void DoxyJson::handle_sp(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print(" ");
  print(input);
}

void DoxyJson::handle_title(json::JsonValue input) { HANDLE_ADD_LINKS(input); }

void DoxyJson::handle_type(json::JsonValue input) { HANDLE_ADD_LINKS(input); }

void DoxyJson::handle_amp_refid(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_bodyend(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_bodyfile(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_bodystart(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_class(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_column(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_const(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_explicit(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_file(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_id(json::JsonValue input) { HANDLE_ADD_LINKS(input); }

void DoxyJson::handle_at_inline(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_kind(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_line(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_prot(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_refid(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_static(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_at_virt(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
}

void DoxyJson::handle_hashtag_text(json::JsonValue input) {
  HANDLE_ADD_LINKS(input);
  print(input);
}
