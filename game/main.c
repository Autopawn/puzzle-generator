#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480

#define N_SPRITES 1
const char* sprite_dirs[] = {
    "game/res/person-8x.png"
};

SDL_Window* window;
SDL_Surface* screen_surface;
SDL_Surface* sprites[N_SPRITES];

void init_game() {
    window = NULL;
    screen_surface = NULL;
    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n",
            SDL_GetError());
        exit(1);
    }
    //Create window
    window = SDL_CreateWindow("Puzzle game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n",
            SDL_GetError());
        exit(1);
    }
    //Get window surface
    screen_surface = SDL_GetWindowSurface(window);
    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n",
            IMG_GetError());
        exit(1);
    }
    //Load sprites
    for(int i = 0; i < N_SPRITES; i++) {
        sprites[i] = NULL;
        //Load image at specified path
        sprites[i] = IMG_Load(sprite_dirs[i]);
        if(sprites[i] == NULL){
            printf("Unable to load image %s! SDL_image Error: %s\n",
                sprite_dirs[i], IMG_GetError());
            exit(1);
        }
    }
}

void close_game() {
    //Deallocate sprites
    for(int i = 0; i < N_SPRITES; i++) {
        SDL_FreeSurface(sprites[i]);
        sprites[i] = NULL;
    }
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char const *argv[]) {
    //Init systems
    init_game();
    SDL_FillRect(screen_surface, NULL,
        SDL_MapRGB(screen_surface->format, 255, 255, 255));
    //Apply the image
    SDL_BlitSurface(sprites[0], NULL, screen_surface, NULL);
    //Update the surface
    SDL_UpdateWindowSurface(window);
    //Wait two seconds
    SDL_Delay(2000);
    //Close systems
    close_game();
    return 0;
}
