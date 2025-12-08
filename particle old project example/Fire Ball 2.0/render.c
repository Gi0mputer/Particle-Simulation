#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "init_cond.h"
#include "render.h"
#include "image.h"
#include "dinam.h"

void initialize_render(){
    
    // Create a window
    window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 860, SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
    }

    SDL_MaximizeWindow(window);

    SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // Get GRID sizes
    GRID_WIDTH = WINDOW_WIDTH/INIT_SCALE;
    PROPORTION = (double)WINDOW_HEIGHT/(double)WINDOW_WIDTH;
    GRID_HEIGHT = GRID_WIDTH * PROPORTION;

    // Create texture                     
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, GRID_WIDTH, GRID_HEIGHT);
    if (!texture) {
        printf("Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}

void initialize_save(){
    
    // Remove the "frames" folder and its contents
    if (system("rm -r frames")<0){
        printf("rm -r frames failed\n");
    }
    // Create the "frames" folder
    if (system("mkdir frames")<0){
        printf("mkdir frames failed\n");
    }
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        // Handle SDL_image initialization error
        printf("\nIMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG  : %s\n", SDL_GetError());  
        SDL_Quit();
    }
}

void cleanup_render(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    SDL_Quit();
    IMG_Quit();
}

void draw_render() {
    
    // Update the texture with the data from the gray matrix
    rgba_convert();

    SDL_UpdateTexture(texture, NULL, rgba, GRID_WIDTH * 4);

    SDL_RenderCopy(renderer, texture, NULL, NULL);

    //Present (display) the rendered frame
    SDL_RenderPresent(renderer);
}

void process_event() {

    // Keeps going in paraller to the main untill it polls an event
    while (SDL_PollEvent(&event)) {

        // End if window is closed
        if (event.type == SDL_QUIT) {
            RUNNING = 0;
        // Store window current size
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {

                // Save new sizes
                WINDOW_WIDTH = event.window.data1;
                WINDOW_HEIGHT = event.window.data2;
            }
        // Handle key pressing
        } else if (event.type == SDL_KEYDOWN) {
            keyPressed = event.key.keysym.sym;
            if (keyPressed == SDLK_p) {
                if (PAUSE == 1) {
                    PAUSE = 0;
                } else if (PAUSE == 0) {
                    PAUSE = 1;
                }
            }
            if (keyPressed == SDLK_l) {
                if (SENSE == 1) {
                    SENSE = 0;
                } else if (SENSE == 0) {
                    SENSE = 1;
                }
            }
            if (keyPressed == SDLK_s) {
                if (SAVE == 1) {
                    SAVE = 0;
                } else if (SAVE == 0) {
                    SAVE = 1;
                    initialize_save();
                }
            }
            if (keyPressed == SDLK_F11) {
                if (WINDOW_FULL == 1) {
                    WINDOW_CHANGE = 1;
                    WINDOW_FULL = 0;
                } else if (WINDOW_FULL == 0) {
                    WINDOW_CHANGE = 1;
                    WINDOW_FULL = 1;
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN){
            if (event.button.button == SDL_BUTTON_LEFT) {
                if(MOUSE_CLICKED == 0){
                    //SDL_ShowCursor(SDL_DISABLE); // Hide the mouse cursor
                    MOUSE_CLICKED = 1;
                }else if(MOUSE_CLICKED == 1){
                    //SDL_ShowCursor(SDL_ENABLE);; // Show the mouse cursor
                    MOUSE_CLICKED = 0;
                } 
            }
        }       
    }

    
    if (WINDOW_FULL == 1 && WINDOW_CHANGE == 1) {    
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        WINDOW_CHANGE = 0;
    }else if (WINDOW_FULL == 0 && WINDOW_CHANGE == 1) {    
        SDL_SetWindowFullscreen(window, 0);
        WINDOW_CHANGE = 0;
    }
    
    // Change dimension of what is displayed
    if (WINDOW_CHANGE == 10) {    
        PROPORTION = (double)WINDOW_HEIGHT/(double)WINDOW_WIDTH;
        int new_width = WINDOW_WIDTH/SCALE;
        int new_height = (int)((WINDOW_WIDTH/SCALE)*PROPORTION);
        resize_image(new_width, new_height);
        GRID_WIDTH = new_width;
        GRID_HEIGHT = new_height;
        resize_texture();
    }
    

    // Change mouse state
    if(MOUSE_CLICKED == 1){
        MOUSE_STATE = SDL_GetMouseState(&MOUSE_X, &MOUSE_Y);
        MOUSE_X = (MOUSE_X*GRID_WIDTH)/WINDOW_WIDTH;
        MOUSE_Y = (MOUSE_Y*GRID_HEIGHT)/WINDOW_HEIGHT;
    }

    /////////  KEYBOARD ////////
    const Uint8* state = SDL_GetKeyboardState(NULL);
    // MOVE
    if (state[SDL_SCANCODE_UP]) {
        move_image('u');
        ARROW_POS.x++;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        move_image('d');
        ARROW_POS.x--;
    }
    if (state[SDL_SCANCODE_LEFT]) {   
        move_image('l');
        ARROW_POS.y++;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        move_image('r');
        ARROW_POS.y--;
    }
}

void resize_texture(){
    SDL_DestroyTexture(texture); // Destroy the old texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, GRID_WIDTH, GRID_HEIGHT);
}

void saveFrame(int frameNumber) {
    
    char filename[100]; // Adjust the buffer size as needed
    sprintf(filename, "frames/frame%d.png", frameNumber);
    SDL_Surface* screenshot = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (screenshot) {
        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, screenshot->pixels, screenshot->pitch);

        // Save the frame in the "frames" folder
        IMG_SavePNG(screenshot, filename);

        SDL_FreeSurface(screenshot);
    } else {
        // Handle SDL_CreateRGBSurface error
        SDL_FreeSurface(screenshot);
        saveFrame(frameNumber);

    }
}

void drawFilledCircle(int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void hsvToRgb(double h, double s, double v, Uint8* r, Uint8* g, Uint8* b) {
    if (s == 0.0) {
        // Achromatic (gray) color
        *r = *g = *b = (int)(v * 255.0);
    } else {
        double hh = h * 6.0;
        int i = (int)hh;
        double f = hh - i;
        double p = v * (1.0 - s);
        double q = v * (1.0 - (s * f));
        double t = v * (1.0 - (s * (1.0 - f)));

        switch (i) {
            case 0:
                *r = (int)(v * 255.0);
                *g = (int)(t * 255.0);
                *b = (int)(p * 255.0);
                break;
            case 1:
                *r = (int)(q * 255.0);
                *g = (int)(v * 255.0);
                *b = (int)(p * 255.0);
                break;
            case 2:
                *r = (int)(p * 255.0);
                *g = (int)(v * 255.0);
                *b = (int)(t * 255.0);
                break;
            case 3:
                *r = (int)(p * 255.0);
                *g = (int)(q * 255.0);
                *b = (int)(v * 255.0);
                break;
            case 4:
                *r = (int)(t * 255.0);
                *g = (int)(p * 255.0);
                *b = (int)(v * 255.0);
                break;
            default: // case 5:
                *r = (int)(v * 255.0);
                *g = (int)(p * 255.0);
                *b = (int)(q * 255.0);
                break;
        }
    }
}