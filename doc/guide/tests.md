# Tests

Cren can run integration tests and unit tests by using the `cren test` command.

The tests must be implemented using the `cren_test` framework.

Like for Cargo, unit tests should be placed directly into src files, while integration tests should be placed in the `tests/` folder.

If we run `cren test` in your brand new package, we'll see that no test is executed, since no test is currently defined:

```sh
$ cren test

Running target/test/hello_world-9c2b65bbb79eabce
running 0 tests
test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out
```

If we had tests we would see a list of tests with the result associated to each of them.

## Implementing tests
