// Tank Game (@kennedyengineering)

#include <stdexcept>

#include "render.hpp"

using namespace TankGame;

RenderEngine::RenderEngine(int imageWidth, int imageHeight)
{
    /* Construct the render engine */
    
    // Allocate the image surface
    mSurface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, imageWidth, imageHeight);
    
    // Create render context
    mContext = cairo_create(mSurface);
}

void RenderEngine::writeToPng(const std::filesystem::path& filePath)
{
    /* Write the current image surface to disk */

    // Check if the file has a ".png" extension
    if (filePath.extension() != ".png")
    {
        throw std::invalid_argument("The file must be a .png");
    }

    // Save to disk
    cairo_surface_write_to_png(mSurface, filePath.c_str());
}

RenderEngine::~RenderEngine()
{
    /* Destroy the render engine */

    // Destroy render context
    cairo_destroy(mContext);

    // Deallocate the image surface
    cairo_surface_destroy(mSurface);
}