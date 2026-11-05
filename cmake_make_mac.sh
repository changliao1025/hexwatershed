#!/bin/bash
set -e

# ==============================================================================
# HexWatershed Build Script for macOS using CMake
# ==============================================================================

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BIN_DIR="${PROJECT_DIR}/bin"

echo "=== HexWatershed macOS CMake Build ==="
echo "Project directory: ${PROJECT_DIR}"

# Locate cmake binary on macOS
CMAKE_BIN=""
if command -v cmake >/dev/null 2>&1; then
    CMAKE_BIN="$(command -v cmake)"
elif [ -x "/opt/homebrew/bin/cmake" ]; then
    CMAKE_BIN="/opt/homebrew/bin/cmake"
elif [ -x "/usr/local/bin/cmake" ]; then
    CMAKE_BIN="/usr/local/bin/cmake"
elif [ -x "/Applications/CMake.app/Contents/bin/cmake" ]; then
    CMAKE_BIN="/Applications/CMake.app/Contents/bin/cmake"
elif [ -n "$CONDA_PREFIX" ] && [ -x "$CONDA_PREFIX/bin/cmake" ]; then
    CMAKE_BIN="$CONDA_PREFIX/bin/cmake"
fi

if [ -z "$CMAKE_BIN" ]; then
    echo "Error: 'cmake' was not found on your system."
    echo "Please install it via Homebrew:"
    echo "    brew install cmake"
    exit 1
fi

echo "Using CMake: ${CMAKE_BIN} ($("${CMAKE_BIN}" --version | head -n 1))"

# Compiler selection: honor user CXX/CC or auto-detect Homebrew GCC 14
if [ -n "$CXX" ]; then
    echo "Using custom C++ compiler: ${CXX}"
elif command -v g++-14 >/dev/null 2>&1; then
    export CC="gcc-14"
    export CXX="g++-14"
    echo "Detected Homebrew GCC 14: using CC=gcc-14 CXX=g++-14"
fi

# Detect OpenMP if libomp is installed via Homebrew (for Apple Clang)
CMAKE_EXTRA_ARGS=()
if [ -d "/opt/homebrew/opt/libomp" ]; then
    CMAKE_EXTRA_ARGS+=("-DOpenMP_ROOT=/opt/homebrew/opt/libomp")
    echo "Found Homebrew libomp at /opt/homebrew/opt/libomp"
elif [ -d "/usr/local/opt/libomp" ]; then
    CMAKE_EXTRA_ARGS+=("-DOpenMP_ROOT=/usr/local/opt/libomp")
    echo "Found Homebrew libomp at /usr/local/opt/libomp"
fi

# Clean and create build and bin directories
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
mkdir -p "${BIN_DIR}"

cd "${BUILD_DIR}"

BUILD_TYPE="${BUILD_TYPE:-Release}"
echo "Configuring project (${BUILD_TYPE})..."
"${CMAKE_BIN}" "${PROJECT_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DINSTALL_DIR="${BIN_DIR}" \
    "${CMAKE_EXTRA_ARGS[@]}"

# Determine number of CPU cores for parallel build
NCPU=$(sysctl -n hw.ncpu 2>/dev/null || echo 4)
echo "Building with ${NCPU} threads..."
make -j"${NCPU}"
make install

if [ -x "${BIN_DIR}/hexwatershed" ]; then
    echo "=========================================="
    echo "Build successful!"
    echo "Binary located at: ${BIN_DIR}/hexwatershed"
    echo "=========================================="
else
    echo "Build finished, but binary not found at ${BIN_DIR}/hexwatershed"
    exit 1
fi
