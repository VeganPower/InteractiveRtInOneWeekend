#include "common.h"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_inverse.hpp>

#include "arcball_imgui.h"
#include "angle.h"
#include "dimension.h"
#include "texture.h"
#include "shader.h"
#include "shapes.h"
#include <stdio.h>

#include "camera.h"
#include "raytracing/raytrace.h"
#include "raytracing/camera.h"

//using namespace glm;

struct AppData
{
    GLFWwindow* window = nullptr;
    ShaderProgram shaders;
    ShaderProgram bg_shader;
    Dimension display_size;

    Mesh points;
    Mesh axis;
    Mesh grid;
    Mesh ball_circle;

    ImVec2 offset;
    float size;
    //int lod_bias=0;
    bool draw_grid = true;
    bool draw_hmd = true;
    bool draw_distored = true;

    // plot data
    // ImVec2* line0;
    // ImVec2* line1;
    // ImVec2* line2;
    // uint line_points_counts;
    ImVec4 clear_color;

    ArcBall arc_ctrl;
    Camera camera;
    Angle fov = Angle::Degree(33.f);
    float near_plane = 0.05f;
    float far_plane = 50.0f;
    bool reverse_z = false;
    bool infinite_proj = false;

    bool pause = false;

    Texture ray_result;
    Image rt_intermediate;
    RtScene scene;
    float scene_scale = 1.f;

    void update_view_size(int w, int h);
    void update_camera();
    void rt_render();
    void reset_image();
    void set_camera();
};

void AppData::set_camera()
{
    set(camera, arc_ctrl);
    scene.cam = RtCamera(inverse(camera.projection_view()));
}

void AppData::update_camera()
{
    glm::mat4 proj_matrix = compute_projection_matrix((float)display_size.width, (float)display_size.height,
        tanf(fov.radian() * 0.5f), near_plane, far_plane, reverse_z, infinite_proj);
    camera.projection(proj_matrix);
    set_camera();
    if (reverse_z)
    {
        glClearDepth(0.f);
        glDepthFunc(GL_GREATER);
    }else
    {
        glClearDepth(1.f);
        glDepthFunc(GL_LESS);
    }
    reset_image();
}

void AppData::reset_image()
{
    scene.samples_per_pixel = 0;
    rt_intermediate.clear(color(0, 0, 0));
}

static double lin_to_sRGB(double x)
{
    double dark = x * 12.92;
    double light = pow((x * 1.055), 1.0 / 2.4) - 0.055;
    return x < 0.0031308 ? dark : light;
}

void convert(Image const& img, Texture & tx, double scale)
{
    unsigned pixels_count = img.width * img.height;
    uint32_t* pixels = new uint32_t[pixels_count];
    for (unsigned i = 0; i < pixels_count; ++i)
    {
        auto r = img.pixels[i].x() * scale;
        auto g = img.pixels[i].y() * scale;
        auto b = img.pixels[i].z() * scale;

        // Divide the color by the number of samples and gamma-correct for gamma=2.0.
        r = lin_to_sRGB(r);
        g = lin_to_sRGB(g);
        b = lin_to_sRGB(b);

        unsigned ur = clamp<int>(static_cast<int>(255 * r + 0.5), 0, 255);
        unsigned ug = clamp<int>(static_cast<int>(255 * g + 0.5), 0, 255);
        unsigned ub = clamp<int>(static_cast<int>(255 * b + 0.5), 0, 255);
        uint32_t color = ur | (ug << 8) | (ub << 16) | 0xFF000000;
        pixels[i] = color;
    }
    glTextureSubImage2D(tx.name, 0, 0, 0, tx.width, tx.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    delete[] pixels;
}

void AppData::rt_render()
{
    if (pause) return;
    raytrace(rt_intermediate, scene);
    scene.samples_per_pixel++;
    if (scene.samples_per_pixel > 0)
    {
       Real sample_scale = scene_scale / (double)scene.samples_per_pixel;
       convert(rt_intermediate, ray_result, sample_scale);
    }
}

void AppData::update_view_size(int w, int h)
{
    display_size.width  = w;
    display_size.height = h;
    release_texture(ray_result);
    rt_intermediate.resize(w, h);
    Texture::Param2d tx_param { (uint16_t)w, (uint16_t)h, nullptr };
    ray_result = make_texture(tx_param);
    update_camera();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    AppData* app = (AppData*)glfwGetWindowUserPointer(window);
    if (app == nullptr)
        return;
    app->update_view_size(width, height);
}

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
   const char* severity_str = "Unknown";
   switch(severity)
   {
   case GL_DEBUG_SEVERITY_HIGH: severity_str = "High"; break;
   case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "Medium"; break;
   case GL_DEBUG_SEVERITY_LOW: severity_str = "Low"; break;
   case GL_DEBUG_SEVERITY_NOTIFICATION: return; //severity_str = "Notification"; break;
   }
   fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity_str, message );
}

void fill_plot(AppData& app)
{
    // const f32 scale = 25.4f / 850.f; // 850 pdi in mm
    // const int res = (int)floor(lens_radius / scale) * 2;
    // app.line0 = new ImVec2[res];
    // app.line1 = new ImVec2[res];
    // app.line2 = new ImVec2[res];
    // app.line_points_counts = res;
    // for (int i = 0; i < res; ++i)
    // {
        // f32 x = (i + 0.5f) * scale - lens_radius;
        // f32 y0 = poly_evaluate_derivative(abs(x) / lens_radius, rDist, kNumDistortionCoeffs);
        // f32 y1 = poly_evaluate_derivative(abs(x) / lens_radius, gDist, kNumDistortionCoeffs);
        // f32 y2 = poly_evaluate_derivative(abs(x) / lens_radius, bDist, kNumDistortionCoeffs);
        // app.line0[i] = { x, 1.f / y0 };
        // app.line1[i] = { x, 1.f / y1 };
        // app.line2[i] = { x, 1.f / y2 };
    // }
}

void init_gl_state(ImVec4 clear_color)
{
    glClearDepth(0.f);
    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE); //GL_LOWER_LEFT
    glDepthFunc(GL_GREATER);
    // glEnable(GL_DEPTH_TEST);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
}

bool init(AppData& app)
{
    if (!glfwInit())
    {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    app.window = glfwCreateWindow(640, 480, "Hmd simulator", NULL, NULL);
    if (app.window == nullptr)
    {
        fprintf(stderr, "Failed to create window!\n");
        glfwTerminate();
        return false;
    }
    glfwSetWindowUserPointer(app.window, &app);
    glfwSetFramebufferSizeCallback(app.window, framebuffer_size_callback);
    glfwMakeContextCurrent(app.window);
    if (!gladLoadGL())
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        glfwDestroyWindow(app.window);
        glfwTerminate();
        return false;
    }
    if (glDebugMessageCallback != nullptr)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    }
    app.clear_color = { 0.0157f, 0.0157f, 0.0157f, 1.f };
    init_gl_state(app.clear_color);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
    ImGui_ImplOpenGL3_Init();

    make_shader(app.shaders);
    make_bg_shader(app.bg_shader);

    app.axis = make_axis();
    GridOptions grid_options;
    grid_options.steps_x        = 10;
    grid_options.steps_y        = 10;
    grid_options.sub_steps      = 10;
    grid_options.space          = 10.f;
    grid_options.axis_color     = 0xfaf9f8;
    grid_options.line_color     = 0xbdb5ad;
    grid_options.sub_line_color = 0x403a34;
    //grid_options.height         = 1.f;
    app.grid = make_grid(grid_options);

    app.points = make_points();
    app.ball_circle = make_circle(0.9f, 128, 0xefece9);
    // ImU32 odd_circle  = 0x575049;
    // ImU32 even_circle = 0x403a34;
    //for (uint i = 1; i < 8; ++i)
    //{
    //    f32 alpha = i * 10.f * k_pi / 180.f;
    //    f32 r = eye_relief * tanf(alpha);
    //    app.lens_circle[i] = make_circle(r, 128, (i & 1) ? odd_circle : even_circle);
    //}
//    app.update_view_size()

    int w, h;
    glfwGetFramebufferSize(app.window, &w, &h);
    app.update_view_size(w, h);
    app.arc_ctrl.focus = glm::vec3(250.f, 250.f, 250.f);
    app.arc_ctrl.distance = 500.f;

    app.set_camera();

    fill_plot(app);
    app.offset = {0.f, 0.f};
    app.size = 35.f;
    return true;
}

void release(AppData& app)
{
    if (app.window == nullptr)
    {
        return;
    }
    release_texture(app.ray_result);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(app.window);
    glfwTerminate();
}

int main(int argc, char const** argv)
{
    AppData app;
    if (!init(app))
    {
        return -1;
    }
    GLuint vertex_array_object = 0;
    glGenVertexArrays(1, &vertex_array_object);

    ImVec2 old_drag;
    while (!glfwWindowShouldClose(app.window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            if (app.arc_ctrl.update(ImGui::GetIO(), app.display_size))
            {
                app.reset_image();
                app.set_camera();
            }
        }
        //static bool demo_open;
        //ImGui::ShowDemoWindow(&demo_open);
        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        //const float screen_ratio = (float)app.display_w / (float)app.display_h;
        //const vec2 uv_coord = { mouse_pos.x / app.display_w, 1.f - (mouse_pos.y / app.display_h) };
        //const vec2 clip_coord = uv_coord * 2.f - 1.f;
        //const vec2 coord = vec2{ clip_coord.x - app.offset.x, (clip_coord.y - app.offset.y) / screen_ratio } * app.size; // mm
        //const f32 distance = sqrt(dot(coord, coord));
        //const f32 angle = atan(distance / eye_relief) * 180.f * k_1_pi;
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Options");
            //ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Draw Grid", &app.draw_grid);
            ImGui::Checkbox("Pause raytracing", &app.pause);
            // ImGui::Checkbox("Draw Hmd", &app.draw_hmd);
            // ImGui::SliderInt("Lod Bias", &app.lod_bias, -5, 5);

            uint mouse_x = mouse_pos.x;
            uint mouse_y = mouse_pos.y;
            if (mouse_x < app.rt_intermediate.width && mouse_y < app.rt_intermediate.height)
            {
                color c = app.rt_intermediate.get_line(mouse_y)[mouse_x];
                float sample_scale = app.scene_scale / (float)app.scene.samples_per_pixel;
                float cr[3] = { sample_scale * (float)c.x(), sample_scale * (float)c.y(), sample_scale * (float)c.z()};
                ImGui::InputFloat3("color", cr);
            }
            ImGui::InputInt("Frame", &app.scene.samples_per_pixel);

            ImGui::InputFloat("Scene scale", &app.scene_scale);


            if (ImGui::CollapsingHeader("Camera"))
            {
                float fov = app.fov.radian();
                if (ImGui::SliderAngle("FOV", &fov, 1.f, 120.0f))
                {
                    app.fov = Angle::Radian(fov);
                    app.update_camera();
                }
                if (ImGui::DragFloatRange2("Clip planes", &app.near_plane, &app.far_plane, 0.25f, 0.001f, 1000.f)) app.update_camera();
                if (ImGui::Checkbox("Reverse z", &app.reverse_z))    app.update_camera();
                if (ImGui::Checkbox("infinite proj", &app.infinite_proj)) app.update_camera();

            }
            ImGui::SliderFloat("Size", &app.size, 1.f, 100.0f);
            if (ImGui::ColorEdit3("clear color", (float*)&app.clear_color))
                glClearColor(app.clear_color.x, app.clear_color.y, app.clear_color.z, app.clear_color.w);
            if (ImGui::Button("Centre"))
            {
               app.offset.x = 0.f;
               app.offset.y = 0.f;
            }

            //ImGui::Text("Mouse pos = %g %g. Angle %g", coord.x, coord.y, angle);
            // ImPlot::SetNextPlotLimits(-lens_radius, lens_radius, 0.f, 4.f);
            // if (ImPlot::BeginPlot("Line Plot", "mm", "zoom")) {
            //     ImPlot::SetNextLineStyle(ImVec4(1.f, 0.f, 0.f, 1.f));
            //     ImPlot::PlotLine("Red",   &app.line0[0].x, &app.line0[0].y, app.line_points_counts, 0, sizeof(ImVec2));
            //     ImPlot::SetNextLineStyle(ImVec4(0.f, 1.f, 0.f, 1.f));
            //     ImPlot::PlotLine("Green", &app.line1[0].x, &app.line1[0].y, app.line_points_counts, 0, sizeof(ImVec2));
            //     ImPlot::SetNextLineStyle(ImVec4(0.f, 0.f, 1.f, 1.f));
            //     ImPlot::PlotLine("Blue",  &app.line2[0].x, &app.line2[0].y, app.line_points_counts, 0, sizeof(ImVec2));
            //     ImPlot::EndPlot();
            // }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();

        app.rt_render();
        /*
        bool left_drag = ImGui::IsMouseDragging(0);

        if (!ImGui::GetIO().WantCaptureMouse && left_drag)
        {
            auto drag = ImGui::GetMouseDragDelta(0);
            app.offset.x = old_drag.x + drag.x / (float)app.display_size.width;
            app.offset.y = old_drag.y - drag.y / (float)app.display_size.height;
        }
        if (!left_drag)
        {
            old_drag = app.offset;
        }
        float wheel = ImGui::GetIO().MouseWheel;
        //if (wheel > 0.f)
        {
           app.size = max_of(1.f, wheel + app.size);
        }
*/

        glViewport(0, 0, app.display_size.width, app.display_size.height);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(app.bg_shader.program);
        glBindTextureUnit(0, app.ray_result.name);
        glBindVertexArray(vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(app.shaders.program);
        auto mpv = app.camera.projection_view();
        glUniformMatrix4fv(app.shaders.proj_view_handle, 1, GL_FALSE, glm::value_ptr(mpv));
        if (app.draw_grid)
        {
            draw(app.grid);
        }
        draw(app.axis);
        //draw(app.points);

        if (!io.WantCaptureMouse && ImGui::IsMouseDragging(0))
        {
           glm::mat4 temp = compute_orthographic_matrix(app.display_size.width, app.display_size.height, -1.f, 1.f, false, false);
           glUniformMatrix4fv(app.shaders.proj_view_handle, 1, GL_FALSE, glm::value_ptr(temp));
           draw(app.ball_circle);
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(app.window);
    }
    release(app);
    return 0;
}