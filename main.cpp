// clang-format off
#include <chrono>
#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
// clang-format on

namespace fs = std::filesystem;
namespace po = boost::program_options;

using namespace std;

int main(int argc, char** argv)
{
  // Expects parameter passed in is the image to apply the gaussian blur to
  // Expects second parameter to be the output path to where you want to save the blurred image.
  // Setup program options
  auto desc = po::options_description("Options");
  // clang-format off
  desc.add_options()
  ("help,h", "Show help message")
  ("source,s", po::value< string >(), "The full file path to the source image to blur")
  ("destination,d", po::value< string >(), "The full file path to where to save the blurred image")
  ("blur,b", po::value< int >(), "The blur intensity to use");
  // clang-format on

  // Parse the passed in options
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // Now act on what we have
  if (vm.count("help"))
  {
    cout << desc << "\n";
    return 1;
  }

  string sourcePath;
  string destPath;
  int blur = -1;

  if (vm.count("source"))
  {
    sourcePath = vm["source"].as<string>();
  }
  else
  {
    printf("Source path parameter missing\n");
    return -1;
  }
  if (vm.count("destination"))
  {
    destPath = vm["destination"].as<string>();
  }
  else
  {
    printf("Destination path parameter missing\n");
    return -1;
  }
  if (vm.count("blur"))
  {
    blur = vm["blur"].as<int>();
  }
  else
  {
    printf("Blur parameter missing\n");
    return -1;
  }

  // Load up an image, process, then save the image
  auto sourceImage = cv::imread(sourcePath.c_str(), 1);
  if (!sourceImage.data)
  {
    printf("No image data \n");
    return -1;
  }

  // Perform a box blur
  auto start = std::chrono::high_resolution_clock::now();
  int blurSize = int((float)blur * 3. * sqrt(2. * M_PI) / 4. + 0.5) | 1;
  for (int i = 0; i < 3; i++)
    cv::blur(sourceImage, sourceImage, cv::Size(blurSize, blurSize));

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  printf("Guassian blur completed in: %lu\n", duration);

  // Save image to disk
  auto result = false;
  try
  {
    result = cv::imwrite(destPath, sourceImage);
  }
  catch (const cv::Exception& ex)
  {
    fprintf(stderr, "Error: %s\n", ex.what());
  }

  if (result)
    printf("Success\n");
  else
    printf("Error\n");
}
