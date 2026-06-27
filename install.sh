#!/bin/sh
set -eu

REPO="${MERCURY_REPO:-MasonSanders/mercurychessengine}"
INSTALL_DIR="${MERCURY_INSTALL_DIR:-/usr/local/bin}"
BINARY_NAME="${MERCURY_BINARY_NAME:-mercury}"

die() {
	echo "error: $*" >&2
	exit 1
}

have() {
	command -v "$1" >/dev/null 2>&1
}

need() {
	have "$1" || die "$1 is required"
}

detect_arch() {
	case "$(uname -m)" in
		x86_64|amd64)
			echo "x86_64"
			;;
		*)
			die "unsupported architecture: $(uname -m). Mercury currently provides Linux x86_64 packages."
			;;
	esac
}

detect_os() {
	case "$(uname -s)" in
		Linux)
			echo "linux"
			;;
		*)
			die "unsupported operating system: $(uname -s). Mercury currently provides Linux packages."
			;;
	esac
}

detect_libc() {
	# Prefer glibc if both glibc and musl tooling are visible.
	if have getconf && getconf GNU_LIBC_VERSION >/dev/null 2>&1; then
		echo "glibc"
		return
	fi

	ldd_output="$(ldd --version 2>&1 || true)"
	case "$ldd_output" in
		*GNU\ libc*|*GLIBC*|*glibc*)
			echo "glibc"
			;;
		*musl*|*Musl*)
			echo "musl"
			;;
		*)
			if ls /lib/ld-musl-*.so.1 /usr/lib/ld-musl-*.so.1 >/dev/null 2>&1; then
				echo "musl"
			else
				die "could not detect glibc or musl libc"
			fi
			;;
	esac
}

latest_tag() {
	if [ -n "${MERCURY_VERSION:-}" ]; then
		case "$MERCURY_VERSION" in
			v*) echo "$MERCURY_VERSION" ;;
			*) echo "v$MERCURY_VERSION" ;;
		esac
		return
	fi

	curl -fsSIL -o /dev/null -w '%{url_effective}' "https://github.com/$REPO/releases/latest" |
		sed 's#.*/tag/##'
}

install_binary() {
	src="$1"
	dst="$INSTALL_DIR/$BINARY_NAME"

	if [ -d "$INSTALL_DIR" ] && [ -w "$INSTALL_DIR" ]; then
		install -m 755 "$src" "$dst"
	elif have sudo; then
		sudo install -m 755 "$src" "$dst"
	else
		die "$INSTALL_DIR is not writable and sudo is not available. Set MERCURY_INSTALL_DIR to a writable directory."
	fi
}

need curl
need tar
need sed
need awk
need sha256sum
need install

os="$(detect_os)"
arch="$(detect_arch)"
libc="$(detect_libc)"
tag="$(latest_tag)"
version="${tag#v}"

[ -n "$version" ] || die "could not determine latest Mercury release version"

asset="mercury-$version-$os-$arch-$libc.tar.gz"
base_url="https://github.com/$REPO/releases/download/$tag"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

archive="$tmpdir/$asset"
checksum="$archive.sha256"

echo "Downloading Mercury $version for $os/$arch/$libc..."
curl -fsSL "$base_url/$asset" -o "$archive"
curl -fsSL "$base_url/$asset.sha256" -o "$checksum"

expected="$(awk '{print $1; exit}' "$checksum")"
actual="$(sha256sum "$archive" | awk '{print $1; exit}')"

[ "$expected" = "$actual" ] || die "checksum verification failed"

tar -xzf "$archive" -C "$tmpdir"
binary="$tmpdir/mercury-$version-$os-$arch-$libc/mercury"

[ -x "$binary" ] || die "release archive did not contain an executable mercury binary"

install_binary "$binary"

echo "Mercury $version installed to $INSTALL_DIR/$BINARY_NAME"
