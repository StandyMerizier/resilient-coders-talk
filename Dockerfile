FROM ubuntu

RUN apt-get update
RUN apt-get install -y strace git curl golang build-essential
RUN bash -c "sh <(curl https://sh.rustup.rs -sSf) -y"
RUN git clone https://github.com/jbaublitz/resilient-coders-talk
RUN cd resilient-coders-talk/sockets/client && make
RUN cd resilient-coders-talk/sockets/server && ~/.cargo/bin/cargo build --release
RUN cd resilient-coders-talk/sockets/proxy && ~/.cargo/bin/cargo build --release
