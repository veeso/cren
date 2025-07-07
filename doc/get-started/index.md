# Get started

You can install Cren by running the following command:

```bash
curl --proto '=https' --tlsv1.2 -sSfL https://get.cren.cc | sh
```

Alternatively, you can download the latest release binary from the [releases page](https://github.com/veeso/cren/releases).

## Install options

You can provide additional options to the installer script:

- `-b=<bin_dir>`: Specify the directory where the Cren binary will be installed. Default is `/usr/local/bin`.
- `-s` or `--source`: Build Cren from source instead of using precompiled binaries.
- `-f`, `-y`, `--yes`, or `--force`: Force the installation, overwriting any existing Cren installation and skipping prompts.
- `-v=<version>` or `--version=<version>`: Specify the version of Cren to install. Default is the latest version.
