#pragma once

#include <chrono>

#include "glm/vec4.hpp"
#include "glad/gl.h"

#include "QuadGeometry.hpp"
#include "ShaderProgram.hpp"

class Renderer
{
private:
  QuadGeometry *quadGeometry;
  std::chrono::system_clock::time_point start;
public:
  glm::vec4 viewport;
  glm::vec4 clearColor;

  Renderer();
  ~Renderer();

  void draw(ShaderProgram *shaderProgram);
  void setState();
};

void Renderer::setState() {
  glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
  glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::draw(ShaderProgram *shaderProgram) {
  auto now = std::chrono::system_clock::now();
  float seconds = float(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) / 1000.0;

  glUseProgram(shaderProgram->program);

  glBindVertexArray(quadGeometry->VAO);

  auto uniforms = shaderProgram->uniforms;

  auto hash = StringUtils::StringHash{ "iTime" };
  auto uniform = uniforms.find(hash);
  if (uniform != uniforms.end()) {
    auto timeUniformLocation = uniform->second.location;
    glUniform1f(timeUniformLocation, seconds);
  }

  hash = StringUtils::StringHash{ "iResolution" };
  uniform = uniforms.find(hash);
  if (uniform != uniforms.end()) {
    auto resolutionUniformLocation = uniform->second.location;
    glUniform2f(resolutionUniformLocation, viewport.z, viewport.w);
  }

  hash = StringUtils::StringHash{ "iChannel0" };
  uniform = uniforms.find(hash);
  if (uniform != uniforms.end()) {
    auto location = uniform->second.location;
    glUniform1i(location, 0);
  }

  glDrawArrays(GL_TRIANGLES, 0, 3);
}

Renderer::Renderer()
{
  start = std::chrono::system_clock::now();
  quadGeometry = new QuadGeometry();
}

Renderer::~Renderer()
{
}
