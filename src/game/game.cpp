#include "game.h"
#include "../graphics/graphics.h"

Game::Game(Graphics* graphics) noexcept : graphics(graphics), map(nullptr) {}

Game::~Game() {
    unload_map();
    delete graphics;
    graphics = nullptr;
}

SDL_AppResult Game::handle_event(SDL_Event* event) {
    float scale = graphics->get_scale();

    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE)
            return SDL_APP_SUCCESS;
        if (event->key.key == SDLK_LEFTBRACKET) {
            scale -= 0.1;
            graphics->set_scale(scale);
        }
        if (event->key.key == SDLK_RIGHTBRACKET) {
            scale += 0.1;
            graphics->set_scale(scale);
        }
        if (!map_completed()) {
            if (event->key.key == SDLK_LEFT) {
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

    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::handle_loop() {
    std::function<void()> func = [this]() {
        Map::Config cfg = map->cfg;

        graphics->draw_map(map);
    };

    graphics->render(func);

    return SDL_APP_CONTINUE;
}

Graphics* Game::get_graphics() const noexcept {
    return graphics;
}

void Game::load_map(const char* map_name) {
    unload_map();
    map = Map::load(map_name);
}

void Game::unload_map() {
    delete map;
    map = nullptr;
}

bool Game::map_completed() const noexcept {
    return map->cfg.objective_remaining == 0;
}

void Game::move_player(const EMove &move) {
    Map::Config &cfg = map->cfg;
    int &pos = map->cfg.player_position;

    if (move == EMove::LEFT && map->oor_check(pos - 1) && map->get_tile(pos - 1).has(MTType::BOX))
        if (move_pos(MTType::BOX, pos - 1, move) == pos - 1)
            return;
    if (move == EMove::RIGHT && map->oor_check(pos + 1) && map->get_tile(pos + 1).has(MTType::BOX))
        if (move_pos( MTType::BOX, pos + 1,move) == pos + 1)
            return;
    if (move == EMove::UP && map->oor_check(pos - cfg.width) && map->get_tile(pos - cfg.width).has(MTType::BOX))
        if (move_pos(MTType::BOX, pos - cfg.width, move) == pos - cfg.width)
            return;
    if (move == EMove::DOWN && map->oor_check(pos + cfg.width) && map->get_tile(pos + cfg.width).has(MTType::BOX))
        if (move_pos(MTType::BOX, pos + cfg.width, move) == pos + cfg.width)
            return;
    pos = move_pos(MTType::PLAYER, pos, move);
}

int Game::move_pos(const MTType &obj, int pos, const EMove &move) {
    Map::Config &cfg = map->cfg;
    int new_pos = pos;

    if (move == EMove::LEFT) {
        new_pos = pos - 1;
        if (
            pos % cfg.width == 0 ||
            map->get_tile(new_pos).has(MTType::WALL) ||
            (obj == MTType::BOX && map->get_tile(pos - 1).has(MTType::BOX))
        )
            return pos;

        update_maptile(obj, pos, new_pos);
            
    } else if (move == EMove::RIGHT) {
        new_pos = pos + 1;
        if (
            (pos + 1) % cfg.width == 0 ||
            map->get_tile(new_pos).has(MTType::WALL) ||
            (obj == MTType::BOX && map->get_tile(pos + 1).has(MTType::BOX))
        )
            return pos;

        update_maptile(obj, pos, new_pos);
    } else if (move == EMove::UP) {
        new_pos = pos - cfg.width;
        if (
            pos - cfg.width < 0 ||
            map->get_tile(new_pos).has(MTType::WALL) ||
            (obj == MTType::BOX && map->get_tile(pos - cfg.width).has(MTType::BOX))
        )
            return pos;

        update_maptile(obj, pos, new_pos);
    } else if (move == EMove::DOWN) {
        new_pos = pos + cfg.width;
        if (
            pos + cfg.width >= cfg.map_size ||
            map->get_tile(new_pos).has(MTType::WALL) ||
            (obj == MTType::BOX && map->get_tile(pos + cfg.width).has(MTType::BOX))
        )
            return pos;

        update_maptile(obj, pos, new_pos);
    }

    return new_pos;
}

void Game::update_maptile(const MTType &obj, int curr_pos, int new_pos) {
    if (
        obj == MTType::BOX &&
        map->get_tile(curr_pos).has(MTType::OBJECTIVE) &&
        !map->get_tile(new_pos).has(MTType::OBJECTIVE)
    )
        map->cfg.objective_remaining++;
    if (
        obj == MTType::BOX &&
        ~map->get_tile(curr_pos).has(MTType::OBJECTIVE) &&
        map->get_tile(new_pos).has(MTType::OBJECTIVE)
    )
        map->cfg.objective_remaining--;

    map->get_tile(curr_pos).remove(obj);
    map->get_tile(new_pos).append(obj);
}
