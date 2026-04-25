<div align="center">

**Minimal X11 menu launcher - suckless, auditable, fast.**

<img src="https://raw.githubusercontent.com/tinyopsec/assets/main/tmenu/tmenu.png" width="720" alt="tmenu in action">

[![License: MIT](https://img.shields.io/badge/license-MIT-blue?style=flat-square&color=4078c0)](LICENSE)
[![C99](https://img.shields.io/badge/C-C99%2FPOSIX-lightgrey?style=flat-square&color=6a4c93)](/)
[![LOC](https://img.shields.io/badge/source-under%20300%20lines-brightgreen?style=flat-square&color=2ea44f)](tmenu.c)
[![Issues](https://img.shields.io/github/issues/tinyopsec/tmenu?style=flat-square&color=e36209)](https://github.com/tinyopsec/tmenu/issues)
[![Stars](https://img.shields.io/github/stars/tinyopsec/tmenu?style=flat-square&color=f0c30f)](https://github.com/tinyopsec/tmenu/stargazers)
[![Minimalist](https://img.shields.io/badge/philosophy-suckless-informational?style=flat-square&color=005577)](https://suckless.org)

</div>

---

`tmenu` reads a newline-delimited list of items from `stdin`, lets you filter by typing, and executes the selected entry via `/bin/sh -c`. The entire implementation fits in a single C source file under 300 lines. The only runtime dependency is Xlib.

---

## Contents

- [Why tmenu?](#-why-tmenu)
- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Usage](#-usage)
- [Key Bindings](#-key-bindings)
- [Configuration](#-configuration)
- [Contributing](#-contributing)
- [Related Projects](#-related-projects)
- [Star History](#star-history)
- [License](#-license)

---

## Why tmenu?

| Feature | tmenu | dmenu | rofi |
|---|---|---|---|
| Source size | under 300 lines | ~2500 lines | ~15000 lines |
| Dependencies | Xlib only | Xlib, Xft | Xlib, cairo, pango, ... |
| Config file | none (compile-time) | none (compile-time) | yes |
| Executes typed text | yes | yes | no |
| Launch overhead | minimal | minimal | moderate |
| Auditable in an hour | yes | hard | no |

`tmenu` follows the suckless philosophy: small, auditable, compile-time configured. No config file parser, no IPC, no scripting layer. Read the source - understand the program.

---

## Quick Start

```sh
git clone https://github.com/tinyopsec/tmenu && cd tmenu && make && sudo make install
tmenu_run
```

---

## Installation

### From source

```sh
git clone https://github.com/tinyopsec/tmenu
cd tmenu
make
sudo make install
```

Installs to `/usr/local/bin/`. To change the prefix:

```sh
sudo make install PREFIX=/usr
```

Uninstall:

```sh
sudo make uninstall
```

### Requirements

| Dependency | ![Arch](https://img.shields.io/badge/Arch-1793D1?style=flat-square&logo=arch-linux&logoColor=white) | ![Debian](https://img.shields.io/badge/Debian-A81D33?style=flat-square&logo=debian&logoColor=white) |
|---|---|---|
| Xlib | `libx11` | `libx11-dev` |
| C compiler | `gcc` | `build-essential` |

### ![AUR](https://img.shields.io/badge/AUR-1793D1?style=flat-square&logo=arch-linux&logoColor=white) Arch Linux (AUR)

```sh
yay -S tmenu
```

### ![macOS](https://img.shields.io/badge/macOS-000000?style=flat-square&logo=apple&logoColor=white) macOS (XQuartz)

Install [XQuartz](https://www.xquartz.org/), then build from source as above.

### ![BSD](https://img.shields.io/badge/BSD-AB2B28?style=flat-square) OpenBSD / FreeBSD / NetBSD

Install `libX11` via ports or pkgsrc, then build from source.

---

## Usage

### Basic

```sh
# Use the bundled launcher (scans $PATH):
tmenu_run

# Pipe items manually:
printf "st\nfirefox\nhtop\n" | tmenu
```

Type to filter, navigate with arrow keys, press `Enter` to execute. If the typed text matches no item, it is executed directly as a shell command.

### Flags

| Flag | Description |
|---|---|
| `-b` | Show menu at the bottom of the screen |
| `-l N` | Vertical list with `N` visible lines |
| `-p text` | Prompt displayed left of the input field |

### Examples

```sh
# Vertical launcher with prompt:
printf "st\nfirefox\nthunderbird\n" | tmenu -l 5 -p "run: "

# Power menu at bottom:
printf "suspend\nreboot\npoweroff\n" | tmenu -b -p "power: "

# Session menu:
printf "lock\nlogout\n" | tmenu -b -p "session: "
```

### Window manager integration

**dwm / swm:**

```c
static const char *dmenucmd[] = { "tmenu_run", NULL };
```

Recompile your WM after editing the config.

**i3:**

```
bindsym $mod+d exec tmenu_run
```

**`.xinitrc` example:**

```sh
export PATH="$HOME/.local/bin:$PATH"
picom &
feh --bg-scale ~/wallpaper.png &
exec swm
```

<details>
<summary>Debugging</summary>

Check binary is available:

```sh
which tmenu
which tmenu_run
```

Run the launcher directly to test:

```sh
tmenu_run
```

If the keybinding does nothing, check:

- `$PATH` is set before your WM starts (set it in `.xinitrc`)
- WM config references `tmenu_run` correctly
- Binary is executable: `chmod +x /usr/local/bin/tmenu_run`

Test with explicit input:

```console
$ printf "echo hello\n" | tmenu
```

</details>

---

## ⌨ Key Bindings

| Key | Action |
|---|---|
| `Enter` / `KP_Enter` | Execute selected item or typed text |
| `Escape` | Exit without executing |
| `BackSpace` | Delete character left of cursor |
| `Up` / `Left` | Move selection up or left |
| `Down` / `Right` | Move selection down or right |
| `Ctrl+U` | Clear entire input field |
| `Ctrl+W` | Delete one word left of cursor |
| `Ctrl+K` | Move selection up (vertical list) |
| `Ctrl+J` | Move selection down (vertical list) |

To remap keys, edit the `kp()` function in `tmenu.c` and recompile.

---

## ⚙ Configuration

`tmenu` has no runtime config file. All settings are macros at the top of `tmenu.c`. Edit and recompile to apply changes:

```sh
$EDITOR tmenu.c
make && sudo make install
```

### Compile-time settings

| Macro | Default | Description |
|---|---|---|
| `FONT` | `"-*-fixed-medium-r-*-*-13-*-*-*-*-*-*-*"` | X11 core font string |
| `FGNORM` | `"#bbbbbb"` | Normal item foreground color |
| `BGNORM` | `"#222222"` | Normal item background color |
| `FGSEL` | `"#eeeeee"` | Selected item foreground color |
| `BGSEL` | `"#005577"` | Selected item background color |
| `TOP` | `1` | `1` = top of screen, `0` = bottom |
| `LINES` | `0` | Default line count (`0` = horizontal) |
| `PROMPT` | `NULL` | Default prompt string |

To increase limits, adjust `MAXITEMS` (default: 65536) and `MAXTEXT` (default: 256) in `tmenu.c`.

### Example customization

```diff
-#define BGSEL   "#005577"
+#define BGSEL   "#8b0000"

-#define FONT    "-*-fixed-medium-r-*-*-13-*-*-*-*-*-*-*"
+#define FONT    "-*-terminus-medium-r-*-*-14-*-*-*-*-*-*-*"
```

---

## Contributing

Bug reports and patches are welcome via [GitHub Issues](https://github.com/tinyopsec/tmenu/issues) and pull requests.

### Code style

- No comments in production code
- No external dependencies beyond Xlib
- No unnecessary abstraction
- Compiles clean: `gcc -std=c99 -pedantic -Wall -Wextra`
- Total source must remain under 300 lines

Features are added by patching the source, not by adding runtime options. Changes that require a config file or a new library dependency will not be accepted.

---

## Related Projects

| Project | Description | Link |
|---|---|---|
| dmenu | The original dynamic menu for X | [suckless.org](https://tools.suckless.org/dmenu/) |
| suckless.org | Philosophy behind tmenu | [suckless.org](https://suckless.org) |
| Xlib manual | Reference for X11 programming | [x.org](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html) |

---
## Star History
<a href="https://www.star-history.com/?repos=tinyopsec%2Ftmenu&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=tinyopsec/tmenu&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=tinyopsec/tmenu&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=tinyopsec/tmenu&type=date&legend=top-left" />
 </picture>
</a>
---

## 📄 License

MIT. See [LICENSE](LICENSE) for details.
