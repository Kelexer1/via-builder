#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

echo "==> Configuring CMake"
emcmake cmake -B build -DCMAKE_BUILD_TYPE=Release

echo "==> Building via-builder.wasm"
cmake --build build

echo "==> Copying build artifacts into dist/"
mkdir -p dist
cp build/via-builder.js dist/via-builder.js
cp build/via-builder.wasm dist/via-builder.wasm
cp types/via-builder.d.ts dist/via-builder.d.ts

echo "==> Compiling TypeScript wrapper"
npx tsc

echo "==> Build complete"