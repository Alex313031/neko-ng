#!/bin/bash

# Copyright (c) 2026 Alex313031.

export HERE=${PWD} &&

export CLANG_FORMAT_FILE=${HERE}/.clang-format &&

# Check cfg program first
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/NekoCFG/{*.h,*.cpp} &&

clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/{*.h,*.cpp} &&

exit 0
