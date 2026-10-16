# ESP32 Blink in Rust

A minimal Rust firmware example for the classic ESP32 using an Arduino C++ bridge and a system-wide NixOS development environment.

The LED is connected to **GPIO 2** and blinks continuously: **500 ms off, then 500 ms on**.

## Requirements

- A classic ESP32 board with an LED wired to GPIO 2.
- A USB data cable and serial-device access.
- The system-wide `esp32-dev` NixOS specialisation from [NixOS-From-Scratch](https://github.com/redterminal-org/NixOS-From-Scratch), or the equivalent setup in [INSTALL-TOOLCHAIN.md](INSTALL-TOOLCHAIN.md).
- Network access for the first build to download Cargo crates, ESP-IDF, and the Arduino component.

GPIO 2 is common on classic ESP32 development boards, but not universal. This example assumes GPIO 2 as requested.

## Build on NixOS

1. Boot into the `esp32-dev` or `ollama-esp32-dev` specialisation.
2. Open a new login shell so the toolchain environment is loaded.
3. Clone and enter the repository:

   ```sh
   git clone https://github.com/redterminal-org/esp32-blink-rust.git
   cd esp32-blink-rust
   ```

4. Check the environment:

   ```sh
   rustc --version
   cargo --version
   echo "$IDF_TARGET"
   ```

   `IDF_TARGET` should be `esp32`, and the selected Rust toolchain should be `esp`.

5. Build:

   ```sh
   cargo build --release
   ```

The first build may take a while because ESP-IDF and the Arduino component need to be downloaded and compiled.

## Build on another Linux distribution

Follow [INSTALL-TOOLCHAIN.md](INSTALL-TOOLCHAIN.md) to install the latest Xtensa Rust toolchain supported by espup and the native build prerequisites. Source the generated `export-esp.sh`, then run:

```sh
cargo build --release
```

## Find the serial port

Connect the board and inspect serial devices:

```sh
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

Common names include `/dev/ttyUSB0` and `/dev/ttyACM0`. The exact name depends on your USB-to-serial adapter and operating system.

On Linux, your user may need permission to access the serial device, commonly through the `dialout` or `uucp` group. Log out and back in after changing group membership. Do not run the whole build as root just to access the port.

## Flash and monitor

Install the pinned flash utility if needed:

```sh
RUSTUP_TOOLCHAIN=1.99.1 cargo install espflash
```

Flash the generated ELF and open the serial monitor. Replace the example port with your device:

```sh
espflash flash --monitor --port /dev/ttyUSB0 target/xtensa-esp32-espidf/release/esp32-blink-rust
```

If your `espflash` version reports different syntax, check `espflash flash --help`. Some boards may require holding the BOOT button while connecting.

Exit the serial monitor using its documented shortcut (commonly Ctrl+]). The LED keeps blinking after the monitor closes.

## Clean rebuild

```sh
cargo clean
cargo build --release
```

## Troubleshooting

- **Xtensa target not found:** source the `export-esp.sh` created by espup and check `RUSTUP_TOOLCHAIN=esp`.
- **Wrong target:** set `export IDF_TARGET=esp32`.
- **OpenSSL build fails with Perl or `sh` missing:** install Perl and a POSIX shell and make sure they are on `PATH`.
- **Serial permission denied:** update serial-device group membership and start a new login session.
- **LED stays off:** confirm GPIO 2 wiring, board power, and LED polarity. Some boards use a different pin or active-low LED.

## Toolchain versions

The project uses Rust and Cargo `1.99.1` as the host toolchain to install espup `0.18.0`. espup installs or updates the classic ESP32 Xtensa toolchain to its latest supported version. ESP-IDF remains `v5.1.4`, Arduino-ESP32 `3.0.7`, and espflash `3.3.0`. See [INSTALL-TOOLCHAIN.md](INSTALL-TOOLCHAIN.md) for setup and update instructions.

## License

MIT; see [LICENSE](LICENSE).
