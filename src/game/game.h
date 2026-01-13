#pragma once
#include "map/map.h"
#include <SDL3/SDL.h>
#include <list>

#define MAX_MOVES 100

enum class GameMode {
    PLAY,
    EDIT
};

class Game {
public:
    enum class EMove {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Game(GameMode mode) noexcept;
    ~Game();

    void handle_event(SDL_Event* event);
    Map* get_map_ptr() const noexcept;
    // Load map from file
    void load_map(const char* map_path);
    // Unload current map
    void unload_map();
    GameMode get_mode() const noexcept;
    bool map_complete() const noexcept;
    void move_player(EMove move);
    // Edit mode
    TilePos get_cursor() const noexcept;
    void move_cursor(EMove move);
    MapTile get_tile_brush() const noexcept;
private:
    GameMode mode;
    Map* map;
    std::list<EMove> move_order;

    TilePos move_pos(MTType type, TilePos pos, EMove move);
    void update_maptile(MTType type, TilePos curr_pos, TilePos new_pos);

    // Edit mode
    TilePos cursor_pos;
    MapTile tile_brush;
};
