# Project 4: KVS Cache

See wiki for instructions: https://github.com/UCSC-CSE-130/project4-kvs-cache/wiki/README

## Notes self
- `cumscript.sh` relies on `parse.rs`, which is currently hosted on `http://cum.ucsc.gay/`. To self host, run `cargo build` and `cargo run` in `parse.rs` working directory, then modify `runscript.sh` line change `server_url='http://cum.ucsc.gay/` to `server_url='localhost:3030'`.