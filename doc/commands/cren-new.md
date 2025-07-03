# Cren New

The `new` command initialize a new cren project in the a directory located at the current working directory with the name provided.

```sh
$ cren new [OPTIONS] PACKAGE_NAME
 
```

The following options can be provided:

- `--bin` - Create a binary package
- `--lib` - Create a library package
- `-L --language <language>` - Specify the language of the package. Default is c11

Package names must follow the following rules:

- Must start with a letter
- Can only contain letters, numbers, and underscores
