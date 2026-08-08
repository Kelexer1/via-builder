#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"
echo "==> Configuring CMake"
emcmake cmake -B build-wasm -DCMAKE_BUILD_TYPE=Release
echo "==> Building via-builder.wasm"
cmake --build build-wasm
echo "==> Copying build artifacts into dist/"
mkdir -p dist
cp build-wasm/via-builder.js dist/via-builder.js
cp build-wasm/via-builder.wasm dist/via-builder.wasm
cp types/via-builder.d.ts dist/via-builder.d.ts
cp types/via-builder.d.ts wrapper/via-builder.d.ts
echo "==> Compiling TypeScript wrapper"
npx tsc
echo "==> Build complete"