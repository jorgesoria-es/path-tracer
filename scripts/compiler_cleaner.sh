#!/bin/bash
# Limpia todo rastro de compilación y salidas de tests

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "Cleaning project at: $ROOT_DIR"

# 1. eliminar carpeta build
if [ -d "$ROOT_DIR/build" ]; then
    echo "  - Deleting build/"
    rm -rf "$ROOT_DIR/build"
fi

# 2. eliminar carpeta output
if [ -d "$ROOT_DIR/output" ]; then
    echo "  - Deleting output/"
    rm -rf "$ROOT_DIR/output"
fi

echo "Cleaning complete."