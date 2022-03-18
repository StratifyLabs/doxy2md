//
// Created by Tyler Gilbert on 3/9/22.
//

#include "Configuration.hpp"

Configuration Configuration::make_defaults() {
  Configuration result;

  result.set_basecompoundref(BaseCompoundRef().set_label("- Inherits: "));

  result.set_detaileddescription(DetailedDescription().set_title("Details"));

  result.set_general(General()
                       .set_link_suffix("")
                       .set_generate_anchor_headers(false)
                       .set_generate_anchor_html(true)
                       .set_generate_summary(true)
                       .set_use_relative_paths(true)
                       .set_front_matter(false));

  result.set_innerclass(InnerClass().set_label("- "));

  result.set_input(Input().set_sources(var::Vector<Input::Source>().push_back(
    Input::Source().set_input(".").set_output("."))));

  result.set_listofallmembers(
    ListOfAllMembers().set_title("List of All Members"));

  result.set_memberdef(MemberDef()
                         .set_define("define")
                         .set_enum("enum")
                         .set_friend("friend")
                         .set_function("function")
                         .set_typedef("typedef")
                         .set_variable("variable")
                         .set_type_label("***Type:*** ")
                         .set_parameters_label("***Parameters:***"));

  result.set_programlisting(ProgramListing().set_language("cpp"));

  result.set_sectiondef(
    SectionDef()
      .set_define("Macro Definitions")
      .set_enum("Enumerations")
      .set_friend("Friends")
      .set_func("Functions")
      .set_private_attribute("Private Attributes")
      .set_private_function("Private Functions")
      .set_private_static_attribute("Private Static Attributes")
      .set_private_static_function("Private Static Functions")
      .set_private_type("Private Types")
      .set_protected_attribute("Protected Attributes")
      .set_protected_function("Protected Functions")
      .set_protected_static_function("Protected Static Functions")
      .set_protected_type("Protected Types")
      .set_public_attribute("Attributes")
      .set_public_function("Functions")
      .set_public_static_attribute("Static Attributes")
      .set_public_static_function("Static Functions")
      .set_public_type("Types")
      .set_typedef("Type Definitions"));

  result.set_simplesect(
    SimpleSect().set_return("Returns:").set_see("See Also:"));

  return result;
}