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
    gdb

# Switch to non-root user
USER ubuntu
