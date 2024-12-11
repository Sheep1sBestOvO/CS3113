#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"


/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    Entity* player;
    Entity* enemies;
    Entity* bullets;
    Entity* enemy_bullets;
    Entity* super_bullets;

    Mix_Music* bgm;
    Mix_Chunk *shoot_sfx;
    Mix_Chunk *fly_sfx;
    
    int next_scene_id = -1;
    
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //
    
    int m_number_of_enemies= 0;
    
    GameState g_game_state;
    
    
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time,int player_lives) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    // ————— GETTERS ————— //
    GameState const get_state()             const { return g_game_state;    }
    int       const get_number_of_enemies() const { return m_number_of_enemies; }
};
