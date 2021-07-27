#include "./common.h"
#include "texture.h"

Texture make_texture(Texture::Param2d const& param)
{
    Texture result;
    result.type = GL_TEXTURE_2D;

    glCreateTextures( GL_TEXTURE_2D, 1, &result.name);
    // glCreateBuffers(1, &result.handle_buffer);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, param.width, param.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, param.data);
    result.data_size = param.width * param.height * 4;
    glTextureStorage2D(result.name, 1, GL_RGBA8, param.width, param.height);
    // glTextureBuffer(result.handle_tx, GL_RGBA8UI, result.handle_buffer);
    result.width  = param.width;
    result.height = param.height;
    return result;
}

void update_data(Texture& tx, void const* pixels)
{
    // void* ptr = glMapNamedBuffer(tx.handle_buffer, GL_WRITE_ONLY);
    // memcpy(ptr, data, tx.data_size);
    // glUnmapNamedBuffer(tx.handle_buffer);
    glTextureSubImage2D(tx.name, 0, 0, 0, tx.width, tx.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void release_texture(Texture& tx)
{
    if (tx.name != -1)
    {
        glDeleteTextures(1, &tx.name);
        tx.name = -1;
    }
    // glDeleteBuffers(1, &tx.handle_buffer);
}