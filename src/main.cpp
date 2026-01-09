// Use SDL callbacks instead of main()
#define SDL_MAIN_USE_CALLBACKS 1
#include "game/game.h"
#include "graphics/graphics.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window;
SDL_Renderer* renderer;
Game* game;

SDL_AppResult SDL_AppInit(void** state, int argc, char* argv[]) {
    SDL_SetAppMetadata("Soukobanki", "0.1.0", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to intialize SDL_INIT_VIDEO: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if ((window = SDL_CreateWindow("Soukobanki", 1280, 960, SDL_WINDOW_OPENGL /* | SDL_WINDOW_RESIZABLE */ )) == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if ((renderer = SDL_CreateRenderer(window, "")) == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Set background color to #000000FF
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    Graphics* graphics = new Graphics(renderer);
    graphics->load_texture_from_file("data/images/texture.bmp");

    game = new Game(graphics);
    if (game == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize game\n");
        delete graphics;
        return SDL_APP_FAILURE;
    }
    game->load_map("1");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* state, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return game->handle_event(event);
}

SDL_AppResult SDL_AppIterate(void* state) {
    return game->handle_loop();
}

void SDL_AppQuit(void* state, SDL_AppResult result) {
    delete game;
    game = nullptr;
    SDL_DestroyWindow(window);
}
