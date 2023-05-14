#!/bin/bash

commands=(
  "make clean"
  "make"
  "./example1"
  "./example2"
  "./example3"
  "./examplediscord1"
  "./examplediscord2"
  "./examplecodex1"
  "./examplecodex2"
)

for cmd in "${commands[@]}"; do
  echo "💦 runscript.sh: Running $cmd"
  $cmd
  echo ""
done