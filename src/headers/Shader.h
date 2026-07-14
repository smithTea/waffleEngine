//
// Created by smith on 13.07.26.
//

#ifndef OPENGLSKELETON_SHADER_H
#define OPENGLSKELETON_SHADER_H
#include <filesystem>
#include <string>
#include <glad/gl.h>


class Shader {
public:
    bool Compile(const std::string& vertex, const std::string& fragment);
    void Bind();
    bool LoadFromFiles(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath);
private:
    std::string ReadFile(const std::filesystem::path& path);
    GLuint m_Program = 0;
};


#endif //OPENGLSKELETON_SHADER_H