#include "map.h"
#include <cstdio>
#include <unistd.h>
#include <vector>
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

    FILE* file = nullptr;

    try {
        if ((file = fopen(map_path, "r")) == nullptr)
            throw std::runtime_error(std::format("Failed to open map {}", map_path));
        cfg.map_path = map_path;

        fscanf(file, "%d %d", &cfg.width, &cfg.height);
        if (cfg.width <= 0 || cfg.height <= 0)
            throw std::out_of_range(std::format("Failed to load map {}: invalid map size", map_path));
        else if (cfg.width > MAP_MAX_WIDTH || cfg.height > MAP_MAX_HEIGHT)
            throw std::out_of_range(std::format("Failed to load map {}: dimension too big", map_path));

        cfg.map_size = cfg.width * cfg.height;
        data = new MapTile[cfg.map_size];

        TilePos pos;
        fscanf(file, "%d", &pos);
        if (!map->oob_check(pos))
            throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
        map->get_tile(pos) += MTType::PLAYER;
        cfg.player_pos = pos;
        
        int count;
        fscanf(file, "%d", &count);
        while (count--) {
            fscanf(file, "%d", &pos);
            if (!map->oob_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
            map->get_tile(pos) += MTType::WORLD;
        }

        fscanf(file, "%d", &count);
        while (count--) {
            fscanf(file, "%d", &pos);
            if (!map->oob_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
            map->get_tile(pos) += MTType::WORLD;
            map->get_tile(pos) += MTType::REACHABLE;
        }

        fscanf(file, "%d", &cfg.objective_count);
        cfg.objective_remaining = cfg.objective_count;
        for (int i = 0; i < cfg.objective_count; ++i) {
            fscanf(file, "%d", &pos);
            if (!map->oob_check(pos))
                throw std::out_of_range(std::format("Failed to load map {}: invalid map position", map_path));
            map->get_tile(pos) += MTType::OBJECTIVE;
        }
        for (int i = 0; i < cfg.objective_count; ++i) {
            fscanf(file, "%d", &pos);
            if (!map->oob_check(pos))
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

bool Map::save(const char* path) const {
    // TODO: Make this operation fail-safe
    std::string t = std::format("{}.tmp", path);
    const char* tmp_path = t.c_str();
    FILE* file = nullptr;
    bool success = false;
    std::vector<TilePos> walls, floors, objectives, boxes;

    for (TilePos i = 0; i < cfg.map_size; ++i) {
        MapTile tile = get_tile(i);
        if (tile.has(MTType::WORLD)) {
            if (tile.has(MTType::REACHABLE))
                floors.push_back(i);
            else
                walls.push_back(i);
        }
        if (tile.has(MTType::OBJECTIVE))
            objectives.push_back(i);
        if (tile.has(MTType::BOX))
            boxes.push_back(i);
    }

    try {
        if (objectives.size() != boxes.size())
            throw std::runtime_error(std::format("Number of objectives and boxes are different: {} != {}", objectives.size(), boxes.size()));   
 
        if ((file = fopen(tmp_path, "w")) == nullptr)
            throw std::runtime_error(std::format("Failed to open map {}", tmp_path));

        fprintf(file, "%d %d\n", cfg.width, cfg.height);
        fprintf(file, "%d\n", cfg.player_pos);
        fprintf(file, "%lu\n", walls.size());
        for (const TilePos& i: walls)
            fprintf(file, "%d ", i);
        fprintf(file, "\n%lu\n", floors.size());
        for (const TilePos& i: floors)
            fprintf(file, "%d ", i);
        fprintf(file, "\n%lu\n", objectives.size());
        for (const TilePos& i: objectives)
            fprintf(file, "%d ", i);
        fprintf(file, "\n");
        for (const TilePos& i: boxes)
            fprintf(file, "%d ", i);

        fclose(file);
        file = nullptr;

        rename(tmp_path, path);
        unlink(tmp_path);
        success = true;
    } catch (std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
    }

    if (file) {
        fclose(file);
        file = nullptr;
    }

    return success;
}

MapTile& Map::get_tile(TilePos pos) const {
    if (!oob_check(pos))
        throw std::out_of_range(std::format("Position ({}) is outside of map", pos));
    return data[pos];
}

MapTile& Map::get_tile(int y, int x) const {
    TilePos pos = point_to_tilepos(y, x);
    if (!oob_check(pos))
        throw std::out_of_range(std::format("Position ({}, {}) is outside of map", y, x));
    return data[pos];
}

bool Map::oob_check(TilePos pos) const noexcept {
    return pos >= 0 && pos < cfg.map_size;
}

TilePos Map::point_to_tilepos(int y, int x) const noexcept {
    return y * cfg.width + x;
}

SDL_Point Map::tilepos_to_point(TilePos pos) const noexcept {
    return SDL_Point { pos % cfg.width, pos / cfg.width };
}
