#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 4;
    bool enemy_remain = true;
    bool set_boss_move = false;
    
    // ————— CONSTRUCTOR ————— //
    ~LevelB();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time, int player_lives) override;
    void render(ShaderProgram *program) override;
};
