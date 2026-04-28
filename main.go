package main

import (
	httpserver "main/httpServer"
	"net/http"
)

func main() {

	http.HandleFunc("/", httpserver.UploadPageHandler)   //first handler for choose you image
	http.HandleFunc("/upload", httpserver.UploadHandler) //second handler for getting and download image

	err := http.ListenAndServe(":8080", nil) //start server
	if err != nil {
		panic(err)
	}
}
