# Cren – A Modern Build and Package Manager for C

[![.github/workflows/ci.yaml](https://github.com/veeso/cren/actions/workflows/ci.yaml/badge.svg)](https://github.com/veeso/cren/actions/workflows/ci.yaml)
[![License-MIT](https://img.shields.io/badge/License-MIT-teal.svg)](https://opensource.org/licenses/MIT)
[![Conventional Commits](https://img.shields.io/badge/Conventional%20Commits-1.0.0-%23FE5196?logo=conventionalcommits&logoColor=white)](https://conventionalcommits.org)

Cren is a tool inspired by [Cargo](https://doc.rust-lang.org/cargo/), designed to modernize and simplify **C development**. With its modular approach and built-in features, Cren allows developers to focus on writing code by automating repetitive tasks like dependency management, building, testing, and formatting.

## 🚀 Key Features

- **Dependency Management**: Initial support for linked libraries (`-lm`, `-lz`, etc.), with future plans for full builds of external dependencies.
- **Simplified Build Process**: Compile your projects with a single command, no need to write `Makefile`s manually.
- **Integrated Testing**: Add tests directly within your source files using macros similar to Rust's `#[test]`.
- **Linting and Formatting**: Ensure consistent code style with dedicated commands.
- **Centralized Configuration**: Describe your project and dependencies in a simple `Cren.toml` file.

## 🛠️ Available Commands

- `cren init`: Initialize a new project with a predefined directory structure.
- `cren build`: Compile your project effortlessly.
- `cren run`: Run compiled project.
- `cren test`: Run integrated tests written directly in your source code.
- `cren lint`: Analyze your code for potential issues.
- `cren format`: Automatically format your code based on specified guidelines.

## 🎯 Why Cren?

Cren is built for developers who want a streamlined workflow for C projects. By combining modern practices with the simplicity of a single tool, Cren bridges the gap between traditional C workflows and the convenience of modern build systems.

## 🔮 Future Plans

- Full build support for external dependencies.
- Parallelized test execution.
- Support for custom build pipelines and hooks.
- Integration with popular CI/CD systems.

## 🛑 Requirements

- A modern C compiler (e.g., GCC, Clang).
- Make or CMake (optional for advanced builds).

## 🌟 Get Started

1. Install Cren (instructions coming soon).
2. Create a new project:

   ```bash
   cren init my_project
   cd my_project
   ```
