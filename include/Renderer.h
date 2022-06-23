#pragma once


#include "glm/vec4.hpp"
#include "glad/gl.h"

#include "Decoder.h"

class Renderer
{
private:
  GLuint emptyVAO = 0;

  GLuint texture0 = 0;
  GLuint texture1 = 0;
  GLuint texture2 = 0;
  GLuint texture3 = 0;

public:
  glm::vec4 viewport;
  glm::vec4 clearColor;
  Decoder* decoder;

  Renderer();
  ~Renderer();
  inline GLuint GetTexture0() { return texture0; }
  inline GLuint GetTexture1() { return texture1; }
  inline GLuint GetTexture2() { return texture2; }
  inline GLuint GetTexture3() { return texture3; }

  void SetTexture0(void* pixels, int width, int height);
  void SetTexture1(void* pixels, int width, int height);
  void SetTexture2(void* pixels, int width, int height);
  void SetTexture3(void* pixels, int width, int height);

  void SetTexture(int unit, const std::string& filename);
  void SetTexture0(const std::string& filename);
  void SetTexture1(const std::string& filename);
  void SetTexture2(const std::string& filename);
  void SetTexture3(const std::string& filename);

  void DrawQuad();
  void SetRenderState();
};
