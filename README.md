# ascii-image-converter-go-cpp
# ASCII Image Converter (Go & C++)

A high-performance image-to-ASCII converter with a Go-based HTTP server and a powerful C++ processing core using OpenCV.

## 🛠 Tech Stack
- **Backend:** Go (Golang)
- **Image Processing:** C++ with OpenCV
- **Communication:** The Go server executes the C++ binary to process uploaded images.

## 📁 Project Structure
- `/httpServer` - Go handlers for processing requests.
- `/processor` - C++ source code for ASCII conversion logic.
- `main.go` - Entry point of the application.

## 🚀 How to Run
1. **Prerequisites:**
   - Go installed
   - OpenCV 4.x installed and configured for your C++ compiler.
2. **Build the C++ processor:**
   Compile the `.cpp` file and place the executable in the `bin/` folder as `processor.exe`.
3. **Start the server:**
   ```bash
   go run main.go
   ```

## 🤝 Contributors
- [windwalker553](https://github.com)
- [1of-avangard](https://github.com)

## 📄 License
This project is licensed under the MIT License.

## 🖼 Result Example


| Original Image | ASCII Art Result |
| :---: | :---: |
| <img src="./assets/rei.jpg" width="400"> | <img src="./assets/processed_rei .jpg" width="400"> |

*Main page of the application:*
<img src="./assets/mainPage.png" width="800">
