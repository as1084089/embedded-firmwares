#!/usr/bin/env bash
#
# One-shot environment setup:
#   - arm-none-eabi-gcc (cross-compiler for Cortex-M3)
#   - qemu-system-arm   (emulator for the LM3S6965 board)
#   - zstd              (runtime dep of cc1 from ARM GNU Toolchain 14.x on macOS)
#
# Idempotent: re-running with everything installed is a no-op.
#

set -euo pipefail

CC=arm-none-eabi-gcc
QEMU=qemu-system-arm

if [ -t 1 ]; then
    GREEN='\033[1;32m'; YELLOW='\033[1;33m'; RED='\033[1;31m'; RESET='\033[0m'
else
    GREEN=''; YELLOW=''; RED=''; RESET=''
fi
log()  { printf "%b==>%b %s\n" "$GREEN"  "$RESET" "$*"; }
warn() { printf "%b==>%b %s\n" "$YELLOW" "$RESET" "$*" >&2; }
err()  { printf "%bxx%b %s\n"  "$RED"    "$RESET" "$*" >&2; }

require_brew_macos() {
    if ! command -v brew >/dev/null 2>&1; then
        err "Homebrew not found. Install it from https://brew.sh and rerun."
        exit 1
    fi
}

ensure_macos_runtime_deps() {
    # ARM GNU Toolchain 14.x's cc1 dyn-links libzstd.1.dylib at the Homebrew
    # prefix (/usr/local/opt/zstd/lib on Intel, /opt/homebrew/opt/zstd/lib on
    # Apple Silicon). The cask doesn't declare this, so install zstd ourselves
    # — even when the toolchain is already on PATH, the missing dep crashes
    # cc1 at first compile.
    [ "$(uname -s)" = "Darwin" ] || return 0
    command -v brew >/dev/null 2>&1 || return 0

    if ! brew list --formula zstd >/dev/null 2>&1; then
        log "Installing zstd (runtime dep of ARM GCC's cc1 on macOS)..."
        brew install zstd
    fi
}

install_arm_gcc_macos() {
    require_brew_macos
    log "Installing ARM GCC toolchain via Homebrew (cask: gcc-arm-embedded)..."
    if brew install --cask gcc-arm-embedded; then
        return 0
    fi
    warn "cask install failed; falling back to the 'arm-none-eabi-gcc' formula."
    brew install arm-none-eabi-gcc
}

install_arm_gcc_linux() {
    if command -v apt-get >/dev/null 2>&1; then
        log "Installing gcc-arm-none-eabi via apt-get (sudo)..."
        sudo apt-get update
        sudo apt-get install -y gcc-arm-none-eabi
    elif command -v dnf >/dev/null 2>&1; then
        log "Installing arm-none-eabi-gcc-cs via dnf (sudo)..."
        sudo dnf install -y arm-none-eabi-gcc-cs
    elif command -v pacman >/dev/null 2>&1; then
        log "Installing arm-none-eabi-{gcc,binutils,newlib} via pacman (sudo)..."
        sudo pacman -S --noconfirm arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib
    else
        err "Unsupported Linux distribution. Install $CC manually."
        exit 1
    fi
}

install_qemu_macos() {
    require_brew_macos
    log "Installing QEMU via Homebrew..."
    brew install qemu
}

install_qemu_linux() {
    if command -v apt-get >/dev/null 2>&1; then
        log "Installing qemu-system-arm via apt-get (sudo)..."
        sudo apt-get update
        sudo apt-get install -y qemu-system-arm
    elif command -v dnf >/dev/null 2>&1; then
        log "Installing qemu-system-arm via dnf (sudo)..."
        sudo dnf install -y qemu-system-arm
    elif command -v pacman >/dev/null 2>&1; then
        log "Installing qemu-arch-extra via pacman (sudo)..."
        sudo pacman -S --noconfirm qemu-arch-extra
    else
        err "Unsupported Linux distribution. Install $QEMU manually."
        exit 1
    fi
}

ensure_arm_gcc() {
    ensure_macos_runtime_deps

    if command -v "$CC" >/dev/null 2>&1; then
        log "$CC found: $("$CC" --version | head -1)"
        return 0
    fi

    warn "$CC not found; attempting auto-install."
    case "$(uname -s)" in
        Darwin) install_arm_gcc_macos ;;
        Linux)  install_arm_gcc_linux ;;
        *)
            err "Unsupported OS: $(uname -s). Install $CC manually."
            exit 1
            ;;
    esac

    hash -r 2>/dev/null || true
    if ! command -v "$CC" >/dev/null 2>&1; then
        err "$CC still not on PATH after install."
        err "Open a new shell or check your PATH, then rerun this script."
        exit 1
    fi
    log "$CC installed: $("$CC" --version | head -1)"
}

ensure_qemu() {
    if command -v "$QEMU" >/dev/null 2>&1; then
        log "$QEMU found: $("$QEMU" --version | head -1)"
        return 0
    fi

    warn "$QEMU not found; attempting auto-install."
    case "$(uname -s)" in
        Darwin) install_qemu_macos ;;
        Linux)  install_qemu_linux ;;
        *)
            err "Unsupported OS: $(uname -s). Install $QEMU manually."
            exit 1
            ;;
    esac

    hash -r 2>/dev/null || true
    if ! command -v "$QEMU" >/dev/null 2>&1; then
        err "$QEMU still not on PATH after install."
        exit 1
    fi
    log "$QEMU installed: $("$QEMU" --version | head -1)"
}

ensure_arm_gcc
ensure_qemu
log "Setup complete."
