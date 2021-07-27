#include "common.h"
#include "shader.h"

#include <cstdio>

const GLchar* const vs_shader = R"-=-(#version 150
in  vec3 in_Position;
in  vec3 in_Color;

//uniform vec4 scale_offset;
uniform mat4 proj_view;

out vec3 ex_Color;


void main(void)
{
    //vec4 grid_pos =
    gl_Position = proj_view * vec4(in_Position, 1.0);//vec4(grid_pos.x, grid_pos.y, 0.5, 1.0);
    ex_Color = in_Color;
}
)-=-";

const GLchar* const ps_shader = R"-=-(#version 450
precision highp float;

layout (location = 0) in vec3 ex_Color;
layout (location = 0) out vec4 color;

void main(void)
{
    color = vec4(ex_Color,1.0);
}
)-=-";

const GLchar* const vs_bg_shader = R"-=-(#version 450 core

in int gl_VertexID;
layout (location = 0) out vec2 uv;

void main(void)
{
    switch(gl_VertexID)
    {
    case 0:
        gl_Position = vec4(-1.0,-1.0, 1.0, 1.0);
        uv = vec2(0.0,-1.0);
        break;
    case 1:
        gl_Position = vec4( 3.0,-1.0, 1.0, 1.0);
        uv = vec2(2.0,-1.0);
        break;
    case 2:
    default:
        gl_Position = vec4(-1.0, 3.0, 1.0, 1.0);
        uv = vec2(0.0, 1.0);
        break;
    }
}
)-=-";


const GLchar* const ps_bg_shader = R"-=-(#version 450 core
precision highp float;

uniform sampler2D image;

layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 color;

void main(void)
{
    color = texture(image, uv);
}
)-=-";

GLuint create_shader(GLenum shader_type, const GLchar* const source)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    int is_compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled)
    {
       int max_length = 0;
       glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
       char* info_log = (char *)malloc(max_length);
       glGetShaderInfoLog(shader, max_length, &max_length, info_log);
       printf("%s", info_log);
       free(info_log);
       return -1;
    }
    return shader;
}

GLuint link_program(GLuint vs, GLuint ps)
{
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, ps);

    glLinkProgram(shader_program);
    int is_linked = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, (int *)&is_linked);
    if (!is_linked)
    {
        int max_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);
        char* info_log = (char *)malloc(max_length);
        glGetProgramInfoLog(shader_program, max_length, &max_length, info_log);
        printf("%s", info_log);
        free(info_log);
        glDeleteShader(vs);
        glDeleteShader(ps);
        return -1;
    }
    return shader_program;
}

void make_shader(ShaderProgram &result)
{
    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vs_shader);
    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, ps_shader);
    GLuint shader_program = link_program(vertex_shader, fragment_shader);

    glBindAttribLocation(shader_program, 0, "in_Position");
    glBindAttribLocation(shader_program, 1, "in_Color");
    //GLint sc_handle = glGetUniformLocation(shader_program, "scale_offset");
    GLint mpj_handle = glGetUniformLocation(shader_program, "proj_view");
    result = ShaderProgram  {
        vertex_shader, fragment_shader,
        shader_program,
        //sc_handle,
        mpj_handle
    };
}

void make_bg_shader(ShaderProgram &result)
{
    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vs_bg_shader);
    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, ps_bg_shader);
    GLuint shader_program = link_program(vertex_shader, fragment_shader);
    result = ShaderProgram  {
        vertex_shader, fragment_shader,
        shader_program,
        //sc_handle,
        0
    };
}