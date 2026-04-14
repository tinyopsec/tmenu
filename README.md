<div align="center">

  # tmenu

**Minimal X11 menu launcher - suckless, auditable, fast.**

<img src="https://raw.githubusercontent.com/tinyopsec/assets/main/tmenu/tmenu.png" width="720" alt="tmenu in action — horizontal launcher with filter input">

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-C99%20%2F%20POSIX-lightgrey.svg)]()
[![LOC](https://img.shields.io/badge/source-under%20250%20lines-brightgreen.svg)]()
[![Issues](https://img.shields.io/github/issues/tinyopsec/tmenu.svg)](https://github.com/tinyopsec/tmenu/issues)
[![Stars](https://img.shields.io/github/stars/tinyopsec/tmenu.svg)](https://github.com/tinyopsec/tmenu/stargazers)

</div>

---

`tmenu` is a minimal X11 menu launcher written in POSIX C99. It reads a newline-delimited list of items from `stdin`, lets you filter by typing, and executes the selected entry via `/bin/sh -c`.

The entire implementation fits in **a single source file** and **under 250 lines**. The only runtime dependency is Xlib.

---

## Contents

- [Overview](#overview)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Key Bindings](#key-bindings)
- [Configuration](#configuration)
- [Contributing](#contributing)
- [Related Projects](#related-projects)
- [License](#license)

---

## Overview

`tmenu` follows the suckless philosophy: small, auditable, compile-time configured. There is no config file parser, no IPC, no scripting layer, and no hidden complexity. Read the source - understand the program.

**Key properties:**

- Single source file, under 250 lines of C99
- Pure Xlib - no Xft, no Xinerama, no fontconfig
- All settings defined at the top of `tmenu.c`, applied at compile time
- Keyboard focus on launch — start typing immediately
- Executes the selected item or typed text directly via `/bin/sh -c`

**Compared to dmenu:** no external launcher wrapper required, smaller codebase, no Xft dependency.

**Supported platforms:** Linux (Arch, Void, Debian), OpenBSD, FreeBSD, NetBSD, macOS (XQuartz), and any POSIX system with X11.

---

## Requirements

| Dependency | Arch Linux | Debian / Ubuntu   |
|------------|------------|-------------------|
| Xlib       | `libx11`   | `libx11-dev`      |
| C compiler | `gcc`      | `build-essential` |

No other libraries are needed at runtime.

---

## Installation

### From source

```sh
git clone https://github.com/tinyopsec/tmenu
cd tmenu
make
sudo make install
```

Installs to `/usr/local/bin/tmenu`. To change the install prefix:

```sh
sudo make install PREFIX=/usr
```

### AUR (Arch Linux)

```sh
yay -S tmenu
```

### Uninstall

```sh
sudo make uninstall
```

---
## Usage

`tmenu` reads newline-delimited items from `stdin`, filters them interactively, and executes the selected entry via `/bin/sh -c`.

---

### Basic usage

Use the bundled launcher:

```sh
tmenu_run
```

Or provide input manually:

```sh
printf "st\nfirefox\nhtop\n" | tmenu
```

Select an item with arrow keys and press `Enter` to execute.

If the input does not match any item, the typed text is executed directly as a shell command.

---

### Integration with window managers

`tmenu` is typically used as an application launcher.

#### Example: replace `dmenu_run` in your WM

In your config (e.g. `swm.h`, `dwm/config.h`):

```c
static const char *dmenucmd[] = { "tmenu_run", NULL };
```

Recompile your WM:

```sh
make && sudo make install
```

After this, your launcher keybinding (usually `Mod + d`) will invoke `tmenu_run`.

---

### Using custom install paths

If `tmenu_run` is installed in `~/.local/bin`:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

Ensure this is set **before** your window manager starts (e.g. in `.xinitrc`), otherwise the launcher will fail silently.

---

### `.xinitrc` example

```sh
# optional: extend PATH for local binaries
export PATH="$HOME/.local/bin:$PATH"

# background services
picom &
feh --bg-scale ~/wallpaper.png &

exec swm
```

---

### Command-line options

| Option    | Description                                     |
| --------- | ----------------------------------------------- |
| `-b`      | Show menu at the bottom (default: top)          |
| `-l N`    | Vertical list with `N` visible lines            |
| `-p text` | Prompt displayed to the left of the input field |

---

### Examples

Vertical launcher with prompt:

```sh
printf "st\nfirefox\nthunderbird\n" | tmenu -l 5 -p "run: "
```

Power menu:

```sh
printf "suspend\nreboot\npoweroff\n" | tmenu -p "power: "
```

Bottom-aligned menu:

```sh
printf "lock\nlogout\n" | tmenu -b -p "session: "
```

---

### Debugging

Check that the launcher is available:

```sh
which tmenu_run
```

Run directly:

```sh
tmenu_run
```

If nothing happens on keybind — verify:

* `$PATH`
* WM config (`dmenucmd`)
* binary permissions (`chmod +x`)

---

### Behavior summary

* Input source: `stdin`
* Selection: keyboard (filter + navigation)
* Execution: `/bin/sh -c`
* Fallback: executes raw input if no match

---

Navigate with arrow keys or filter by typing. Press `Enter` to execute the selected item. If the typed text does not match any item, `Enter` executes it directly as a shell command.

### Command-line options

| Option    | Description                                          |
|-----------|------------------------------------------------------|
| `-b`      | Show menu at the bottom of the screen (default: top) |
| `-l N`    | Show a vertical list with `N` visible lines          |
| `-p text` | Display a prompt to the left of the input field      |

### Examples

Vertical list with a prompt:

```sh
printf "st\nfirefox\nthunderbird\n" | tmenu -p "run: " -l 5
```

Bottom-aligned horizontal menu:

```sh
printf "suspend\nreboot\npoweroff\n" | tmenu -b -p "power: "
```

---

## Key Bindings

| Key                  | Action                                    |
|----------------------|-------------------------------------------|
| `Enter` / `KP_Enter` | Execute selected item or typed text       |
| `Escape`             | Exit without executing                    |
| `BackSpace`          | Delete character left of cursor           |
| `Up` / `Left`        | Move selection up or left                 |
| `Down` / `Right`     | Move selection down or right              |
| `Ctrl+U`             | Clear entire input field                  |
| `Ctrl+W`             | Delete one word left of cursor            |
| `Ctrl+K`             | Select next item (vertical list)          |
| `Ctrl+J`             | Select previous item (vertical list)      |

Key handling is a straightforward `switch` in `tmenu.c`. To remap keys, edit the source and recompile.

---

## Configuration

`tmenu` has no runtime configuration file. All settings are defined as macros at the top of `tmenu.c` and take effect after recompilation:

```sh
$EDITOR tmenu.c
make && sudo make install
```

### Available settings

| Setting   | Default                                       | Description                           |
|-----------|-----------------------------------------------|---------------------------------------|
| `FONT`    | `"-*-fixed-medium-r-*-*-13-*-*-*-*-*-*-*"`   | X11 core font string                  |
| `FGNORM`  | `"#bbbbbb"`                                   | Normal item foreground color          |
| `BGNORM`  | `"#222222"`                                   | Normal item background color          |
| `FGSEL`   | `"#eeeeee"`                                   | Selected item foreground color        |
| `BGSEL`   | `"#005577"`                                   | Selected item background color        |
| `TOP`     | `1`                                           | `1` = top of screen, `0` = bottom     |
| `LINES`   | `0`                                           | Default line count (`0` = horizontal) |
| `PROMPT`  | `NULL`                                        | Default prompt string                 |

To increase the maximum number of items or input length, adjust `MAXITEMS` and `MAXTEXT` in `tmenu.c`.

---

## Contributing

Bug reports and patches are welcome via [GitHub issues](https://github.com/tinyopsec/tmenu/issues) and pull requests.

### Code style

- No comments in production code
- No external dependencies beyond Xlib
- No unnecessary abstraction
- Compiles clean: `gcc -std=c99 -pedantic -Wall -Wextra`
- Total source must remain under 300 lines

Features are added by patching the source, not by adding runtime options. Changes that require a config file or a new library dependency will not be accepted.

---

## Related Projects

| Project      | Description                              | Link |
|--------------|------------------------------------------|------|
| dmenu        | The original dynamic menu for X         | [suckless.org](https://tools.suckless.org/dmenu/) |
| suckless.org | Philosophy and tooling behind tmenu      | [suckless.org](https://suckless.org) |
| Xlib manual  | Reference for X11 programming            | [x.org](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html) |

---

## Star History

[![Star History Chart](https://api.star-history.com/chart?repos=tinyopsec/tmenu&type=date&legend=top-left)](https://www.star-history.com/?repos=tinyopsec%2Ftmenu&type=date&legend=top-left)

---

## License

MIT. See [LICENSE](LICENSE) for details.
