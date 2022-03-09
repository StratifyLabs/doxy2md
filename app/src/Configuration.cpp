//
// Created by Tyler Gilbert on 3/9/22.
//

#include "Configuration.hpp"

Configuration Configuration::make_defaults() {
  Configuration result;
  result.set_detaileddescription(DetailedDescription().set_title("Details"));

  result.set_general(General()
                       .set_link_suffix(".md")
                       .set_generate_summary(true)
                       .set_front_matter(false));

  result.set_listofallmembers(
    ListOfAllMembers().set_title("List of All Members"));

  result.set_memberdef(MemberDef().set_function("function"));

  result.set_programlisting(ProgramListing().set_language("cpp"));

  result.set_sectiondef(SectionDef()
                          .set_func("Functions")
                          .set_typedef("Type Definitions")
                          .set_enum("Enumerations")
                          .set_define("Macro Definitions")
                          .set_public_attribute("Members")
                          .set_private_attribute("Private Members")
                          .set_public_function("Functions")
                          .set_private_function("Private Functions")
                          .set_protected_function("Protected Functions")
                          .set_protected_attribute("Protected Attributes")
                          .set_public_static_function("Static Functions")
                          .set_private_static_function("Private Static Functions")
  );

  result.set_simplesect(SimpleSect().set_return("Returns:"));

  return result;
}