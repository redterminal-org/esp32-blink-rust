use esp_idf_sys as _;

extern "C" {
    fn arduino_bridge_init();
    fn arduino_bridge_step();
}

fn main() {
    unsafe {
        arduino_bridge_init();
    }

    loop {
        unsafe {
            arduino_bridge_step();
        }
    }
}
