<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=222222&height=80&section=header&text=tmenu&fontSize=52&fontColor=bbbbbb&fontAlignY=72&animation=fadeIn" alt="tmenu header">
  <br>
  <img src="https://readme-typing-svg.demolab.com?font=JetBrains+Mono&size=14&pause=1200&color=5599bb&center=true&vCenter=true&width=600&lines=Minimal+X11+menu+launcher;Small+source.+No+runtime+dependencies;Compile-time+configuration+only" alt="tmenu typing">
  <br>
  <img src="https://raw.githubusercontent.com/tinyopsec/assets/main/tmenu/tmenu.svg" style="width: 65%; height: auto;" alt="tmenu logo">
</div>

<p align="center">
  <img src="https://img.shields.io/badge/license-MIT-005577?style=flat&labelColor=222222&logo=opensourceinitiative&logoColor=005577" alt="license MIT">
  <img src="https://img.shields.io/badge/236%20lines-5599bb?style=flat&label=source&labelColor=222222&logo=files&logoColor=5599bb" alt="236 lines">
  <img src="https://img.shields.io/badge/%7E3%20MB%20RAM-5599bb?style=flat&label=idle&labelColor=222222&logo=speedtest&logoColor=5599bb" alt="about 3 MB RAM">
  <img src="https://img.shields.io/badge/X11%20%2F%20Xlib-005577?style=flat&label=display&labelColor=222222&logo=x.org&logoColor=005577" alt="X11 / Xlib">
</p>

> [!TIP]
> **Edit the macros in `tmenu.c` and rebuild.** There is no config file and no runtime flags for colors or fonts. Change `FGNORM`, `BGNORM`, `FONT` and run `make`.

## Contents

- [Quick Start](#quick-start)
- [Why small](#why-small)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Key Bindings](#key-bindings)
- [Configuration](#configuration)
- [How It Works](#how-it-works)
- [Contributing](#contributing)
- [Star History](#star-history)
- [Related](#related)

---

## Quick Start

```sh
git clone --depth 1 https://github.com/tinyopsec/tmenu
cd tmenu
make && sudo make install
printf 'st\nfirefox\nhtop\n' | tmenu | sh
```

> [!NOTE]
> tmenu reads stdin until EOF. Start it from a terminal without a pipe and nothing appears until you press Ctrl+D. That is expected, not a bug.
>
> The default launcher wrapper is `tmenu_run`, which scans `$PATH` and caches the result.

---

## Why small

I cut out what I do not use: no config file parser, no IPC, no scripting layer, no drawing library. Only the filter-and-print loop is left.

| Area | tmenu |
| --- | --- |
| Lines of code | 236 (fits in one reading session, verified with `wc -l`) |
| RAM at idle | ~3 MB on my PC (2.7 MB RSS, 2822 items in the list) |
| Runtime dependencies | Xlib only |
| Config | single flat set of macros in `tmenu.c`, no patch stack |
| Drawing | X core fonts, four colors, no images |
| IPC / scripting | none |
| Audit surface | one file, one function per concern |

No text shaping, no color scheme beyond four hex values, no runtime configuration. Read the source, understand the program.

---

## Features

What I actually use, nothing extra:

| Category | Details |
| --- | --- |
| Input | newline-delimited list on stdin, typed filter, one line back on stdout |
| Filtering | two passes: prefix matches first, then substring matches; selection resets to first match |
| No match | the typed text is returned, so Enter on an unknown query still yields something useful |
| Modes | one-line bar (default) or N-line list with `-l N` |
| Position | top (default) or bottom with `-b` |
| Prompt | optional label with `-p text` |
| Keys | Enter, Escape, Backspace, arrows, Ctrl+U, Ctrl+W, Ctrl+K, Ctrl+J |
| Cache | `tmenu_run` keeps the `$PATH` listing in `$XDG_CACHE_HOME/tmenu_run`, rebuilds only when stale |
| Startup | about 2.5 ms after warmup (X connection dominates the first launch) |
| Memory | ~3 MB RSS, independent of list size |
| Build | clean under `gcc` or `clang` with `-std=c99 -pedantic -Wall -Wextra` |

---

<details>
<summary><h2>Installation</h2></summary>

### Requirements

You need a C compiler (`gcc` or `clang`) and the Xlib headers. Install them with your system package manager. No runtime dependencies beyond Xlib.

### From Source

```sh
git clone --depth 1 https://github.com/tinyopsec/tmenu
cd tmenu
make
sudo make install   # installs to /usr/local/bin
```

Change `PREFIX` in the `Makefile` (or pass it on the command line) to install elsewhere, e.g. `sudo make install PREFIX=/usr`. To stage the install (packaging), use `make DESTDIR=/tmp/stage install`.

### AUR (Arch Linux)

```sh
yay -S tmenu
```

Package: [aur.archlinux.org/packages/tmenu](https://aur.archlinux.org/packages/tmenu)

<details>
<summary>FreeBSD / OpenBSD / NetBSD / DragonFly</summary>

Install Xlib via the system package manager. The `Makefile` defaults to the Linux paths. For BSDs, edit `X11INC` and `X11LIB` at the top of the `Makefile`; the alternatives are already present but commented out, for example FreeBSD / DragonFly:

```make
X11INC = /usr/local/include
X11LIB = /usr/local/lib
```

NetBSD (pkgsrc) uses `/usr/pkg/include` and `/usr/pkg/lib`. OpenBSD uses `/usr/X11R6`.

Then build normally:

```sh
make && sudo make install
```

</details>

### Uninstall

```sh
sudo make uninstall
```

</details>

---

## Usage

### Starting tmenu

Add to `~/.xinitrc` or a wrapper script:

```sh
printf 'st\nfirefox\nhtop\n' | tmenu | sh
```

`tmenu` has no built-in autostart and no display manager integration. It is a filter: pipe a list in, get a line out.

### Launcher

`tmenu_run` scans `$PATH`, caches the result, filters, and runs the selection through `sh`. That is the usual entry point:

```sh
tmenu_run
```

### Display managers

tmenu is not a session and provides no `.desktop` file. Launch it from your WM's keybinding or from `.xinitrc`.

---

<details>
<summary><h2>Key Bindings</h2></summary>

The window is driven by the keyboard. There is no mouse support and no configuration at runtime.

| Key | Action |
| --- | --- |
| Enter, KP_Enter | take the selection, or the typed text if nothing matches |
| Escape | quit, status 1 |
| Backspace | delete one character |
| Up, Left | previous item |
| Down, Right | next item |
| Ctrl+U | clear the input |
| Ctrl+W | delete the last word |
| Ctrl+K | previous item (vi style) |
| Ctrl+J | next item (vi style) |

Key handling lives in `kp()`. Remap by editing the switch and rebuilding.

</details>

---

## Configuration

I configure tmenu by editing `tmenu.c`. There is no config file, no IPC, no reload mechanism.

> [!IMPORTANT]
> After every change to `tmenu.c`, run `make && sudo make install` and restart tmenu.

### Macros

| Macro | Default | Meaning |
| --- | --- | --- |
| `FONT` | `-*-fixed-medium-r-*-*-13-*-*-*-*-*-*-*` | X core font string |
| `FGNORM` | `#bbbbbb` | normal text |
| `BGNORM` | `#222222` | normal background |
| `FGSEL` | `#eeeeee` | selected text |
| `BGSEL` | `#005577` | selected background |
| `TOP` | `1` | 1 puts the window at the top, 0 at the bottom |
| `LINES` | `0` | 0 means one line, any other value means a list |
| `PROMPT` | `NULL` | default prompt text |

`MAXITEMS` (65536) and `MAXTEXT` (256) cap the item count and the input length. Both are compile-time too.

### Colors

Colors go through `XAllocColor`, so anything `XParseColor` understands works: `#8b0000`, `red`, `rgb:aa/00/00`. A string X cannot parse is fatal, by design, so a typo shows up at startup instead of an unreadable menu.

### Cache

`tmenu_run` keeps the `$PATH` listing in `$XDG_CACHE_HOME/tmenu_run` (or `~/.cache/tmenu_run`) and only rebuilds it when the file is missing or older than one of the `$PATH` directories. Adding or removing a binary touches the directory, so the next launch rescans. A warm launch skips the scan entirely.

---

## How It Works

tmenu reads stdin until EOF, so the window does not appear until the input is complete. That is why `printf ... | tmenu` works and a bare `tmenu` in a terminal waits for Ctrl+D.

Filtering is two passes over the list: items that start with the query go first, items that merely contain it follow. Filtering also resets the selection to the first match, so Enter takes the best one without touching the arrows.

If the query matches nothing, the typed text is returned as-is. Enter on an unknown query prints the query; Escape and empty input print nothing and exit with status 1.

The window is an override-redirect X window, one line tall by default, full screen width. With `-l N` it becomes a list of N lines plus one row for the input. Keyboard input is grabbed with `XGrabKeyboard`, so keys reach tmenu regardless of focus.

`tmenu_run` scans `$PATH` once, sorts the names, and writes them to the cache file. On later launches it compares the cache mtime against each `$PATH` directory mtime and skips the scan if nothing changed.

---

## Contributing

Found a bug, or something just feels off? Open an issue. Steps to reproduce and your setup, that's all I need.

<details>
<summary>Code requirements</summary>

- Xlib and nothing else
- No wrappers, no abstraction layers
- Clean build: `gcc -std=c99 -pedantic -Wall -Wextra`
- Optional stuff goes behind `#ifdef` or a compile-time constant. Core event loop changes get a second look from me.
- Under 300 lines

</details>

---

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=tinyopsec/tmenu&type=Date)](https://star-history.com/#tinyopsec/tmenu&Date)

---

## Related

### Suckless ecosystem

| Project | Link |
| --- | --- |
| st terminal | [st.suckless.org](https://st.suckless.org) |
| dmenu | [tools.suckless.org/dmenu](https://tools.suckless.org/dmenu/) |
| suckless.org | [suckless.org](https://suckless.org) |

### Specifications

| Spec | Link |
| --- | --- |
| Xlib manual | [x.org](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html) |

### Packages

| Distro | Link |
| --- | --- |
| AUR (Arch) | [aur.archlinux.org/packages/tmenu](https://aur.archlinux.org/packages/tmenu) |


<img src="https://capsule-render.vercel.app/api?type=waving&color=222222&height=80&section=footer" alt="tmenu footer">
