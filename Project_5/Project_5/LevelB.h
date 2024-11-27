#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
       int ENEMY_COUNT = 1;
       
       // ————— CONSTRUCTOR ————— //
       ~LevelB();
       
       // ————— METHODS ————— //
       void initialise() override;
       void update(float delta_time, int player_lives) override;
       void render(ShaderProgram *program) override;
};
