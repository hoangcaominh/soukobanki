#include "map.h"
#include <cstdio>
#include <format>
#include <exception>

Map::Map() noexcept : data(nullptr) {}

Map::~Map() {
    delete[] data;
    data = nullptr;
    // delete[] config.box_locations;
}

MapConfig Map::get_cfg() const noexcept {
    return cfg;
}

Map* Map::load(const char* map_path)
{
    Map* map = new Map();
    MapConfig &cfg = map->cfg;
    MapTile* &data = map->data;

    FILE* file;

    try {
        if ((file = fopen(map_path, "r")) == nullptr)
            throw std::runtime_error(std::format("Failed to open map {}", map_path));

        fscanf(file, "%d %d", &cfg.width, &cfg.height);
        if (cfg.width <= 0 || cfg.height <= 0)
            throw std::out_of_range(std::format("Failed to load map {}: invalid map size", map_path));
        else if (cfg.width > MAP_MAX_WIDTH || cfg.height > MAP_MAX_HEIGHT)
            throw std::out_of_range(std::format("Failed to load map {}: dimension too big", map_path));

        cfg.map_size = cfg.width * cfg.height;
        data = new MapTile[cfg.map_size];

        int pos;
        fscanf(file, "%d", &pos);
        if (!map->oor_check(pos))
            throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
        map->get_tile(pos) += MTType::PLAYER;
        cfg.player_position = pos;
        
        int count;
        fscanf(file, "%d", &count);
        while (count--) {
            fscanf(file, "%d", &pos);
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
            map->get_tile(pos) += MTType::WORLD;
        }

        fscanf(file, "%d", &count);
        while (count--) {
            fscanf(file, "%d", &pos);
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
            map->get_tile(pos) += MTType::WORLD;
            map->get_tile(pos) += MTType::REACHABLE;
        }

        fscanf(file, "%d", &cfg.objective_count);
        cfg.objective_remaining = cfg.objective_count;
        for (int i = 0; i < cfg.objective_count; ++i) {
            fscanf(file, "%d", &pos);
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
            map->get_tile(pos) += MTType::OBJECTIVE;
        }
        for (int i = 0; i < cfg.objective_count; ++i) {
            fscanf(file, "%d", &pos);
            if (!map->oor_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map positionn", map_path));
            map->get_tile(pos) += MTType::BOX;
            if (map->get_tile(pos).has(MTType::OBJECTIVE))
                cfg.objective_remaining--;
        }
    } catch (std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
        delete map;
        map = nullptr;
    }

    if (file) {
        fclose(file);
        file = nullptr;
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
