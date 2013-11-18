#pragma once

#include <string>
#include "glew.h"

namespace OmochiRenderer {
  class GLShaderUtils {
  public:
    explicit GLShaderUtils(GLenum type);
    ~GLShaderUtils();
    bool ReadShaderSource(const std::string &file);
    bool CompileShader();

    void PrintShaderInfoLog();

    GLuint GetShader() const { return m_shader; }

  private:
    GLuint m_shader;
  };

  class GLProgramUtils {
  public:
    GLProgramUtils();
    GLProgramUtils(const GLShaderUtils &vertShader, const GLShaderUtils &fragShader);


    void Init(const GLShaderUtils &vertShader, const GLShaderUtils &fragShader);

    void BindAttributeLocation(GLuint index, GLchar *name);
    bool LinkProgram();
    void UseShader();

    void PrintProgramInfoLog();
  private:
    GLuint m_program;
  };
}
