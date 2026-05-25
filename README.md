# World of Warcraft 3.4.3 Source Source code.

This used to be a project of mine in 2025 called Wrathion.

Since I'm not using it nor developing it anymore I decided to share it.

I'd say it's around 70% playable

Here's some gameplay on YouTube:

[![Gameplay](https://i3.ytimg.com/vi/0-zKKAwukdA/maxresdefault.jpg)](https://www.youtube.com/watch?v=0-zKKAwukdA)

## Local development

On Arch-based systems this project uses **MariaDB** (`mariadb` package and `mariadb.service`).

```bash
cp local.env.example local.env
./scripts/deploy-local.sh --import-db   # full setup; imports sql/Databases/*.sql if present
./scripts/status.sh
./scripts/stop.sh
```

Individual steps: `install-deps.sh`, `build.sh`, `fetch-sql.sh`, `fetch-tdb.sh`, `setup-config.sh`, `setup-database.sh [--import]`, `extract-client.sh`, `start.sh`.

The characters database must include `character_instance_lock` (with `difficulty`, `entranceWorldSafeLocId`) and `instance` for heroic lockouts and dungeon progress. Bundled dumps live under `sql/Databases/`; without them, run `fetch-sql.sh` and import `sql/base/characters_database.sql` before starting the worldserver. `./scripts/setup-database.sh --import` verifies those tables after import.
