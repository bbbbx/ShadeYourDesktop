#pragma once

#include <stdexcept>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/gl.h"

float vertices[] = {
  -1.0f, -1.0f, 0.0f,
   3.0f, -1.0f, 0.0f,
  -1.0f,  3.0f, 0.0f
};

class QuadGeometry
{
private:
  GLuint vertexBuffer;
public:
  GLuint VAO;
  QuadGeometry();
  ~QuadGeometry();
};

QuadGeometry::QuadGeometry()
{
  GLFWwindow *window = glfwGetCurrentContext();
  if (window == nullptr) {
    throw std::runtime_error("Context isn't initialized.");
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &vertexBuffer);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLint posAttribLocation = 0;
  glEnableVertexAttribArray(posAttribLocation);
  glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

QuadGeometry::~QuadGeometry()
{
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteVertexArrays(1, &VAO);
}
