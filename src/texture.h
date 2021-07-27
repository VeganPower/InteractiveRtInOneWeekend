#pragma once


struct Texture
{
#if 0
    enum Format
    {
        k_8_8_8_8
    };

    enum Type
    {

    };

    struct Storage
    {
        Format format;
        Type type;
    };
#endif
    struct Param2d
    {
        uint width;
        uint height;
        // Storage storage; // Assumes rgb 8 bit sRGB
        void* data = nullptr;
    };

    GLenum type;
    uint name = -1;
    // uint handle_buffer;
    size_t data_size;
    uint width;
    uint height;
};

Texture make_texture(Texture::Param2d const& param);
void release_texture(Texture& tx);
void update_data(Texture& tx, void const* data);