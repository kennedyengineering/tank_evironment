FROM ubuntu:24.04

# Install necessary packages
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt upgrade -y

# Build packages
RUN apt install -y \
    git \
    g++ \
    cmake

# Debug packages (optional)
RUN apt install -y \
    gdb \
    valgrind

# Cairo packages
RUN apt install -y \
    libcairo2-dev

# PyBind11 packages
RUN apt install -y \
    python3-dev

# Python packages
RUN apt install -y \
    python3-pip \
    python3-venv

# Switch to non-root user
USER ubuntu
