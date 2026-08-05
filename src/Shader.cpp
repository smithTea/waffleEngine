//
// Created by smith on 13.07.26.
//

#include "headers/Shader.h"



std::string Shader::ReadFile(const std::filesystem::path& path)
{
   std::ifstream file(path);

   if (!file)
   {
      throw std::runtime_error("Couldn't open shader.");
   }

   // Minimal #include "file" support so shared GLSL snippets (lighting,
   // post-fx helpers) can be pulled into a shader without a real preprocessor.
   std::string source;
   std::string line;
   while (std::getline(file, line))
   {
      if (line.rfind("#include", 0) == 0)
      {
         const auto first = line.find('"');
         const auto last = line.rfind('"');
         if (first != std::string::npos && last != std::string::npos && last > first)
         {
            const std::string includeName = line.substr(first + 1, last - first - 1);
            source += ReadFile(path.parent_path() / includeName);
            continue;
         }
      }
      source += line + "\n";
   }

   return source;
}


bool Shader::LoadFromFiles(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
   const std::string vertexCode = ReadFile(vertexPath);
   const std::string fragmentCode = ReadFile(fragmentPath);
   return Compile(vertexCode, fragmentCode);
}


bool Shader::Compile(const std::string& vertex, const std::string& fragment)
{
   const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

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

   const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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

GLint Shader::GetUniformLocation(const std::string& name)
{
   const auto it = m_UniformLocations.find(name);

   if (it != m_UniformLocations.end()) {
      return it->second;
   }

   const GLint location = glGetUniformLocation(m_Program, name.c_str());
   m_UniformLocations.emplace(name, location);

   return location;
}

void Shader::SetMat4(
    const std::string& name,
    const glm::mat4& matrix)
{
   const GLint location = GetUniformLocation(name);

   if (location == -1) {
      return;
   }

   glUniformMatrix4fv(
       location,
       1,
       GL_FALSE,
       glm::value_ptr(matrix)
   );
}

void Shader::SetVec3(
    const std::string& name,
    const glm::vec3& value)
{
   const GLint location = GetUniformLocation(name);

   if (location == -1) {
      return;
   }

   glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetFloat(
    const std::string& name,
    const float value)
{
   const GLint location = GetUniformLocation(name);

   if (location == -1) {
      return;
   }

   glUniform1f(location, value);
}

void Shader::SetInt(
    const std::string& name,
    const int value)
{
   const GLint location = GetUniformLocation(name);

   if (location == -1) {
      return;
   }

   glUniform1i(location, value);
}
