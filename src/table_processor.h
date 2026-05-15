#ifndef _TABLE_CONVERTER_H

#define _TABLE_CONVERTER_H

#include "csv_converter.h"

enum process_result {
  PROCESS_OK = 0,
  PROCESS_ZERO_DIV,
  PROCESS_UNSUP_OPER,
};

enum process_result process_table(struct table* table);

void print_table(const struct table* table);

int process_print_err(enum process_result pr);

#endif // !_TABLE_CONVERTER_H
