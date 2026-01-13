#pragma once
#include "../map/map.h"
#include "../game/game.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>

#define TILE_LENGTH 64

class Graphics {
public:
    enum class TileType {
        WALL,
        FLOOR,
        OBJECTIVE,
        BOX,
        BOX_PLACED,
        PLAYER,
        CURSOR
    };

    Graphics();
    ~Graphics();

    bool set_renderer(SDL_Renderer* renderer);
    bool load_texture_from_file(const char* path);
    bool load_font_from_file(const char* path);
    SDL_FRect get_tile_src(TileType type) noexcept;
    float get_scale() const noexcept;
    void set_scale(float val) noexcept;
    void draw_text(const char* text, float x, float y);
    void draw_tile(TileType type, SDL_FRect dest);
    void draw_tile(MapTile tile, SDL_FRect dest);
    void draw_game(const Game* const game);
    void draw_map_complete();
    void render(const Game* const game);
private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Texture* screen_map_complete;
    TTF_TextEngine* text_engine;
    TTF_Font* font;

    int width, height;
    float scale;
    std::unordered_map<TileType, SDL_FRect> tile_src;
};
