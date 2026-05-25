#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

IMPORT=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    --import) IMPORT=1; shift ;;
    -h|--help)
      echo "Usage: $0 [--import]"
      echo "  Creates MariaDB user/databases. --import loads SQL dumps."
      exit 0
      ;;
    *) log_err "Unknown option: $1"; exit 1 ;;
  esac
done

require_cmd "$(basename "$(resolve_mysql_client)")"

MYSQL_USER="${MYSQL_USER:-trinity}"
MYSQL_PASS="${MYSQL_PASS:-trinity}"
REALM_NAME="${REALM_NAME:-Trinity}"
REALM_PORT="${REALM_PORT:-8085}"
REALM_GAMEBUILD="${REALM_GAMEBUILD:-54261}"

log_info "Using MariaDB client: $(resolve_mysql_client)"

mysql_exec_root <<SQL
CREATE DATABASE IF NOT EXISTS auth CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE DATABASE IF NOT EXISTS world CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE DATABASE IF NOT EXISTS characters CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE DATABASE IF NOT EXISTS hotfixes CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'localhost' IDENTIFIED BY '${MYSQL_PASS}';
CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'127.0.0.1' IDENTIFIED BY '${MYSQL_PASS}';
GRANT ALL PRIVILEGES ON auth.* TO '${MYSQL_USER}'@'localhost', '${MYSQL_USER}'@'127.0.0.1';
GRANT ALL PRIVILEGES ON world.* TO '${MYSQL_USER}'@'localhost', '${MYSQL_USER}'@'127.0.0.1';
GRANT ALL PRIVILEGES ON characters.* TO '${MYSQL_USER}'@'localhost', '${MYSQL_USER}'@'127.0.0.1';
GRANT ALL PRIVILEGES ON hotfixes.* TO '${MYSQL_USER}'@'localhost', '${MYSQL_USER}'@'127.0.0.1';
FLUSH PRIVILEGES;
SQL

log_info "Databases and user '${MYSQL_USER}' ready."

import_sql() {
  local db="$1" file="$2"
  if [[ ! -f "$file" ]]; then
    log_warn "Skip missing: $file"
    return 1
  fi
  log_info "Importing $(basename "$file") -> ${db}..."
  mysql_exec_root "$db" <"$file"
}

if [[ "$IMPORT" -eq 1 ]]; then
  if [[ -f "${REPO_ROOT}/sql/Databases/auth.sql" ]]; then
    import_sql auth "${REPO_ROOT}/sql/Databases/auth.sql"
    import_sql characters "${REPO_ROOT}/sql/Databases/characters.sql"
    import_sql world "${REPO_ROOT}/sql/Databases/world.sql"
    import_sql hotfixes "${REPO_ROOT}/sql/Databases/hotfixes.sql"
  else
    [[ -f "${REPO_ROOT}/sql/base/auth_database.sql" ]] && \
      import_sql auth "${REPO_ROOT}/sql/base/auth_database.sql"
    [[ -f "${REPO_ROOT}/sql/base/characters_database.sql" ]] && \
      import_sql characters "${REPO_ROOT}/sql/base/characters_database.sql"
    TDB_WORLD_FILE="${TDB_WORLD_FILE:-TDB_full_world_343.23121_2023_12_20.sql}"
    TDB_HOTFIXES_FILE="${TDB_HOTFIXES_FILE:-TDB_full_hotfixes_343.23121_2023_12_20.sql}"
    for candidate in "${LOCAL_SQL}/${TDB_WORLD_FILE}" "${BIN_DIR}/${TDB_WORLD_FILE}"; do
      [[ -f "$candidate" ]] && import_sql world "$candidate" && break
    done
    for candidate in "${LOCAL_SQL}/${TDB_HOTFIXES_FILE}" "${BIN_DIR}/${TDB_HOTFIXES_FILE}"; do
      [[ -f "$candidate" ]] && import_sql hotfixes "$candidate" && break
    done
  fi

  mysql_exec "${MYSQL_USER}" "${MYSQL_PASS}" auth <<SQL
UPDATE realmlist SET
  name='${REALM_NAME}',
  address='127.0.0.1',
  localAddress='127.0.0.1',
  port=${REALM_PORT},
  gamebuild=${REALM_GAMEBUILD}
WHERE id=1;
INSERT IGNORE INTO realmlist (id,name,address,localAddress,localSubnetMask,port,icon,flag,timezone,allowedSecurityLevel,population,gamebuild,Region,Battlegroup)
VALUES (1,'${REALM_NAME}','127.0.0.1','127.0.0.1','255.255.255.0',${REALM_PORT},1,0,1,0,0,${REALM_GAMEBUILD},1,1);
SQL
  log_info "Seeded realmlist (port ${REALM_PORT}, build ${REALM_GAMEBUILD})."
fi

log_info "Done. Use --import to load SQL, or let servers AutoSetup on first start."
