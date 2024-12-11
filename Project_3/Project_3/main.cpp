#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 200
#define LEVEL1_HEIGHT 200
#define LEVEL1_LEFT_EDGE 50.0f

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
#include "Utility.h"
#include "Scene.h"
#include "menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "Win.h"
#include "lose.h"
#include "Effects.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED     = 0.0f;
constexpr float BG_BLUE    = 0.0f;
constexpr float BG_GREEN   = 0.0f;
constexpr float BG_OPACITY = 1.0f;


constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;

Effects *g_effects;

Menu   *g_menu;
LevelA *g_level_a;
LevelB *g_level_b;
Win    *g_win;
Lose   *g_lose;


SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
int g_player_lives = 3;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("AirCraft War",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    // ————— LEVEL A SETUP ————— //
    g_menu    = new Menu();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_win     = new Win();
    g_lose    = new Lose();
    switch_to_scene(g_menu);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
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
                    case SDLK_f:
                        
                        if(g_current_scene->get_state().player->m_can_shoot){
                            g_current_scene->get_state().bullets->set_position(g_current_scene->get_state().player->get_position());
                            g_current_scene->get_state().bullets->activate();
                            g_current_scene->get_state().bullets->set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
                            g_current_scene->get_state().bullets->activate();
                            g_current_scene->get_state().player->m_can_shoot = false;
                            Mix_PlayChannel(-1, g_current_scene->g_game_state.shoot_sfx, 0);
                        }
                    case SDLK_RETURN:
                        if (g_current_scene == g_menu)
                        {
                            switch_to_scene(g_level_a);
                        }
                        break;

                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT]){       g_current_scene->get_state().player->move_left();
//        Mix_PlayChannel(-1, g_current_scene->g_game_state.fly_sfx, 0);
    }
    else if (key_state[SDL_SCANCODE_RIGHT]) {
        g_current_scene->get_state().player->move_right();
//        Mix_PlayChannel(-1, g_current_scene->g_game_state.fly_sfx, 0);
    }
     
    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
 
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
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
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP, g_player_lives);
        g_effects->update(FIXED_TIMESTEP);
        g_player_lives = g_current_scene-> g_game_state.player->m_player_lives;
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    g_view_matrix = glm::translate(g_view_matrix, g_effects->get_view_offset());
    
    
    
    if (g_current_scene == g_level_a && !g_level_a->enemy_remain){
        switch_to_scene(g_level_b);
    }else if  (g_current_scene == g_level_b && !g_level_b->enemy_remain){
        switch_to_scene(g_win);
    }else if(g_player_lives == 0){
                switch_to_scene(g_lose);
        }
    
    if ( g_current_scene == g_level_b && g_level_b->get_state().enemies[3].m_player_lives == 1){
        g_effects->start(SHAKE, 2.0f);
    }
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    

    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);
    
    g_effects->render();
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
}

// ————— GAME LOOP ————— //
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
