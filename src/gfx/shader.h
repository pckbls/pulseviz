#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include "./texture.h"

class Shader
{
public:
    Shader(const std::string& vertex_shader_source, const std::string& fragment_shader_source);
    Shader(const std::string& name);
    Shader(Shader const& other) = delete;
    ~Shader();
    void bind();
    void unbind();
    GLint getAttribLocation(const std::string &name);
    GLint getUniformLocation(const std::string &name);

    // TODO: Remove?
    void bindTextureToUniform(const std::string& uniform, const Texture& texture, unsigned int unit);

private:
    void initialize(const std::string& vertex_shader_source, const std::string& fragment_shader_source);
    void compileAndLink();
    GLuint compileAndAttachShader(GLuint type, const std::string& source);
    void link();

    GLuint handle;
    bool linked;
};

// TODO
#ifndef INCLUDE_SHADER
    #define INCLUDE_SHADER(x) ""
#endif

#endif // SHADER_H
