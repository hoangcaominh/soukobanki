#include "game.h"

Game::Game(GameMode mode) noexcept :
    mode(mode), map(nullptr), move_order(std::list<EMove>()), cursor_pos(0), tile_brush(MTType::WORLD) {}

Game::~Game() {
    unload_map();
}

void Game::handle_event(SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (mode == GameMode::PLAY && !map_complete()) {
            if (event->key.key == SDLK_Z && SDL_GetModState() & SDL_KMOD_CTRL) {
                // TODO: Implement undo functionality
            } else if (event->key.key == SDLK_LEFT) {
                move_player(Game::EMove::LEFT);
            } else if (event->key.key == SDLK_RIGHT) {
                move_player(Game::EMove::RIGHT);
            } else if (event->key.key == SDLK_UP) {
                move_player(Game::EMove::UP);
            } else if (event->key.key == SDLK_DOWN) {
                move_player(Game::EMove::DOWN);
            }
        } else if (mode == GameMode::EDIT) {
            if (event->key.key == SDLK_LEFT) {
                move_cursor(Game::EMove::LEFT);
            } else if (event->key.key == SDLK_RIGHT) {
                move_cursor(Game::EMove::RIGHT);
            } else if (event->key.key == SDLK_UP) {
                move_cursor(Game::EMove::UP);
            } else if (event->key.key == SDLK_DOWN) {
                move_cursor(Game::EMove::DOWN);
            } else if (event->key.key == SDLK_S) {
                // Save map
                if (map->save(map->cfg.map_path))
                    SDL_Log("Map saved to %s", map->cfg.map_path);
                else
                    SDL_Log("Failed to save map to %s", map->cfg.map_path);
            } else if (event->key.key == SDLK_1) {
                tile_brush.set(MTType::WORLD);
            } else if (event->key.key == SDLK_2) {
                tile_brush.set(MTType::WORLD);
                tile_brush += MTType::REACHABLE;
            } else if (event->key.key == SDLK_3) {
                tile_brush.set(MTType::OBJECTIVE);
            } else if (event->key.key == SDLK_4) {
                tile_brush.set(MTType::BOX);
            } else if (event->key.key == SDLK_5) {
                tile_brush.set(MTType::PLAYER);
            } else if (event->key.key == SDLK_Z) {
                MapTile &tile = map->get_tile(cursor_pos);
                // Special case
                if (tile_brush.has(MTType::PLAYER)) {
                    map->get_tile(map->cfg.player_pos).remove(MTType::PLAYER);
                    map->cfg.player_pos = cursor_pos;
                    tile.append(MTType::PLAYER);
                // Even more special case
                } else if (tile.has(MTType::REACHABLE) && tile_brush.has(WORLD) && !tile_brush.has(MTType::REACHABLE)) {
                    tile.remove(MTType::REACHABLE);
                // lmao
                } else if (tile.has((MTType) tile_brush.val())) {
                    tile.remove((MTType) tile_brush.val());
                } else {
                    tile.append((MTType) tile_brush.val());
                }
            } else if (event->key.key == SDLK_X) {
                MapTile &tile = map->get_tile(cursor_pos);
                if (tile.has(MTType::PLAYER)) {
                    tile.set(MTType::BLANK);
                    tile.set(MTType::PLAYER);
                } else {
                    tile.set(MTType::BLANK);
                }
            }
        }
    }
}

Map* Game::get_map_ptr() const noexcept {
    return map;
}

void Game::load_map(const char* map_path) {
    unload_map();
    map = Map::load(map_path);
}

void Game::unload_map() {
    delete map;
    map = nullptr;
}

GameMode Game::get_mode() const noexcept {
    return mode;
}

bool Game::map_complete() const noexcept {
    return map->cfg.objective_remaining == 0;
}

void Game::move_player(EMove move) {
    TilePos &pos = map->cfg.player_pos;
    pos = move_pos(MTType::PLAYER, pos, move);
}

TilePos Game::move_pos(MTType type, TilePos pos, EMove move) {
    MapConfig &cfg = map->cfg;
    TilePos new_pos = pos;

    if (move == EMove::LEFT) {
        new_pos = pos - 1;
        if (
            pos % cfg.width == 0 ||
            !map->get_tile(new_pos).has(MTType::REACHABLE) ||
            (type == MTType::BOX && map->get_tile(new_pos).has(MTType::BOX))
        )
            return pos;
        
        if (type == MTType::PLAYER && map->get_tile(new_pos).has(MTType::BOX))
            if (move_pos(MTType::BOX, new_pos, move) == new_pos)
                return pos;

        update_maptile(type, pos, new_pos);
            
    } else if (move == EMove::RIGHT) {
        new_pos = pos + 1;
        if (
            (pos + 1) % cfg.width == 0 ||
            !map->get_tile(new_pos).has(MTType::REACHABLE) ||
            (type == MTType::BOX && map->get_tile(new_pos).has(MTType::BOX))
        )
            return pos;
        
        if (type == MTType::PLAYER && map->get_tile(new_pos).has(MTType::BOX))
            if (move_pos(MTType::BOX, new_pos, move) == new_pos)
                return pos;

        update_maptile(type, pos, new_pos);
    } else if (move == EMove::UP) {
        new_pos = pos - cfg.width;
        if (
            pos - cfg.width < 0 ||
            !map->get_tile(new_pos).has(MTType::REACHABLE) ||
            (type == MTType::BOX && map->get_tile(new_pos).has(MTType::BOX))
        )
            return pos;
        
        if (type == MTType::PLAYER && map->get_tile(new_pos).has(MTType::BOX))
            if (move_pos(MTType::BOX, new_pos, move) == new_pos)
                return pos;

        update_maptile(type, pos, new_pos);
    } else if (move == EMove::DOWN) {
        new_pos = pos + cfg.width;
        if (
            pos + cfg.width >= cfg.map_size ||
            !map->get_tile(new_pos).has(MTType::REACHABLE) ||
            (type == MTType::BOX && map->get_tile(new_pos).has(MTType::BOX))
        )
            return pos;
        
        if (type == MTType::PLAYER && map->get_tile(new_pos).has(MTType::BOX))
            if (move_pos(MTType::BOX, new_pos, move) == new_pos)
                return pos;

        update_maptile(type, pos, new_pos);
    }

    return new_pos;
}

void Game::update_maptile(MTType type, TilePos curr_pos, TilePos new_pos) {
    if (
        type == MTType::BOX &&
        map->get_tile(curr_pos).has(MTType::OBJECTIVE) &&
        !map->get_tile(new_pos).has(MTType::OBJECTIVE)
    )
        map->cfg.objective_remaining++;
    if (
        type == MTType::BOX &&
        !map->get_tile(curr_pos).has(MTType::OBJECTIVE) &&
        map->get_tile(new_pos).has(MTType::OBJECTIVE)
    )
        map->cfg.objective_remaining--;

    map->get_tile(curr_pos).remove(type);
    map->get_tile(new_pos).append(type);
}

TilePos Game::get_cursor() const noexcept {
    return cursor_pos;
}

void Game::move_cursor(EMove move) {
    MapConfig &cfg = map->cfg;
    TilePos &pos = cursor_pos;

    if (move == EMove::LEFT && pos % cfg.width != 0)
        pos -= 1;
    else if (move == EMove::RIGHT && (pos + 1) % cfg.width != 0)
        pos += 1;
    else if (move == EMove::UP && pos - cfg.width >= 0)
        pos -= cfg.width;
    else if (move == EMove::DOWN && pos + cfg.width < cfg.map_size)
        pos += cfg.width;
}

MapTile Game::get_tile_brush() const noexcept {
    return tile_brush;
}
