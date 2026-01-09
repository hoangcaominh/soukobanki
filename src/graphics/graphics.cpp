#include "graphics.h"
#include <SDL3/SDL.h>
#include <format>
#include <exception>

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = nullptr;

	if ((surface = SDL_LoadBMP(path)) == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load image %s: %s\n", path, SDL_GetError());
	} else {
		// Create texture from surface pixels
		if ((texture = SDL_CreateTextureFromSurface(renderer, surface)) == nullptr) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed create texture from %s: %s\n", path, SDL_GetError());
		}

		// Get rid of old loaded surface
		SDL_DestroySurface(surface);
	}

	return texture;
}

Graphics::Graphics(SDL_Renderer* renderer) : renderer(renderer), texture(nullptr), tile_src({}), scale(1.0) {
	SDL_GetCurrentRenderOutputSize(renderer, &width, &height);
}

Graphics::~Graphics() {
	SDL_DestroyTexture(texture);
	texture = nullptr;
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
}

bool Graphics::load_texture_from_file(const char* path) {
    SDL_Texture* texture;
    if ((texture = load_texture(renderer, path)) == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load texture at %s\n", path);
        return false;
    }

	// Replace old with new texture
	SDL_DestroyTexture(this->texture);
	this->texture = texture;

	// TODO: Move this data to a metadata file
	tile_src[TileType::WALL] = { 0, TILE_LENGTH * 2, TILE_LENGTH, TILE_LENGTH };
	tile_src[TileType::FLOOR] = { TILE_LENGTH * 3, TILE_LENGTH * 2, TILE_LENGTH, TILE_LENGTH };
	tile_src[TileType::OBJECTIVE] = { 0, TILE_LENGTH, TILE_LENGTH, TILE_LENGTH };
	tile_src[TileType::BOX] = { TILE_LENGTH, TILE_LENGTH * 2, TILE_LENGTH, TILE_LENGTH };
	tile_src[TileType::BOX_PLACED] = { TILE_LENGTH * 2, TILE_LENGTH * 2, TILE_LENGTH, TILE_LENGTH };
	tile_src[TileType::PLAYER] = { 0, 0, TILE_LENGTH, TILE_LENGTH };

	return true;
}

SDL_FRect Graphics::get_tile_src(const TileType &type) noexcept {
	return tile_src[type];
}

float Graphics::get_scale() const noexcept {
    return scale;
}

void Graphics::set_scale(float val) noexcept {
    scale = val;
}

void Graphics::draw_tile(const TileType &type, SDL_FRect dest) {
	SDL_FRect src = get_tile_src(type);
	SDL_RenderTexture(renderer, texture, &src, &dest);
}

void Graphics::draw_map(const Map* const map) {
	Map::Config cfg = map->get_cfg();

	int offset_x = (width - cfg.width * TILE_LENGTH * scale) / 2;
	int offset_y = (height - cfg.height * TILE_LENGTH * scale) / 2;
	SDL_FRect dest { 0, 0, TILE_LENGTH * scale, TILE_LENGTH * scale };

	for (int y = 0; y < cfg.height; ++y) {
		dest.y = y * dest.h + offset_y;

		for (int x = 0; x < cfg.width; ++x) {
			dest.x = x * dest.w + offset_x;
			MapTile tile = map->get_tile(y, x);

			if (tile.has(MTType::WALL))
				draw_tile(Graphics::TileType::WALL, dest);
			else if (tile.has(MTType::FLOOR))
				draw_tile(Graphics::TileType::FLOOR, dest);

			if (tile.has(MTType::OBJECTIVE) && tile.has(MTType::BOX))
				draw_tile(Graphics::TileType::BOX_PLACED, dest);
			else if (tile.has(MTType::OBJECTIVE))
				draw_tile(Graphics::TileType::OBJECTIVE, dest);
			else if (tile.has(MTType::BOX))
				draw_tile(Graphics::TileType::BOX, dest);

			if (tile.has(MTType::PLAYER))
				draw_tile(Graphics::TileType::PLAYER, dest);
		}
	}
}

void Graphics::render(std::function<void()> func) {
	SDL_RenderClear(renderer);
	func();
	SDL_RenderPresent(renderer);
}
