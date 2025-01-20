extern crate serde;
extern crate serde_json;

use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
struct Person {
    name: String,
    age: u8,
}

// Function hello_rust_cargo without manglng
#[no_mangle]
pub extern "C" fn hello_rust_cargo_main() {
    ////
    use nix::fcntl::{open, OFlag};
    use nix::sys::stat::Mode;
    use nix::ioctl_write_int_bad;
    use nix::unistd::sleep;
    let fd = open(
        "/dev/userleds",
        OFlag::O_WRONLY,
        Mode::empty()
    ).unwrap();
    println!("fd={fd}");

    const ULEDIOC_SETALL: i32 = 0x1d03;
    ioctl_write_int_bad!(led_set_all, ULEDIOC_SETALL);

    // Equivalent to ioctl(fd, ULEDIOC_SETALL, 1)
    unsafe { led_set_all(fd, 1).unwrap(); }

    sleep(2);

    // Equivalent to ioctl(fd, ULEDIOC_SETALL, 0)
    unsafe { led_set_all(fd, 0).unwrap(); }
    ////

    // Print hello world to stdout

    let john = Person {
        name: "John".to_string(),
        age: 30,
    };

    let json_str = serde_json::to_string(&john).unwrap();
    println!("{}", json_str);

    let jane = Person {
        name: "Jane".to_string(),
        age: 25,
    };

    let json_str_jane = serde_json::to_string(&jane).unwrap();
    println!("{}", json_str_jane);

    let json_data = r#"
        {
            "name": "Alice",
            "age": 28
        }"#;

    let alice: Person = serde_json::from_str(json_data).unwrap();
    println!("Deserialized: {} is {} years old", alice.name, alice.age);

    let pretty_json_str = serde_json::to_string_pretty(&alice).unwrap();
    println!("Pretty JSON:\n{}", pretty_json_str);

    tokio::runtime::Builder::new_current_thread()
        .enable_all()
        .build()
        .unwrap()
        .block_on(async {
            println!("Hello world from tokio!");
        });

    loop {
        // Do nothing
    }
}
