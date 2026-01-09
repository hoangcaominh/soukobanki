#include "map.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <string>
#include <iostream>
#include <format>
#include <exception>

Map::Map() noexcept : data(nullptr) {}

Map::~Map() {
    delete[] data;
    data = nullptr;
    // delete[] config.box_locations;
}

Map::Config Map::get_cfg() const noexcept {
    return cfg;
}

Map* Map::load(const char* map_name)
{
    Map* map = new Map();
    Map::Config &cfg = map->cfg;
    MapTile* &data = map->data;

    std::string map_path = std::string("data/maps/") + map_name;
    std::ifstream file;

    try {
        file = std::ifstream(map_path);

        if (!file.is_open())
            throw std::runtime_error(std::format("Failed to open map {}", map_path.c_str()));

        file >> cfg.width >> cfg.height;
        if (cfg.width <= 0 || cfg.height <= 0)
            throw std::out_of_range(std::format("Failed to load map {}: invalid map size", map_path.c_str()));
        else if (cfg.width > MAP_MAX_WIDTH || cfg.height > MAP_MAX_HEIGHT)
            throw std::out_of_range(std::format("Failed to load map {}: dimension too big", map_path.c_str()));

        cfg.map_size = cfg.width * cfg.height;
        data = new MapTile[cfg.map_size];
        
        int count, pos;
        file >> count;
        while (count--) {
            file >> pos;
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path.c_str()));
            map->get_tile(pos) += MTType::WALL;
        }

        file >> count;
        while (count--) {
            file >> pos;
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path.c_str()));
            map->get_tile(pos) += MTType::FLOOR;
        }

        file >> cfg.objective_count;
        cfg.objective_remaining = cfg.objective_count;
        for (int i = 0; i < cfg.objective_count; ++i) {
            file >> pos;
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path.c_str()));
            map->get_tile(pos) += MTType::OBJECTIVE;
        }
        for (int i = 0; i < cfg.objective_count; ++i) {
            file >> pos;
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map positionn", map_path.c_str()));
            map->get_tile(pos) += MTType::BOX;
            if (map->get_tile(pos).has(MTType::OBJECTIVE))
                cfg.objective_remaining--;
        }

        file >> pos;
        if (!map->oor_check(pos))
            throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path.c_str()));
        map->get_tile(pos) += MTType::PLAYER;
        cfg.player_position = pos;
    } catch (std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", e.what());
        delete[] data;
        data = nullptr;
        delete map;
        map = nullptr;
    }

    return map;
}

MapTile& Map::get_tile(int pos) const {
    if (!oor_check(pos))
        throw std::out_of_range(std::format("Position ({}) is outside of map", pos));
    return data[pos];
}

MapTile& Map::get_tile(int y, int x) const {
    int pos = y * cfg.width + x;
    if (!oor_check(pos))
        throw std::out_of_range(std::format("Position ({}, {}) is outside of map", y, x));
    return data[pos];
}

bool Map::oor_check(int pos) const noexcept {
    return pos >= 0 && pos < cfg.map_size;
}
