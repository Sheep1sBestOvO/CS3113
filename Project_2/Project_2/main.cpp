#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 1.0f;

constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT= WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char  PLAYER_1_WIN_FILEPATH[]  = "Assets/player_1_win.png",
                PLAYER_2_WIN_FILEPATH[]  = "Assets/player_2_win.png",
                BALL_SPRITE_FILEPATH[]   = "Assets/ball.png",
                PADDLE_SPRITE_FILEPATH[] = "Assets/paddle.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_PADDLE  = glm::vec3(1.0f, 2.0f, 0.0f),
                    INIT_SCALE_WIN_MSG = glm::vec3(10.0f, 4.0f, 0.0f),
                    INIT_SCALE_BALL    = glm::vec3(1.0f, 1.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_projection_matrix, g_paddle_1_matrix, g_ball_matrix, g_paddle_2_matrix, g_player_1_win_matrix, g_player_2_win_matrix;

float g_previous_ticks = 0.0f;

GLuint g_ball_texture_id,
       g_player_1_win_texture_id,
       g_player_2_win_texture_id,
       g_paddle_texture_id;


glm::vec3 g_paddle_1_position = glm::vec3(-4.75f, 0.0f, 0.0f);
glm::vec3 g_paddle_1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_paddle_2_position = glm::vec3(4.9f, 0.0f, 0.0f);
glm::vec3 g_paddle_2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball_position     = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement     = glm::vec3(-1.0f, -1.0f, 0.0f);


float g_paddle_speed = 2.0f,
      g_ball_speed   = 2.0f,
      g_upper_bound  = 3.75f,
      g_lower_bound  = -3.75f,
      g_left_bound   = -5.0f,
      g_right_bound  = 5.0f;

bool g_player_1_win  = false,
     g_player_2_win  = false,
     g_paddle_2_up   = true,
     g_player_2_mode = true;

void initialise();
void process_input();
void update();
void render();
void shutdown();

constexpr GLint NUMBER_OF_TEXTURES = 1;  // to be generated, that is
constexpr GLint LEVEL_OF_DETAIL    = 0;  // base image level; Level n is the nth mipmap reduction image
constexpr GLint TEXTURE_BORDER     = 0;  // this value MUST be zero


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

bool check_collision(glm::vec3 &position_a, glm::vec3 &position_b)
{
    
    float x_distance = fabs(position_a.x - position_b.x) - (MINIMUM_COLLISION_DISTANCE / 2.0f);
    float y_distance = fabs(position_a.y - position_b.y) - (MINIMUM_COLLISION_DISTANCE / 2.0f);
    
    if (x_distance < 0 && y_distance <= 0){
        return true;
    }
    else{
        return false;
    }
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pong Clone",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
    
    if (g_display_window == nullptr)
    {
        shutdown();
    }
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_paddle_1_matrix = glm::mat4(1.0f);
    g_paddle_2_matrix = glm::mat4(1.0f);
    g_ball_matrix     = glm::mat4(1.0f);
    g_player_1_win_matrix = glm::mat4(1.0f);
    g_player_2_win_matrix = glm::mat4(1.0f);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_paddle_texture_id = load_texture(PADDLE_SPRITE_FILEPATH);
    g_ball_texture_id   = load_texture(BALL_SPRITE_FILEPATH);
    g_player_1_win_texture_id = load_texture(PLAYER_1_WIN_FILEPATH);
    g_player_2_win_texture_id = load_texture(PLAYER_2_WIN_FILEPATH);
    
    
    g_player_1_win_matrix =  glm::scale(g_player_1_win_matrix, INIT_SCALE_WIN_MSG);
    g_player_2_win_matrix =  glm::scale(g_player_2_win_matrix, INIT_SCALE_WIN_MSG);
    
    

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_paddle_1_movement = glm::vec3(0.0f);
    g_paddle_2_movement = glm::vec3(0.0f);


    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                                                                             
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                                                                             
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_t:
                        g_player_2_mode = !g_player_2_mode;
                                                                             
                    default:
                        break;
                }
                                                                             
            default:
                break;
        }
    }
                                                                             
                                                                             
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
                                                                             
    if (key_state[SDL_SCANCODE_W] && g_paddle_1_position.y < g_upper_bound - 0.9f)
    {
        g_paddle_1_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S] && g_paddle_1_position.y > g_lower_bound + 0.76f)
    {
        g_paddle_1_movement.y = -1.0f;
    }
    
    if (g_player_2_mode){
        if (key_state[SDL_SCANCODE_UP] && g_paddle_2_position.y < g_upper_bound - 0.9f)
        {
            g_paddle_2_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_DOWN] && g_paddle_2_position.y > g_lower_bound + 0.76f)
        {
            g_paddle_2_movement.y = -1.0f;
        }
    }else{

        if (g_paddle_2_position.y > g_upper_bound - 0.95f){
            g_paddle_2_up = false;
        }else if (g_paddle_2_position.y < g_lower_bound + 0.75f){
            g_paddle_2_up = true;
        }
        if (g_paddle_2_up){
            g_paddle_2_movement.y = 1.0f;
        }else{
            g_paddle_2_movement.y = -1.0f;
        }
    }
    
    // This makes sure that the player can't "cheat" their way into moving
    // faster
    if (glm::length(g_paddle_1_movement) > 1.0f || glm::length(g_paddle_2_movement) > 1.0f)
    {
        g_paddle_1_movement = glm::normalize(g_paddle_1_movement);
        g_paddle_2_movement = glm::normalize(g_paddle_2_movement);
    }
}

void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // Add direction * units per second * elapsed time
    g_paddle_1_position += g_paddle_1_movement * g_paddle_speed * delta_time;
    g_paddle_2_position += g_paddle_2_movement * g_paddle_speed * delta_time;
    
    g_paddle_1_matrix = glm::mat4(1.0f);
    g_paddle_1_matrix = glm::translate(g_paddle_1_matrix, g_paddle_1_position);

    
    g_paddle_2_matrix = glm::mat4(1.0f);
    g_paddle_2_matrix = glm::translate(g_paddle_2_matrix, g_paddle_2_position);

    g_paddle_1_matrix  = glm::scale(g_paddle_1_matrix, INIT_SCALE_PADDLE);
    g_paddle_2_matrix  = glm::scale(g_paddle_2_matrix, INIT_SCALE_PADDLE);
    
    //check how the ball goes
    
    if (g_ball_position.y > 3.75f) {
        g_ball_movement.y = -1.0f;
    }else if (g_ball_position.y < -3.75f){
        g_ball_movement.y = 1.0f;
    }
    if(g_ball_position.x < -5.5f){
        g_ball_movement = glm::vec3(0.0f);
        g_player_2_win = true;
    }else if (g_ball_position.x > 5.5f){
        g_ball_movement = glm::vec3(0.0f);
        g_player_1_win = true;
    }
    
    if (check_collision(g_ball_position, g_paddle_1_position)){
        g_ball_movement.x = 1.0f;
    }else if (check_collision(g_ball_position, g_paddle_2_position)){
        g_ball_movement.x = -1.0f;
    }
    
    
    if (glm::length(g_ball_movement) > 1.0f)
        {
            g_ball_movement = glm::normalize(g_ball_movement);
        }
    
    g_ball_matrix = glm::mat4(1.0f);
    g_ball_position += g_ball_movement * g_ball_speed * delta_time;
    g_ball_matrix = glm::translate(g_ball_matrix, g_ball_position);
    g_ball_matrix = glm::scale(g_ball_matrix, INIT_SCALE_BALL);
}



void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // Bind texture
    if (!g_player_1_win && !g_player_2_win){
        draw_object(g_paddle_1_matrix, g_paddle_texture_id);
        draw_object(g_paddle_2_matrix, g_paddle_texture_id);
        draw_object(g_ball_matrix, g_ball_texture_id);
    }
    
    
    if (g_player_1_win){
        draw_object(g_player_1_win_matrix, g_player_1_win_texture_id);
    }else if (g_player_2_win){
        draw_object(g_player_2_win_matrix, g_player_2_win_texture_id);
    }

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
