# Creating a new Package

To start a new package with Cren, use `cren new`:

In case you want to initialize a bin package use:

```sh
$ cren new hello_world --bin
 
```

where `hello_world` is the package name, otheriwise if you want a library use:

```sh
$ cren new libfoo --lib
 
```

By default `lib` is assumed, so the `--lib` argument may be omitted.

## Language version

By default `C11` is assumed as the standard, you can specify the language version by using `--language <version>`

such as:

```sh
$ cren new hello_world --bin --language c99
 
```

The following versions are supported:

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

## Command Output

The output generated will be:

```sh
$ cd hello_world
 
$ tree .
```

If we take a look at the output, we can see a file called `Cren.toml`.

`Cren.toml` is a file called **Manifest** and it contains both the metadata of your package and
the dependencies of your package, used to build it.

The file is writtne in [TOML](https://toml.io/) a language for value serialization.

The `cren new` command will also generate a source file for your package called as the package name.

In this example the file will be called `src/hello_world.c` and in case you want to build a binary it will already include the `cren_main` function.

We can actually already build it and run it:

```sh
$ cren build
Compiling hello_world v0.1.0 (file://path/to/file/hello_world)
```

And run it:

```sh
$ cren run
Compiling hello_world v0.1.0 (file://path/to/file/hello_world)
  Running `target/debug/hello_world`

Hello, World!
```

When you're ready with the release version of your package, you can run build/run commands with
`--release` flag.

```sh
$ cren run --release
Compiling hello_world v0.1.0 (file://path/to/file/hello_world)
  Running `target/release/hello_world`

Hello, World!
```
