#include <regex>
#include <iostream>

#include "Program.h"
#include "string_utils.hpp"

namespace {

#define DEBUG false

const char VERTEX_SHADER_SOURCE[] = R"(
#version 330 core
precision highp float;

out VS_OUT {
  vec2 uv;
} vs_out;

vec2 positions[3] = vec2[3](
  vec2(-1.0, -1.0),
  vec2( 3.0, -1.0),
  vec2(-1.0,  3.0)
);

void main() {
  vec2 position = positions[gl_VertexID];
  gl_Position = vec4(position, 0.0, 1.0);
  vs_out.uv = position * 0.5 + 0.5;
}

)";


const char GEOMETRY_SHADER_SOURCE[] = R"(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
  vec2 uv;
} gs_in[];

out vec2 v_uv;

void main() {
  gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
  v_uv = vec2(0.0, 0.0);
  EmitVertex();

  gl_Position = vec4(3.0, -1.0, 0.0, 1.0);
  v_uv = vec2(2.0, 0.0);
  EmitVertex();

  gl_Position = vec4(-1.0, 3.0, 0.0, 1.0);
  v_uv = vec2(0.0, 2.0);
  EmitVertex();

  EndPrimitive();
}
)";

const char FRAGMENT_SHADER_SOURCE_PREFIX[] = R"(
#version 330 core
precision highp float;

in vec2 v_uv;

uniform float iTime;
uniform vec2 iResolution;
vec2 iMouse = vec2(1);
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;

#define u_time iTime
#define u_resolution iResolution
#define u_mouse iMouse

out vec4 fragColor;

)";

const char FRAGMENT_SHADER_SOURCE_MAIN_WRAPPER[] = R"(
void main() {
  vec2 fragCoord = gl_FragCoord.xy;
  mainImage(fragColor, fragCoord);
}

)";

void PrintShaderLog(GLuint shader) {
  GLsizei info_log_length = 0;
  GLsizei shader_source_length = 0;
  glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &info_log_length );
  glGetShaderiv( shader, GL_SHADER_SOURCE_LENGTH, &shader_source_length );

  GLchar* info_log = new GLchar[info_log_length+1];
  GLchar* shader_source = new GLchar[shader_source_length];

  glGetShaderInfoLog(shader, info_log_length, &info_log_length, info_log);
  glGetShaderSource(shader, shader_source_length, &shader_source_length, shader_source);
  std::cerr << "Compile Shader failed:\n"
    << info_log
    << "\nShader Source:\n"
    << shader_source
    << std::endl;

  delete [] info_log;
  delete [] shader_source;
}

void CheckProgram(GLuint program) {
  int success = 0;
  GLsizei info_log_length = 0;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
  GLchar* info_log = new GLchar[info_log_length+1];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, info_log_length, &info_log_length, info_log);
    std::cerr << "Link Program failed:\n\t"
      << info_log
      << std::endl;
  }

  delete [] info_log;
}

void PrintUniformLog(const std::string& uniform_name) {
  if (!DEBUG) {
    return;
  }
  std::cout << "BindInt: uniform " << uniform_name << " is not an active uniform." << std::endl;
}

void CheckShader(GLuint shader) {
  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    PrintShaderLog(shader);
  }
}

GLuint CreateShader(GLenum shader_type, const std::string &shader_source) {
  GLuint shader = glCreateShader(shader_type);
  const char *source = shader_source.c_str();
  glShaderSource(shader, 1, &source, NULL /* &size */);
  glCompileShader(shader);

  CheckShader(shader);

  return shader;
}

GLuint CreateProgram(GLuint vertex_shader, GLuint frag_shader) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, frag_shader);

  GLuint geom_shader = CreateShader(GL_GEOMETRY_SHADER, GEOMETRY_SHADER_SOURCE);
  glAttachShader(program, geom_shader);

  glLinkProgram(program);

  CheckProgram(program);

  glDeleteShader(vertex_shader);
  glDeleteShader(geom_shader);
  glDeleteShader(frag_shader);

  return program;
}

std::unordered_map<std::string, Uniform> GetActiveUniforms(GLuint program) {
  std::unordered_map<std::string, Uniform> uniforms;

  GLint uniformCount = 0;
  // GLint maxLength = 0;
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);
  // glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

  std::vector<GLuint> indices;
  indices.resize(uniformCount);
  for (int i = 0; i < uniformCount; i++)
  {
    indices[i] = i;
  }

  std::vector<GLint> sizes;
  std::vector<GLint> types;
  std::vector<GLint> lengths;
  sizes.resize(uniformCount);
  types.resize(uniformCount);
  lengths.resize(uniformCount);
  glGetActiveUniformsiv(program, uniformCount, indices.data(), GL_UNIFORM_SIZE, sizes.data());
  glGetActiveUniformsiv(program, uniformCount, indices.data(), GL_UNIFORM_TYPE, types.data());
  glGetActiveUniformsiv(program, uniformCount, indices.data(), GL_UNIFORM_NAME_LENGTH, lengths.data());

  for (int i = 0; i < uniformCount; i++) {
    GLint size = sizes[i];
    GLenum type = types[i];
    GLint length = lengths[i];
    std::string name;
    name.resize(length);

    glGetActiveUniform(program, i, length, nullptr, nullptr, nullptr, name.data());
    GLint location = glGetUniformLocation(program, name.data());

    Uniform uniform = { name, type, size, location };
    uniforms[name] = uniform;
  }

  return uniforms;
}

std::string GetFullFragmentShaderSource(const std::string& fragment_shader_source) {
  static const std::regex mainImageReg("void\\s+mainImage\\(\\s*out\\s+vec4\\s+fragColor\\s*,\\s*in\\s+vec2\\s+fragCoord\\s*\\)");
  static const std::regex mainReg("void\\s+main\\(\\s*\\)");

  std::string full_frag_shader_source =
    std::string(FRAGMENT_SHADER_SOURCE_PREFIX) +
    fragment_shader_source;

  std::smatch mainImageFuncMatch;
  std::smatch mainFuncMatch;

  if ( std::regex_search(full_frag_shader_source, mainImageFuncMatch, mainImageReg) &&
       !std::regex_search(full_frag_shader_source, mainFuncMatch, mainReg)
  ) {
    full_frag_shader_source += std::string(FRAGMENT_SHADER_SOURCE_MAIN_WRAPPER);
  }

  return full_frag_shader_source;
}

}  // anonymous namespace

Program::Program(
  const std::string &vertex_shader_source,
  const std::string &fragment_shader_source) {
  if (glfwGetCurrentContext() == NULL) {
    throw std::runtime_error("Context isn't initialized.");
  }

  if (glIsProgram(program)) {
    return;
  }

  GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_source);
  GLuint frag_shader = CreateShader(GL_FRAGMENT_SHADER, GetFullFragmentShaderSource(fragment_shader_source));
  program  = CreateProgram(vertex_shader, frag_shader);

  uniforms = GetActiveUniforms(program);
}

Program::Program(
    const std::string &fragment_shader_source)
  : Program(VERTEX_SHADER_SOURCE, fragment_shader_source) {
  
}

Program::~Program()
{
  glDeleteProgram(program);
}

void Program::Use() const {
  glUseProgram(program);
}

void Program::BindInt(const std::string& uniform_name, int value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniform1iv(location, 1, &value);
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindFloat(const std::string& uniform_name, float value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniform1fv(location, 1, &value);
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindVec2(const std::string& uniform_name, std::array<float, 2> value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniform2fv(location, 1, value.data());
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindVec3(const std::string& uniform_name, std::array<float, 3> value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniform3fv(location, 1, value.data());
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindVec4(const std::string& uniform_name, std::array<float, 4> value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniform4fv(location, 1, value.data());
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindMat4(const std::string& uniform_name, const std::array<float, 16>& value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniformMatrix4fv(location, 1, false /* transpose */, value.data());
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindMat3(const std::string& uniform_name, const std::array<float, 9>& value) const {
  GLint location = glGetUniformLocation(program, uniform_name.c_str());
  if (location != -1) {
    glUniformMatrix3fv(location, 1, false /* transpose */, value.data());
  } else {
    PrintUniformLog(uniform_name);
  }
}

void Program::BindTexture2D(const std::string& sampler_uniform_name, GLuint texture, GLuint texture_unit) const {
  GLint location = glGetUniformLocation(program, sampler_uniform_name.c_str());
  if (location != -1) {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    BindInt(sampler_uniform_name, texture_unit);
  } else {
    PrintUniformLog(sampler_uniform_name);
  }
}

void Program::BindTexture3D(const std::string& sampler_uniform_name, GLuint texture, GLuint texture_unit) const {
  GLint location = glGetUniformLocation(program, sampler_uniform_name.c_str());
  if (location != -1) {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_3D, texture);
    BindInt(sampler_uniform_name, texture_unit);
  } else {
    PrintUniformLog(sampler_uniform_name);
  }
}
