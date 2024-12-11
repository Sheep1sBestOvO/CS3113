#include "win.h"
#include "Utility.h"


const char  SPRITESHEET_FILEPATH[]  = "Assets/planesheet.png",
            ENEMY_FILEPATH[]        = "Assets/spider.png",
            BULLET_FILEPATH[]       = "Assets/ball.png",
            TEXT_FILEPATH[]         = "Assets/font1.png";



Win::~Win()
{
    delete    g_game_state.player;
    delete [] g_game_state.enemies;
    delete [] g_game_state.super_bullets;
    delete    g_game_state.bullets;

//    
}

void Win::initialise()
{
    // ————— PLAYER ————— //
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    g_game_state.next_scene_id = -1;
    //    
    //    int player_walking_animation[4][5] =
    //    {
    //        { 1, 2, 3, 4, 5},// for George to move to the left,
    //        { 6, 7, 8, 9, 10},
    //        { 1, 2, 3, 4, 5},// for George to move to the left,
    //        { 6, 7, 8, 9, 10}
    //        
    //    };
    //    
    //    glm::vec3 acceleration = glm::vec3(0.0f,0.0f, 0.0f);
    //
    //    g_game_state.player = new Entity(
    //        player_texture_id,         // texture id
    //        2.0f,                      // speed
    //        acceleration,              // acceleration
    //        0.0f,                      // jumping power
    //        player_walking_animation,  // animation index sets
    //        0.0f,                      // animation time
    //        5,                         // animation frame amount
    //        0,                         // current animation index
    //        5,                         // animation column amount
    //        8,                         // animation row amount
    //        1.0f,                      // width
    //        1.0f,                     // height
    //        PLAYER
    //    );
    //    
    //    
    //    
    //    g_game_state.player->set_position(glm::vec3(2.0f, -7.00f, 0.0f));
    //    
    //    
    //    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    //    
    //    
    //    
    //    g_game_state.bullets = new Entity(
    //          bullet_texture_id,         // texture id
    //          2.0f,                      // speed
    //          1.0f,                      // width
    //          1.0f,                       // height
    //          BULLET
    //                                      );
    //    
    //    g_game_state.bullets->deactivate();
    //    
    //
    //    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    //    
    //    g_game_state.enemies = new Entity[ENEMY_COUNT];
    //    
    //    g_game_state.enemies[0] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, H_WALKING);
    //    
    //    g_game_state.enemies[0].set_position(glm::vec3(5.0f, -2.0f, 0.0f));
    //    g_game_state.enemies[0].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    //    
    //    g_game_state.enemies[1] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, V_WALKING);
    //    
    //    g_game_state.enemies[1].set_position(glm::vec3(6.0f, 0.0f, 0.0f));
    //    g_game_state.enemies[1].set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
    //    g_game_state.enemies[1].deactivate();
    //
    //}
}

void Win::update(float delta_time, int player_lives)
{
        
    
}


void Win::render(ShaderProgram *program)
{
    GLuint text_texture_id = Utility::load_texture(TEXT_FILEPATH);
    
    Utility::draw_text(program, text_texture_id, "You Win!!", 0.5f, -0.1f, glm::vec3(3.8f,-2.0f, 0.0f));
    Utility::draw_text(program, text_texture_id, "HOOOOO!", 0.5f, -0.1f, glm::vec3(3.8f,-2.0f, 0.0f));
}
