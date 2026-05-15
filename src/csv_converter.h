#ifndef _CSV_CONVERTER_H

#define _CSV_CONVERTER_H

#include <stdbool.h>
#include <stddef.h>

enum cell_type {
  CELL_NUMBER = 0,
  CELL_OPERATION,
};

enum operand_type {
  OT_NUMBER = 0,
  OT_LINK,
};

enum operation {
  OP_UNKNOWN = 0,
  OP_PLUS,
  OP_MINUS,
  OP_MUL,
  OP_DIV,
};

enum parse_result {
  PARSE_OK = 0,
  PARSE_WRFMT,
  PARSE_ILLEGCHAR,
  PARSE_NULLARG,
  PARSE_UNEXPECTED_EOF,
  PARSE_CELL_NOT_FOUND,
};

struct operand {
  enum operand_type ot;
  union {
    int number;
    struct cell* link;
  };
};

struct operation_cell {
  struct operand op1;
  enum operation operation;
  struct operand op2;
};

struct cell {
  enum cell_type ct;
  union {
    int number;
    struct operation_cell oc;
  };
};

struct table {
  size_t columns;
  size_t rows;
  char** column_names;
  int* row_numbers;
  struct cell** cells;
  enum parse_result result;
};

struct table parse_csv(char* text);

void destroy_table(struct table* table);

int parse_print_err(enum parse_result result_code);

#endif //! _CSV_CONVERTER_H
