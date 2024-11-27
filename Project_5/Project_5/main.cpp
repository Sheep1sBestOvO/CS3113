#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Win.h"
#include "Lose.h"

constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };


// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;
Menu   *g_menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Win* g_level_win;
Lose* g_level_lose;


Scene   *g_levels[6];

SDL_Window* g_display_window;
GLuint g_font_texture_id;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

int player_lives = 3;

bool g_is_colliding_bottom = false;

AppStatus g_app_status = RUNNING;

const char TEXT_FILEPATH[]  = "Assets/font1.png";
void swtich_to_scene(Scene *scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Platformer",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    g_font_texture_id = Utility::load_texture(TEXT_FILEPATH);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_menu = new Menu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_level_win = new Win();
    g_level_lose = new Lose();
    
    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = g_level_win;
    g_levels[5] = g_level_lose;
    
    // Start at level A
    switch_to_scene(g_levels[0]);
    
}

void process_input()
{
    g_current_scene->g_game_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    g_app_status = TERMINATED;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                            // Quit the game with a keystroke
                            g_app_status = TERMINATED;
                            break;
                        
                    case SDLK_SPACE:
                        // Jump
                        if (g_current_scene->g_game_state.player->get_collided_bottom())
                        {
                            g_current_scene->g_game_state.player->jump();
                            Mix_PlayChannel(-1, g_current_scene->g_game_state.jump_sfx, 0);
                        }
                        break;
                    case SDLK_RETURN:
                        if (g_current_scene == g_levels[0])
                        {
                            switch_to_scene(g_levelA);

                        }
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])       g_current_scene->g_game_state.player->move_left();
    else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->g_game_state.player->move_right();
        
    if (glm::length( g_current_scene->g_game_state.player->get_movement()) > 1.0f)
        g_current_scene->g_game_state.player->normalise_movement();
   
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP, player_lives);
        player_lives = g_current_scene-> g_game_state.player->m_player_lives;
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene->g_game_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->g_game_state.player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    if (g_current_scene == g_levelA && g_current_scene->g_game_state.player->get_position().y < -10.0f) switch_to_scene(g_levelB);
    if (g_current_scene == g_levelB && g_current_scene->g_game_state.player->get_position().y < -10.0f) switch_to_scene(g_levelC);
    if (g_current_scene == g_levelC && g_current_scene->g_game_state.player->get_position().y < -10.0f)
        switch_to_scene(g_levels[4]);
    if(player_lives == 0){
        switch_to_scene(g_levels[5]);
        }
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);
    
    
    SDL_GL_SwapWindow(g_display_window);
    
    if (g_current_scene == g_levelC && g_current_scene->g_game_state.player->get_position().y < -10.0f)
        Utility::draw_text(&g_shader_program, g_font_texture_id, std::string("YOU WIN"), 0.25f, 0.0f, glm::vec3(-1.25f, 0.0f, 0.0f));
}

void shutdown()
{
    SDL_Quit();
    
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_level_win;
    delete g_level_lose;
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        
        render();
    }
    
    shutdown();
    return 0;
}
