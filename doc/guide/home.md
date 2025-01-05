# Cren Home

The **Cren home" functions as a download and source cache. When building a package, Cren
stores downloaded build dependencies in the Cren home. You can alter the location of the
Cren home by setting the `CREN_HOME` environmental variable. The home package provides an
API for getting this location if you need this information inside your Rust crate. By default,
the Cren home is located in `$HOME/.cren/`.

The Cren home consists of following components:

## Files

- `cren.toml`: Cren's global configuration. See the [config entry in the reference](../config/index.md).

## Directories

- `registry/`: contains the build of all the cached packages. Sub directories contains the build output. Each directory is identified by the library name and the version or git hash (e.g. `libm@0.2.0`)

## Cleaning Cache

You can clean the cache at any time by running `cren cache clean`
