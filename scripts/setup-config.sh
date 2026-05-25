#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

ensure_local_dirs
require_cmd sed

MYSQL_USER="${MYSQL_USER:-trinity}"
MYSQL_PASS="${MYSQL_PASS:-trinity}"
MYSQL_HOST="${MYSQL_HOST:-127.0.0.1}"
MYSQL_PORT="${MYSQL_PORT:-3306}"
MYSQL_CLIENT="$(resolve_mysql_client)"

DB_CONN="${MYSQL_HOST};${MYSQL_PORT};${MYSQL_USER};${MYSQL_PASS}"

patch_conf() {
  local src="$1" dest="$2"
  sed -e "s|^DataDir = .*|DataDir = \"${LOCAL_DATA}\"|" \
      -e "s|^LogsDir = .*|LogsDir = \"${LOCAL_LOGS}\"|" \
      -e "s|^SourceDirectory  = .*|SourceDirectory  = \"${REPO_ROOT}\"|" \
      -e "s|^MySQLExecutable = .*|MySQLExecutable = \"${MYSQL_CLIENT}\"|" \
      -e "s|^LoginDatabaseInfo     = .*|LoginDatabaseInfo     = \"${DB_CONN};auth\"|" \
      -e "s|^WorldDatabaseInfo     = .*|WorldDatabaseInfo     = \"${DB_CONN};world\"|" \
      -e "s|^CharacterDatabaseInfo = .*|CharacterDatabaseInfo = \"${DB_CONN};characters\"|" \
      -e "s|^HotfixDatabaseInfo    = .*|HotfixDatabaseInfo    = \"${DB_CONN};hotfixes\"|" \
      -e "s|^LoginREST.LocalAddress=.*|LoginREST.LocalAddress=127.0.0.1|" \
      -e "s|^LoginREST.ExternalAddress=.*|LoginREST.ExternalAddress=127.0.0.1|" \
      -e "s|^BindIP = .*|BindIP = \"127.0.0.1\"|" \
      "$src" >"$dest"
}

WORLD_DIST="${REPO_ROOT}/src/server/worldserver/worldserver.conf.dist"
BNET_DIST="${REPO_ROOT}/src/server/bnetserver/bnetserver.conf.dist"

[[ -f "$WORLD_DIST" ]] || { log_err "Missing ${WORLD_DIST}"; exit 1; }
[[ -f "$BNET_DIST" ]] || { log_err "Missing ${BNET_DIST}"; exit 1; }

patch_conf "$WORLD_DIST" "${LOCAL_ETC}/worldserver.conf"

sed -e "s|^LogsDir = .*|LogsDir = \"${LOCAL_LOGS}\"|" \
    -e "s|^SourceDirectory  = .*|SourceDirectory  = \"${REPO_ROOT}\"|" \
    -e "s|^MySQLExecutable = .*|MySQLExecutable = \"${MYSQL_CLIENT}\"|" \
    -e "s|^LoginDatabaseInfo = .*|LoginDatabaseInfo = \"${DB_CONN};auth\"|" \
    -e "s|^LoginREST.LocalAddress=.*|LoginREST.LocalAddress=127.0.0.1|" \
    -e "s|^LoginREST.ExternalAddress=.*|LoginREST.ExternalAddress=127.0.0.1|" \
    -e "s|^BindIP = .*|BindIP = \"127.0.0.1\"|" \
    "$BNET_DIST" >"${LOCAL_ETC}/bnetserver.conf"

if [[ -d "$BIN_DIR" ]]; then
  ln -sf "${LOCAL_ETC}/worldserver.conf" "${BIN_DIR}/worldserver.conf"
  ln -sf "${LOCAL_ETC}/bnetserver.conf" "${BIN_DIR}/bnetserver.conf"
fi

log_info "Configs written to ${LOCAL_ETC}/"
log_info "MariaDB CLI in config: ${MYSQL_CLIENT}"
log_info "Done."
