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


/////////// RENDER ///////////
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture* texture;

SDL_Color color;
Hsv_Struct HSV;
Arrow_Pos_Struct ARROW_POS;
SDL_Event event;
SDL_Keycode keyPressed;
int WINDOW_CHANGE = 0;
int WINDOW_FULL = 0;
int MOUSE_X, MOUSE_Y;
Uint32 MOUSE_STATE;
Uint8 MOUSE_CLICKED = 0;

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int FRAME;
Uint8 SAVE = SAVE_FRAME;
Uint8 RUNNING = 1;
Uint8 PAUSE = 0;

///////// IMAGE ////////
uint8_t** gray;
Uint8* rgba;

int GRID_WIDTH;
int GRID_HEIGHT;
float PROPORTION;
Uint8 SCALE = INIT_SCALE;
float REST = 0;

float DT;
int INCREMENT;

///////// DINAM ////////

Agent_Struct agents[NUM_AGENTS+1];
Uint8 DINAM = IN_DINAM;
Uint8 SENSE = IN_SENSE;
float dinam_change;


int main() {

    srand(time(NULL));
    initialize_render();
    initialize_image();
    initialize_dinam(INITIALIZE_DINAM);
    if(SAVE)
        initialize_save();
        
    

    while(RUNNING){

        process_event(); // has to be the first to check window sizes or actions
        if(!PAUSE){

            for(int i = 0; i<CALC_TO_RENDER; i++){ // Perform a siries of calculation before render
                if(DINAM)
                    play_dinam();
                update_image();
            }    
        }
   
        draw_render();

        if(SAVE){
            saveFrame(++FRAME);
        }

        //DELAY
        SDL_Delay(DELAY*250);

    }
    
    // Cleanup and exit
    cleanup_render();
    cleanup_image();

    return 0;
}