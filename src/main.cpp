// Use SDL callbacks instead of main()
#define SDL_MAIN_USE_CALLBACKS 1
#include "game/game.h"
#include "graphics/graphics.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window;
Game* game;
Graphics* graphics;

SDL_AppResult SDL_AppInit(void** state, int argc, char* argv[]) {
    SDL_SetAppMetadata("Soukobanki", "0.1.0", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to intialize SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to intialize SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if ((window = SDL_CreateWindow("Soukobanki", 1280, 960, SDL_WINDOW_OPENGL /* | SDL_WINDOW_RESIZABLE */ )) == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Renderer* renderer = nullptr;
    if ((renderer = SDL_CreateRenderer(window, "")) == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Set background color to #000000FF
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    GameMode mode = GameMode::PLAY;
    const char* map_path = "data/maps/demo";
    if (argc == 3) {
        if (strcasecmp(argv[1], "e") == 0 || strcasecmp(argv[1], "edit") == 0) {
            mode = GameMode::EDIT;
        } else if (strcasecmp(argv[1], "p") == 0 || strcasecmp(argv[1], "play") == 0) {
            mode = GameMode::PLAY;
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Invalid command: %s\n", argv[1]);
            return SDL_APP_FAILURE;
        }
        map_path = argv[2];
    }

    game = new Game(mode);
    if (game == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize game\n");
        return SDL_APP_FAILURE;
    }

    graphics = new Graphics();
    if (!graphics->set_renderer(renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to set graphics renderer\n");
        return SDL_APP_FAILURE;
    }

    game->load_map(map_path);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* state, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE)
            return SDL_APP_SUCCESS;

        if (event->key.key == SDLK_LEFTBRACKET) {
            graphics->set_scale(graphics->get_scale() - 0.1);
        } else if (event->key.key == SDLK_RIGHTBRACKET) {
            graphics->set_scale(graphics->get_scale() + 0.1);
        }
    }

    game->handle_event(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* state) {
    graphics->render(game);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* state, SDL_AppResult result) {
    delete graphics;
    graphics = nullptr;
    delete game;
    game = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
}
