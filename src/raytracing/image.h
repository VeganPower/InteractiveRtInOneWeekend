#pragma once

struct Image
{
    Image() = default;
    Image(unsigned w, unsigned h)
        : width{w}
        , height{h}
    {
        pixels = (color*)malloc(sizeof(color) * w * h);
    }
    ~Image()
    {
        free(pixels);
    }

    void clear(color const& c)
    {
        unsigned pix_count = height * width;
        for (unsigned x = 0; x < pix_count; ++x)
        {
            pixels[x] = c;
        }
    }

    void resize(unsigned w, unsigned h)
    {
        free(pixels);
        width = w;
        height = h;
        pixels = (color*)malloc(sizeof(color) * w * h);
    }

    color const* get_line(unsigned i) const
    {
        assert(i < height);
        return pixels + width * i;
    }
    color* get_line(unsigned i)
    {
        assert(i < height);
        return pixels + width * i;
    }

    unsigned width = 0;
    unsigned height = 0;
    color* pixels = nullptr;
};
