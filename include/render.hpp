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

        void clearImage(b2HexColor color);

        void renderPolygon(std::vector<b2Vec2> vertices, b2HexColor color);
        void renderCircle(b2Vec2 center, float radius, b2HexColor color);

        void writeToPng(const std::filesystem::path& filePath);

        std::vector<unsigned char> getBuffer();
        std::pair<int, int> getDimensions();
        int getChannels();

    private:
        using RGB_t = std::array<float, 3>;
        inline RGB_t getRGB(b2HexColor color);

    private:
        cairo_surface_t *mSurface;
        cairo_t *mContext;
    };
}
