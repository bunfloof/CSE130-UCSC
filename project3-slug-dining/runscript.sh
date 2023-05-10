#!/bin/bash

commands=(
  "make clean"
  "make"
  "./example1"
  "./example2"
)

for cmd in "${commands[@]}"; do
  echo "💦 runscript.sh: Running $cmd"
  $cmd
  echo ""
done