// Tank Game (@kennedyengineering)

#include <cmath>
#include <stdexcept>

#include "render.hpp"

using namespace TankGame;

RenderEngine::RenderEngine(int imageWidth, int imageHeight) {
  /* Construct the render engine */

  // Allocate the image surface
  mSurface =
      cairo_image_surface_create(CAIRO_FORMAT_RGB24, imageWidth, imageHeight);

  // Create render context
  mContext = cairo_create(mSurface);
}

RenderEngine::~RenderEngine() {
  /* Destroy the render engine */

  // Destroy render context
  cairo_destroy(mContext);

  // Deallocate the image surface
  cairo_surface_destroy(mSurface);
}

void RenderEngine::clearImage(b2HexColor color) {
  /* Clear the surface */

  // Set color
  RGB_t rgb = getRGB(color);
  cairo_set_source_rgb(mContext, rgb[0], rgb[1], rgb[2]);

  // Clear surface
  cairo_paint(mContext);
}

void RenderEngine::renderPolygon(std::vector<b2Vec2> vertices,
                                 b2HexColor color) {
  /* Render a polygon */

  // Check input
  if (vertices.size() < 3) {
    throw std::invalid_argument("Vector must have at least 3 entries.");
  }

  // Set color
  RGB_t rgb = getRGB(color);
  cairo_set_source_rgb(mContext, rgb[0], rgb[1], rgb[2]);

  // Create path
  bool first = true;

  for (const b2Vec2 &point : vertices) {
    if (first) {
      cairo_move_to(mContext, point.x, point.y);
      first = false;
    } else {
      cairo_line_to(mContext, point.x, point.y);
    }
  }

  cairo_close_path(mContext);

  // Fill color
  cairo_fill(mContext);
}

void RenderEngine::renderCircle(b2Vec2 center, float radius, b2HexColor color) {
  /* Render a circle */

  // Set color
  RGB_t rgb = getRGB(color);
  cairo_set_source_rgb(mContext, rgb[0], rgb[1], rgb[2]);

  // Create path
  cairo_arc(mContext, center.x, center.y, radius, 0, 2.0f * M_PIf);

  // Fill color
  cairo_fill(mContext);
}

void RenderEngine::writeToPng(const std::filesystem::path &filePath) {
  /* Write the current image surface to disk */

  // Check if the file has a ".png" extension
  if (filePath.extension() != ".png") {
    throw std::invalid_argument("The file must be a .png");
  }

  // Save to disk
  cairo_surface_write_to_png(mSurface, filePath.c_str());
}

std::vector<unsigned char> RenderEngine::getBuffer() {
  /* Get a copy of the surface as a pixel buffer */

  // Get image dimensions
  int height = cairo_image_surface_get_height(mSurface);
  int stride = cairo_image_surface_get_stride(mSurface);
  int size = height * stride;

  // Get data
  cairo_surface_flush(mSurface);
  unsigned char *data = cairo_image_surface_get_data(mSurface);

  // Copy data into vector and remove unused alpha channel
  std::vector<unsigned char> pixelBuffer;
  pixelBuffer.reserve(size);

  // Remove the unused alpha channel
  for (int i = 0; i < size; i += 4) {
    pixelBuffer.push_back(data[i]);
    pixelBuffer.push_back(data[i + 1]);
    pixelBuffer.push_back(data[i + 2]);
  }

  // Return pixel buffer
  return pixelBuffer;
}

int RenderEngine::getChannels() {
  /* Get the number of channels in the surface */

  // Return channels
  return 3;
}

std::pair<int, int> RenderEngine::getDimensions() {
  /* Get the dimensions of the surface */

  // Return dimensions
  return std::pair<int, int>(cairo_image_surface_get_width(mSurface),
                             cairo_image_surface_get_height(mSurface));
}

inline RenderEngine::RGB_t RenderEngine::getRGB(b2HexColor color) {
  /* Convert b2HexColor into RGB */

  // Convert a box2d color to std::tuple of RGB
  return RGB_t{
      (((color >> 16) & 0xFF) / 255.0f), // Red
      (((color >> 8) & 0xFF) / 255.0f),  // Green
      ((color & 0xFF) / 255.0f)          // Blue
  };
}
