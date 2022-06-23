#include <chrono>
#include <iostream>

#include "glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "QuadGeometry.hpp"
#include "Program.h"
#include "string_utils.hpp"
#include "Renderer.h"

namespace {

/**
 * @brief Create a texture with linear filter and clamp to edge wrap mode.
 * 
 * @param width 
 * @param height 
 * @param pixels 
 * @param texture optional, default is 0
 * @return GLuint 
 */
GLuint NewTexture2D(int width, int height, const void* pixels = NULL, GLuint texture = 0) {
  if (texture == 0) {
    glGenTextures(1, &texture);
  }
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, NULL);

  return texture;
}

GLuint NewTexture2DFromFile(const std::string& filename, GLuint texture = 0) {
  stbi_set_flip_vertically_on_load(true);

  int width, height, channel;
  stbi_uc *pixels = stbi_load(filename.c_str(), &width, &height, &channel, STBI_rgb_alpha);

  if ( !pixels ) {
    std::cerr << "Failed to load texture file " << filename << std::endl;
    return 0;
  }

  texture = NewTexture2D( width, height, pixels, texture );
  stbi_image_free( pixels );

  return texture;
}

} // anonymous namespace

void Renderer::SetRenderState() {
  glViewport( viewport.x, viewport.y, viewport.z, viewport.w );
  glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
  glClear( GL_COLOR_BUFFER_BIT );

  glEnable( GL_BLEND );
  glBlendEquation( GL_FUNC_ADD );
  glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
}

void Renderer::DrawQuad() {
  glBindVertexArray( emptyVAO );
  glDrawArrays( GL_POINTS, 0, 1 );
}

Renderer::Renderer()
{
  glGenVertexArrays( 1, &emptyVAO );

  GLuint textures[ 4 ];
  glGenTextures( 4, textures );
  texture0 = textures[ 0 ];
  texture1 = textures[ 1 ];
  texture2 = textures[ 2 ];
  texture3 = textures[ 3 ];
}

void Renderer::SetTexture0(void* pixels, int width, int height) {
  NewTexture2D(width, height, pixels, texture0);
}
void Renderer::SetTexture1(void* pixels, int width, int height) {
  NewTexture2D(width, height, pixels, texture1);
}
void Renderer::SetTexture2(void* pixels, int width, int height) {
  NewTexture2D(width, height, pixels, texture2);
}
void Renderer::SetTexture3(void* pixels, int width, int height) {
  NewTexture2D(width, height, pixels, texture3);
}

void Renderer::SetTexture0(const std::string& filename) {
  texture0 = NewTexture2DFromFile( filename, texture0 );
}
void Renderer::SetTexture1(const std::string& filename) {
  texture1 = NewTexture2DFromFile( filename, texture1 );
}
void Renderer::SetTexture2(const std::string& filename) {
  texture2 = NewTexture2DFromFile( filename, texture2 );
}
void Renderer::SetTexture3(const std::string& filename) {
  texture3 = NewTexture2DFromFile( filename, texture3 );
}

Renderer::~Renderer() {
  glDeleteVertexArrays( 1, & emptyVAO );
  glDeleteTextures( 1, &texture0 );
  glDeleteTextures( 1, &texture1 );
  glDeleteTextures( 1, &texture2 );
  glDeleteTextures( 1, &texture3 );
}
