# Cren Build

The `build` command compiles the project and generates the target directory with the build artifacts.

## Usage

```sh
$ cren build [OPTIONS]
 
```

The `build` command accepts the following options:

- `-a, --all-targets`: Build all targets in the project.
- `-b, --bin <TARGET>`: Build the binary target.
- `-B, --bins`: Build all binary targets.
- `-e, --example <EXAMPLE>`: Build the specified example.
- `-E, --examples`: Build all examples.
- `-l, --lib`: Build the library target.
- `-F, --features <FEATURES>`: Specify features to enable during the build. More features can be specified by separating them with commas.
- `-r, --release`: Build in release mode.
- `--all-features`: Enable all features during the build.
- `--no-default-features`: Disable default features during the build.
- `--manifest-path <PATH>`: Specify the path to the manifest file.
- `--help`: Display help information for the command.
