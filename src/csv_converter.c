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

typedef bool (*sep_typedef) (char);

static int parse_number(const char* str, sep_typedef sep) {
  int ret = 0;
  for (const char* s = str; !sep(*s) && *s != 0; ++s) {
    if (is_digit(*s)) {
      ret *= 10;
      ret += *s - '0';
    }
  }
  return ret;
}

static struct cell* find_cell(const struct table* table, const char* column, int row) {
  for (size_t row_idx = 0; row_idx < table->rows; ++row_idx) {
    if (table->row_numbers[row_idx] != row)
      continue;
    for (size_t column_idx = 0; column_idx < table->columns; ++column_idx) {
      if (strcmp(table->column_names[column_idx], column) == 0) {
        return table->cells[row_idx] + column_idx;
      }
    }
    return NULL;
  }
  return NULL;
}

static struct operation_cell parse_operation(const char* text, struct table* table) {
  struct operation_cell ret = {0};
  char buf[128] = {0};
  if (*text == '=')
    ++text;
  size_t idx = 0;
  size_t buf_idx = 0;

  if (is_digit(text[idx])) {
    ret.op1 = (struct operand) {
      .ot = OT_NUMBER,
      .number = parse_number(text + idx, (sep_typedef)is_operator),
    };
  }
  else if (is_letter(text[idx])) {
    for (; is_letter(text[idx]); ++idx) {
      buf[buf_idx++] = text[idx];
    }
    ret.op1 = (struct operand) {
      .ot = OT_LINK,
      .link = find_cell(table, buf, parse_number(text + idx, (sep_typedef)is_operator)),
    };
    if (ret.op1.link == NULL) {
      table->result = PARSE_CELL_NOT_FOUND;
      return ret;
    }
  }
  else {
    table->result = PARSE_WRFMT;
    return ret;
  }
  buf_idx = 0;

  while (!(ret.operation = is_operator(text[idx++])));

  if (is_digit(text[idx])) {
    ret.op2 = (struct operand) {
      .ot = OT_NUMBER,
      .number = parse_number(text + idx, is_separator),
    };
  }
  else if (is_letter(text[idx])) {
    memset(buf, 0, sizeof(buf));
    for (; is_letter(text[idx]); ++idx) {
      buf[buf_idx++] = text[idx];
    }
    ret.op2 = (struct operand) {
      .ot = OT_LINK,
      .link = find_cell(table, buf, parse_number(text + idx, is_separator)),
    };
    if (ret.op2.link == NULL) {
      table->result = PARSE_CELL_NOT_FOUND;
      return ret;
    }
  }
  else {
    table->result = PARSE_WRFMT;
    return ret;
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
      table->row_numbers[row_no++] = parse_number(text + idx, is_separator);
      while(text[++idx] != '\n');
    }
  }
}

static void set_contents(const char* text, struct table* table) {
  size_t idx;
  char buf[128];
  memset(buf, 0, sizeof(buf));
  size_t row_no = 0, rows_begin;

  for (rows_begin = 0; text[rows_begin] != '\n'; ++rows_begin);
  ++rows_begin; // skip header

  table->cells = (struct cell**) malloc(sizeof(struct cell*) * table->rows);
  for (size_t i = 0; i < table->rows; ++i) {
    table->cells[i] = (struct cell*) malloc(sizeof(struct cell) * table->columns);
    memset(table->cells[i], 0, sizeof(struct cell) * table->columns);
  }

  for (idx = rows_begin; text[idx] != 0; ++idx) {
  break_while:
    while (text[idx] != '\n') {

      while(!is_separator(text[idx++])) {
        if (text[idx] == 0 || text[idx] == '\n') {
          goto break_while;
        }
      }

      size_t col_no = 0;
      if (is_digit(text[idx])) {
        table->cells[row_no][col_no].ct = CELL_NUMBER;
        table->cells[row_no][col_no].number = parse_number(text + idx, is_separator);
        ++col_no;
      }
      else if (text[idx] == '=') {
        table->cells[row_no][col_no].ct = CELL_OPERATION;
        table->cells[row_no][col_no].oc = parse_operation(text + idx, table);
        if (table->result)
          return;
        ++col_no;
      }
      else {
        table->result = PARSE_WRFMT;
        return;
      }
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

  set_contents(text, &ret);
  if (ret.result)
    goto end;

end:
  return ret;
}

void destroy_table(struct table* table) {
  if (table == NULL)
    return;

  if (table->cells) {
    for (size_t i = 0; i < table->rows; ++i) {
      if (table->cells[i]) {
        free(table->cells[i]);
        table->cells[i] = NULL;
      }
    }
    free(table->cells);
    table->cells = NULL;
  }

  if (table->column_names) {
    for (size_t i = 0; i < table->columns; ++i) {
      if (table->column_names[i]) {
        free(table->column_names[i]);
        table->column_names[i] = NULL;
      }
    }
    table->columns = 0;
    free(table->column_names);
    table->column_names = NULL;
  }
  if (table->row_numbers) {
    free(table->row_numbers);
    table->row_numbers = NULL;
    table->rows = 0;
  }
}

int print_err(enum parse_result result_code) {
  if (result_code == PARSE_OK)
    return 0;
  if (result_code == PARSE_WRFMT) {
    fprintf(stderr, "Wrong format\n");
    return EINVAL;
  }
  if (result_code == PARSE_ILLEGCHAR) {
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
  if (result_code == PARSE_CELL_NOT_FOUND) {
    fprintf(stderr, "Trying to address cell that does not exist\n");
    return ENOENT;
  }
  fprintf(stderr, "Unknown error code: %d\n", result_code);
  return EINVAL;
}
