*CHANGELOG*

## 0.1.0
### Configure ESP32 toolchain and standard-library build
* Use Rust and Cargo 1.99.1 as the host toolchain and espup 0.18.0.
* Install or update the classic ESP32 Xtensa toolchain to the latest version supported by espup instead of pinning 1.82.0.0.
* Enable Cargo's build-std configuration for xtensa-esp32-espidf and document installing rust-src for the esp toolchain.
* Update INSTALL-TOOLCHAIN.md while retaining ESP-IDF v5.1.4, Arduino-ESP32 3.0.7, and espflash 3.3.0.
