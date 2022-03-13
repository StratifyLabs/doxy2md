//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef DOXY2MD_MACROS_HPP
#define DOXY2MD_MACROS_HPP

#define HANDLE(x, input)                                                       \
  if (name == MCU_STRINGIFY(x)) {                                              \
    m_name_container.push_back(name);                                               \
    handle_##x(input);                                                         \
    m_name_container.pop_back();                                                    \
    return;                                                                    \
  }

#define HANDLE_AT(x, input)                                                    \
  if (name == "@" MCU_STRINGIFY(x)) {                                          \
    m_name_container.push_back(name);                                               \
    handle_at_##x(input);                                                      \
    m_name_container.pop_back();                                                    \
   return;                                                                    \
  }

#define HANDLE_HASHTAG(x, input)                                               \
  if (name == "#" MCU_STRINGIFY(x)) {                                          \
    m_name_container.push_back(name);                                               \
    handle_hashtag_##x(input);                                                 \
    m_name_container.pop_back();                                                    \
   return;                                                                    \
  }

#define HANDLE_ADD_LINKS(input)                                                \
  if (add_links(input)) {                                                      \
    return;                                                                    \
  }                                                                            \
  printer::Printer::Object enter_function(                                     \
    printer(),                                                                 \
    __FUNCTION__,                                                              \
    printer::Printer::Level::message)

#endif // DOXY2MD_MACROS_HPP
