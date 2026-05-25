#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

stop_pidfile() {
  local name="$1" file="${LOCAL_RUN}/${1}.pid"
  if [[ -f "$file" ]]; then
    local pid
    pid="$(cat "$file")"
    if kill -0 "$pid" 2>/dev/null; then
      log_info "Stopping ${name} (pid ${pid})..."
      kill -TERM "$pid" 2>/dev/null || true
      sleep 2
      kill -KILL "$pid" 2>/dev/null || true
    fi
    rm -f "$file"
  fi
}

stop_pidfile worldserver
stop_pidfile bnetserver
pkill -f "${BIN_DIR}/worldserver" 2>/dev/null || true
pkill -f "${BIN_DIR}/bnetserver" 2>/dev/null || true
log_info "Stopped."
