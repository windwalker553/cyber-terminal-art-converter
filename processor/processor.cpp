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

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

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
    // Load source image in BGR color
    cv::Mat colorImg = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (colorImg.empty())
    {
        throw std::runtime_error(std::string("Can't open image: ") + inputPath);
    }

    // Create grayscale copy for brightness analysis
    cv::Mat grayImg;
    cv::cvtColor(colorImg, grayImg, cv::COLOR_BGR2GRAY);

    // Improve contrast and brightness for better character mapping
    cv::equalizeHist(grayImg, grayImg);
    grayImg.convertTo(grayImg, -1, 1.1, 10);

    // Set output canvas scale and background color (White)
    float scale = 3.0f;
    cv::Mat asciiArt = cv::Mat(colorImg.rows * scale, colorImg.cols * scale, CV_8UC3, cv::Scalar(255, 255, 255));

    // Characters sorted by visual density
    const std::string chars = "@%#*+=-:. ";
    float stepX = 2;
    float stepY = 4;

    // Process image by cells
    for (int y = stepY; y < colorImg.rows; y += stepY)
    {
        for (int x = 0; x < colorImg.cols; x += stepX)
        {
            if (x + stepX > colorImg.cols || y > colorImg.rows) continue;

            cv::Rect rect(x, y - stepY, (int)stepX, (int)stepY);

            // Get average color from source and brightness from grayscale
            cv::Scalar meanColor = cv::mean(colorImg(rect));
            cv::Scalar meanBrightness = cv::mean(grayImg(rect));
            int brightness = static_cast<int>(meanBrightness.val[0]);

            // Map brightness to ASCII character index
            int index = brightness * (static_cast<int>(chars.length()) - 1) / 255;
            std::string symbol(1, chars[index]);

            // Draw colored character on the upscale canvas
            cv::putText(asciiArt, symbol,
                cv::Point(x * scale, (y + stepY / 2) * scale),
                cv::FONT_HERSHEY_DUPLEX, 0.4,
                meanColor,
                1, 8);
        }
    }
    if (!cv::imwrite(outputPath, asciiArt))
    {
        return;
    }
}