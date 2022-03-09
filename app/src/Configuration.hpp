//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef CLI_CONFIGURATION_HPP
#define CLI_CONFIGURATION_HPP

#include <json/Json.hpp>

class Configuration : public json::JsonValue {
public:
  JSON_ACCESS_CONSTRUCT_OBJECT(Configuration);

  class DetailedDescription : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(DetailedDescription);
    JSON_ACCESS_STRING(DetailedDescription,title);
  };

  class General : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(General);
    JSON_ACCESS_STRING_WITH_KEY(General,linkSuffix,link_suffix);
    JSON_ACCESS_BOOL_WITH_KEY(General,generateSummary,generate_summary);
    JSON_ACCESS_BOOL_WITH_KEY(General,frontMatter,front_matter);
    JSON_ACCESS_BOOL_WITH_KEY(General,showPrivate,show_private);
    JSON_ACCESS_BOOL_WITH_KEY(General,showProtected,show_protected);
  };

  class ListOfAllMembers : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(ListOfAllMembers);
    JSON_ACCESS_STRING(ListOfAllMembers,title);
  };

  class MemberDef : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(MemberDef);
    JSON_ACCESS_STRING(MemberDef,function);
  };

  class ProgramListing : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(ProgramListing);
    JSON_ACCESS_STRING(ProgramListing,language);
  };

  class SectionDef : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(SectionDef);
    JSON_ACCESS_STRING(SectionDef,func);
    JSON_ACCESS_STRING(SectionDef,typedef);
    JSON_ACCESS_STRING(SectionDef,define);
    JSON_ACCESS_STRING(SectionDef,enum);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,public-attrib,public_attribute);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,public-func,public_function);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,private-attrib,private_attribute);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,private-func,private_function);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,protected-func,protected_function);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,private-static-func,private_static_function);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,public-static-func,public_static_function);
    JSON_ACCESS_STRING_WITH_KEY(SectionDef,protected-attrib,protected_attribute);

    static bool is_private(var::StringView kind){
      return kind.find("private") != var::StringView::npos;
    }

    static bool is_proteted(var::StringView kind){
      return kind.find("protected") != var::StringView::npos;
    }
  };

  class SimpleSect : public json::JsonValue {
  public:
    JSON_ACCESS_CONSTRUCT_OBJECT(SimpleSect);
    JSON_ACCESS_STRING(SimpleSect,return);
  };

  JSON_ACCESS_OBJECT(Configuration,DetailedDescription,detaileddescription);
  JSON_ACCESS_OBJECT(Configuration,General,general);
  JSON_ACCESS_OBJECT(Configuration,ListOfAllMembers,listofallmembers);
  JSON_ACCESS_OBJECT(Configuration,MemberDef,memberdef);
  JSON_ACCESS_OBJECT(Configuration,ProgramListing,programlisting);
  JSON_ACCESS_OBJECT(Configuration,SectionDef,sectiondef);
  JSON_ACCESS_OBJECT(Configuration,SimpleSect,simplesect);

  static Configuration make_defaults();

};

#endif // CLI_CONFIGURATION_HPP
