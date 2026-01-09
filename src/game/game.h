#pragma once
#include "../map/map.h"
#include "../graphics/graphics.h"
#include <SDL3/SDL.h>

class Game {
public:
    enum class EMove {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Game(Graphics* graphics) noexcept;
    ~Game();

    SDL_AppResult handle_event(SDL_Event* event);
    SDL_AppResult handle_loop();
    Graphics* get_graphics() const noexcept;
    // Load map from file
    void load_map(const char* map_name);
    // Unload current map
    void unload_map();
    bool map_completed() const noexcept;
    void move_player(const EMove &move);
private:
    Map* map;
    Graphics* graphics;

    int move_pos(const MTType &obj, int pos, const EMove &move);
    void update_maptile(const MTType &obj, int curr_pos, int new_pos);
};
