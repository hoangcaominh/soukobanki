#pragma once
#include "maptile.h"
#include <cstdint>

#define MAP_MAX_WIDTH 256
#define MAP_MAX_HEIGHT 256

struct MapConfig {
    int width, height;
    uint32_t map_size;
    int objective_count;
    int objective_remaining;
    // int* box_positions;
    int player_position;
};

class Map {
    friend class Game;
public:
    ~Map();

    MapConfig get_cfg() const noexcept;
    // Allow creating map instances by loading from files only
    static Map* load(const char* map_path);
    MapTile& get_tile(int pos) const;
    MapTile& get_tile(int y, int x) const;
private:
    Map() noexcept;
    MapTile* data;
    MapConfig cfg;

    bool oor_check(int pos) const noexcept;
};
