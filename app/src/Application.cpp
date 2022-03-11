//
// Created by Tyler Gilbert on 3/9/22.
//

#include <printer.hpp>

#include "Application.hpp"
#include "DoxyJson.hpp"
#include "Options.hpp"

void Application::run(const sys::Cli &cli) {
  printer().set_verbose_level(cli.get_option("verbose"));


  const auto options = Options(cli);

  Printer::Object run_object(printer(), "doxy2md");

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

        const auto exclude_container = source_object.get_exclude();
        const auto include_container = source_object.get_include();

        const auto is_excluded = [&]() {

          //if no exclude is specified, but include is specified
          //exclude everything but that is included
          if (
            (exclude_container.count() == 0)
            && (include_container.count() > 0)) {
            return true;
          }
          for (const auto &item : exclude_container) {
            if (source_path.string_view().find(item) != StringView::npos) {
              return true;
            }
          }
          return false;
        }();

        const auto is_included = [&]() {
          //included items will always be included even
          //if they match an exclude
          for (const auto &item : include_container) {
            if (source_path.string_view().find(item) != StringView::npos) {
              return true;
            }
          }
          return false;
        }();

        if (!is_excluded || is_included) {

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
            for (const auto &item : doxy_json.link_container()) {
              link_container.push(item);
            }
            File(File::IsOverwrite::yes, output_path).write(doxy_json.output());
          }
          check_error();
        }
      } else {
        printer().info("skipping");
      }
    }
    {
      const auto base_url = source_object.get_base_url();
      const auto path = base_url.is_empty() ? PathString(source_object.get_output()) : base_url / source_object.get_output();
      link_tag_container.push(
        {.link_container = link_container, .path = path});
    }
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
