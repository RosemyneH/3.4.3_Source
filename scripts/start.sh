#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

FOREGROUND=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    -f|--foreground) FOREGROUND=1; shift ;;
    -h|--help)
      echo "Usage: $0 [--foreground]"
      echo "  Default: daemon mode (no TC> console on your terminal)"
      echo "  --foreground: run worldserver in this shell with Console.Enable"
      exit 0
      ;;
    *) log_err "Unknown option: $1"; exit 1 ;;
  esac
done

ensure_local_dirs
ensure_build_bins
mkdir -p "$LOCAL_LOGS"

for stale in bnetserver worldserver; do
  if [[ -f "${LOCAL_RUN}/${stale}.pid" ]] && ! kill -0 "$(cat "${LOCAL_RUN}/${stale}.pid")" 2>/dev/null; then
    rm -f "${LOCAL_RUN}/${stale}.pid"
  fi
done

if ! mysql_exec_trinity -e "SELECT 1" &>/dev/null; then
  log_err "Cannot connect to MariaDB as ${MYSQL_USER:-trinity}. Run ./scripts/setup-database.sh"
  exit 1
fi

[[ -f "${BIN_DIR}/bnetserver" ]] || { log_err "Build first: ./scripts/build.sh"; exit 1; }
[[ -f "${LOCAL_ETC}/worldserver.conf" ]] || { log_err "Run ./scripts/setup-config.sh"; exit 1; }

if [[ ! -d "${LOCAL_DATA}/dbc" ]]; then
  log_warn "Missing ${LOCAL_DATA}/dbc/ — run ./scripts/extract-client.sh or copy extracted data."
fi

start_daemon() {
  local name="$1" conf="$2"
  local pidfile="${LOCAL_RUN}/${name}.pid"
  local out="${LOCAL_LOGS}/${name}.out"
  local err="${LOCAL_LOGS}/${name}.err"

  if [[ -f "$pidfile" ]] && kill -0 "$(cat "$pidfile")" 2>/dev/null; then
    log_warn "${name} already running (pid $(cat "$pidfile"))"
    return 0
  fi

  log_info "Starting ${name}..."
  cd "$BIN_DIR"
  nohup ./"${name}" -c "${conf}" >>"$out" 2>>"$err" </dev/null &
  echo $! >"$pidfile"
}

if [[ -f "${LOCAL_RUN}/bnetserver.pid" ]] && kill -0 "$(cat "${LOCAL_RUN}/bnetserver.pid")" 2>/dev/null; then
  log_warn "bnetserver already running (pid $(cat "${LOCAL_RUN}/bnetserver.pid"))"
else
  start_daemon bnetserver "${LOCAL_ETC}/bnetserver.conf"
  sleep 2
fi

if [[ "$FOREGROUND" -eq 1 ]]; then
  if [[ -f "${LOCAL_RUN}/worldserver.pid" ]] && kill -0 "$(cat "${LOCAL_RUN}/worldserver.pid")" 2>/dev/null; then
    log_err "worldserver already running (pid $(cat "${LOCAL_RUN}/worldserver.pid")). Run ./scripts/stop.sh first."
    exit 1
  fi
  log_info "Starting worldserver in foreground (TC> console)..."
  cd "$BIN_DIR"
  exec ./worldserver -c "${LOCAL_ETC}/worldserver.conf"
fi

start_daemon worldserver "${LOCAL_ETC}/worldserver.conf"

if ! wait_for_port 127.0.0.1 1119 30; then
  log_err "bnetserver did not open port 1119. Check ${LOCAL_LOGS}/Bnet.log"
  exit 1
fi
if ! wait_for_port 127.0.0.1 8081 30; then
  log_err "bnetserver REST login did not open port 8081."
  exit 1
fi
sleep 8
if ! wait_for_port 127.0.0.1 8085 120; then
  log_err "worldserver did not open port 8085. Check ${LOCAL_LOGS}/Server.log"
  exit 1
fi

realm_flag="$(mysql_exec_trinity auth -N -e "SELECT flag FROM realmlist WHERE id=1" 2>/dev/null || echo "")"
if [[ -n "$realm_flag" ]] && ((realm_flag & 2)); then
  log_warn "Realm still marked offline in DB; clearing flag..."
  mysql_exec_trinity auth -e "UPDATE realmlist SET flag = flag & ~2, population = 0 WHERE id = 1"
fi

log_info "Servers started. Logs: ${LOCAL_LOGS}/"
log_info "  tail -f ${LOCAL_LOGS}/worldserver.out"
log_info "Ports: bnet 1119, REST 8081, world 8085"
