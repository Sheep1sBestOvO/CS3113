#include "Scene.h"

class Win : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 0;
    bool enemy_remain = true;
    // ————— CONSTRUCTOR ————— //
    ~Win();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time,int player_lives) override;
    void render(ShaderProgram *program) override;
};
