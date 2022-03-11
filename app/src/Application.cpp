//
// Created by Tyler Gilbert on 3/9/22.
//

#include <printer.hpp>

#include "Application.hpp"
#include "DoxyJson.hpp"
#include "Options.hpp"

void Application::run(const sys::Cli &cli) {
  printer().set_verbose_level(cli.get_option("verbose"));

  Printer::Object run_object(printer(), "doxy2md");

  const auto options = Options(cli);

  if (check_error() == IsAbort::yes) {
    return;
  }

  printer().object(
    "configuration",
    options.configuration(),
    Printer::Level::debug);

  DoxyJson::LinkTagContainer link_tag_container;

  const auto source_container = options.configuration().input().get_sources();
  for (const auto &source_object : source_container) {

    const auto source_folder = source_object.get_input();
    printer().debug(
      "process source folder "
      | (options.source() / source_object.get_input()).string_view());

    const auto source_path_container
      = FileSystem().read_directory(options.source() / source_folder);
    DoxyJson::LinkContainer link_container;
    for (const auto &source_path : source_path_container) {

      Printer::Object run_object(printer(), source_path);
      if (Path::suffix(source_path) == "xml") {
        api::ErrorScope error_scope;

        const auto output_directory
          = options.destination() / source_object.get_output();
        FileSystem().create_directory(output_directory);

        const auto output_path
          = output_directory / Path::base_name(source_path) & ".md";

        printer().key("output", output_path);

        DoxyJson doxy_json(options, link_tag_container);

        doxy_json.set_relative_output_folder(source_object.get_output())
          .process_file(options.source() / source_folder / source_path);

        if (is_success()) {
          for(const auto & item: doxy_json.link_container()){
            link_container.push(item);
          }
          File(File::IsOverwrite::yes, output_path).write(doxy_json.output());
        }
        check_error();
      } else {
        printer().info("skipping");
      }
    }
    link_tag_container.push(
      {.link_container = link_container,
       .path = source_object.get_output()});
  }
}

Application::IsAbort Application::check_error() {
  if (is_error()) {
    printer().error(error().message());
    printer().object("error", error(), Printer::Level::debug);
    return IsAbort::yes;
  }
  return IsAbort::no;
}
