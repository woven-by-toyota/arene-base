// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// https://docs.rs/rand_xoshiro/latest/rand_xoshiro/struct.Xoshiro256PlusPlus.html
use rand_xoshiro::Xoshiro256PlusPlus;
use rand_xoshiro::rand_core::{Rng,SeedableRng};
use std::io::{self, Write};

fn main() {
    let seed: u64 = std::env::args()
        .nth(1)
        .unwrap_or_else(|| {
            eprintln!(
                "usage: bazel run {} -- <seed>",
                "//arene/base/testing/tests:rand_xoshiro256plusplus_dump"
            );
            std::process::exit(1);
        })
        .parse()
        .unwrap_or_else(|_| {
            eprintln!("error: invalid seed");
            std::process::exit(1);
        });

    let mut rng = Xoshiro256PlusPlus::seed_from_u64(seed);

    let stdout = std::io::stdout();
    let mut out = stdout.lock();

    loop {
        let value = rng.next_u64();
        match out.write_all(&value.to_ne_bytes()) {
            Ok(()) => {}
            Err(e) if e.kind() == io::ErrorKind::BrokenPipe => {
                // Reader closed the pipe (e.g. `head` exited). Exit cleanly.
                return;
            }
            Err(e) => {
                eprintln!("write error: {e}");
                std::process::exit(1);
            }
        }
    }
}
