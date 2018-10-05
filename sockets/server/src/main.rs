extern crate hyper;
extern crate tokio;

use std::error::Error;
use std::io::{Cursor,Write};

use tokio::io::{self,AsyncRead,AsyncWrite,Read};
use tokio::net::{TcpListener};
use tokio::prelude::{future,stream,Async,Future,Stream};

fn serve_http() -> Result<(), Box<Error>> {
    tokio::run(TcpListener::bind(&"127.0.0.1:8080".parse()?)?.incoming().for_each(move |conn| {

        let future = future::lazy(move || { Ok(conn.split()) }).and_then(|(mut read, write)| {
            future::poll_fn(move || {
                let mut vec = vec![0; 1400];
                read.poll_read(&mut vec).map(move |async_res| {
                    match async_res {
                        Async::Ready(b) => {
                            vec.truncate(b);
                            Async::Ready((b, vec))
                        },
                        Async::NotReady => Async::NotReady,
                    }
                })
            }).map(|(b, v)| {
                (b, v, write)
            })
        }).and_then(move |(b, vec, write)| {
            let pre_s = b"HTTP/1.1 200 OK\nContent-Length: ";
            let post_s = b"\n\n";
            let mut cur = Cursor::new(Vec::with_capacity(b + pre_s.len() + post_s.len() + 10));
            cur.write(pre_s);
            cur.write(b.to_string().as_bytes());
            cur.write(post_s);
            cur.write(&vec);
            Ok((write, cur))
        }).and_then(|(mut w, cur)| {
            future::poll_fn(move || {
                w.poll_write(&cur.get_ref()).map(move |async_res| {
                    match async_res {
                        Async::Ready(_) => Async::Ready(()),
                        Async::NotReady => Async::NotReady,
                    }
                })
            })
        }).map(|_| ()).map_err(|e| {
            println!("{}", e);
            ()
        });
        tokio::spawn(future);
        Ok(())
    }).map_err(|_| ()));
    Ok(())
}

fn main() {
    match serve_http() {
        Ok(_) => (),
        Err(e) => {
            println!("{}", e);
            std::process::exit(1);
        }
    };
}
