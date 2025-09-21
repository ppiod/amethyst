# Amethyst Compiler

A simple toy compiler written in C that translates mathematical expressions into native x86-64 assembly for Linux.

This project was built as a learning exercise to understand the complete pipeline of a compiler, from source code to a running executable.

## Features

  - `let` variable assignments
  - Integer arithmetic (`+`, `-`, `*`, `/`)
  - Operator precedence and `()` grouping

## Requirements

  - `gcc`
  - `make`
  - `nasm`

On a Debian-based system (like Ubuntu), you can install them with:

```bash
sudo apt-get install build-essential nasm
```

## Usage

The `Makefile` handles the entire process.

1.  **Build the compiler, generate assembly, and run the final executable:**

    ```bash
    make run-asm
    ```

2.  **Clean up all generated files:**

    ```bash
    make clean
    ```

## License

This project is licensed under the [LICENSE](LICENSE).