# Dev container for Arene Base C++ project
FROM ubuntu:24.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install system dependencies and development tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    clang-format \
    clang-tidy \
    llvm \
    llvm-dev \
    libc++-dev \
    libc++abi-dev \
    git \
    curl \
    wget \
    ca-certificates \
    pkg-config \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    python3-venv \
    openssh-client \
    vim \
    nano \
    less \
    gdb \
    bash-completion \
    && rm -rf /var/lib/apt/lists/*

# Install Bazelisk (installer for Bazel)
RUN curl -Lo /usr/local/bin/bazelisk https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64 && \
    chmod +x /usr/local/bin/bazelisk && \
    ln -s /usr/local/bin/bazelisk /usr/local/bin/bazel

# Create a non-root user
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Set working directory and permissions
WORKDIR /workspace
RUN chown -R ubuntu:ubuntu /workspace

CMD ["/bin/bash"]
