#!/usr/bin/env bash
#
# Compile the LM3S6965 firmware. Forwards arguments to make:
#   ./scripts/build.sh           # default target (elf + bin)
#   ./scripts/build.sh clean
#   ./scripts/build.sh asm
#
# Requires arm-none-eabi-gcc on PATH. Run ./scripts/setup.sh first if missing.
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PROJECT_DIR="$REPO_ROOT/arm-cortex-m3/LM3S6965"
CC=arm-none-eabi-gcc

if [ -t 1 ]; then
    GREEN='\033[1;32m'; RED='\033[1;31m'; RESET='\033[0m'
else
    GREEN=''; RED=''; RESET=''
fi
log() { printf "%b==>%b %s\n" "$GREEN" "$RESET" "$*"; }
err() { printf "%bxx%b %s\n"  "$RED"   "$RESET" "$*" >&2; }

if ! command -v "$CC" >/dev/null 2>&1; then
    err "$CC not found on PATH."
    err "Run ./scripts/setup.sh to install the toolchain, then retry."
    exit 1
fi

log "make $* in $PROJECT_DIR"
make -C "$PROJECT_DIR" "$@"

if [ -d "$PROJECT_DIR/builds" ]; then
    log "Build artifacts:"
    ls -lh "$PROJECT_DIR/builds/"
fi
