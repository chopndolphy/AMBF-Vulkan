#!/usr/bin/env bash
#
# setup.sh — install FIRE-3DV's system prerequisites on Ubuntu.
#
# Idempotent: each step checks whether the dependency is already present and
# skips it if so, so the script is safe to re-run. Installs into the system
# via apt, so it needs sudo (you'll be prompted once).
#
# Prerequisites handled:
#   1. Vulkan SDK (provides the loader, validation layers, and glslc)
#   2. SDL2 development headers
#   3. Sample glTF assets
#
set -euo pipefail

# ---- pretty output --------------------------------------------------------
if [ -t 1 ]; then
    GREEN=$'\033[0;32m'; YELLOW=$'\033[0;33m'; RED=$'\033[0;31m'; BOLD=$'\033[1m'; NC=$'\033[0m'
else
    GREEN=; YELLOW=; RED=; BOLD=; NC=
fi
ok()   { echo "${GREEN}✓${NC} $*"; }
skip() { echo "${YELLOW}•${NC} $*"; }
info() { echo "${BOLD}==>${NC} $*"; }
die()  { echo "${RED}✗ $*${NC}" >&2; exit 1; }

# ---- sanity checks --------------------------------------------------------
[ "$(uname -s)" = "Linux" ] || die "This project targets Linux (Ubuntu). Detected $(uname -s)."
command -v apt-get >/dev/null 2>&1 || die "apt-get not found. This script supports Debian/Ubuntu only."

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CODENAME="$(. /etc/os-release && echo "${UBUNTU_CODENAME:-${VERSION_CODENAME:-jammy}}")"

info "Detected Ubuntu codename: ${CODENAME}"

# ---- 1. Vulkan SDK --------------------------------------------------------
info "Checking Vulkan SDK..."
if command -v vulkaninfo >/dev/null 2>&1 && command -v glslc >/dev/null 2>&1; then
    skip "Vulkan SDK already installed (vulkaninfo and glslc found)."
else
    info "Installing Vulkan SDK from the LunarG repository..."
    if [ ! -f /etc/apt/trusted.gpg.d/lunarg.asc ]; then
        wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc \
            | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc >/dev/null
    fi
    if [ ! -f "/etc/apt/sources.list.d/lunarg-vulkan-${CODENAME}.list" ]; then
        sudo wget -qO "/etc/apt/sources.list.d/lunarg-vulkan-${CODENAME}.list" \
            "http://packages.lunarg.com/vulkan/lunarg-vulkan-${CODENAME}.list"
    fi
    sudo apt-get update
    sudo apt-get install -y vulkan-sdk vulkan-validationlayers-dev spirv-tools
    ok "Vulkan SDK installed."
fi

# ---- 2. SDL2 --------------------------------------------------------------
info "Checking SDL2..."
if pkg-config --exists sdl2 2>/dev/null || dpkg -s libsdl2-dev >/dev/null 2>&1; then
    skip "SDL2 development headers already installed."
else
    info "Installing SDL2..."
    sudo apt-get install -y libsdl2-dev
    ok "SDL2 installed."
fi

# ---- 3. Sample assets -----------------------------------------------------
info "Checking sample assets..."
ASSETS_DIR="${REPO_ROOT}/assets"
if [ -d "${ASSETS_DIR}" ] && [ -n "$(ls -A "${ASSETS_DIR}" 2>/dev/null)" ]; then
    skip "assets/ already present and non-empty."
else
    skip "No assets found. Download glTF assets from:"
    echo "      https://github.com/FIRE-3DV-repositories/src_sample_assets/tree/main/glTF"
    echo "      and place them in: ${ASSETS_DIR}/"
fi

# ---- 4. Build -------------------------------------------------------------
echo
info "Building FIRE-3DV..."
mkdir -p "${REPO_ROOT}/build"
cd "${REPO_ROOT}/build"
cmake ..
make -j"$(nproc)"

echo
ok "${BOLD}Done.${NC} Run the engine from the build/ directory:"
echo "      cd build && ./FIRE3D                 # default sample scene"
echo "      cd build && ./FIRE3D ../assets/your_scene.glb   # a specific glTF/glb"
