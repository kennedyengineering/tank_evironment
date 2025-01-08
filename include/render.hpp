// Tank Game (@kennedyengineering)

#pragma once

#include <cairo.h>
#include <filesystem>
#include <vector>

namespace TankGame
{
    struct Point
    {
        float x, y;
    };

    struct Color
    {
        float r, g, b;
    };

    class RenderEngine
    {
    public:
        RenderEngine(int imageWidth, int imageHeight);
        ~RenderEngine();
        
        // TODO: render tank
        // TODO: render projectiles (separate methods, don't need debug draw since all shapes are being manually tracked. shape in list -> get body -> get world transform. shape -> get polygon -> vertices && count)
        // TODO: render lidar scan
        // TODO: implement the above methods in engine, have engine keep track of pixel density etc... initialize this class in initialization list

        // TODO: get pixel buffer

        void clearImage(Color color);

        void renderPolygon(std::vector<Point> vertices, Color color);
        void renderCircle(Point center, float radius, Color color);

        void writeToPng(const std::filesystem::path& filePath);

    private:
        cairo_surface_t *mSurface;
        cairo_t *mContext;
    };
}
