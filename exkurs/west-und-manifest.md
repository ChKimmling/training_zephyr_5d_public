# west Meta-Tool und Manifest

`west` ist Zephyrs **Meta-Tool**: Es verwaltet einen Workspace aus mehreren Git-Repositories
(Zephyr, Module, HALs, eigene Projekte) und stellt Befehle zum Bauen, Flashen und Debuggen
bereit. Welche Repositories in welcher Version dazugehören, steht im **Manifest** (`west.yml`).

## Aufbau eines Workspaces

```text
training-zephyr-ws/
├── .west/config        # Workspace-Konfiguration (Pfad zum Manifest-Repo)
├── training-zephyr/    # Manifest-Repo, enthält west.yml
├── zephyr/             # vom Manifest verwaltete Projekte
├── modules/            # (Module, HALs, ...)
└── build/              # Build-Verzeichnisse
```

In diesem Workspace ist `training-zephyr` das Manifest-Repository (siehe `.west/config`:
`[manifest] path = training-zephyr`, `file = west.yml`). Zephyr selbst ist ein normales
Projekt, das im Manifest auf eine feste Version (hier `v4.4.2`) gepinnt ist.

## Befehle

| Befehl | Zweck |
|---|---|
| `west init -l <manifest-repo>` | Workspace um ein lokales Manifest-Repo anlegen |
| `west init -m <url> [--mr <rev>] <ordner>` | Workspace aus einem Remote-Manifest anlegen |
| `west update` | Alle Projekte auf die im Manifest angegebenen Revisionen bringen |
| `west list` | Projekte, Pfade und Revisionen anzeigen |
| `west status` / `west diff` | `git status`/`git diff` über alle Projekte |
| `west manifest --resolve` | Aufgelöstes Manifest (inkl. Imports) ausgeben |
| `west build`, `flash`, `debug` | Zephyr-Erweiterungsbefehle (aus dem Zephyr-Repo) |
| `west config` | Workspace-/Benutzer-Konfiguration lesen und setzen |
| `west help` | Alle verfügbaren Befehle anzeigen |

Die Befehle `build`, `flash` und `debug` stammen nicht aus west selbst, sondern aus dem
Zephyr-Repository (`west-commands: scripts/west-commands.yml` im Manifest).

## Das Manifest (`west.yml`)

```yaml
manifest:
  version: "1.0"

  remotes:
    - name: upstream
      url-base: https://github.com/zephyrproject-rtos

  defaults:
    remote: upstream

  group-filter: [-hal-stm32]

  projects:
    - name: zephyr
      revision: v4.4.2
      clone-depth: 1
      west-commands: scripts/west-commands.yml

    - name: cmsis
      revision: 512cc7e895e8491696b61f7ba8066b4a182569b8
      path: modules/hal/cmsis
```

| Schlüssel | Bedeutung |
|---|---|
| `remotes` | Basis-URLs, aus denen Projekte geklont werden |
| `defaults` | Vorgabe, z. B. das Standard-Remote |
| `projects` | Liste der Repositories; `name`, `revision` (Tag, Branch oder SHA), `path`, `repo-path` |
| `clone-depth` | Flacher Klon, spart Platz und Zeit |
| `groups`, `group-filter` | Projekte gruppieren und gezielt ausschließen/einschließen |
| `west-commands` | Pfad zu einer YAML-Datei mit weiteren west-Befehlen |
| `import` | Manifest eines anderen Projekts einbinden (z. B. Zephyrs `west.yml`) |
| `self` | Eigenschaften des Manifest-Repos selbst (z. B. `path`) |

Im Training werden alle Projekte **explizit** mit festen Revisionen aufgeführt; Zephyrs
eigenes Manifest wird nicht importiert. Dadurch ist der Stand reproduzierbar.

## Praxis

- Nach dem Wechsel der Manifest-Revision (oder einem `git pull` des Manifest-Repos):
  `west update`.
- Reproduzierbarkeit: Revisionen als **Tag oder SHA** pinnen, keine wandernden Branches.
- Ein Projekt gezielt weglassen: `group-filter` oder `west update <projekt>` nutzen.
- Ist ein Modul nicht im Manifest, findet Zephyr es nicht (typisch: fehlendes HAL beim Build).

## Referenz

- Zephyr-Dokumentation: [West (Zephyr's meta-tool)](https://docs.zephyrproject.org/latest/develop/west/index.html)
- Zephyr-Dokumentation: [West Manifests](https://docs.zephyrproject.org/latest/develop/west/manifest.html)
- Zephyr-Dokumentation: [Built-in Commands](https://docs.zephyrproject.org/latest/develop/west/built-in.html)
- Zephyr-Dokumentation: [Workspace-Struktur](https://docs.zephyrproject.org/latest/develop/west/workspaces.html)
