// Tank Game (@kennedyengineering)

#include <box2d/box2d.h>
#include <gtest/gtest.h>
#include <vector>

#include "render.hpp"

TEST(RenderTest, Initialization) {
  // Ensure RenderEngine initializes correctly

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Check dimensions were stored
  auto [width, height] = reng.getDimensions();
  ASSERT_EQ(width, 100);
  ASSERT_EQ(height, 200);

  // Check channels
  ASSERT_EQ(reng.getChannels(), 3);

  // Get and validate buffer
  std::vector<unsigned char> buff = reng.getBuffer();

  for (const unsigned char &c : buff) {
    ASSERT_EQ(c, 0);
  }

  // Save to PNG
  reng.writeToPng("RenderTest_Initialization.png");
}

TEST(RenderTest, ClearAndGetBufferBlack) {
  // Ensure RenderEngine clearImage and getBuffer methods work correctly

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with black
  reng.clearImage(b2_colorBlack);

  // Get and validate buffer
  std::vector<unsigned char> buff = reng.getBuffer();

  for (const unsigned char &c : buff) {
    ASSERT_EQ(c, 0);
  }
}

TEST(RenderTest, ClearAndGetBufferWhite) {
  // Ensure RenderEngine clearImage and getBuffer methods work correctly

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with white
  reng.clearImage(b2_colorWhite);

  // Get and validate buffer
  std::vector<unsigned char> buff = reng.getBuffer();

  for (const unsigned char &c : buff) {
    ASSERT_EQ(c, 255);
  }
}

TEST(RenderTest, ClearAndGetBufferRed) {
  // Ensure RenderEngine clearImage and getBuffer methods work correctly

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with red
  reng.clearImage(b2_colorRed);

  // Get and validate buffer
  std::vector<unsigned char> buff = reng.getBuffer();

  for (size_t i = 2; i < buff.size(); i += 3) {
    ASSERT_EQ(buff[i], 255);
  }
}

TEST(RenderTest, ClearAndGetBufferGreen) {
  // Ensure RenderEngine clearImage and getBuffer methods work correctly

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with green
  reng.clearImage(b2_colorGreen);

  // Get and validate buffer
  std::vector<unsigned char> buff = reng.getBuffer();

  for (size_t i = 1; i < buff.size(); i += 3) {
    ASSERT_EQ(buff[i], 255);
  }
}

TEST(RenderTest, ClearAndGetBufferBlue) {
  // Ensure RenderEngine clearImage and getBuffer methods work correctly

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with blue
  reng.clearImage(b2_colorBlue);

  // Get and validate buffer
  std::vector<unsigned char> buff = reng.getBuffer();

  for (size_t i = 0; i < buff.size(); i += 3) {
    ASSERT_EQ(buff[i], 255);
  }
}

TEST(RenderTest, PolygonToPNG) {
  // Ensure RenderEngine correctly draws polygons

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with blue
  reng.clearImage(b2_colorBlue);

  // Render polygon
  std::vector<b2Vec2> vertices;
  vertices.push_back({50, 100});
  vertices.push_back({25, 175});
  vertices.push_back({75, 175});

  reng.renderPolygon(vertices, b2_colorRed);

  // Save to PNG
  reng.writeToPng("RenderTest_PolygonToPNG.png");
}

TEST(RenderTest, PolygonOutOfBoundsToPNG) {
  // Ensure RenderEngine correctly draws polygons

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with blue
  reng.clearImage(b2_colorBlue);

  // Define polygons
  std::vector<b2Vec2> vertices;
  vertices.push_back({50, 100});
  vertices.push_back({25, 175});
  vertices.push_back({75, 175});

  std::vector<b2Vec2> translatedVerticesRight(vertices.size());
  std::transform(vertices.begin(), vertices.end(),
                 translatedVerticesRight.begin(),
                 [](b2Vec2 point) { return b2Add(point, {50, 0}); });

  std::vector<b2Vec2> translatedVerticesDown(vertices.size());
  std::transform(vertices.begin(), vertices.end(),
                 translatedVerticesDown.begin(),
                 [](b2Vec2 point) { return b2Add(point, {0, 75}); });

  std::vector<b2Vec2> translatedVerticesAbsurd(vertices.size());
  std::transform(vertices.begin(), vertices.end(),
                 translatedVerticesAbsurd.begin(),
                 [](b2Vec2 point) { return b2Add(point, {1000, 1000}); });

  // Render partially observable polygons
  reng.renderPolygon(translatedVerticesRight, b2_colorRed);

  reng.renderPolygon(translatedVerticesDown, b2_colorRed);

  // Render unobservable polygon
  reng.renderPolygon(translatedVerticesAbsurd, b2_colorRed);

  // Save to PNG
  reng.writeToPng("RenderTest_PolygonOutOfBoundsToPNG.png");
}

TEST(RenderTest, CircleToPNG) {
  // Ensure RenderEngine correctly draws circles

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with blue
  reng.clearImage(b2_colorBlue);

  // Render circles
  reng.renderCircle({50, 100}, 1.0f, b2_colorRed);
  reng.renderCircle({25, 175}, 5.0f, b2_colorRed);
  reng.renderCircle({75, 175}, 15.0f, b2_colorRed);

  // Save to PNG
  reng.writeToPng("RenderTest_CircleToPNG.png");
}

TEST(RenderTest, CircleOutOfBoundsToPNG) {
  // Ensure RenderEngine correctly draws circles

  // Construct render engine
  TankGame::RenderEngine reng(100, 200);

  // Fill with blue
  reng.clearImage(b2_colorBlue);

  // Render partially observable circles
  reng.renderCircle({100, 50}, 1.0f, b2_colorRed);
  reng.renderCircle({100, 100}, 5.0f, b2_colorRed);
  reng.renderCircle({100, 150}, 15.0f, b2_colorRed);

  reng.renderCircle({25, 200}, 1.0f, b2_colorRed);
  reng.renderCircle({50, 200}, 5.0f, b2_colorRed);
  reng.renderCircle({75, 200}, 15.0f, b2_colorRed);

  // Render unobservable circle
  reng.renderCircle({1000, 1000}, 15.0f, b2_colorRed);

  // Save to PNG
  reng.writeToPng("RenderTest_CircleOutOfBoundsToPNG.png");
}
