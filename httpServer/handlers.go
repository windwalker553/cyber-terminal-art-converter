package httpserver

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"time"
)

func UploadPageHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/html; charset=utf-8") //HTML code for to select an image
	fmt.Fprint(w, `
		<h3>Upload you image</h3>
		<!-- Important: enctype="multipart/form-data" required for files-->
		<form action="/upload" method="POST" enctype="multipart/form-data">
			<input type="file" name="myFile">
			<button type="submit">Download</button>
		</form>
	`)
}
func UploadHandler(w http.ResponseWriter, r *http.Request) {

	logFile, _ := os.OpenFile("log.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0666) //open-create log
	defer logFile.Close()

	timenow := time.Now().Format("2006-01-02 15:04:05") //create time-variable for logs

	file, header, err := r.FormFile("myFile") //we receive a file and an error from the website

	if err != nil {
		logFile.WriteString(timenow + " - ERROR: receiving a file\n") //Erorr whith file(size itc.)
		http.Error(w, "-ERORR: when receiving a file", 400)
		return
	}
	defer file.Close()

	logFile.WriteString(timenow + " - INFO: File uploaded" + header.Filename + "\n") //write logs when download file from web

	filename := filepath.Base(header.Filename) //Cleaning the file name from unnecessary paths (security)

	os.MkdirAll("uploads", os.ModePerm) //create or skip(if created) to coppy
	os.MkdirAll("results", os.ModePerm) //create or skip(if created)where c++ chaneged

	//Use absolute path (this will solve the 0 KB problem)
	inputPath, _ := filepath.Abs(filepath.Join("uploads", filename))
	outputPath, _ := filepath.Abs(filepath.Join("results", "processed_"+filename))

	outFile, err := os.Create(inputPath) //Create destination file on disk

	if err != nil {
		http.Error(w, "Failed to create file on the server", 500) //Erorr related to Save file in directory uploads
		return
	}
	_, err = io.Copy(outFile, file) // Transfer data from request to disk
	outFile.Close()                 // Flush buffers and release file for C++ processor

	if err != nil {
		logFile.WriteString(timenow + " - ERROR: Failed to write data to file: " + err.Error() + "\n") // If an error occurs during io.Copy, we will end up here
		http.Error(w, "Error saving file", 500)
		return
	}

	cmd := exec.Command("./bin/processor.exe", inputPath, outputPath) //  LAUNCH C++

	cmd.Stderr = os.Stderr // If OpenCV will give an error, she will be in the Go console

	err = cmd.Run()
	if err != nil {
		logFile.WriteString(timenow + " - ERROR: C++ erorr: " + err.Error() + "\n")
		http.Error(w, "C++ processor could not process the file", 500)
		return
	}

	defer os.Remove(inputPath)  //delete file in directory
	defer os.Remove(outputPath) //delete file in directory

	//CHECKING THE RESULT
	fileInfo, err := os.Stat(outputPath)
	if err != nil || fileInfo.Size() == 0 {
		logFile.WriteString(timenow + " - ERROR: The output file is empty\n") //if file is empty
		http.Error(w, "ERORR: the processor saved an empty file", 500)
		return
	}

	logFile.WriteString(timenow + " - SUCCESS: file" + filename + "processed\n") //log good(file processed)

	// DOWNLOAD SETTINGS

	// 1. We indicate to the browser that the file should not be opened in the window, but DOWNLOADED.
	// The 'attachment' parameter forcibly triggers the save file dialog.
	// 'filename=' sets the name under which the file will be saved for the user.
	w.Header().Set("Content-Disposition", "attachment; filename=processed_"+filename)

	// 2. Set the content type as "binary stream".
	// This indicates to the browser that the data is not plain text or a web page,
	// which further confirms the need for downloading.
	w.Header().Set("Content-Type", "application/octet-stream")

	// 3. Reads a file from disk at the path 'outputPath' and sends its content in the response (w).
	// This function itself takes care of proper byte transfer and closing the stream.
	http.ServeFile(w, r, outputPath)

}
