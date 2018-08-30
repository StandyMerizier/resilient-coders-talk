package main

import "fmt"
import "syscall"

func main() {
	fd, err := syscall.Open("./test.txt", syscall.O_RDWR | syscall.O_CREAT, 0666)
	if err != nil {
		fmt.Println(err)
	}
	stringSlice := []byte("this is the test text")
	if _, err := syscall.Write(fd, stringSlice); err != nil {
		fmt.Println(err)
	}
}
