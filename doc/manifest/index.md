# Cren Manifest

The **Cren Manifest** file contains both build options, metadata and the dependencies of each of your packages.

The manifest file can be found at the root folder of a package under the name **Cren.toml**.

Every manifest consists of the following sections:

- `[package]`
  - `name`: the name of the package
  - `description`: a description of the package
  - `version`: the version of the package
  - `edition`: the Cren manifest version
  - `language`: C/C++ version of the language
  - `authors`: the authors of the package
  - `documentation`: url to the package documentation
  - `homepage`: URL to the homepage of the package
  - `repository`: URL to the repository containing the source of the package
  - `license`: type of LICENSE
  - `license-file`: path containing license
- Target tables
  - `[lib]`: library target settings
    - `name`: override library name; defaults to package name
    - `requires-features`: list of requires features
  - `[[bin]]`: binary target settings
    - `name`: binary name
    - `path`: path to main source file
    - `required-features`: list of required features
  - `[[example]]`: example target settings
    - `name`: example name
    - `path`: path to main source file
    - `required-features`: list of requires features
- `[dependencies]`: dependencies for the lib or bin targets
  - `lib-name`: name of the library to depend on
- `[dev-dependencies]`: dependencies for tests
  - `lib-name`: name of the library to depend on for tests
- `[features]`: table of optional features to enable

## The package section

The first section in a Cren.toml is `[package]`.

The only field required by Cren are:

- `name`
- `version`
- `edition`
- `language`

### The name field

The name field identifies the name of the package.

The name is mandatory and should only contain alphanumeric characters, `-` or `_`, and cannot be empty.

### The version field

Cren bakes in the concept of [Semantic Versioning](https://semver.org/), so make sure you follow some basic
rules:

1. Before you reach 1.0.0, anything goes, but if you make breaking changes, increment
the minor version. Breaking changes should include adding elds to structs or variants to enums.
2. After 1.0.0, only make breaking changes when you increment the major version. Don’t
break the build.
3. After 1.0.0, don’t add any new public API (no new public anything) in patch-level versions.
4. Always increment the minor version if you add any new public classes, structs, traits, elds,
types, functions, methods or anything else.
5. Use version numbers with three numeric parts such as 1.0.0 rather than 1.0.

### The edition field

The `edition` field contains the current cren manifest edition to be used.

The edition is used to track how Cren should interpret the Manifest, so in case there will be breaking changes to the manifest in the future, an increment of the edition will occur, causing cren to use different parsers based on the edition.

### The language field

The `language` field contains the C or C++ version of the language to be used to build the project.

The value should be any of these:

- `c89`
- `c90` / `ansic`
- `c99`
- `c11`
- `c17`
- `c23`
- `c++99`
- `c++98` / `ansicpp`
- `c++03`
- `c++11`
- `c++17`
- `c++20`
- `c++26`

### The authors field

The optional `authors` field contains a list of string representing the authors of the package.

The author name may include the email address and should follow this syntax:

- `John Doe` without email address

Or

- `Christian Visintin <christian.visintin@veeso.dev>` to specify also the email address

### The description field

The optional `description` field should contain a short description of what the package is about.

### The documentation field

The optional `documentation` field should lead to a website containing the documentation for this package.

### The homepage field

The optional `homepage` field should lead to homepage of the website for this package

### The license field

The optional `license` field should contain the license under which the package is distributed.

The license *must* be any of the license contained in the [SPDX v3.20](https://github.com/spdx/license-list-data/tree/v3.20).

### The license-file field

In case the `license` field is missing, it is also possible to specify a custom license file to be used. So this field, which is optional anyway, should lead to a URL containing the license of the package.

## Targets table

The targets table defines the different targets to be built for a project

### Library

The library target defines a **library** that can be used and linked by other libraries and
executables. The library name defaults to package name, and the path defaults to `src/package_name.c`. A package can have only one library.

### Binary

The binary target defines a **binary**.

Each binary must be defined in this section.

In case you have multiple binaries, it is possible to specify the binary to run with cren by using

```sh
$ cren run --bin <bin_name>
 
```

### Example

The example target defines an **example**.

Each example must be defined in this section.

You can run examples with cren by using:

```sh
$ cren run --example <example_name>
 
```

## Dependencies section

The dependencies section allows you to specify all the dependencies for your package.

Each dependency must be identified by its `name` and may contain these fields:

- `git`: URL to git to download the dependency
- `link`: link option for the compiler (e.g. `-lm`)
- `optional`: makes the dependency to be feature-gated. (See [features](#features)).
- `defines`: a list of defines to build the library

## Features

The features section allows you to specify both "DEFINES" to be set or dependencies to be enabled at build time.

In particular features can be used to enable dependencies which have the `optional` flag set to `true`.

Each feature must be specified under the `[features]` section with the following syntax:

```toml
[features]
default = ["quick-maths"]
# a feature which enables a dependency
quick-maths = { dependencies = [ "libm" ] }
# a feature which defines symbols
skip-validation = { defines = [ "SKIP_VALIDATION", "VALIDATE=0" ] } 
```

`default` is an optional key which defines the name of the default features to be enabled. In case it is not set it defaults to `[]`. The defined features must exist.

So each feature is defined by its name and two optional keys:

- `dependencies`: list of dependencies to be enabled. Mind that these dependencies must be defined in `[dependencies]` and must be have `optional = true`.
- `defines`: a list of defines to be passed to the compiler. It supports also the `=` symbol to provide a certain value.

Features can be passed both to the `build`, to the `test` and to the `run` commands by specifying the `--features <feature>` flag.

```sh
$ cren build --features quick-maths
 
```

Multiple features can also be comma-separated:

```sh
$ cren build --features quick-maths,skip-validation
 
```

Default features can be disabled by passing the `--no-default-features` flag, while all features can be enabled by passing `--all-features`.
