#include "game.h"

Game::Game() noexcept : map(nullptr), move_order(std::list<EMove>()) {}

Game::~Game() {
    unload_map();
}

void Game::handle_event(SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (!map_complete()) {
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

bool Game::map_complete() const noexcept {
    return map->cfg.objective_remaining == 0;
}

void Game::move_player(EMove move) {
    int &pos = map->cfg.player_position;
    pos = move_pos(MTType::PLAYER, pos, move);
}

int Game::move_pos(MTType type, int pos, EMove move) {
    MapConfig &cfg = map->cfg;
    int new_pos = pos;

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

void Game::update_maptile(MTType type, int curr_pos, int new_pos) {
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
