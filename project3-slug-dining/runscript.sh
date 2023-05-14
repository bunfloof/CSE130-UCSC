#!/bin/bash

commands=(
  "make clean"
  "make"
  "./example1"
  "./example2"
  "./example3"
  "./examplecodex1"
  "./examplecodex2"
  "./examplediscord1"
  "./examplediscord2"
  "./examplediscord3"
  "./examplediscord4"
  "./examplediscord5"
)

for cmd in "${commands[@]}"; do
  echo "💦 runscript.sh: Running $cmd"
  $cmd
  echo ""
done