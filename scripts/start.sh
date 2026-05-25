#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

ensure_local_dirs
ensure_build_bins

if ! mysql_exec_trinity -e "SELECT 1" &>/dev/null; then
  log_err "Cannot connect to MariaDB as ${MYSQL_USER:-trinity}. Run ./scripts/setup-database.sh"
  exit 1
fi

[[ -f "${BIN_DIR}/bnetserver" ]] || { log_err "Build first: ./scripts/build.sh"; exit 1; }
[[ -f "${LOCAL_ETC}/worldserver.conf" ]] || { log_err "Run ./scripts/setup-config.sh"; exit 1; }

if [[ ! -d "${LOCAL_DATA}/dbc" ]]; then
  log_warn "Missing ${LOCAL_DATA}/dbc/ — run ./scripts/extract-client.sh or copy extracted data."
fi

if [[ -f "${LOCAL_RUN}/bnetserver.pid" ]] && kill -0 "$(cat "${LOCAL_RUN}/bnetserver.pid")" 2>/dev/null; then
  log_warn "bnetserver already running (pid $(cat "${LOCAL_RUN}/bnetserver.pid"))"
else
  log_info "Starting bnetserver..."
  cd "$BIN_DIR"
  ./bnetserver -c "${LOCAL_ETC}/bnetserver.conf" &
  echo $! >"${LOCAL_RUN}/bnetserver.pid"
  sleep 2
fi

if [[ -f "${LOCAL_RUN}/worldserver.pid" ]] && kill -0 "$(cat "${LOCAL_RUN}/worldserver.pid")" 2>/dev/null; then
  log_warn "worldserver already running (pid $(cat "${LOCAL_RUN}/worldserver.pid"))"
else
  log_info "Starting worldserver..."
  cd "$BIN_DIR"
  ./worldserver -c "${LOCAL_ETC}/worldserver.conf" &
  echo $! >"${LOCAL_RUN}/worldserver.pid"
fi

log_info "Servers started. Logs: ${LOCAL_LOGS}/"
log_info "Ports: bnet 1119, REST 8081, world 8085"
