extern crate hyper;
extern crate tokio;

use std::error::Error;
use std::net::{ToSocketAddrs};

use hyper::{Body,Method,Response};
use hyper::server::conn::Http;
use hyper::service;

use tokio::io::{copy,AsyncRead};
use tokio::net::{TcpListener,TcpStream};
use tokio::prelude::{future,Future,Stream};

fn serve_proxy() -> Result<(), Box<Error>> {
    tokio::run(TcpListener::bind(&"127.0.0.1:8080".parse()?)?.incoming().for_each(move |conn| {
        let future = future::lazy(move || {
            let conn_clone = conn.try_clone().map_err(|e| {
                println!("{}", e);
                ()
            })?;
            Ok((conn, conn_clone))
        }).and_then(|(conn, conn_clone)| {
            tokio::spawn(Http::new().serve_connection(conn, service::service_fn(move |req| {
                let mut response_builder = Response::builder();
                if req.method() == Method::CONNECT {
                    let mut sock_addr_iter = if let Some(auth) = req.uri().authority_part() {
                        match auth.to_string().to_socket_addrs() {
                            Ok(u) => u,
                            Err(e) => {
                                println!("{}", e);
                                response_builder.status(400);
                                return response_builder.body(Body::from("Bad URI passed in CONNECT request"));
                            }
                        }
                    } else {
                        response_builder.status(400);
                        return response_builder.body(Body::from("Bad URI passed in CONNECT request"));
                    };
                    let sock_addr = if let Some(sa) = sock_addr_iter.next() {
                        sa
                    } else {
                        response_builder.status(400);
                        return response_builder.body(Body::from("Bad URI passed in CONNECT request"));
                    };
                    let (read_server, write_server) = conn_clone.try_clone().unwrap().split();
                    tokio::spawn(TcpStream::connect(&sock_addr).and_then(|tcp| {
                        let (read_proxy, write_proxy) = tcp.split();
                        tokio::spawn(copy(read_server, write_proxy).map(|_| ()).map_err(|e| {
                            println!("{}", e);
                            ()
                        }));
                        tokio::spawn(copy(read_proxy, write_server).map(|_| ()).map_err(|e| {
                            println!("{}", e);
                            ()
                        }));
                        Ok(())
                    }).map_err(|e| {
                        println!("{}", e);
                        ()
                    }));
                    response_builder.body(Body::from("Connected!"))
                } else {
                    response_builder.status(405);
                    response_builder.body(Body::from("Only CONNECT request are allowed."))
                }
            })).map_err(|e| {
                println!("{}", e);
                ()
            }));
            Ok(())
        });
        tokio::spawn(future.map(|_| ()));
        Ok(())
    }).map_err(|e| {
        println!("{}", e);
        ()
    }));
    Ok(())
}

fn main() {
    if let Err(e) = serve_proxy() {
        println!("{}", e);
        std::process::exit(1);
    }
}
