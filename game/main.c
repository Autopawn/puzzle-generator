#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BORDER 16

#define MAX_TILE_SIZE 64

#define TILE_BORDER 4

#define N_SPRITES 5
const char* sprite_dirs[] = {
    "game/res/person-8x.png",
    "game/res/box-8x.png",
    "game/res/globe-8x.png",
    "game/res/sun-8x.png",
    "game/res/fire-8x.png",
};
const char* background_dir = "game/res/background.png";

SDL_Window* window;
SDL_Surface* screen_surface;
SDL_Surface* background;
SDL_Surface* sprites[N_SPRITES];
pglevel level;
pgstate state;
int event_start_x;
int event_start_y;

int bg_r = 64;
int bg_g = 127;
int bg_b = 127;

static unsigned char colors[][3] = {{255,255,255},{0,0,0},{127,127,255}};

#define PUZZLE_RULE slide_rule

int get_tile_size(){
    int tile_size_x = (SCREEN_WIDTH - BORDER * 2) / (level.max_x + 1) / 2;
    int tile_size_y = (SCREEN_HEIGHT - BORDER * 2) / (level.max_y + 1) / 2;
    int tile_size = (tile_size_x > tile_size_y) ? tile_size_y : tile_size_x;
    tile_size *= 2;
    tile_size = (tile_size > MAX_TILE_SIZE) ? MAX_TILE_SIZE : tile_size;
    return tile_size;
}

void pix_to_tile(int p_x, int p_y, int *res_x, int *res_y){
    int tile_size = get_tile_size();
    *res_x = (p_x-SCREEN_WIDTH/2+(level.max_x+1)*tile_size/2)/tile_size;
    *res_y = (p_y-SCREEN_HEIGHT/2+(level.max_y+1)*tile_size/2)/tile_size;
}

void tile_to_pix(int t_x, int t_y, int *res_x, int *res_y){
    int tile_size = get_tile_size();
    *res_x = tile_size/2*(2*t_x-level.max_x-1) + SCREEN_WIDTH/2;
    *res_y = tile_size/2*(2*t_y-level.max_y-1) + SCREEN_HEIGHT/2;
}

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
    // Load background
    background = NULL;
    background = IMG_Load(background_dir);
    if(background == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n",
            background_dir, IMG_GetError());
        exit(1);
    }
    printf("mode: %d\n",SDL_SetSurfaceBlendMode(background,SDL_BLENDMODE_MOD));
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
    // Deallocate background
    SDL_FreeSurface(background);
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

void display_level() {
    int tile_size = get_tile_size();
    // Draw background
    SDL_FillRect(screen_surface, NULL,
        SDL_MapRGB(screen_surface->format, bg_r, bg_g, bg_b));
    int rep_x = (SCREEN_WIDTH+background->w-1)/background->w;
    int rep_y = (SCREEN_HEIGHT+background->h-1)/background->h;
    for(int y = 0; y <= rep_y; y++) {
        for(int x = 0; x <= rep_x; x++) {
            SDL_Rect dstrect;
            dstrect.x = x*background->w;
            dstrect.y = y*background->h;
            dstrect.w = background->w;
            dstrect.h = background->h;
            SDL_BlitScaled(background, NULL, screen_surface, &dstrect);
        }
    }
    // Draw tiles background
    for(int y = 0; y <= level.max_y; y++) {
        for(int x = 0; x <= level.max_x; x++) {
            if(level.cells[y][x] != 1) {
                SDL_Rect dstrect;
                tile_to_pix(x, y, &dstrect.x, &dstrect.y);
                dstrect.x -= TILE_BORDER;
                dstrect.y -= TILE_BORDER;
                dstrect.w = tile_size+2*TILE_BORDER;
                dstrect.h = tile_size+2*TILE_BORDER;
                SDL_FillRect(screen_surface, &dstrect,
                    SDL_MapRGB(screen_surface->format, bg_r, bg_g, bg_b));
            }
        }
    }
    // Draw tiles
    for(int y = 0; y <= level.max_y; y++) {
        for(int x = 0; x <= level.max_x; x++) {
            if(level.cells[y][x] != 1) {
                SDL_Rect dstrect;
                tile_to_pix(x, y, &dstrect.x, &dstrect.y);
                dstrect.w = tile_size;
                dstrect.h = tile_size;
                SDL_FillRect(screen_surface, &dstrect,
                    SDL_MapRGB(screen_surface->format,
                    colors[level.cells[y][x]][0],
                    colors[level.cells[y][x]][1],
                    colors[level.cells[y][x]][2]));
            }
        }
    }
    // Draw pieces
    for(int i=0;i<state.n_pieces;i++) {
        // Decide sprite for the kind of the piece
        int spr_index = state.pieces[i].kind;
        if(spr_index>=N_SPRITES) spr_index = N_SPRITES-1;
        // Draw piece in the right place
        SDL_Rect dstrect;
        tile_to_pix(state.pieces[i].p_x, state.pieces[i].p_y,
            &dstrect.x, &dstrect.y);
        int dezpl_x = dir_x[state.pieces[i].stat]*tile_size/5;
        int dezpl_y = dir_y[state.pieces[i].stat]*tile_size/5;
        dstrect.x += dezpl_x;
        dstrect.y += dezpl_y;
        dstrect.w = tile_size;
        dstrect.h = tile_size;
        SDL_BlitScaled(sprites[spr_index], NULL, screen_surface, &dstrect);
    }
    // Update display
    SDL_UpdateWindowSurface(window);
}

int read_choice(pgresult *result, SDL_Event ev) {
    int tile_size = get_tile_size();
    // Commencing touch:
    if(ev.type == SDL_FINGERDOWN) {
        event_start_x = (int) (ev.tfinger.x * screen_surface->w);
        event_start_y = (int) (ev.tfinger.y * screen_surface->h);
    } else if(ev.type == SDL_MOUSEBUTTONDOWN) {
        SDL_GetMouseState(&event_start_x, &event_start_y);
    }
    // Moving touch:
    if((ev.type == SDL_FINGERMOTION || ev.type == SDL_MOUSEMOTION) &&
            event_start_x >= 0 && event_start_y >= 0) {
        int evpos_x, evpos_y, ev_dir = 0;
        if(ev.type == SDL_FINGERMOTION){
            evpos_x = (int) (ev.tfinger.x * screen_surface->w);
            evpos_y = (int) (ev.tfinger.y * screen_surface->h);
        }else{
            SDL_GetMouseState(&evpos_x, &evpos_y);
        }
        int delta_x = evpos_x - event_start_x;
        int delta_y = evpos_y - event_start_y;
        if(abs(delta_x) > abs(delta_y)) {
            ev_dir = (delta_x > 0)? 1 : 3;
        } else {
            ev_dir = (delta_y > 0)? 4 : 2;
        }
        int dist = delta_x * delta_x + delta_y * delta_y;
        if(dist >= tile_size * tile_size / 4) {
            // Get event tile:
            int pchoi_x, pchoi_y;
            pix_to_tile(event_start_x, event_start_y, &pchoi_x, &pchoi_y);
            // Reset event start:
            event_start_x = -1;
            event_start_y = -1;
            // Find if there is a compatible choice:
            char phchoi_desc[CHOICE_DESCRIPTION_BUFFER];
            sprintf(phchoi_desc,"move (%d,%d) %s",
                pchoi_x,pchoi_y,dir_name[ev_dir]);
            for(int i = 0; i < result->n_choices; i++){
                if(strcmp(phchoi_desc,result->next.choices[i].description)==0){
                    return i;
                }
            }
        }
    }
    // Releasing touch:
    if(ev.type == SDL_FINGERUP || ev.type == SDL_MOUSEBUTTONUP) {
        event_start_x = -1;
        event_start_y = -1;
    }
    //
    return -1;
}

int play_level(pglevel *level, pgstate *state) {
    event_start_x = -1;
    event_start_y = -1;
    int n_moves = 0;
    pgstate_hash(state);
    while(1) {
        pgresult result = PUZZLE_RULE(level, state);
        display_level(level, state);
        SDL_Delay(50);
        // Advance steps
        if(result.conclusion == STEP) {
            *state = result.next.step;
            pgstate_hash(state);
        }
        // React to no more choices:
        else if(result.conclusion == CHOICE && result.n_choices <= 0) {
            printf("NO MORE CHOICES!\n");
            return -1;
        }
        // React to a choice
        else if(result.conclusion == CHOICE) {
            // Wait for user input
            SDL_Event ev;
            while(SDL_PollEvent(&ev) != 0) {
                // User requests quit
                if(ev.type == SDL_QUIT) {
                    return -3;
                }
                // Read player choice:
                int answer = read_choice(&result, ev);
                if(answer >= 0){
                    n_moves++;
                    *state = result.next.choices[answer].resulting;
                    pgstate_hash(state);
                    break;
                }
            }
        }
        // Win
        else if(result.conclusion==WIN) {
            printf("WIN ON %d CHOICES!\n",n_moves);
            return n_moves;
        }
        // Strange
        else {
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
    // Load the level
    pgread_from_file("lvls/slide02.txt", &level, &state);
    // Play the level
    play_level(&level, &state);
    // Update the surface
    SDL_UpdateWindowSurface(window);
    // Close systems
    close_game();
    return 0;
}
