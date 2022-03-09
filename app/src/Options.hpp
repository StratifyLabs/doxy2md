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

    if (const auto value = cli.get_option("source"); !value.is_empty()) {
      m_source = value;
    } else {
      API_RETURN_ASSIGN_ERROR(
        "You must specify `--source=<path to xml folder>`",
        EEXIST);
    }

    if (const auto value = cli.get_option("destination"); !value.is_empty()) {
      m_destination = value;
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
    } else {
      API_RETURN_ASSIGN_ERROR(
        "You must specify `--destination=<path to output folder>`",
        EEXIST);
    }

    if (const auto value = cli.get_option("configuration"); !value.is_empty()) {
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

    if (const auto value = cli.get_option("template"); !value.is_empty()) {
      m_template_path = value;
    }
  }

  const Configuration &configuration() const { return m_configuration; }

private:
  API_AC(Options, PathString, source);
  API_AC(Options, PathString, destination);
  API_AC(Options, PathString, template_path);

  Configuration m_configuration;
};

#endif // CLI_OPTIONS_HPP
