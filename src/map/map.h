#pragma once
#include "maptile.h"
#include <SDL3/SDL.h>
#include <cstdint>

#define MAP_MAX_WIDTH 256
#define MAP_MAX_HEIGHT 256

typedef int TilePos;

struct MapConfig {
    const char* map_path;
    int width, height;
    uint32_t map_size;
    TilePos player_pos;
    int objective_count;
    int objective_remaining;
    // int* box_positions;
};

class Map {
    friend class Game;
public:
    ~Map();

    MapConfig get_cfg() const noexcept;
    // Allow creating map instances by loading from files only
    static Map* load(const char* map_path);
    bool save(const char* path) const;
    MapTile& get_tile(TilePos pos) const;
    MapTile& get_tile(int y, int x) const;
    bool oob_check(TilePos pos) const noexcept;
    TilePos point_to_tilepos(int y, int x) const noexcept;
    SDL_Point tilepos_to_point(TilePos pos) const noexcept;
private:
    Map() noexcept;
    MapTile* data;
    MapConfig cfg;
};
