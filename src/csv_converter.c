#include "csv_converter.h"

#include <errno.h>
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

static void set_header(char* text, struct table* table) {
  if (!is_separator(text[0])) {
    table->result = PARSE_WRFMT;
    return;
  }
  // TODO:
}

struct table parse_csv(char* text) {
  struct table ret = (struct table){0};
  size_t idx = 0;

  if (text == NULL) {
    ret.result = PARSE_NULLARG;
    goto end;
  }
  set_header(text, &ret);
  if (ret.result)
    goto end;

  while (text[idx]) {
    // TODO:
    ++idx;
  }

end:
  return ret;
}

void destroy_table(struct table* table) {
  if (table == NULL)
    return;
  // TODO:
}

int print_err(enum parse_result result_code) {
  if (result_code == PARSE_OK)
    return 0;
  if (result_code == PARSE_WRFMT) {
    fprintf(stderr, "Wrong format\n");
    return EINVAL;
  }
  if (result_code == PARSE_WRFMT) {
    fprintf(stderr, "Illegal character\n");
    return EINVAL;
  }
  if (result_code == PARSE_NULLARG) {
    fprintf(stderr, "String arg is NULL\n");
    return EINVAL;
  }
  fprintf(stderr, "Unknown error code: %d\n", result_code);
  return EINVAL;
}
