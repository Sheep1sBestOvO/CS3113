#include "Scene.h"

class Lose : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 0;
    // ————— CONSTRUCTOR ————— //
    ~Lose();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time,int player_lives) override;
    void render(ShaderProgram *program) override;
};
