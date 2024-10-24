FROM ubuntu:24.04

# Install necessary packages
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt upgrade -y

# Update Mesa driver (optional)
RUN apt install -y software-properties-common
RUN add-apt-repository ppa:kisak/kisak-mesa
RUN apt update && apt upgrade -y

# Build packages
RUN apt install -y \ 
    git \
    g++ \
    cmake

# Debug packages (optional)
RUN apt install -y \
    gdb

# Graphics packages (optional)
RUN apt install -y \
    xorg-dev \
    mesa-utils

# Glad packages (optional)
RUN apt install -y \
    python3-jinja2

# Switch to non-root user
USER ubuntu
