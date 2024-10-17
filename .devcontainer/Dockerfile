FROM ubuntu:24.04

# Install necessary packages
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt upgrade -y

RUN apt install -y \ 
    git \
    g++ \
    cmake

# Switch to non-root user
USER ubuntu
