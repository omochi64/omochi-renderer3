#include "stdafx.h"

#include "GLUtils.h"

#ifndef NO_PREVIEW_WINDOW

#include <gl/GL.h>
#include <sstream>
#include <fstream>

using namespace std;

namespace OmochiRenderer {
  GLShaderUtils::GLShaderUtils(GLenum type)
  {
    m_shader = glCreateShader(type);
  }

  GLShaderUtils::~GLShaderUtils()
  {
    glDeleteShader(m_shader);
  }

  bool GLShaderUtils::ReadShaderSource(const std::string &file) {
    ifstream ifs(file.c_str());

    if (!ifs) {
      cerr << "failed to load " << file << endl;
      return false;
    }

    ifs.seekg(0, fstream::end);
    auto eofPos = ifs.tellg();

    ifs.clear();
    ifs.seekg(0, fstream::beg);
    auto begPos = ifs.tellg();

    GLint size = static_cast<GLint>(eofPos) - static_cast<GLint>(begPos);

    shared_ptr<GLchar> source(new GLchar[size+1], std::default_delete<GLchar[]>());
    //GLchar *source = new GLchar[size+1];
    memset(source.get(), 0, sizeof(GLchar)*(size + 1));

    ifs.read(source.get(), size);
    //ifs.read(source, size);

    const GLchar *p = source.get();
    glShaderSource(m_shader, 1, &p, &size);

    return true;
  }


  void GLShaderUtils::PrintShaderInfoLog() {
    GLsizei bufSize;

    glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1) {
      GLchar *log = new GLchar[bufSize];

      if (log) {
        GLsizei length;
        glGetShaderInfoLog(m_shader, bufSize, &length, log);
        cout << "InfoLog:" << endl << log << endl;

        delete[] log;
      }
    }
  }

  bool GLShaderUtils::CompileShader() {
    glCompileShader(m_shader);
    GLint compiled;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compiled);

    return compiled != GL_FALSE;
  }

  //
  // GLProgramUtils
  //
  GLProgramUtils::GLProgramUtils()
    : m_program(0)
  {
  }
  GLProgramUtils::GLProgramUtils(const GLShaderUtils &vertShader, const GLShaderUtils &fragShader) {
    Init(vertShader, fragShader);
  }

  void GLProgramUtils::Init(const GLShaderUtils &vertShader, const GLShaderUtils &fragShader) {
    m_program = glCreateProgram();

    glAttachShader(m_program, vertShader.GetShader());
    glAttachShader(m_program, fragShader.GetShader());
  }

  void GLProgramUtils::PrintProgramInfoLog() {
    GLsizei bufSize;

    /* シェーダのリンク時のログの長さを取得する */
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1) {
      GLchar *infoLog = (GLchar *)malloc(bufSize);

      if (infoLog != NULL) {
        GLsizei length;

        /* シェーダのリンク時のログの内容を取得する */
        glGetProgramInfoLog(m_program, bufSize, &length, infoLog);
        fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
      }
      else
        fprintf(stderr, "Could not allocate InfoLog buffer.\n");
    }
  }

  void GLProgramUtils::BindAttributeLocation(GLuint index, GLchar *name) {
    glBindAttribLocation(m_program, index, name);
  }

  bool GLProgramUtils::LinkProgram() {
    glLinkProgram(m_program);
    GLint linked;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

    return linked != GL_FALSE;
  }

  void GLProgramUtils::UseShader() {
    if (m_program != 0) {
      glUseProgram(m_program);
    }
  }
}

#endif // NO_PREVIEW_WINDOW
