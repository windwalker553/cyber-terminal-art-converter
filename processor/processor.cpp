#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Function prototype for ASCII processing
void ProcessAscii(const std::string&, const std::string&);

int main(int argc, char** argv)
{
    // Ensure input and output paths are provided
    if (argc < 3) return 1;
    
    try
    {
        std::string inputPath = argv[1];
        std::string outputPath = argv[2];

        ProcessAscii(inputPath, outputPath);

        std::cout << "Success: file saved in " << outputPath << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

void ProcessAscii(const std::string& inputPath, const std::string& outputPath)
{
    // Load the image in grayscale mode
    cv::Mat img = cv::imread(inputPath, cv::IMREAD_GRAYSCALE);
    if (img.empty()) 
    {
        throw std::runtime_error(std::string("Can't open image: ") + inputPath);
    }

    // Create a white background for the ASCII art
    cv::Mat asciiArt = cv::Mat(img.rows, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

    // Characters sorted by visual density (darkest to lightest)
    const std::string chars = "@$#8W9H4Zl(r/!:. ";
    int stepX = 3;// Cell width for character sampling
    int stepY = 5;// Cell height for character sampling

    // Image pre-processing: equalize histogram and adjust contrast/brightness
    equalizeHist(img, img);
    img.convertTo(img, -1, 1.3, -40);

    // Iterate through image blocks (cells)
    for (int y = stepY; y < img.rows; y += stepY)
    {
        for (int x = 0; x < img.cols; x += stepX)
        {
            if (x + stepX > img.cols || y > img.rows) continue;

            // Calculate average brightness of the current cell
            cv::Rect rect(x, y - stepY, stepX, stepY);
            cv::Scalar meanBrightness = cv::mean(img(rect));
            int brightness = static_cast<int>(meanBrightness.val[0]);

            // Map brightness to corresponding ASCII character
            int index = brightness * (static_cast<int>(chars.length()) - 1) / 255;
            std::string symbol(1, chars[index]);

            // Draw the character on the resulting image
            putText(asciiArt, symbol, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 0.22, cv::Scalar(0, 0, 0), 1, cv::LINE_4);
        }
    }

    // Save the final ASCII art image
    if (!cv::imwrite(outputPath, asciiArt))
    {
        return;
    }
}