#include "HTTPRequest/include/HTTPRequest.hpp" // library to make a GET web request
#include "json.hpp" // library for parsing json returned from thecolorapi.com
#include <Magick++.h> // library to enable reading of pixels in different image formats
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char **argv) {

  Magick::InitializeMagick(*argv);
  Magick::Image image;

  if (argc < 2) {
    std::cerr << "No image input was provided." << std::endl;
    return 1;
  }

  // verify that the image exists & that it can be read by magick++
  try {
    image = Magick::Image(argv[1]);
  } catch (Magick::Exception &error_) {
    std::cerr << "Caught Magick++ exception: " << error_.what() << std::endl;
    return 1;
  }

  int width = image.columns();
  int height = image.rows();

  // construct a map to count the recurrences of each pixel
  std::map<std::string, int> colors;
  Magick::PixelPacket *pixels = image.getPixels(0, 0, width, height);

  for (int column = 0; column < width; column++) {
    for (int row = 0; row < height; row++) {

      // each pixel can be accessed in the created pixelpacket similar to how a
      // vector would be accessed. The formula is width * row + column. While
      // potentially not necessary for agnostic analysis of all pixels, it does
      // make it more clear as to what is going on
      Magick::ColorRGB rgb = (Magick::ColorRGB)pixels[width * row + column];

      // the rgb values range from 0 to 1. For thecolorapi, they need to range
      // from 0 to 255.
      std::string pixelColor = std::to_string((int)(rgb.red() * 255)) + '.' +
                               std::to_string((int)(rgb.green() * 255)) + '.' +
                               std::to_string((int)(rgb.blue() * 255));

      if (colors.count(pixelColor) < 1) {
        colors[pixelColor] = 1;
      } else {
        colors[pixelColor] += 1;
      }
    }
  }

  std::string mostCommonPixel = "ERR";
  int mostCommonPixelCount = 0;

  // utilize a simple loop to find the most commonly appearing pixel and save
  // its r.g.b value and appearance count
  for (const auto &p : colors) {
    if (p.second > mostCommonPixelCount) {
      mostCommonPixel = p.first;
      mostCommonPixelCount = p.second;
    }
  }

  // more for debugging purposes than anything, I don't see a world where this
  // happens. Better safe than sorry
  if (mostCommonPixel == "ERR") {
    std::cerr << "Image does not contain any pixels that can be read."
              << std::endl;
    return 1;
  }

  // parse the string of the form r.g.b into a token vector for easier access
  std::istringstream iss(mostCommonPixel);
  std::vector<std::string> tokens;
  std::string token;

  while (getline(iss, token, '.')) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }

  // the api is of the format /id?rgb=rgb(r,g,b)
  std::string requestUrl = "http://www.thecolorapi.com/id?rgb=rgb(" +
                           tokens[0] + ',' + tokens[1] + ',' + tokens[2] + ')';

  http::Request request(requestUrl);

  // the http library returns a character array, so it needs to be converted to
  // a string before the json library can parse it
  const auto response = request.send("GET");
  const nlohmann::json jsonResponse = nlohmann::json::parse(
      std::string(response.body.begin(), response.body.end()));

  std::cout << "Closest Name: " << jsonResponse["name"]["value"] << std::endl;
  std::cout << "Closest Hex: " << jsonResponse["name"]["closest_named_hex"]
            << std::endl;
  std::cout << "Exact Match: "
            << (jsonResponse["name"]["exact_match_name"] == "true" ? "Yes"
                                                                   : "No")
            << std::endl;

  return 0;
}
