//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef CLI_IO_HPP
#define CLI_IO_HPP

#include <api/api.hpp>
#include <printer/JsonPrinter.hpp>
#include <printer/YamlPrinter.hpp>

class IO {
  API_SINGLETON(IO);
public:

  enum class UsePrinter { yaml, json };

  UsePrinter use_printer = UsePrinter::yaml;
  printer::YamlPrinter yaml_printer;
  printer::JsonPrinter json_printer;
};

class IOAccess : public api::ExecutionContext {
public:
  static printer::Printer &printer() {
    return (IO::instance().use_printer == IO::UsePrinter::yaml)
             ? static_cast<printer::Printer&>(IO::instance().yaml_printer)
             : static_cast<printer::Printer&>(IO::instance().json_printer);
  }

  static void switch_to_json_printer(){
    IO::instance().use_printer = IO::UsePrinter::json;
  }
};

#endif // CLI_IO_HPP
