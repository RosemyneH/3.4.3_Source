#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

log_info "Installing build and runtime dependencies..."

if is_arch; then
  require_cmd pacman sudo
  arch_packages=(base-devel cmake git boost openssl mariadb readline ncurses)
  if pacman -Q zlib-ng-compat &>/dev/null; then
    log_info "zlib-ng-compat present — skipping conflicting zlib package."
  else
    arch_packages+=(zlib)
  fi
  sudo pacman -S --needed --noconfirm "${arch_packages[@]}"
  "${SCRIPT_DIR}/install-boost-process.sh"
  if [[ ! -d /var/lib/mysql/mysql ]] && [[ ! -f /var/lib/mysql/ibdata1 ]]; then
    log_info "Initializing MariaDB data directory..."
    sudo mariadb-install-db --user=mysql --basedir=/usr --datadir=/var/lib/mysql
  fi
  if ! systemctl is-active --quiet mariadb 2>/dev/null; then
    log_info "Enabling and starting mariadb.service..."
    sudo systemctl enable --now mariadb
  fi
  if ! mysql_exec_root -e "SELECT 1" &>/dev/null; then
    log_warn "MariaDB root login failed. Run: sudo mariadb-install-db --user=mysql --basedir=/usr --datadir=/var/lib/mysql"
    log_warn "Then: sudo mariadb-secure-installation (or set MYSQL_ROOT_PASS in local.env)"
  fi
else
  log_warn "Non-Arch system: install cmake, boost, openssl, mariadb (server + client), zlib, readline, ncurses manually."
  require_cmd cmake git
fi

log_info "MariaDB client: $(resolve_mysql_client)"
log_info "Done. Next: ./scripts/build.sh"
