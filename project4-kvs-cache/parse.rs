use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use warp::Filter;
use std::convert::Infallible;
use warp::http::StatusCode;
use tokio::fs::write;
use futures::stream::StreamExt;
use bytes::Buf;

async fn handle_upload(mut files: warp::multipart::FormData) -> Result<impl warp::Reply, Infallible> {
    let mut commands = Vec::new();
    let mut output = Vec::new();
    let mut append_mode = false;
    
    while let Some(part) = files.next().await {
        let part = part.expect("multipart error");
        let name = part.name().to_string();
        let mut content = part.stream();
        let mut buffer = Vec::new();
        while let Some(chunk) = content.next().await {
            let mut chunk = chunk.unwrap();
            while chunk.has_remaining() {
                let chunk_slice = chunk.chunk();
                buffer.extend_from_slice(chunk_slice);
                chunk.advance(chunk_slice.len());
            }
        }
        match name.as_str() {
            "commands.txt" => commands = buffer,
            "output.txt" => output = buffer,
            "append_mode" => {
                if buffer == b"true" {
                    append_mode = true;
                }
            },
            _ => continue,
        };
    }

    write("commands.txt", &commands).await.unwrap();
    write("output.txt", &output).await.unwrap();
    
    let interleaved = interleaved("commands.txt", "output.txt", append_mode).unwrap();
    
    Ok(warp::reply::with_status(
        format!("{:?}", interleaved),
        StatusCode::OK,
    ))
}

fn interleaved<P: AsRef<Path>>(commands_filename: P, output_filename: P, append_mode: bool) -> io::Result<Vec<String>> {
    let commands = read_lines(commands_filename)?;
    let output = read_lines(output_filename)?;

    let mut interleaved = Vec::new();

    if append_mode {
        interleaved.extend(commands);
        interleaved.extend(output);
    } else {
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
        if output_index < output.len() {
            for line in &output[output_index..] {
                interleaved.push(line.clone());
            }
        }
    }

    Ok(interleaved)
}


fn read_lines<P>(filename: P) -> io::Result<Vec<String>>
where P: AsRef<Path>, {
    let file = File::open(filename)?;
    let buf = io::BufReader::new(file);
    buf.lines().collect()
}

#[tokio::main]
async fn main() {
    let upload = warp::multipart::form()
        .and_then(handle_upload);

    warp::serve(upload)
        .run(([127, 0, 0, 1], 3030))
        .await;
}
