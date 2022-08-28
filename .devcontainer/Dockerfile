# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.245.0/containers/debian/.devcontainer/base.Dockerfile

# [Choice] Debian version (use bullseye on local arm64/Apple Silicon): bullseye, buster
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}

# ** [Optional] Uncomment this section to install additional packages. **
RUN dpkg --add-architecture armel && \
    apt-get update && \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install --no-install-recommends make libasound2-dev:armel gcc-arm-linux-gnueabi libc6-dev-armel-cross && \
    rm -rf /var/lib/apt/lists/*
