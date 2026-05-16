# Development Container for Arene Base

This directory contains the configuration for a development container (devcontainer) that provides a complete C++ development environment for the Arene Base project.

## Features

- **Base Image**: Ubuntu 24.04 (matches GitHub Actions CI environment)
- **Compiler**: Clang (default from Ubuntu package manager)
- **C++ Standard Library**: libc++ (LLVM C++ Standard Library)
- **C++ Standard**: C++14 compatible
- **Build System**: Bazel 7 (via Bazelisk)
- **Additional Tools**:
  - Build essentials (gcc, g++, make, etc.)
  - CMake and Ninja
  - Git and GitHub CLI
  - Python 3 with pip
  - GDB for debugging
  - Clang-format and clang-tidy for code quality
  - Code editors (vim, nano)

## VS Code Extensions

The following extensions are automatically installed:
- C/C++ Tools
- C/C++ Extension Pack
- clangd (Clang Language Server)
- Makefile Tools
- GitLens
- CMake Tools
- Ruff (Python linter)
- Clang-format

## Getting Started

### Prerequisites

- Docker installed on your system
- VS Code with the "Dev Containers" extension installed

### Opening the Project in a Dev Container

1. Open the repository in VS Code
2. Click the green button in the bottom-left corner: `><`
3. Select "Reopen in Container"
4. VS Code will build the container and set it up (this may take a few minutes on first build)

### Using the Container

Once inside the container, you can use standard commands:

#### Build the project
```bash
bazel build --config=local_toolchain -- //arene/base/... //stdlib/...
```

#### Run tests
```bash
bazel test --config=local_toolchain -- //arene/base/... //stdlib/...
```

#### Run specific component tests
```bash
bazel test --config=local_toolchain -- //arene/base/array/tests:all
```

#### Format code
```bash
clang-format -i <file>
```

#### Check code quality
```bash
clang-tidy <file>
```

## Environment Variables

The following environment variables are set automatically:
- `USE_BAZEL_VERSION=7.0.0` - Bazel version to use

## Customization

To modify the container configuration:

1. **Dockerfile**: Add system packages or tools in the `apt-get install` section
2. **devcontainer.json**: 
   - Add VS Code extensions in the `extensions` array
   - Modify settings in the `settings` object
   - Add port forwarding if needed

After making changes, rebuild the container by:
- Running the Dev Containers command palette: `Ctrl+Shift+P` → "Dev Containers: Rebuild Container"

## Troubleshooting

### Container build fails
- Ensure Docker is running
- Try rebuilding: `Ctrl+Shift+P` → "Dev Containers: Rebuild Container"
- Check Docker disk space: `docker system prune`

### Bazel cache issues
- The Bazel cache is stored in `/root/.cache/bazel`
- To clear the cache: `bazel clean --expunge`

### SSH/Git credentials
- Mount your SSH keys by uncommenting the SSH mount in `devcontainer.json`
- Git config is mounted automatically if available locally

## Additional Resources

- [Dev Containers Documentation](https://containers.dev/)
- [Bazel Documentation](https://bazel.build/docs)
- [Clang Documentation](https://clang.llvm.org/docs/)
- [Arene Base README](../README.md)
