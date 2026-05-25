#!/usr/bin/env bash
# shellcheck source=/dev/null
if [[ -n "${_TC_COMMON_LOADED:-}" ]]; then
  return 0
fi
_TC_COMMON_LOADED=1

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
LOCAL_ENV="${REPO_ROOT}/local.env"
LOCAL_DIR="${REPO_ROOT}/local"
BUILD_DIR="${BUILD_DIR:-build}"
BIN_DIR="${REPO_ROOT}/${BUILD_DIR}/bin"
BUILD_BIN_NAMES=(worldserver bnetserver mapextractor vmap4extractor vmap4assembler mmaps_generator)
LOCAL_ETC="${LOCAL_DIR}/etc"
LOCAL_DATA="${LOCAL_DIR}/data"
LOCAL_LOGS="${LOCAL_DIR}/logs"
LOCAL_RUN="${LOCAL_DIR}/run"
LOCAL_SQL="${LOCAL_DIR}/sql"
LOCAL_CACHE="${LOCAL_DIR}/.cache"

load_local_env() {
  if [[ -f "$LOCAL_ENV" ]]; then
    # shellcheck disable=SC1090
    set -a
    source "$LOCAL_ENV"
    set +a
  fi
  BUILD_DIR="${BUILD_DIR:-build}"
  BIN_DIR="${REPO_ROOT}/${BUILD_DIR}/bin"
}

load_local_env

log_info() { printf '\033[1;32m[INFO]\033[0m %s\n' "$*"; }
log_warn() { printf '\033[1;33m[WARN]\033[0m %s\n' "$*"; }
log_err()  { printf '\033[1;31m[ERR]\033[0m %s\n' "$*" >&2; }

require_cmd() {
  local cmd
  for cmd in "$@"; do
    if ! command -v "$cmd" &>/dev/null; then
      log_err "Required command not found: $cmd"
      exit 1
    fi
  done
}

resolve_mysql_client() {
  if [[ -n "${MYSQL_CLIENT:-}" ]] && command -v "$MYSQL_CLIENT" &>/dev/null; then
    printf '%s\n' "$(command -v "$MYSQL_CLIENT")"
    return 0
  fi
  if command -v mariadb &>/dev/null; then
    printf '%s\n' "$(command -v mariadb)"
    return 0
  fi
  if command -v mysql &>/dev/null; then
    printf '%s\n' "$(command -v mysql)"
    return 0
  fi
  log_err "No MariaDB/MySQL client found. On Arch: sudo pacman -S mariadb"
  exit 1
}

mysql_cli_args() {
  local client
  client="$(resolve_mysql_client)"
  MYSQL_CLIENT="$client"
  if [[ -n "${MYSQL_SOCKET:-}" && -S "$MYSQL_SOCKET" ]]; then
    printf '%s' "$client"
    printf ' --protocol=SOCKET --socket=%q' "$MYSQL_SOCKET"
  else
    printf '%s -h%q -P%s' "$client" "${MYSQL_HOST:-127.0.0.1}" "${MYSQL_PORT:-3306}"
  fi
}

mysql_exec() {
  local user="$1" pass="$2"
  shift 2
  local args client
  client="$(resolve_mysql_client)"
  if [[ -n "${MYSQL_SOCKET:-}" && -S "$MYSQL_SOCKET" ]]; then
    args=(--protocol=SOCKET --socket="$MYSQL_SOCKET" -u"$user")
  else
    args=(-h"${MYSQL_HOST:-127.0.0.1}" -P"${MYSQL_PORT:-3306}" -u"$user")
  fi
  [[ -n "$pass" ]] && args+=(-p"$pass")
  "$client" "${args[@]}" "$@"
}

mysql_exec_root() {
  if mysql_exec "${MYSQL_ROOT_USER:-root}" "${MYSQL_ROOT_PASS:-}" "$@" 2>/dev/null; then
    return 0
  fi
  if is_arch && command -v sudo &>/dev/null; then
    local client args
    client="$(resolve_mysql_client)"
    args=()
    if [[ -n "${MYSQL_SOCKET:-}" && -S "$MYSQL_SOCKET" ]]; then
      args=(--protocol=SOCKET --socket="$MYSQL_SOCKET")
    else
      args=(-h"${MYSQL_HOST:-127.0.0.1}" -P"${MYSQL_PORT:-3306}")
    fi
    sudo "$client" "${args[@]}" "$@"
    return $?
  fi
  mysql_exec "${MYSQL_ROOT_USER:-root}" "${MYSQL_ROOT_PASS:-}" "$@"
}

mysql_exec_trinity() {
  mysql_exec "${MYSQL_USER:-trinity}" "${MYSQL_PASS:-trinity}" "$@"
}

ensure_local_dirs() {
  mkdir -p "$LOCAL_DIR" "$LOCAL_ETC" "$LOCAL_DATA" "$LOCAL_LOGS" "$LOCAL_RUN" "$LOCAL_SQL" "$LOCAL_CACHE"
  mkdir -p "$LOCAL_DATA"/{maps,vmaps,mmaps,dbc,gt}
}

ensure_build_bins() {
  local name src
  local -a missing=()
  mkdir -p "$BIN_DIR"
  for name in "${BUILD_BIN_NAMES[@]}"; do
    if [[ -x "${BIN_DIR}/${name}" ]]; then
      continue
    fi
    src="$(find "${REPO_ROOT}/${BUILD_DIR}" -name "$name" -type f -executable 2>/dev/null | head -1)"
    if [[ -n "$src" ]]; then
      ln -sf "$src" "${BIN_DIR}/${name}"
    else
      missing+=("$name")
    fi
  done
  if ((${#missing[@]} > 0)); then
    log_err "Build did not produce: ${missing[*]}"
    log_err "Re-run with TOOLS=ON and SERVERS=ON (see scripts/build.sh)."
    exit 1
  fi
  if [[ -f "${REPO_ROOT}/src/server/bnetserver/bnetserver.cert.pem" ]]; then
    cp -fn "${REPO_ROOT}/src/server/bnetserver/bnetserver.cert.pem" "${BIN_DIR}/" 2>/dev/null || true
    cp -fn "${REPO_ROOT}/src/server/bnetserver/bnetserver.key.pem" "${BIN_DIR}/" 2>/dev/null || true
  fi
}

wait_for_port() {
  local host="$1" port="$2" tries="${3:-30}"
  local i
  for ((i = 0; i < tries; i++)); do
    if (echo >/dev/tcp/"$host"/"$port") &>/dev/null; then
      return 0
    fi
    sleep 1
  done
  return 1
}

is_arch() {
  [[ -f /etc/arch-release ]] || grep -qi arch /etc/os-release 2>/dev/null
}
