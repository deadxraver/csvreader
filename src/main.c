#include <errno.h>
#include <stdio.h>
#include <malloc.h>

#include "csv_converter.h"
#include "table_processor.h"

int main(int argc, char* argv[]) {
  FILE* fd = NULL;
  int ret = 0;
  struct table table = {0};
  char* csvtext = NULL;

  if (argc != 2) {
    fprintf(stderr, "Wrong number of args\n");
    ret = EINVAL;
    goto end;
  }

  if ((fd = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Failed to open file %s\n", argv[1]);
    ret = ENOENT;
    goto end;
  }

  fseek(fd, 0, SEEK_END);
  size_t sz = ftell(fd);
  rewind(fd);
  csvtext = (char*)malloc((sz + 1) * sizeof(char));

  if (csvtext == NULL) {
    fprintf(stderr, "Failed to alloc mem for file content\n");
    ret = ENOMEM;
    goto end;
  }

  if (fread(csvtext, sizeof(char), sz, fd) != sz) {
    fprintf(stderr, "Failed to read file contents\n");
    ret = EIO;
    goto end;
  }

  csvtext[sz] = 0;

  for (char* c = csvtext; *c != 0; ++c) {
    if (*c == '\n' && c[1] == '\n') {
      c[1] = 0;
      break;
    }
  }

  if ((table = parse_csv(csvtext)).result) {
    ret = parse_print_err(table.result);
    goto end;
  }

  if (ret = process_table(&table)) {
    ret = process_print_err(ret);
    goto end;
  }

  print_table(&table);

end:
  destroy_table(&table);
  if (csvtext != NULL)
    free(csvtext);
  csvtext = NULL;
  if (fd != NULL)
    fclose(fd);
  fd = NULL;
  return ret;
}
