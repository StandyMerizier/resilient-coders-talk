all: syscall-c syscall-go

syscall-c: syscall.c
	gcc -o syscall-c syscall.c

syscall-go: syscall.go
	go build -o syscall-go syscall.go

clean:
	rm syscall-go syscall-c
