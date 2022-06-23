#pragma once

#include <unordered_map>
#include <string>
#include <array>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

struct Uniform {
  std::string name;
  GLenum type;
  GLsizei size;
  GLint location;
};

class Program
{
private:
  GLuint program;

public:
  std::unordered_map<std::string, Uniform> uniforms;

  Program(const std::string &fragment_shader_source);
  Program(const std::string &vertex_shader_source, const std::string &fragment_shader_source);

  ~Program();

  void Use() const;
  void BindInt(const std::string& uniform_name, int value) const;
  void BindFloat(const std::string& uniform_name, float value) const;
  void BindVec2(const std::string& uniform_name, std::array<float, 2> value) const;
  void BindVec3(const std::string& uniform_name, std::array<float, 3> value) const;
  void BindVec4(const std::string& uniform_name, std::array<float, 4> value) const;
  void BindMat3(const std::string& uniform_name, const std::array<float, 9>& value) const;
  void BindMat4(const std::string& uniform_name, const std::array<float, 16>& value) const;
  void BindTexture2D(const std::string& sampler_uniform_name, GLuint texture, GLuint texture_unit) const;
  void BindTexture3D(const std::string& sampler_uniform_name, GLuint texture, GLuint texture_unit) const;
};
