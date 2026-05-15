#include "table_processor.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

static enum process_result process_cell(struct cell* cell) {
  if (cell->ct == CELL_NUMBER)
    return PROCESS_OK;
  int n1, n2;
  struct operand* op1 = &cell->oc.op1;
  struct operand* op2 = &cell->oc.op2;
  enum operation oper = cell->oc.operation;

  if (op1->ot == OT_NUMBER) {
    n1 = op1->number;
  }
  else {
    process_cell(op1->link);
    n1 = op1->link->number;
  }

  if (op2->ot == OT_NUMBER) {
    n2 = op2->number;
  }
  else {
    process_cell(op2->link);
    n2 = op2->link->number;
  }

  if (cell->ct == CELL_NUMBER)
    return PROCESS_OK;
  cell->ct = CELL_NUMBER;

  if (oper == OP_PLUS) {
    cell->number = n1 + n2;
    return PROCESS_OK;
  }
  else if (oper == OP_MINUS) {
    cell->number = n1 - n2;
    return PROCESS_OK;
  }
  else if (oper == OP_MUL) {
    cell->number = n1 * n2;
    return PROCESS_OK;
  }
  else if (oper == OP_DIV) {
    if (n2 == 0)
      return PROCESS_ZERO_DIV;
    cell->number = n1 / n2;
    return PROCESS_OK;
  }
  return PROCESS_UNSUP_OPER;

}

enum process_result process_table(struct table* table) {
  for (size_t i = 0; i < table->rows; ++i) {
    for (size_t j = 0; j < table->columns; ++j) {
      if (table->cells[i][j].ct == CELL_NUMBER)
        continue;
      enum process_result pr = process_cell(&table->cells[i][j]);
      if (pr)
        return pr;
    }
  }
  return PROCESS_OK;
}

void print_table(const struct table* table) {
  for (size_t i = 0; i < table->columns; ++i) {
    printf(",%s", table->column_names[i]);
  }
  printf("\n");
  for (size_t i = 0; i < table->rows; ++i) {
    printf("%d", table->row_numbers[i]);
    for (size_t j = 0; j < table->columns; ++j) {
      if (table->cells[i][j].ct == CELL_NUMBER) {
        printf(",%d", table->cells[i][j].number);
      }
      else if (table->cells[i][j].ct == CELL_OPERATION) {
        printf(",OP");
      }
      else {
        printf(",UNKNOWN");
      }
    }
    printf("\n");
  }
}

int process_print_err(enum process_result pr) {
  if (pr == PROCESS_OK)
    return 0;
  if (pr == PROCESS_ZERO_DIV) {
    fprintf(stderr, "Zero division found\n");
    return EDOM;
  }
  if (pr == PROCESS_UNSUP_OPER) {
    fprintf(stderr, "Use of unknown or unsupported operation");
    return EINVAL;
  }
  fprintf(stderr, "Unknown error: %d\n", (int)pr);
  return EINVAL;
}
