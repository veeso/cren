# Cren Run

The `run` command compiles the project and runs the specified target, such as a binary or example, with the option to pass additional arguments.

## Usage

```sh
$ cren run [OPTIONS] -- [ARGS]...
 
```

The `run` command accepts the following options:

- `-b, --bin <TARGET>`: Build the binary target.
- `-e, --example <EXAMPLE>`: Build the specified example.
- `-F, --features <FEATURES>`: Specify features to enable during the build. More features can be specified by separating them with commas.
- `-r, --release`: Build in release mode.
- `--all-features`: Enable all features during the build.
- `--no-default-features`: Disable default features during the build.
- `--manifest-path <PATH>`: Specify the path to the manifest file.
- `--help`: Display help information for the command.
