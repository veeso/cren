# Package Layout

Cren follows and requires you to follow this layout for your package:

.
├── Cren.toml
├── examples/
│   └── example.c
├── include/
│   └── package_name.h
├── src/
│   ├── package_name.c
│   └── bin/
│       └── another_bin.c
└── tests/
    ├── my_integration_test.c
    └── other_tests/
        └── another_test.c

- Cren.toml: **Manifest file** for the package
- examples/: may contain example to showcase usage for your libraries
- include/: a folder containing the header files
- src/: a folder containing the source files.
- tests/: a folder containing the ingration tests.

> Mind that in case you're using cpp, the default name for source files will have extensions `.hpp` and `.cpp`.
