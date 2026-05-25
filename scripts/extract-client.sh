#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

SKIP_MMAPS=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-mmaps) SKIP_MMAPS=1; shift ;;
    -h|--help)
      echo "Usage: $0 [--skip-mmaps]"
      echo "  Requires WOW_CLIENT_PATH in local.env"
      exit 0
      ;;
    *) log_err "Unknown option: $1"; exit 1 ;;
  esac
done

WOW_CLIENT_PATH="${WOW_CLIENT_PATH:-}"
if [[ -z "$WOW_CLIENT_PATH" || ! -d "$WOW_CLIENT_PATH" ]]; then
  log_err "Set WOW_CLIENT_PATH in local.env to your WoW install (CASC Data/)."
  exit 1
fi

require_cmd "$BIN_DIR/mapextractor" "$BIN_DIR/vmap4extractor" "$BIN_DIR/vmap4assembler"

ensure_local_dirs
WORK="${LOCAL_CACHE}/extract"
mkdir -p "$WORK"
cd "$WORK"

export WOW_CLIENT_PATH
log_info "Running mapextractor (client: ${WOW_CLIENT_PATH})..."
"$BIN_DIR/mapextractor" -i "$WOW_CLIENT_PATH" -o "$LOCAL_DATA"

log_info "Running vmap4extractor..."
"$BIN_DIR/vmap4extractor"
log_info "Running vmap4assembler..."
"$BIN_DIR/vmap4assembler" Buildings vmaps

if [[ -d vmaps ]]; then
  rm -rf "${LOCAL_DATA}/vmaps"
  mv vmaps "${LOCAL_DATA}/"
fi
if [[ -d Buildings ]]; then
  rm -rf Buildings
fi

if [[ "$SKIP_MMAPS" -eq 0 ]]; then
  require_cmd "$BIN_DIR/mmaps_generator"
  log_info "Running mmaps_generator (this may take a long time)..."
  "$BIN_DIR/mmaps_generator"
  if [[ -d mmaps ]]; then
    rm -rf "${LOCAL_DATA}/mmaps"
    mv mmaps "${LOCAL_DATA}/"
  fi
else
  log_info "Skipped mmaps (--skip-mmaps)."
fi

log_info "Extracted data in ${LOCAL_DATA}/"
