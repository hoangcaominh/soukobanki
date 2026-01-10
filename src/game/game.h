#pragma once
#include "../map/map.h"
#include <SDL3/SDL.h>
#include <list>

#define MAX_MOVES 100

class Game {
public:
    enum class EMove {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Game() noexcept;
    ~Game();

    void handle_event(SDL_Event* event);
    Map* get_map_ptr() const noexcept;
    // Load map from file
    void load_map(const char* map_path);
    // Unload current map
    void unload_map();
    bool map_complete() const noexcept;
    void move_player(EMove move);
private:
    Map* map;
    std::list<EMove> move_order;

    int move_pos(MTType type, int pos, EMove move);
    void update_maptile(MTType type, int curr_pos, int new_pos);
};
