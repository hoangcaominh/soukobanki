#include "graphics.h"

SDL_Texture* create_texture_from_image(SDL_Renderer* renderer, const char* path) {
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = nullptr;

	if ((surface = SDL_LoadBMP(path)) == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load image %s: %s\n", path, SDL_GetError());
		return nullptr;
	}

	if ((texture = SDL_CreateTextureFromSurface(renderer, surface)) == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed create texture from %s: %s\n", path, SDL_GetError());
	}
	SDL_DestroySurface(surface);

	return texture;
}

SDL_Texture* create_texture_from_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color) {
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = nullptr;

	if ((surface = TTF_RenderText_Blended(font, text, 0, color)) == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to render text surface: %s\n", SDL_GetError());
		return nullptr;
	}

	if ((texture = SDL_CreateTextureFromSurface(renderer, surface)) == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed create texture from rendered text: %s\n", SDL_GetError());
	}
	SDL_DestroySurface(surface);

	return texture;
}

Graphics::Graphics() :
	renderer(nullptr), texture(nullptr), screen_map_complete(nullptr), tile_src({}), scale(1.0) {}

Graphics::~Graphics() {
	TTF_CloseFont(font);
	font = nullptr;
	TTF_DestroyRendererTextEngine(text_engine);
	text_engine = nullptr;
	SDL_DestroyTexture(texture);
	texture = nullptr;
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
}

bool Graphics::set_renderer(SDL_Renderer* renderer) {
	this->renderer = renderer;

	if (!SDL_GetCurrentRenderOutputSize(renderer, &width, &height)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to get renderer dimension: %s\n", SDL_GetError());
	}

    if (!load_texture_from_file("data/images/texture.bmp")) {
		return false;
    }

	if ((screen_map_complete = create_texture_from_image(renderer, "data/images/map_complete.bmp")) == nullptr) {
		return false;
	}
	SDL_SetTextureScaleMode(screen_map_complete, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);

    if (!load_font_from_file("data/fonts/benguiat_r.ttf")) {
		return false;
    }

	if ((text_engine = TTF_CreateRendererTextEngine(renderer)) == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create text engine from renderer: %s\n", SDL_GetError());
	}

	return true;
}

bool Graphics::load_texture_from_file(const char* path) {
    SDL_Texture* texture = nullptr;
    if ((texture = create_texture_from_image(renderer, path)) == nullptr) {
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
	tile_src[TileType::CURSOR] = { 0, TILE_LENGTH * 3, TILE_LENGTH, TILE_LENGTH };

	return true;
}

bool Graphics::load_font_from_file(const char* path) {
    TTF_Font* font = nullptr;
    if ((font = TTF_OpenFont(path, 12)) == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font %s: %s\n", path, SDL_GetError());
        return false;
    }

	// Replace old with new font
	TTF_CloseFont(this->font);
	this->font = font;

	return true;
}

SDL_FRect Graphics::get_tile_src(TileType type) noexcept {
	return tile_src[type];
}

float Graphics::get_scale() const noexcept {
    return scale;
}

void Graphics::set_scale(float val) noexcept {
	if (val < 0.1 || val > 5)
		return;
    scale = val;
}

void Graphics::draw_text(const char* text, float x, float y) {
	TTF_Text* text_obj = TTF_CreateText(text_engine, font, text, 0);
	TTF_DrawRendererText(text_obj, x, y);
}

void Graphics::draw_tile(TileType type, SDL_FRect dest) {
	SDL_FRect src = get_tile_src(type);
	SDL_RenderTexture(renderer, texture, &src, &dest);
}

void Graphics::draw_tile(MapTile tile, SDL_FRect dest) {
	if (tile.has(MTType::WORLD)) {
		if (tile.has(MTType::REACHABLE))
			draw_tile(TileType::FLOOR, dest);
		else
			draw_tile(TileType::WALL, dest);
	}

	if (tile.has(MTType::OBJECTIVE) && tile.has(MTType::BOX))
		draw_tile(TileType::BOX_PLACED, dest);
	else if (tile.has(MTType::OBJECTIVE))
		draw_tile(TileType::OBJECTIVE, dest);
	else if (tile.has(MTType::BOX))
		draw_tile(TileType::BOX, dest);

	if (tile.has(MTType::PLAYER))
		draw_tile(TileType::PLAYER, dest);
}

void Graphics::draw_game(const Game* const game) {
	Map* map = game->get_map_ptr();
	MapConfig cfg = map->get_cfg();

	int offset_x = (width - cfg.width * TILE_LENGTH * scale) / 2;
	int offset_y = (height - cfg.height * TILE_LENGTH * scale) / 2;
	SDL_FRect dest { 0, 0, TILE_LENGTH * scale, TILE_LENGTH * scale };

	for (int y = 0; y < cfg.height; ++y) {
		dest.y = y * dest.h + offset_y;

		for (int x = 0; x < cfg.width; ++x) {
			dest.x = x * dest.w + offset_x;

			draw_tile(map->get_tile(y, x), dest);
		}
	}

	if (game->get_mode() == GameMode::PLAY && game->map_complete()) {
		draw_map_complete();
	} else if (game->get_mode() == GameMode::EDIT) {
		MapTile tile_brush = game->get_tile_brush();
		draw_tile(tile_brush, SDL_FRect {0, 0, TILE_LENGTH, TILE_LENGTH });

		TilePos cursor = game->get_cursor();
		SDL_Point point = map->tilepos_to_point(cursor);
		dest.x = point.x * dest.w + offset_x;
		dest.y = point.y * dest.h + offset_y;
		draw_tile(TileType::CURSOR, dest);
	} 
}

void Graphics::draw_map_complete() {
	float w, h, scale = 0.8;
	SDL_GetTextureSize(screen_map_complete, &w, &h);
	SDL_FRect dest {0, 0, width * scale, width * scale / w * h };
	SDL_FPoint center { (float) width / 2, (float) height / 2 };
	dest.x = center.x - dest.w / 2;
	// dest.y = center.y - dest.h / 2;
	SDL_RenderTexture(renderer, screen_map_complete, nullptr, &dest);
}

void Graphics::render(const Game* const game) {
	SDL_RenderClear(renderer);

	draw_game(game);

	SDL_RenderPresent(renderer);
}
