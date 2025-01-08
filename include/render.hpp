// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <cairo.h>
#include <filesystem>
#include <vector>
#include <array>

namespace TankGame
{
    class RenderEngine
    {
    public:
        RenderEngine(int imageWidth, int imageHeight);
        ~RenderEngine();
        
        // TODO: render tank
        // TODO: render lidar scan
        // TODO: get pixel buffer (either create struct to hold vector (buffer) and image dimensions, or create another method to get image dimensions and call that in bindings lambda. probably the latter.)

        void clearImage(b2HexColor color);

        void renderPolygon(std::vector<b2Vec2> vertices, b2HexColor color);
        void renderCircle(b2Vec2 center, float radius, b2HexColor color);

        void writeToPng(const std::filesystem::path& filePath);

    private:
        using RGB_t = std::array<float, 3>;
        inline RGB_t getRGB(b2HexColor color);

    private:
        cairo_surface_t *mSurface;
        cairo_t *mContext;
    };
}
