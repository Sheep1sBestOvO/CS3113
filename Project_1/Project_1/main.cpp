#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 1.0f,        // Maximum red value for pink
                BG_GREEN   = 0.75f,     // Moderate green value for pink
                BG_BLUE    = 0.85f,     // Higher blue value for a pinkish tone
                BG_OPACITY = 1.0f;       // Full opacity

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
                LEVEL_OF_DETAIL    = 0, // mipmap reduction image level
                TEXTURE_BORDER     = 0; // this value MUST be zero


constexpr char GINTOKI_SPRITE_FILEPATH[]    = "Assets/gintama.png",
               MILK_SPRITE_FILEPATH[]       = "Assets/strawberrymilk.png";

constexpr glm::vec3 GINTOKI_INIT_SCALE       = glm::vec3(1.48f, 2.00f, 0.0f),
                    INIT_POS_MILK            = glm::vec3(-2.0f, 0.0f, 0.0f);

glm::vec3 g_gintoki_pos = glm::vec3(0.0f, 0.0f, 0.0f);

float TRANS_VAL       = 0.0f,
      G_TRANS_SPEED   = 1.0f;



SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_gintoki_matrix,
          g_milk_matrix,
          g_projection_matrix;

float g_previous_ticks = 0.0f;

constexpr float G_RADIUS        = 2.0f,
                MILK_BASE_SCALE = 1.0f,
                MAX_AMPLITUDE   = 0.1f,
                PULSE_SPEED     = 0.2f,
                G_ROT_INCREMENT = 1.0f,
                G_ROT_SPEED     = 1.0f;



float g_x_coord          = 0.0f,
      g_y_coord          = 0.0f,
      scale_factor       = 0.0f,
      g_angle            = 0.0f;



GLuint g_gintoki_texture_id,
       g_milk_texture_id;

glm::vec3 g_rotation_milk = glm::vec3(0.0f, 0.0f, 0.0f);


GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Gintoki and milk!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_gintoki_matrix       = glm::mat4(1.0f);
    g_milk_matrix          = glm::mat4(1.0f);
    g_view_matrix          = glm::mat4(1.0f);
    g_projection_matrix    = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_milk_matrix          = glm::translate(g_milk_matrix, INIT_POS_MILK);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_gintoki_texture_id   = load_texture(GINTOKI_SPRITE_FILEPATH);
    g_milk_texture_id = load_texture(MILK_SPRITE_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


void update()
{
    glm::vec3 rotation_triggers;
    
    /* Delta time calculations */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    if (g_gintoki_pos.x >= 3.0f){
        G_TRANS_SPEED = -1.0f;
        std::cout << G_TRANS_SPEED << std::endl;
    }
    if (g_gintoki_pos.x <= -3.0f){
        G_TRANS_SPEED = 1.0f;
    }
    
    g_gintoki_matrix  = glm::mat4(1.0f);
    g_milk_matrix     = glm::mat4(1.0f);
    rotation_triggers = glm::vec3(0.0f, 1.0f, 0.0f);
    
    scale_factor       = MILK_BASE_SCALE + MAX_AMPLITUDE * glm::cos(ticks / PULSE_SPEED);
    g_angle           += G_ROT_SPEED * delta_time;
    g_x_coord          = G_RADIUS * cos(g_angle);
    g_y_coord          = G_RADIUS * sin(g_angle);
    g_gintoki_pos.x   += G_TRANS_SPEED * delta_time;
    g_rotation_milk.y += G_ROT_INCREMENT * delta_time;
    
    glm::vec3 MILK_SCALE_FACTORS = glm::vec3(scale_factor, scale_factor, 0.0f);
    
    g_gintoki_matrix       = glm::translate(g_gintoki_matrix, g_gintoki_pos);
    g_milk_matrix          = glm::translate(g_gintoki_matrix, glm::vec3(g_x_coord, g_y_coord, 0));
    
    g_milk_matrix          = glm::rotate(g_milk_matrix, g_rotation_milk.y, rotation_triggers);
    g_milk_matrix          = glm::scale(g_milk_matrix, MILK_SCALE_FACTORS);
    g_gintoki_matrix       = glm::scale(g_gintoki_matrix, GINTOKI_INIT_SCALE);
    
    
    
}


void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                          0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_gintoki_matrix, g_gintoki_texture_id);
    draw_object(g_milk_matrix, g_milk_texture_id);

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

