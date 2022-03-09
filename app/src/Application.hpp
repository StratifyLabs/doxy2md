//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef DOXY2MD_APPLICATION_HPP
#define DOXY2MD_APPLICATION_HPP

#include <sys/Cli.hpp>

#include <fs.hpp>
#include <var.hpp>

#include "IO.hpp"

class Application : public IOAccess {
public:
  static void run(const sys::Cli &cli);

private:

  enum class IsAbort {
    no, yes
  };

  static IsAbort check_error();
};

#endif // DOXY2MD_APPLICATION_HPP
