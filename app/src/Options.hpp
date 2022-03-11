//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef CLI_OPTIONS_HPP
#define CLI_OPTIONS_HPP

#include <fs.hpp>
#include <json/JsonDocument.hpp>
#include <sys/Cli.hpp>
#include <var.hpp>

#include "IO.hpp"

#include "Configuration.hpp"

class Options : public IOAccess {
public:
  explicit Options(const sys::Cli &cli) {

    const auto is_help = cli.get_option("help") == "true";


    if (const auto value
        = cli.get_option("json", "Format the output as JSON (default is YAML)");
        !value.is_empty()) {

      m_is_json = value == "true";
      if (m_is_json) {
        switch_to_json_printer();
      }
    }

    if (const auto value = cli.get_option(
          "configuration",
          "Specify a path to the configuration file.");
        !value.is_empty()) {
      if (!is_help) {

        if (!FileSystem().exists(value)) {
          API_RETURN_ASSIGN_ERROR(
            "`configuration` does not exist (" | value | ")",
            EEXIST);
        }

        json::JsonDocument json_document;
        const auto user_configuration
          = json_document.load(File(value)).to_object();
        if (is_error()) {
          printer().object("jsonError", json_document.error());
          API_RETURN_ASSIGN_ERROR(
            "failed to parse JSON configuration file (" | value | ")",
            EINVAL);
        }

        m_configuration = Configuration::make_defaults().to_object().update(
          user_configuration,
          json::JsonObject::UpdateFlags::recursive);
      }
    } else {
      m_configuration = Configuration::make_defaults();
    }

    if (const auto value = cli.get_option(
          "showConfiguration",
          "Show the active configuration. If no `--configuration` is "
          "specified, the default is shown. If `--configuration` is specified, "
          "it is merged with the default options to show the complete "
          "configuration. User configuration files only need to specify "
          "changes from the default.");
        !value.is_empty()) {

      m_is_show_configuration = value == "true";
      if (m_is_show_configuration && !is_help) {
        printer().object("configuration", m_configuration);
        if( is_json() ){
          printer().newline();
        }
        exit(0);
      }
    }

    if (const auto value
        = cli.get_option("source", "Specify the path to the source folder");
        !value.is_empty()) {
      m_source = value;
    } else if (!is_help) {
      API_RETURN_ASSIGN_ERROR(
        "You must specify `--source=<path to xml folder>`",
        EEXIST);
    }

    if (const auto value = cli.get_option(
          "destination",
          "Specify the path to the destiation folder");
        !value.is_empty()) {
      m_destination = value;
      if (!is_help) {
        if (FileSystem().exists(m_destination)) {
          if (FileSystem().get_info(m_destination).is_directory() == false) {
            printer().error("`destination` exists and is not a directory");
            exit(1);
          }
        }
        if (!FileSystem().directory_exists(m_destination)) {
          const auto parent = Path::parent_directory(m_destination);
          if (!parent.is_empty()) {
            if (FileSystem().directory_exists(m_destination)) {
              FileSystem().create_directory(m_destination);
            } else {
              API_RETURN_ASSIGN_ERROR(
                "`destination` cannot be created (parent directory must exist)",
                EEXIST);
            }
          } else {
            FileSystem().create_directory(m_destination);
          }
        }
      }
    } else if (!is_help) {
      API_RETURN_ASSIGN_ERROR(
        "You must specify `--destination=<path to output folder>`",
        EEXIST);
    }

    // needs to be last
    if (const auto value = cli.get_option("help", "Show this help menu");
        !value.is_empty()) {
      if (value == "true") {

        auto show_help = [&]() {
          cli.show_help(sys::Cli::ShowHelp()
                          .set_publisher("Stratify Labs, Inc")
                          .set_version(VERSION)
                          .set_printer(&printer()));
        };

        if (is_json()) {
          {
            printer::Printer::Object po(printer(), "root");
            show_help();
          }
          printer().newline();
        } else {
          show_help();
        }

        exit(0);
      }
    }
  }

  const Configuration &configuration() const { return m_configuration; }

private:
  API_AC(Options, PathString, source);
  API_AC(Options, PathString, destination);
  API_AB(Options, help, false);
  API_AB(Options, json, false);
  API_AB(Options, show_configuration, false);

  Configuration m_configuration;
};

#endif // CLI_OPTIONS_HPP
