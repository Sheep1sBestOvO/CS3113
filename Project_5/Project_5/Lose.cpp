#include "Lose.h"
#include "Utility.h"


#define LEVELlose_WIDTH 20
#define LEVELlose_HEIGHT 8


const char  SPRITESHEET_FILEPATH[]  = "Assets/walk.png",
            MAP_TILESET_FILEPATH[]  = "Assets/oak_woods_tileset.png",
            BGM_FILEPATH[]          = "Assets/Ancient Mystery Waltz Presto.mp3",
            ENEMY_FILEPATH[]        = "Assets/jump.png",
            TEXT_FILEPATH[]         = "Assets/font1.png";

unsigned int LEVEL_Lose_DATA[] =
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

Lose::~Lose()
{
    delete[] g_game_state.enemies;
    delete   g_game_state.player;
    delete   g_game_state.map;

    Mix_FreeMusic(g_game_state.bgm);
}

void Lose::initialise()
{
    GLuint map_texture_id =  Utility::load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVELlose_WIDTH, LEVELlose_HEIGHT, LEVEL_Lose_DATA, map_texture_id, 1.0f, 25, 24);

    
    }


void Lose::update(float delta_time, int player_lives)
{
    
        
    
}


void Lose::render(ShaderProgram *program)
{
    g_game_state.map->render(program);
    GLuint text_texture_id = Utility::load_texture(TEXT_FILEPATH);
    
    Utility::draw_text(program, text_texture_id, "You Lose", 0.5f, -0.1f, glm::vec3(3.8f,-2.0f, 0.0f));
   
}
