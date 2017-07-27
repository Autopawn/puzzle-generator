#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480
#define BORDER 16

#define MAX_TILE_SIZE 64

#define N_SPRITES 1
const char* sprite_dirs[] = {
    "game/res/person-8x.png"
};

SDL_Window* window;
SDL_Surface* screen_surface;
SDL_Surface* sprites[N_SPRITES];

#define PUZZLE_RULE slide_rule

void init_game() {
    window = NULL;
    screen_surface = NULL;
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n",
            SDL_GetError());
        exit(1);
    }
    // Create window
    window = SDL_CreateWindow("Puzzle game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n",
            SDL_GetError());
        exit(1);
    }
    // Get window surface
    screen_surface = SDL_GetWindowSurface(window);
    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n",
            IMG_GetError());
        exit(1);
    }
    // Load sprites
    for(int i = 0; i < N_SPRITES; i++) {
        sprites[i] = NULL;
        // Load image at specified path
        sprites[i] = IMG_Load(sprite_dirs[i]);
        if(sprites[i] == NULL) {
            printf("Unable to load image %s! SDL_image Error: %s\n",
                sprite_dirs[i], IMG_GetError());
            exit(1);
        }
    }
}

void close_game() {
    // Deallocate sprites
    for(int i = 0; i < N_SPRITES; i++) {
        SDL_FreeSurface(sprites[i]);
        sprites[i] = NULL;
    }
    // Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    // Quit SDL subsystems
    SDL_Quit();
}

void display_level(pglevel *level, pgstate *state) {
    int tile_size_x = (SCREEN_WIDTH - BORDER * 2) / (level->max_x + 2) / 2;
    int tile_size_y = (SCREEN_HEIGHT - BORDER * 2) / (level->max_y + 2) / 2;
    int tile_size = (tile_size_x > tile_size_y) ? tile_size_y : tile_size_x;
    tile_size *= 2;
    tile_size = (tile_size > MAX_TILE_SIZE) ? MAX_TILE_SIZE : tile_size;
    // Draw background
    SDL_FillRect(screen_surface, NULL,
        SDL_MapRGB(screen_surface->format, 255, 255, 255));
    // Draw level
    for(int y = 0; y <= level->max_y; y++) {
        for(int x = 0; x <= level->max_x; x++) {
            // Draw tile
            // ...
        }
    }
    // Draw pieces
    for(int i=0;i<state->n_pieces;i++) {
        // Decide sprite for the kind of the piece
        int spr_index = state->pieces[i].kind;
        if(spr_index<=N_SPRITES) spr_index = N_SPRITES-1;
        // Draw piece in the right place
        SDL_Rect dstrect;
        dstrect.x = tile_size/2*(2*state->pieces[i].p_x-level->max_x-1);
        dstrect.x += SCREEN_WIDTH/2;
        dstrect.y = tile_size/2*(2*state->pieces[i].p_y-level->max_y-1);
        dstrect.y += SCREEN_HEIGHT/2;
        dstrect.w = tile_size;
        dstrect.h = tile_size;
        SDL_BlitScaled(sprites[spr_index], NULL, screen_surface, &dstrect);
    }
    // Update display
    SDL_UpdateWindowSurface(window);
}

int play_level(pglevel *level, pgstate *state) {
    int n_moves = 0;
    pgstate_hash(state);
    while(1) {
        display_level(level,state);
        pgresult result;
		while(1) {
			result = PUZZLE_RULE(level,state);
			if(result.conclusion!=STEP) break;
			*state = result.next.step;
            pgstate_hash(state);
            // Display level, left some time to display the previous
            SDL_Delay(30);
			display_level(level,state);
		}
        // React to a choice
        if(result.conclusion==CHOICE) {
            if(result.n_choices<=0) {
                printf("NO CHOICES!\n");
                return -1;
            }else{
                printf("CHOICE %d:\n",++n_moves);
                for(int i=0;i<result.n_choices;i++) {
                    printf("%6d) %s\n",i,result.next.choices[i].description);
                }
                // Wait for user input
                while(1) {
                    SDL_Event ev;
                    while(SDL_PollEvent(&ev) != 0) {
                        if(ev.type == SDL_QUIT) {
                            // User requests quit
                            return -3;
                        }else if(ev.type == SDL_KEYDOWN) {
                            // Handle key presses
                            // ...
                        }
                    }
                    // int answer;
                    // printf("Choice: ");
                    // scanf("%d",&answer);
                    // if(0<=answer && answer<result.n_choices) {
                    //     *state = result.next.choices[answer].resulting;
                    //     pgstate_hash(state);
                    //     break;
                    // }else{
                    //     printf("Enter a valid number!\n");
                    // }
                }
            }
        }else if(result.conclusion==WIN) {
            printf("WIN ON %d CHOICES!\n",n_moves);
            return n_moves;
        }else{
            printf("STRANGE CONCLUSION: %d\n",result.conclusion);
            return -2;
        }
    }
}

int main(int argc, char const *argv[]) {
    // Init systems
    init_game();
    SDL_FillRect(screen_surface, NULL,
        SDL_MapRGB(screen_surface->format, 255, 255, 255));
    // Play the level
    pglevel level;
    pgstate state;
    pgread_from_file("lvls/slide02.txt", &level, &state);
    play_level(&level, &state);
    // Update the surface
    SDL_UpdateWindowSurface(window);
    // Close systems
    close_game();
    return 0;
}
