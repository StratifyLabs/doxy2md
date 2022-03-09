//
// Created by Tyler Gilbert on 3/9/22.
//

#ifndef DOXY2MD_MACROS_HPP
#define DOXY2MD_MACROS_HPP

#define HANDLE(x, input)                                                       \
  if (name == MCU_STRINGIFY(x)) {                                              \
    handle_##x(input);                                                         \
    return;                                                                    \
  }

#define HANDLE_AT(x, input)                                                    \
  if (name == "@" MCU_STRINGIFY(x)) {                                          \
    handle_at_##x(input);                                                      \
    return;                                                                    \
  }

#define HANDLE_HASHTAG(x, input)                                               \
  if (name == "#" MCU_STRINGIFY(x)) {                                          \
    handle_hashtag_##x(input);                                                 \
    return;                                                                    \
  }

#define HANDLE_AMP(x, input)                                                   \
  if (name == "&" MCU_STRINGIFY(x)) {                                          \
    handle_amp_##x(input);                                                     \
    return;                                                                    \
  }

#endif // DOXY2MD_MACROS_HPP
