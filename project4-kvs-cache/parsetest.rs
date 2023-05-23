use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

fn read_lines<P>(filename: P) -> io::Result<Vec<String>>
where P: AsRef<Path>, {
    let file = File::open(filename)?;
    let buf = io::BufReader::new(file);
    buf.lines().collect()
}

fn main() -> io::Result<()> {
    let commands = read_lines("commands.txt")?;
    let output = read_lines("output.txt")?;

    let mut interleaved = Vec::new();
    let mut output_index = 0;

    for command in commands {
        interleaved.push(command.clone());

        if !command.starts_with("SET") {
            if let Some(output_line) = output.get(output_index) {
                interleaved.push(output_line.clone());
            }
            output_index += 1;
        }
    }

    // Append the remaining lines from output
    if output_index < output.len() {
        for line in &output[output_index..] {
            interleaved.push(line.clone());
        }
    }

    for line in interleaved {
        println!("{}", line);
    }

    Ok(())
}
