// Tank Game (@kennedyengineering)

#include <stdexcept>
#include <cmath>

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

void RenderEngine::clearImage(Color color)
{
    /* Clear the surface */

    // Set color
    cairo_set_source_rgb(mContext, color.r, color.g, color.b);

    // Clear surface
    cairo_paint(mContext);
}

void RenderEngine::renderPolygon(std::vector<Point> vertices, Color color)
{
    /* Render a polygon */

    // Check input
    if (vertices.size() < 3)
    {
        throw std::invalid_argument("Vector must have at least 3 entries.");
    }

    // Set color
    cairo_set_source_rgb(mContext, color.r, color.g, color.b);

    // Create path
    bool first = true;

    for (const Point& point : vertices)
    {
        if (first)
        {
            cairo_move_to(mContext, point.x, point.y);
            first = false;
        }
        else
        {
            cairo_line_to(mContext, point.x, point.y);
        }
    }

    cairo_close_path(mContext);

    // Fill color
    cairo_fill(mContext);
}

void RenderEngine::renderCircle(Point center, float radius, Color color)
{
    /* Render a circle */

    // Set color
    cairo_set_source_rgb(mContext, color.r, color.g, color.b);

    // Create path
    cairo_arc(mContext, center.x, center.y, radius, 0, 2.0f*M_PIf);

    // Fill color
    cairo_fill(mContext);
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