#pragma once

struct ShaderProgram
{
    GLuint vertex_shader, fragment_shader;
    GLuint program;
    //GLint scale_offset_handle;
    GLint proj_view_handle;
};

void make_shader(ShaderProgram &result);
void make_bg_shader(ShaderProgram &result);