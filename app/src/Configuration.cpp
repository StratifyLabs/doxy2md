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
  result.set_memberdef(MemberDef().set_function("function"));
  result.set_programlisting(ProgramListing().set_language("cpp"));
  result.set_sectiondef(SectionDef()
                          .set_func("Functions")
                          .set_typedef("Type Definitions")
                          .set_enum("Enumerations")
                          .set_define("Macro Definitions"));
  result.set_simplesect(SimpleSect().set_return("Returns:"));

  return result;
}