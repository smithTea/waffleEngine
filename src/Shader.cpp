//
// Created by smith on 13.07.26.
//

#include "headers/Shader.h"

#include <fstream>
#include <iostream>
#include <ostream>

std::string Shader::ReadFile(const std::filesystem::path& path)
{
   std::ifstream file(path);

   if (!file)
   {
      throw std::runtime_error("Couldn't open shader.");
   }

   return std::string(
       std::istreambuf_iterator<char>(file),
       std::istreambuf_iterator<char>());
}

bool Shader::Compile(const std::string& vertex, const std::string& fragment)
{
   GLuint vertexShader =  glCreateShader(GL_VERTEX_SHADER);

   const char* v_char = vertex.c_str();
   glShaderSource(vertexShader, 1, &v_char, nullptr);

   glCompileShader(vertexShader);

   GLint success = 0;

   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

   if (success == GL_FALSE) {
      GLchar infoLog[512];
      glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
      glDeleteShader(vertexShader);
      return false;
   }

   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   const char* f_char = fragment.c_str();
   glShaderSource(fragmentShader, 1, &f_char, nullptr);
   glCompileShader(fragmentShader);

   glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
   if (success == GL_FALSE) {
      GLchar infoLog[512];
      glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
      glDeleteShader(fragmentShader);
      glDeleteShader(vertexShader);
      return false;
   }

   m_Program = glCreateProgram();
   glAttachShader(m_Program, vertexShader);
   glAttachShader(m_Program, fragmentShader);
   glLinkProgram(m_Program);

   glGetProgramiv(m_Program, GL_LINK_STATUS, &success);

   if (success == GL_FALSE) {
      GLchar infoLog[512];
      glGetProgramInfoLog(m_Program, 512, nullptr, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
      glDeleteProgram(m_Program);
      return false;
   }

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   return true;
}

void Shader::Bind() {
   glUseProgram(m_Program);
}