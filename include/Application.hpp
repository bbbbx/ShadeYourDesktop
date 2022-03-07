#pragma once

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#if defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(_WIN32) || defined(_WIN64)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Renderer.hpp"
#include "put_window_behind_desktop_icons.h"


class Application
{
private:
  void initWindow();
public:
  GLFWwindow *window;
  Renderer *renderer;

  ShaderProgram *mainShaderProgram;
  GLuint texture0;
  GLuint texture1;
  GLuint texture2;
  GLuint texture3;

  ShaderProgram *bufferAShaderProgram;
  ShaderProgram *bufferBShaderProgram;
  ShaderProgram *bufferCShaderProgram;
  ShaderProgram *bufferDShaderProgram;

  Application();
  ~Application();

  void run();
  void terminate();
};

void Application::run()
{
  if (mainShaderProgram == nullptr) {
    throw std::runtime_error("mainShaderProgram is nullptr, setting it before running.");
    return;
  }

  while (!glfwWindowShouldClose(window)) {
    renderer->setState();

    renderer->draw(mainShaderProgram);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
}

void Application::terminate() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

Application::Application(/* args */)
{
  initWindow();

  renderer = new Renderer();

  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  renderer->viewport.x = 0;
  renderer->viewport.y = 0;
  renderer->viewport.z = width;
  renderer->viewport.w = height;

  renderer->clearColor.x = 0.0;
  renderer->clearColor.y = 0.0;
  renderer->clearColor.z = 0.0;
  renderer->clearColor.w = 1.0;

  GLuint textures[4];
  glGenTextures(4, textures);
  texture0 = textures[0];
  texture1 = textures[1];
  texture2 = textures[2];
  texture3 = textures[3];

// 
  const char *filename = "assets/glyph.png";
  int imgWidth, imgHeight, imgChannelCount;
  stbi_set_flip_vertically_on_load(true);
  stbi_uc *pixels = stbi_load(filename, &imgWidth, &imgHeight, &imgChannelCount, STBI_rgb_alpha);
  if (!pixels) {
    printf("Failed to load texture file %s\n", filename);
    return;
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
  // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_image_free(pixels);
}

Application::~Application()
{
}

void Application::initWindow()
{
  glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

  if (!glfwInit()) {
    throw std::runtime_error("GLFW init failed!\n");
    return;
  }

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);
#endif
  window = glfwCreateWindow(mode->width, mode->height, "ShadeYourDesktop", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Create window failed!\n");
    return;
  }

#if defined(__APPLE__)
  id nativeWindow = glfwGetCocoaWindow(window);
  put_window_behind_desktop_icons(nativeWindow);
#elif defined(_WIN32) || defined(_WIN64)
  HWND nativeWindow = glfwGetWin32Window(window);
  put_window_behind_desktop_icons(nativeWindow);
#endif

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
}
