# CSV Reader

This is an app which parses a csv-file and processes
excel-style operations of type `=<ARG1><OP><ARG2>`.
`ARG*` can be a number or a link to the cell in the table.
Every column has a name and every row has a number.
So args look like this `<column_name><row_number>` or `<inplace_number>`.

## Build & Test

### Install dependencies

```bash
sudo apt-get update
sudo apt install -y build-essential
```

### Build

```bash
make build
```

### Run Tests

```bash
make tests
```
