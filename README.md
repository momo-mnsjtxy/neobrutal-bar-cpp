# neobrutal-bar

A cross-platform (Linux + Windows) neobrutalist desktop status bar, written in
**C++17 / Qt 6**.

This is a from-scratch reimplementation of
[adriankarlen/neobrutal-zebar](https://github.com/adriankarlen/neobrutal-zebar)
— a [Zebar](https://github.com/glzr-io/zebar) configuration (Svelte + Tailwind)
that renders a status bar for Windows + GlazeWM. Because Zebar is a
web-rendered bar, this port keeps the **look and behaviour** but ships as a
single native Qt 6 executable, swapping the platform-specific pieces for native
equivalents on each OS:

| Concern        | Upstream (neobrutal-zebar)  | This port — Linux                  | This port — Windows                 |
| -------------- | --------------------------- | ---------------------------------- | ----------------------------------- |
| Runtime        | Zebar (web view) on Windows | Native Qt 6 Widgets app            | Native Qt 6 Widgets app             |
| Styling        | Tailwind CSS                | Custom `QPainter` drawing          | Custom `QPainter` drawing           |
| Workspaces     | GlazeWM (npm provider)      | EWMH / X11 virtual desktops (Xlib) | GlazeWM IPC (WebSocket)             |
| Now playing    | Spotify via GlazeWM         | MPRIS over D-Bus                   | System Media Transport Controls     |
| System metrics | Zebar providers             | `/proc` + `/sys`                   | Win32 (`GetSystemTimes`, WMI-free)  |
| Weather        | Zebar weather provider      | Open-Meteo HTTP API                | Open-Meteo HTTP API                 |
| Icons          | Tabler Icons webfont        | Same font, bundled as a resource   | Same font, bundled as a resource    |

On Windows the workspace widget talks to [GlazeWM](https://github.com/glzr-io/glazewm)
over its WebSocket IPC (default port `6123`) — the same window manager the
upstream targets. Without GlazeWM running, the centre workspace group is simply
empty; the rest of the bar works regardless.

## Look

The bar keeps the original's three-group layout and neobrutalist aesthetic —
thick borders, hard offset shadows (zero blur), flat bold colours, and buttons
that "press" into their shadow on hover:

- **Left** — a heart, plus memory / CPU / battery meters that reveal their exact
  percentage on hover.
- **Centre** — one button per workspace (filled dot = focused) followed by the
  icons of the apps open on the active workspace.
- **Right** — now-playing, network (wifi signal tiers / ethernet), weather, and
  a clock.

## Themes

Four built-in themes, ported from upstream: `rose-pine` (default),
`catppuccin`, `nord`, `material`.

```bash
neobrutal-bar --theme catppuccin
```

## Building

Requires CMake ≥ 3.16, a C++17 compiler, and Qt 6 (Widgets + Network; plus
WebSockets on Windows and D-Bus on Linux).

### Linux

```bash
# Debian/Ubuntu
sudo apt-get install -y cmake build-essential qt6-base-dev libx11-dev libgl1-mesa-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/neobrutal-bar
```

### Windows

Install Qt 6 (MSVC build) with the **Qt WebSockets** module — e.g. via the
[Qt online installer](https://www.qt.io/download) or
[`aqtinstall`](https://github.com/miurahr/aqtinstall) — then build with MSVC:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
cmake --build build --config Release --parallel
.\build\Release\neobrutal-bar.exe
```

Workspace switching requires [GlazeWM](https://github.com/glzr-io/glazewm) to be
running.

## Configuration

Without a config file the bar uses sensible defaults (Rosé Pine, primary
monitor, weather geolocated via IP). To customise, copy
[`config.example.json`](config.example.json) to
`~/.config/neobrutal-bar/config.json`:

```json
{
  "theme": "rose-pine",
  "screen": 0,
  "sizes": { "height": 40, "radius": 4, "shadowSizeBar": 4 },
  "weather": { "latitude": 59.3293, "longitude": 18.0686 }
}
```

CLI flags override the file: `--config <path>`, `--theme <name>`,
`--screen <index>`.

### Process icons

App → icon mapping is ported from upstream (`assets/data/icon_map.json` /
`ignored_apps.json`) and matched against each window's `WM_CLASS`. Unmapped apps
fall back to a generic icon; ignored apps are hidden.

## Notes & differences

- **Battery / CPU colours are usage-aware**: the meters tint toward the theme's
  "mid"/"low"/"high-usage" colours as values cross thresholds (upstream used a
  single static colour).
- On **Linux**, workspace switching is performed via `_NET_CURRENT_DESKTOP`
  client messages, so it works with any EWMH-compliant window manager; on
  **Windows** it sends a `focus --workspace` command over GlazeWM's IPC.
- If no now-playing source is present (no MPRIS player on Linux / no active
  media session on Windows) the now-playing widget hides itself; if no battery
  is present the battery meter hides itself.
- The config file lives at the platform config dir resolved by Qt —
  `~/.config/neobrutal-bar/config.json` on Linux and
  `%APPDATA%\neobrutal-bar\config.json` on Windows.

## Credits

- Original design & implementation:
  [adriankarlen/neobrutal-zebar](https://github.com/adriankarlen/neobrutal-zebar)
- [Tabler Icons](https://github.com/tabler/tabler-icons) (MIT) — bundled webfont.
- Weather data: [Open-Meteo](https://open-meteo.com/).

## License

MIT — see [LICENSE](LICENSE).
