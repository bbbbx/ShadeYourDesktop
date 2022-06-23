#pragma once

#include "Renderer.h"
#include "Program.h"

class Application
{
private:
  void initWindow();
public:
  GLFWwindow *window = nullptr;
  Renderer *renderer = nullptr;

  Program *mainShaderProgram = nullptr;

  Program *bufferAShaderProgram = nullptr;
  Program *bufferBShaderProgram = nullptr;
  Program *bufferCShaderProgram = nullptr;
  Program *bufferDShaderProgram = nullptr;

  Application();
  ~Application();

  void run();
  void terminate();
};

