#pragma once
#include "../map/map.h"
#include <SDL3/SDL.h>
#include <unordered_map>
#include <functional>

#define TILE_LENGTH 64

class Graphics {
    friend class Game;
public:
    enum class TileType {
        WALL,
        FLOOR,
        OBJECTIVE,
        BOX,
        BOX_PLACED,
        PLAYER
    };

    Graphics(SDL_Renderer* renderer);
    ~Graphics();

    bool load_texture_from_file(const char* path);
    SDL_FRect get_tile_src(const TileType &type) noexcept;
    float get_scale() const noexcept;
    void set_scale(float val) noexcept;
    void draw_tile(const TileType &type, SDL_FRect dest);
    void draw_map(const Map* const map);
    void render(std::function<void()> func);
private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::unordered_map<TileType, SDL_FRect> tile_src;
    int width, height;
    float scale;
};
