#include <chrono>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#if defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(_WIN32) || defined(_WIN64)
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include <glad/gl.h>

#include "Application.h"
#include "put_window_behind_desktop_icons.h"

namespace {

float GetElapsedSeconds(std::chrono::system_clock::time_point start) {
  auto now = std::chrono::system_clock::now();
  return float(
      std::chrono::duration_cast<std::chrono::milliseconds>( now - start ).count()
    ) / 1000.0;
}

} // anonymous namespace

void Application::run() {
  if ( mainShaderProgram == nullptr ) {
    throw std::runtime_error("mainShaderProgram is nullptr, setting it before running.");
    return;
  }

  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

  Decoder* decoder = renderer->decoder;
  int prev_frames = -1;
  void* pixels = nullptr;
  while ( !glfwWindowShouldClose( window ) ) {
    float elapsed_seconds = GetElapsedSeconds(start);
    std::array<float, 2> resolution = { renderer->viewport.z, renderer->viewport.w };

    if ( decoder ) {
      int curr_frames = elapsed_seconds * decoder->avg_frame_rate;
      curr_frames = curr_frames % decoder->nb_frames;
      int n = curr_frames - prev_frames;

      while (n > 0) {
        pixels = decoder->GetFrame( elapsed_seconds );
        --n;
      }

      renderer->SetTexture0(pixels, decoder->width, decoder->height);
      prev_frames = curr_frames;
    }

    mainShaderProgram->Use();
    mainShaderProgram->BindFloat( "iTime", elapsed_seconds );
    mainShaderProgram->BindVec2( "iResolution", resolution );
    mainShaderProgram->BindTexture2D( "iChannel0", renderer->GetTexture0(), 0 );
    mainShaderProgram->BindTexture2D( "iChannel1", renderer->GetTexture1(), 1 );
    mainShaderProgram->BindTexture2D( "iChannel2", renderer->GetTexture2(), 2 );
    mainShaderProgram->BindTexture2D( "iChannel3", renderer->GetTexture3(), 3 );

    renderer->SetRenderState();
    renderer->DrawQuad();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Application::terminate() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

Application::Application() {
  initWindow();

  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);

  renderer = new Renderer();

  renderer->viewport.x = 0;
  renderer->viewport.y = 0;
  renderer->viewport.z = width;
  renderer->viewport.w = height;

  renderer->clearColor.x = 0.0;
  renderer->clearColor.y = 0.0;
  renderer->clearColor.z = 0.0;
  renderer->clearColor.w = 1.0;
}

Application::~Application() {
  terminate();
}

void Application::initWindow() {
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
#elif defined(_WIN32) || defined(_WIN64)
  HWND nativeWindow = glfwGetWin32Window(window);
#endif
  put_window_behind_desktop_icons(nativeWindow);

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
}
