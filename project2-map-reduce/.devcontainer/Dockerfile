FROM ubuntu:22.04

RUN apt-get clean && apt-get update && apt-get install -y \
  git \
  python3 \
  python3-pip \
  python3-venv \
  clang-14 \
  clang-format-14 \
  llvm-14 \
  lldb \
  bats \
  valgrind

RUN ln -s /usr/bin/clang-14 /usr/bin/clang
RUN ln -s /usr/bin/clang-format-14 /usr/bin/clang-format
