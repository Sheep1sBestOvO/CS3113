#include "LevelA.h"
#include "Utility.h"


#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 8


const char  SPRITESHEET_FILEPATH[]  = "Assets/planesheet.png",
            BGM_FILEPATH[]          = "Assets/bgmusic.ogg",
            FLY_FILEPATH[]          = "Assets/fly.wav",
            SHOOT_FILEPATH[]        = "Assets/shoot.wav",
            ENEMY_FILEPATH[]        = "Assets/spider.png",
            BULLET_FILEPATH[]       = "Assets/bullet2.png";


LevelA::~LevelA()
{
   
    delete    g_game_state.player;
    delete [] g_game_state.enemies;
    delete    g_game_state.bullets;
//    
}

void LevelA::initialise()
{
    
    // ————— PLAYER ————— //
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    int player_walking_animation[4][5] =
    {
        { 1, 2, 3, 4, 5},// for George to move to the left,
        { 6, 7, 8, 9, 10},
        { 1, 2, 3, 4, 5},// for George to move to the left,
        { 6, 7, 8, 9, 10}
        
    };
    
    glm::vec3 acceleration = glm::vec3(0.0f,0.0f, 0.0f);

    g_game_state.player = new Entity(
        player_texture_id,         // texture id
        2.0f,                      // speed
        acceleration,              // acceleration
        0.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        5,                         // animation frame amount
        0,                         // current animation index
        5,                         // animation column amount
        8,                         // animation row amount
        1.0f,                      // width
        1.0f,                     // height
        PLAYER
    );
    
    
    
    g_game_state.player->set_position(glm::vec3(2.0f, -7.00f, 0.0f));
    
    
    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    
    
    
    g_game_state.bullets = new Entity(
          bullet_texture_id,         // texture id
          2.0f,                      // speed
          1.0f,                      // width
          1.0f,                       // height
          BULLET
                                      );
    
    g_game_state.bullets->deactivate();
    

    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    g_game_state.enemies = new Entity[ENEMY_COUNT];
    
    g_game_state.enemies[0] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, H_WALKING);
    
    g_game_state.enemies[0].set_position(glm::vec3(5.0f, -2.0f, 0.0f));
    g_game_state.enemies[0].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    
    g_game_state.enemies[1] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, V_WALKING);
    
    g_game_state.enemies[1].set_position(glm::vec3(6.0f, 0.0f, 0.0f));
    g_game_state.enemies[1].set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
    g_game_state.enemies[1].deactivate();
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);
    
    g_game_state.shoot_sfx = Mix_LoadWAV(SHOOT_FILEPATH);
    g_game_state.fly_sfx = Mix_LoadWAV(FLY_FILEPATH);

}


void LevelA::update(float delta_time,int player_lives)
{
    g_game_state.player->m_player_lives = player_lives;
    g_game_state.player->update(delta_time, g_game_state.player, g_game_state.enemies, ENEMY_COUNT);
    
    if (!g_game_state.bullets->m_is_active){
        g_game_state.player->m_can_shoot = true;
    }

    if (g_game_state.bullets->get_position().y > 0.1f){
//        std::cout << 1 << std::endl;
        g_game_state.bullets->deactivate();
    }
    
    g_game_state.bullets->update(delta_time, g_game_state.bullets, g_game_state.enemies, ENEMY_COUNT);
   
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        g_game_state.enemies[i].update(delta_time, g_game_state.player, NULL, NULL);
        if (!g_game_state.enemies[0].m_is_active && g_game_state.enemies[1].m_is_alive ){
            g_game_state.enemies[1].activate();
        }
    }
//    std::cout << g_game_state.enemies[1].m_is_alive << std::endl;
    if (!g_game_state.enemies[0].m_is_alive && !g_game_state.enemies[1].m_is_alive){
        enemy_remain = false;
        g_game_state.next_scene_id = 2;
    }
    
}


void LevelA::render(ShaderProgram *program)
{

    g_game_state.player->render(program);
    
    if (g_game_state.bullets->m_is_active)
    g_game_state.bullets->render(program);
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        if ( g_game_state.enemies[i].m_is_active){
            g_game_state.enemies[i].render(program);
        }
    }

}
