#include "csv_converter.h"

#include <malloc.h>

static enum operation is_operator(char c) {
  if (c == '+')
    return OP_PLUS;
  if (c == '-')
    return OP_MINUS;
  if (c == '*')
    return OP_MUL;
  if (c == '/')
    return OP_DIV;
  return OP_UNKNOWN;
}

static inline bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline bool is_letter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline bool is_separator(char c) {
  return c == ',';
}

struct table parse_csv(char* text) {
  struct table ret = (struct table){0};
  size_t idx = 0;
  if (!is_separator(text[0])) {
    //
  }
  while (text[idx]) {
    // TODO:
  }
  return ret;
}

void destroy_table(struct table* table) {
  if (table == NULL)
    return;
  // TODO:
}
