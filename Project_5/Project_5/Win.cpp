#include "Win.h"
#include "Utility.h"


#define LEVEL_win_WIDTH 20
#define LEVEL_win_HEIGHT 8


const char  SPRITESHEET_FILEPATH[]  = "Assets/walk.png",
            MAP_TILESET_FILEPATH[]  = "Assets/oak_woods_tileset.png",
            BGM_FILEPATH[]          = "Assets/Ancient Mystery Waltz Presto.mp3",
            ENEMY_FILEPATH[]        = "Assets/jump.png",
            TEXT_FILEPATH[]         = "Assets/font1.png";

unsigned int LEVEL_Win_DATA[] =
{
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

Win::~Win()
{
    delete[] g_game_state.enemies;
    delete   g_game_state.player;
    delete   g_game_state.map;

    Mix_FreeMusic(g_game_state.bgm);
}

void Win::initialise()
{
    GLuint map_texture_id =  Utility::load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL_win_WIDTH, LEVEL_win_HEIGHT, LEVEL_Win_DATA, map_texture_id, 1.0f, 25, 24);
    // ————— GEORGE SET-UP ————— //
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    int player_walking_animation[4][8] =
    {
    { 8, 9, 10, 11, 12, 13, 14, 15 },  // for George to move to the left,
    { 40, 41, 42, 43, 44, 45, 46, 47}, // for George to move to the right,
    { 24, 25, 26, 27, 28, 29, 30, 31},
    { 0, 1, 2, 3, 4, 5, 6, 7}

    };

    glm::vec3 acceleration = glm::vec3(0.0f,-4.905f, 0.0f);

    g_game_state.player = new Entity(
        player_texture_id,         // texture id
        1.0f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        6,                         // animation row amount
        0.9f,                      // width
        0.75f,                       // height
        PLAYER
    );
    
   // Jumping
   g_game_state.player->set_jumping_power(5.0f);
   g_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
   g_game_state.player->set_scale(glm::vec3(1.0f, 2.0f, 0.0f));
    
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    g_game_state.enemies = new Entity[ENEMY_COUNT];

    
    g_game_state.enemies[0] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);

    g_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    g_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
   
   

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);
    
    }


void Win::update(float delta_time, int player_lives)
{
    if (delta_time == -1) {
        g_game_state.next_scene_id = 4;
    }
        
    
}


void Win::render(ShaderProgram *program)
{
    g_game_state.map->render(program);
    GLuint text_texture_id = Utility::load_texture(TEXT_FILEPATH);
    
    Utility::draw_text(program, text_texture_id, "You Lose", 0.5f, -0.1f, glm::vec3(3.8f,-2.0f, 0.0f));
}
