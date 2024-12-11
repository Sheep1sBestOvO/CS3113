#include "LevelB.h"
#include "Utility.h"




const char  SPRITESHEET_FILEPATH[]    = "Assets/planesheet.png",
            BGM_FILEPATH[]            = "Assets/bgmusic.ogg",
            SHOOT_FILEPATH[]          = "Assets/shoot.wav",
            FLY_FILEPATH[]            = "Assets/fly.wav",
            ENEMY_FILEPATH[]          = "Assets/middle.png",
            BOSS_FILEPATH[]           = "Assets/boss.png",
            BULLET_FILEPATH[]         = "Assets/bullet2.png",
            ENEMY_BULLET_FILEPATH[]   = "Assets/bullet1.png",
            SPECIAL_BULLET_FILEPATH[] = "Assets/bullet3.png";


LevelB::~LevelB()
{
   
    delete    g_game_state.player;
    delete [] g_game_state.enemies;
    delete [] g_game_state.super_bullets;
    delete    g_game_state.bullets;

}

void LevelB::initialise()
{
    float x_positions[4] = {5.0f, 2.0f, 8.0f, 5.0f};
    float x_movement[3]  = {1.0f, -1.0f, 0.0f};
    float y_movement[3]  = {-1.0f, -1.0f, -1.0f};
    
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
    g_game_state.player->m_origin_posi = glm::vec3(2.0f, -7.00f, 0.0f);
    
    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    
    GLuint enemy_bullet_texture_id = Utility::load_texture(ENEMY_BULLET_FILEPATH);
    GLuint special_bullet_texture_id = Utility::load_texture(SPECIAL_BULLET_FILEPATH);
    
    
    
    g_game_state.bullets = new Entity(
          bullet_texture_id,         // texture id
          2.0f,                      // speed
          1.0f,                      // width
          1.0f,                       // height
          BULLET
                                      );
    
    g_game_state.bullets->deactivate();
    

    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    GLuint boss_texture_id = Utility::load_texture(BOSS_FILEPATH);
    
    g_game_state.enemies = new Entity[ENEMY_COUNT];
    

    for (int i = 0; i < 3; ++i){
        g_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, SHOOTER);
        
        g_game_state.enemies[i].set_position(glm::vec3(x_positions[i], -1.0f, 0.0f));
        g_game_state.enemies[i].set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    
    g_game_state.enemies[3] = Entity(boss_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, BOSS);
    
    g_game_state.enemies[3].set_position(glm::vec3(5.0f, -1.0f, 0.0f));
    g_game_state.enemies[3].set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    g_game_state.enemies[3].set_scale(glm::vec3(2.0f, 2.0f, 0.0f));
    g_game_state.enemies[3].deactivate();
    
    g_game_state.super_bullets = new Entity[8];
    g_game_state.super_bullets[0] =  Entity(
          special_bullet_texture_id,         // texture id
          1.0f,                      // speed
          1.0f,                      // width
          1.0f,                       // height
          SUPER_BULLET
                                      );
    g_game_state.super_bullets[0].set_position(g_game_state.enemies[1].get_position());
    g_game_state.super_bullets[0].set_movement(glm::vec3(1.0f, -1.0f, 0.0f));
    
    g_game_state.super_bullets[1] =  Entity(
          special_bullet_texture_id,         // texture id
          1.0f,                      // speed
          1.0f,                      // width
          1.0f,                       // height
          SUPER_BULLET
                                      );
    g_game_state.super_bullets[1].set_position(g_game_state.enemies[2].get_position());
    g_game_state.super_bullets[1].set_movement(glm::vec3(1.0f, -1.0f, 0.0f));
    
    for (int i = 2; i < 5; ++i){
        g_game_state.super_bullets[i] =  Entity(
              special_bullet_texture_id,         // texture id
              1.0f,                      // speed
              1.0f,                      // width
              1.0f,                       // height
              SUPER_BULLET
                                          );
        g_game_state.super_bullets[i].set_position(g_game_state.enemies[3].get_position());
        g_game_state.super_bullets[i].set_movement(glm::vec3(x_movement[i-2], y_movement[i-2], 0.0f));
        g_game_state.super_bullets[i].deactivate();
    }
    
    for (int i = 5; i < 7; ++i){
        g_game_state.super_bullets[i] =  Entity(
              special_bullet_texture_id,         // texture id
              1.0f,                      // speed
              1.0f,                      // width
              1.0f,                       // height
              SUPER_BULLET
                                          );
        g_game_state.super_bullets[i].set_position(g_game_state.enemies[3].get_position());
        g_game_state.super_bullets[i].set_movement(glm::vec3(x_movement[i-5], y_movement[i-5], 0.0f));
        g_game_state.super_bullets[i].deactivate();
    }

    g_game_state.enemy_bullets = new Entity(
          enemy_bullet_texture_id,         // texture id
          2.0f,                      // speed
          1.0f,                      // width
          1.0f,                       // height
          ENEMY_BULLET
                                      );
    g_game_state.enemy_bullets->set_position(g_game_state.enemies[0].get_position());
    g_game_state.enemy_bullets->set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
   
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);
    
    g_game_state.shoot_sfx = Mix_LoadWAV(SHOOT_FILEPATH);
    g_game_state.fly_sfx = Mix_LoadWAV(FLY_FILEPATH);
}


void LevelB::update(float delta_time,int player_lives)
{
    g_game_state.player->m_player_lives = player_lives;
    
    
    if (!g_game_state.bullets->m_is_active){
        g_game_state.player->m_can_shoot = true;
    }
    
    if (g_game_state.bullets->get_position().y > 0.1f){
        g_game_state.bullets->deactivate();
    }
    
    g_game_state.player->update(delta_time, g_game_state.player, g_game_state.enemies, ENEMY_COUNT);
    
    g_game_state.bullets->update(delta_time, g_game_state.bullets, g_game_state.enemies, ENEMY_COUNT);
    
    g_game_state.enemy_bullets->update(delta_time, g_game_state.player, g_game_state.player, 1);
    
    for (int i = 0; i < 7; i++)
    {
        g_game_state.super_bullets[i].update(delta_time, g_game_state.player, g_game_state.player, 1);
    }
    
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        g_game_state.enemies[i].update(delta_time, g_game_state.player, NULL, NULL);
        if (!g_game_state.enemies[0].m_is_active && !g_game_state.enemies[1].m_is_alive && !g_game_state.enemies[2].m_is_alive){
            g_game_state.enemies[3].activate();
            for (int i = 2; i < 5; ++i){
                g_game_state.super_bullets[i].activate();
            }
            if (g_game_state.enemies[3].m_player_lives == 1 && !set_boss_move ){
                g_game_state.enemies[3].set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
                for (int i = 5; i < 7; ++i){
                    g_game_state.super_bullets[i].activate();
                }
                set_boss_move = true;
            }
            if (g_game_state.enemies[3].get_position().x > 9.0f) {
                g_game_state.enemies[3].set_movement (glm::vec3(-1.0f, 0.0f, 0.0f));
            }else if (g_game_state.enemies[3].get_position().x < 0.5f) {
                g_game_state.enemies[3].set_movement (glm::vec3(1.0f, 0.0f, 0.0f));
            }
            if (g_game_state.enemies[3].m_player_lives == 0){
                g_game_state.enemies[3].m_is_alive = false;
            }
        }
    }
    if (g_game_state.enemies[3].m_is_active){
        
        g_game_state.enemies[3].update(delta_time, g_game_state.player, NULL, NULL);
    }
    if (!g_game_state.enemies[3].m_is_alive){
        g_game_state.enemies[3].deactivate();
        enemy_remain = false;
    }
}


void LevelB::render(ShaderProgram *program)
{
    if (g_game_state.player->m_is_active)
    g_game_state.player->render(program);
    
    if (g_game_state.bullets->m_is_active)
    g_game_state.bullets->render(program);
    
    if (g_game_state.enemy_bullets->m_is_active)
    g_game_state.enemy_bullets->render(program);
    
    if (g_game_state.super_bullets->m_is_active)
        g_game_state.super_bullets->render(program);
        
  
    for (int i = 0; i < 7; i++)
    {
        if (g_game_state.super_bullets[i].m_is_active)
        g_game_state.super_bullets[i].render(program);
    }
    
    for (int i = 0; i < 4; i++){
        if ( g_game_state.enemies[i].m_is_active){
            g_game_state.enemies[i].render(program);
        }
    }

}
