# Install the ESP32 Toolchain on Linux

This guide recreates the toolchain used by the system-wide `esp32-dev` NixOS specialisation in [NixOS-From-Scratch](https://github.com/redterminal-org/NixOS-From-Scratch). The host Rust toolchain and espup version are pinned; espup installs the latest supported Xtensa toolchain.

## Toolchain versions

| Component | Version |
| --- | --- |
| Rust and Cargo host toolchain used to install espup | `1.99.1` |
| espup | `0.18.0` |
| Xtensa Rust toolchain installed by espup | Latest supported by espup (not pinned) |
| ESP-IDF | `v5.1.4` |
| Arduino-ESP32 component | `3.0.7` |
| espflash | `3.3.0` |

Install native prerequisites and run the setup as your regular user, not as root.

## 1. Native prerequisites

### Debian and Ubuntu

```sh
sudo apt update
sudo apt install -y \
  build-essential git curl wget cmake ninja-build pkg-config \
  python3 python3-venv python3-pip flex bison gperf ccache \
  libffi-dev libssl-dev libusb-1.0-0-dev libncurses-dev \
  dfu-util perl clang lld llvm unzip
```

### Fedora

```sh
sudo dnf install -y \
  @development-tools git curl wget cmake ninja-build pkgconf-pkg-config \
  python3 python3-pip flex bison gperf ccache \
  libffi-devel openssl-devel libusb1-devel ncurses-devel \
  dfu-util perl clang lld llvm unzip
```

### Arch Linux

```sh
sudo pacman -Syu --needed \
  base-devel git curl wget cmake ninja pkgconf python python-pip \
  flex bison gperf ccache libffi openssl libusb ncurses \
  dfu-util perl clang lld llvm unzip
```

Package names vary by release; install equivalent development packages if a name is unavailable.

## 2. Install rustup and the host toolchain

If Rust and rustup are not installed, use the official installer at [rustup.rs](https://rustup.rs/), review it, and follow its instructions. Open a new shell and verify:

```sh
rustup --version
rustup toolchain install 1.99.1 --profile minimal
RUSTUP_TOOLCHAIN=1.99.1 rustc --version
RUSTUP_TOOLCHAIN=1.99.1 cargo --version
```

## 3. Install espup and Xtensa tools

Install espup `0.18.0` using the host toolchain:

```sh
RUSTUP_TOOLCHAIN=1.99.1 cargo install espup --version 0.18.0 --locked --force
```

Install for the classic ESP32 and save the environment script:

```sh
mkdir -p "$HOME/.config/esp32-dev"
"$HOME/.cargo/bin/espup" install \
  --targets esp32 \
  --export-file "$HOME/.config/esp32-dev/export-esp.sh"
```

Omitting `--toolchain-version` asks espup to install its latest supported Xtensa toolchain. For an existing installation, run `espup update --targets esp32 --export-file "$HOME/.config/esp32-dev/export-esp.sh"` to update it to the latest available version. If `espup` is installed elsewhere, find it with `command -v espup` and use that path.

Load the toolchain in the current shell and ensure its Rust source component is installed. Cargo's project configuration enables `build-std` for this Xtensa target, which needs `rust-src`:

```sh
. "$HOME/.config/esp32-dev/export-esp.sh"
rustup component add rust-src --toolchain esp
export RUSTUP_TOOLCHAIN=esp
export IDF_TARGET=esp32
export ESP_IDF_VERSION=tag:v5.1.4
```

The export script configures the Xtensa compiler and related paths. To load it automatically in Bash, add this to `~/.bashrc`:

```sh
if [ -r "$HOME/.config/esp32-dev/export-esp.sh" ]; then
  . "$HOME/.config/esp32-dev/export-esp.sh"
  export RUSTUP_TOOLCHAIN=esp
  export IDF_TARGET=esp32
  export ESP_IDF_VERSION=tag:v5.1.4
fi
```

Open a new shell after editing the file. If the toolchain is already installed, run the `rustup component add rust-src --toolchain esp` command once before building.

## 4. ESP-IDF tools

This project configures `esp-idf-sys` to use ESP-IDF `v5.1.4` and install its native tools in the standard user tool directory on first build. You do not need to clone ESP-IDF manually for this Cargo project. Keep Perl, Make, a POSIX shell, OpenSSL development files, CMake, Ninja, and Python available on `PATH`.

Verify the environment:

```sh
printf 'IDF_TARGET=%s\nESP_IDF_VERSION=%s\nRUSTUP_TOOLCHAIN=%s\n' \
  "$IDF_TARGET" "$ESP_IDF_VERSION" "$RUSTUP_TOOLCHAIN"
rustc --version
cargo --version
xtensa-esp32-elf-gcc --version
```

The expected values are `esp32`, `tag:v5.1.4`, and `esp`; the Xtensa compiler should be available after sourcing the export script.

## 5. Build and flash

Install the project's pinned flash utility using the host toolchain:

```sh
RUSTUP_TOOLCHAIN=1.99.1 cargo install espflash --version 3.3.0 --locked
```

Clone and build:

```sh
git clone https://github.com/redterminal-org/esp32-blink-rust.git
cd esp32-blink-rust
cargo build --release
```

The repository's `.cargo/config.toml` enables `build-std = ["std", "panic_abort"]` for `xtensa-esp32-espidf`. This requires the `rust-src` component to be installed for the `esp` toolchain as described above.

Connect the board and find its serial device:

```sh
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

Flash the firmware and open the serial monitor (change the port as required):

```sh
espflash flash --monitor --port /dev/ttyUSB0 target/xtensa-esp32-espidf/release/esp32-blink-rust
```

The first build downloads and compiles Rust crates, ESP-IDF, and the Arduino-ESP32 component.

## 6. Serial permissions

Inspect the serial device with `ls -l /dev/ttyUSB0`. If necessary, add your user to the appropriate serial access group (often `dialout` or `uucp`) and log out and back in. Do not run the build as root.

## Troubleshooting

- **Missing `core` crate or Xtensa target:** check that `RUSTUP_TOOLCHAIN=esp` is active, `rust-src` is installed for `esp`, and the project's `build-std` configuration is present.
- **Perl or `sh` missing:** install Perl and a POSIX shell (usually supplied by Bash) and ensure both are on `PATH`.
- **OpenSSL fails to build:** install OpenSSL development files and verify that Perl, Make, and the shell are available.
- **Xtensa tools not found:** source `~/.config/esp32-dev/export-esp.sh`.
- **Wrong Rust toolchain:** set `RUSTUP_TOOLCHAIN=esp`.
- **Serial access denied:** check device-group membership and start a new login session.
- **LED does not blink:** this example expects an LED on GPIO 2 of a classic ESP32.

See [README.md](README.md) for the project workflow.
