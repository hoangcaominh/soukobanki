// Use SDL callbacks instead of main()
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

SDL_AppResult SDL_AppInit(void **state, int argc, char* argv[]) {
    SDL_SetAppMetadata("Soukobanki", "0.1.0", "");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to intialize SDL_INIT_VIDEO: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Soukobanki", 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    // Set background color to #FF0000FF
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *state, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *state) {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *state, SDL_AppResult result) {

}
