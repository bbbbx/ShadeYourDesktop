#pragma once

#include <unordered_map>
#include <regex>
#include <string>
#include <iostream>
#include <fstream>

#include "string_utils.hpp"

static const char *VERTEX_SHADER_SOURCE =
  "#version 330 core\n"
  "layout (location = 0) in vec3 a_pos;\n"
  "out vec2 v_uv;\n"
  "void main()\n"
  "{\n"
  "    gl_Position = vec4(a_pos, 1.0);\n"
  "    v_uv = a_pos.xy * 0.5 + 0.5;\n"
  "}\n";

static const char *FRAGMENT_SHADER_SOURCE_PREFIX =
  "#version 330 core\n"
  "in vec2 v_uv;\n"
  "uniform float iTime;\n"
  "uniform vec2 iResolution;\n"
  "vec2 iMouse = vec2(1);\n"
  "uniform sampler2D iChannel0;\n"
  "#define u_time iTime\n"
  "#define u_resolution iResolution\n"
  "#define u_mouse iMouse\n"
  "out vec4 fragColor;\n";

static const char *FRAGMENT_SHADER_SOURCE_SUFFIX =
  "\n"
  "void main() {\n"
  "  vec2 fragCoord = gl_FragCoord.xy;\n"
  "  mainImage(fragColor, fragCoord);\n"
  "}\n";

std::regex mainImageReg("void\\s+mainImage\\(\\s*out\\s+vec4\\s+fragColor\\s*,\\s*in\\s+vec2\\s+fragCoord\\s*\\)");
std::regex mainReg("void\\s+main\\(\\s*\\)");

struct Uniform {
  std::string name;
  GLenum type;
  GLsizei size;
  GLint location;
};

void getActiveUniforms(GLuint program, std::unordered_map<uint32_t, Uniform> &uniforms) {
  GLint uniformCount = 0;
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);
  for (size_t i = 0; i < uniformCount; i++)
  {
    GLsizei bufSize = 64;
    GLchar name[bufSize];
    GLsizei length = 0;
    GLint size;
    GLenum type;
    glGetActiveUniform(program, i, bufSize, &length, &size, &type, name);
    uint32_t namehash = StringUtils::StringHash{ name };

    Uniform uniform = Uniform{};
    uniform.name = std::string(name).substr(0, length);
    uniform.type = type;
    uniform.size = size;
    uniform.location = glGetUniformLocation(program, uniform.name.c_str());

    uniforms[namehash] = uniform;
  }
}

class ShaderProgram
{
private:
  GLuint vertShader, fragShader;

public:
  std::vector<char> vertShaderSource;
  std::vector<char> fragShaderSource;
  GLuint program;
  std::unordered_map<uint32_t, Uniform> uniforms;

  ShaderProgram();

  ~ShaderProgram();

  void createGLProgram();

  static GLuint createShader(GLenum shaderType, std::vector<char> shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    int size = shaderSource.size();
    const char *source = shaderSource.data();
    glShaderSource(shader, 1, &source, &size);
    glCompileShader(shader);

    int success = 0;
    int bufSize = 512;
    char infoLog[bufSize];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      int length = 0;
      glGetShaderInfoLog(shader, bufSize, &length, infoLog);
      std::string s;
      for (auto c : shaderSource) {
        s.push_back(c);
      }
      std::cout << "Compile shader failed:\n" << infoLog << "\nShader Source:\n" << s << std::endl;
      return -1;
    }

    return shader;
  }

  static GLuint createProgram(GLuint vertShader, GLuint fragShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    int success = 0;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(program, 512, NULL, infoLog);
      std::cerr << "Link Program failed:\n\t" << infoLog << std::endl;
      return -1;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
  }

  static ShaderProgram *loadFromFilename(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate);

    if (!file.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
      throw std::runtime_error("Failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize / sizeof(char));

    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);

    file.close();

    ShaderProgram *shaderProgram = new ShaderProgram();
    shaderProgram->fragShaderSource = std::move(buffer);
    shaderProgram->createGLProgram();

    return shaderProgram;
  }
};

ShaderProgram::ShaderProgram() {
  int size = strlen(VERTEX_SHADER_SOURCE);
  vertShaderSource.resize(size);
  vertShaderSource.clear();

  for (size_t i = 0; i < size; i++)
  {
    vertShaderSource.push_back(VERTEX_SHADER_SOURCE[i]);
  }

}

void ShaderProgram::createGLProgram()
{
  GLFWwindow *window = glfwGetCurrentContext();
  if (window == nullptr) {
    throw std::runtime_error("Context isn't initialized.");
  }

  if (glIsProgram(program)) {
    return;
  }

  int size = strlen(FRAGMENT_SHADER_SOURCE_PREFIX);
  std::vector<char> fullFragShaderSource;
  for (size_t i = 0; i < size; i++)
  {
    fullFragShaderSource.push_back(FRAGMENT_SHADER_SOURCE_PREFIX[i]);
  }

  size = fragShaderSource.size();
  char *data = fragShaderSource.data();
  for (size_t i = 0; i < size; i++)
  {
    fullFragShaderSource.push_back(data[i]);
  }

  std::string tmp = std::string(fullFragShaderSource.begin(), fullFragShaderSource.end());
  std::smatch searchResult;
  std::regex_search(tmp, searchResult, mainImageReg);
  if (searchResult.length() > 0) {
    searchResult.empty();

    std::regex_search(tmp, searchResult, mainReg);

    if (searchResult.length() <= 0) {
      size = strlen(FRAGMENT_SHADER_SOURCE_SUFFIX);
      for (size_t i = 0; i < size; i++)
      {
        fullFragShaderSource.push_back(FRAGMENT_SHADER_SOURCE_SUFFIX[i]);
      }
    }
  }


  vertShader = ShaderProgram::createShader(GL_VERTEX_SHADER,   vertShaderSource);
  fragShader = ShaderProgram::createShader(GL_FRAGMENT_SHADER, fullFragShaderSource);
  program    = ShaderProgram::createProgram(vertShader, fragShader);

  GLint attribCount = 0;
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribCount);
  // printf("attribCount: %d\n", attribCount);
  for (size_t i = 0; i < attribCount; i++)
  {
    GLsizei bufSize = 64;
    GLchar name[bufSize];
    GLsizei length = 0;
    GLint size;
    GLenum type;
    glGetActiveAttrib(program, i, bufSize, &length, &size, &type, name);
    // printf("\t %zu: %s %d %d\n", i, name, type, size);
  }

  getActiveUniforms(program, uniforms);
}

ShaderProgram::~ShaderProgram()
{
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  glDeleteProgram(program);
}
