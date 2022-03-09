//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef CLI_IO_HPP
#define CLI_IO_HPP

#include <api/api.hpp>
#include <printer/YamlPrinter.hpp>

class IO {
public:
  API_SINGLETON(IO);
  printer::YamlPrinter printer;
};

class IOAccess : public api::ExecutionContext {
public:
  static printer::YamlPrinter & printer() {
    return IO::instance().printer;
  }

};

#endif // CLI_IO_HPP
