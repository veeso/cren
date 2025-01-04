# Cren – A Modern Build and Package Manager for C

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

