// COPYING: Copyright 2017-2021 Tyler Gilbert and Stratify Labs. All rights
// reserved

#include <fs/FileSystem.hpp>
#include <fs/Path.hpp>
#include <printer/JsonPrinter.hpp>
#include <printer/Printer.hpp>
#include <sys/Cli.hpp>
#include <var/StackString.hpp>

#include "Application.hpp"

using namespace var;
using namespace fs;



int main(int argc, char *argv[]) {
  sys::Cli cli(argc, argv);

  Application::run(cli);

  return 0;
}
