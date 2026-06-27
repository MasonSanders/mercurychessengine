# Mercury Chess Engine v1.0.0
Mercury is an actively maintained and updated CLI-based chess engine written in modern C++ (C++23). As it currently is, the engine is fairly weak, sitting at around a 1100 - 1250 ELO playing strength. The plan is to drastically increase playing strength as much as possible in future updates. Right now, Mercury is availible for x86_64 based linux with future plans for ARM support, as well as a potential port for MacOS. As of right now, there is no plan to ever implement a port for Microsoft Windows. Users on Windows who wish to use Mercury are free to install it through WSL.

## Features in v1.0.0
- Support for board representation using bitboards.
- Support for user positional input using FEN notation.
- Minimax with alpha-beta pruning for deciding best move up to a depth of 8.
- Transposition tables to avoid evaluating the same position twice.
- Multithreading at the root level for parallel search.
- Low-intermediate level playing strength.

## Installation
Mercury currently provides Linux x86_64 release packages for glibc and musl libc systems. The installer detects the user's libc and downloads the matching package from the latest GitHub release. If both glibc and musl are detected, it defaults to the glibc package.

Install the latest release:

```sh
curl -fsSL https://raw.githubusercontent.com/MasonSanders/mercurychessengine/master/install.sh | sh
```

By default, Mercury is installed to `/usr/local/bin/mercury`. To install into a different directory:

```sh
curl -fsSL https://raw.githubusercontent.com/MasonSanders/mercurychessengine/master/install.sh | MERCURY_INSTALL_DIR="$HOME/.local/bin" sh
```

To install a specific release version:

```sh
curl -fsSL https://raw.githubusercontent.com/MasonSanders/mercurychessengine/master/install.sh | MERCURY_VERSION=1.0.0 sh
```
