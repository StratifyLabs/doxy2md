//
// Created by Tyler Gilbert on 3/9/22.
//

#include <printer.hpp>

#include "DoxyJson.hpp"
#include "Options.hpp"
#include "Application.hpp"

void Application::run(const sys::Cli &cli) {
  printer().set_verbose_level(cli.get_option("verbose"));

  Printer::Object run_object(printer(), "doxy2md");

  const auto options = Options(cli);

  if( check_error() == IsAbort::yes ){
    return;
  }

  const auto source_path_container
    = FileSystem().read_directory(options.source());


  printer().object(
    "configuration",
    options.configuration(),
    Printer::Level::debug);

  for (const auto &source_path : source_path_container) {
    Printer::Object run_object(printer(), source_path);
    if (Path::suffix(source_path) == "xml") {
      api::ErrorScope error_scope;
      const auto output_path
        = options.destination() / Path::base_name(source_path) & ".md";
      printer().key("output", output_path);

      DoxyJson doxy_json(options);
      doxy_json.process_file(options.source() / source_path);
      if( is_success() ){
        File(File::IsOverwrite::yes, output_path).write(doxy_json.output());
      }
      check_error();
    } else {
      printer().info("skipping");
    }
  }
}

Application::IsAbort Application::check_error() {
  if( is_error() ){
    printer().error(error().message());
    printer().object("error", error(), Printer::Level::debug);
    return IsAbort::yes;
  }
  return IsAbort::no;
}
