// Tank Game (@kennedyengineering)

#pragma once

#include <cairo.h>
#include <filesystem>

namespace TankGame
{
    class RenderEngine
    {
    public:
        RenderEngine(int imageWidth, int imageHeight);
        ~RenderEngine();

        void writeToPng(const std::filesystem::path& filePath);

    private:
        cairo_surface_t *mSurface;
        cairo_t *mContext;
    };
}
