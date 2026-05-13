#include "csv_converter.h"

#include <errno.h>
#include <malloc.h>
#include <string.h>

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

static int parse_number(char* str, char sep) {
  int ret = 0;
  for (char* s = str; *s != sep; ++s) {
    if (is_digit(*s)) {
      ret *= 10;
      ret += *s - '0';
    }
  }
  return ret;
}

static void set_header(char* text, struct table* table) {
  size_t idx = 0;
  char buf[128];
  size_t buf_idx = 0;
  size_t colmn_no = 0;

  if (!is_separator(text[0])) {
    table->result = PARSE_WRFMT;
    return;
  }
  memset(buf, 0, sizeof(buf));

  for (idx = 1;; ++idx) {
    if (is_separator(text[idx]) || text[idx] == '\n') {
      ++table->columns;
    }
    if (text[idx] == '\n')
      break;
    if (text[idx] == 0) {
      table->result = PARSE_UNEXPECTED_EOF;
      return;
    }
  }

  table->column_names = (char**) malloc(table->columns * sizeof(char*));
  memset(table->column_names, 0, table->columns * sizeof(char*));

  for (idx = 1;; ++idx) {
    if (is_separator(text[idx]) || text[idx] == '\n') {
      size_t sz = strlen(buf) + 1;
      table->column_names[colmn_no] = (char*) malloc(sizeof(char) * sz);
      table->column_names[colmn_no][sz - 1] = 0;
      strcpy(table->column_names[colmn_no++], buf);
      memset(buf, 0, sizeof(buf));
      buf_idx = 0;
      if (text[idx] == '\n')
        break;
      continue;
    }
    buf[buf_idx++] = text[idx];
  }
}

static void set_row_numbers(char* text, struct table* table) {
  size_t idx;
  int number = 0;
  size_t row_no = 0;
  size_t rows_begin;

  for (rows_begin = 0; text[rows_begin] != '\n'; ++rows_begin);
  ++rows_begin; // skip header

  for (idx = rows_begin; text[idx] != 0; ++idx) {
    if (is_digit(text[idx])) {
      ++table->rows;
      while(text[++idx] != '\n');
    }
  }

  table->row_numbers = (int*) malloc(sizeof(int) * table->rows);
  for (idx = rows_begin; text[idx] != 0; ++idx) {
    if (is_digit(text[idx])) {
      table->row_numbers[row_no++] = parse_number(text + idx, ',');
      while(text[++idx] != '\n');
    }
  }
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

  set_row_numbers(text, &ret);
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
  if (table->column_names) {
    for (size_t i = 0; i < table->columns; ++i) {
      if (table->column_names[i]) {
        printf("%s\n", table->column_names[i]); // TODO: remove
        free(table->column_names[i]);
        table->column_names[i] = NULL;
      }
    }
    table->columns = 0;
    free(table->column_names);
    table->column_names = NULL;
  }
  if (table->row_numbers) {
    for (size_t i = 0; i < table->rows; ++i) { // TODO: remove
      printf("%d\n", table->row_numbers[i]);
    }
    free(table->row_numbers);
    table->row_numbers = NULL;
    table->rows = 0;
  }
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
    return EPERM;
  }
  if (result_code == PARSE_NULLARG) {
    fprintf(stderr, "String arg is NULL\n");
    return EINVAL;
  }
  if (result_code == PARSE_UNEXPECTED_EOF) {
    fprintf(stderr, "Unexpected EOF in file\n");
    return EIO;
  }
  fprintf(stderr, "Unknown error code: %d\n", result_code);
  return EINVAL;
}
