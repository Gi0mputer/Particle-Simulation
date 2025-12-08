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

void initialize_image() {

    // Allocate the GRID
    gray   = (uint8_t**)malloc(GRID_WIDTH * sizeof(uint8_t*));
    for (int i = 0; i < GRID_WIDTH; i++) {
        gray[i]   = (uint8_t*)malloc(GRID_HEIGHT * sizeof(uint8_t));
    }

    rgba = (Uint8*)malloc(GRID_WIDTH * GRID_HEIGHT * 4 *(sizeof(Uint8)));

    // Put the grid at 0 then add the initialize option
    add_to_image('z');
    add_to_image(INITIALIZE_OPTION);
}

void cleanup_image(){
    for (int i = 0; i < GRID_WIDTH; i++) {
        free(gray[i]);
    }
    free(gray);
    free(rgba);
}
void resize_image(int new_width, int new_height){

    resize_dinam(new_width, new_height);

    // Create the new grid with new dimentions
    uint8_t** new_gray = (uint8_t**)malloc(new_width* sizeof(uint8_t*));
    for (int i = 0; i < new_width; i++){
        new_gray[i] = (uint8_t*)calloc(new_height, sizeof(uint8_t));
    }

    // Copy a part of the bigger old grid inside the new one
    if(new_width < GRID_WIDTH){
        
        // inside the new smaller dimension
        for (int x = -new_width/2; x < new_width/2; x++){
            for (int y = -new_height/2; y < new_height/2; y++){
                // from the center
                new_gray[x + new_width/2][y + new_height/2] = gray[x + new_width/2][y + new_height/2];
            }
        }
       

    // Copy all the smaller old grid inside the center of the new bigger one
    }else if(new_width > GRID_WIDTH){

        for (int x = -GRID_WIDTH/2; x < GRID_WIDTH/2; x++){
            for (int y = -GRID_HEIGHT/2; y < GRID_HEIGHT/2; y++) {

                new_gray[x + new_width/2][y + new_height/2] = gray[x + GRID_WIDTH/2][y + GRID_HEIGHT/2];
            }
        }
    }

    //Free the memory of the old grid
    for (int i = 0; i < GRID_WIDTH; i++) {
        free(gray[i]);
    }
    free(gray);

    //Update the global variables with the new dimensions
    gray = new_gray;

}

void add_to_image(char option){
    //ALL ZEROS
    if(option == 'z'){
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = 0; y < GRID_HEIGHT; y++){
                gray[x][y] = 0;
            
            }
        }
    }
    //ALL RANDOM
    if(option == 'r'){
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = 0; y < GRID_HEIGHT; y++){
                gray[x][y] = rand() % 255;
            }
        }
    }
    //ONLY BORDER
    if(option == 'b'){
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = 0; y < GRID_HEIGHT; y++){
                if (x == 0 || x == GRID_WIDTH - 1 || y == 0 || y == GRID_HEIGHT - 1){
                    gray[x][y] = rand() % 255;
                } else {
                    gray[x][y] = 0;
                }
            }
        }
    }
}

void update_image() {

    INCREMENT++;

    if(AGENT_SIZE == 0){
        // Add one pixel
        for (int i = 0; i < NUM_AGENTS; i++) {
            if((int)agents[i].x < GRID_WIDTH && (int)agents[i].x > 0 && (int)agents[i].y < GRID_HEIGHT && (int)agents[i].x > 0)
                gray[(int)agents[i].x][(int)agents[i].y] = 255;
        }
    }else{
        // Create a circle in agents positions
        for (int i = 0; i < NUM_AGENTS; i++) {
            for (int x = agents[i].x - AGENT_SIZE; x <= agents[i].x + AGENT_SIZE; x++) {
                for (int y = agents[i].y - AGENT_SIZE; y <= agents[i].y + AGENT_SIZE; y++) {
                    int nx = x - agents[i].x;
                    int ny = y - agents[i].y;

                    // Check if the point (x, y) is within the circle's boundaries
                    if (nx * nx + ny * ny <= AGENT_SIZE * AGENT_SIZE) {
                        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                            gray[x][y] = 255;
                        }
                    }
                }
            }
        }
    }   
    
    if(FAT_ONE){
        int AGENT_X_SIZE =100;
        for (int x = agents[NUM_AGENTS].x - AGENT_X_SIZE; x <= agents[NUM_AGENTS].x + AGENT_X_SIZE; x++) {
            for (int y = agents[NUM_AGENTS].y - AGENT_X_SIZE; y <= agents[NUM_AGENTS].y + AGENT_X_SIZE; y++) {
                int nx = x - agents[NUM_AGENTS].x;
                int ny = y - agents[NUM_AGENTS].y;

                // Check if the point (x, y) is within the circle's boundaries
                if (nx * nx + ny * ny <= AGENT_X_SIZE * AGENT_X_SIZE) {
                    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                        if(gray[x][y] > 0){
                            gray[x][y] = gray[x][y]-1;
                        }
                        
                    }
                }
            }
        }
        AGENT_X_SIZE =10;
        for (int x = agents[NUM_AGENTS].x - AGENT_X_SIZE; x <= agents[NUM_AGENTS].x + AGENT_X_SIZE; x++) {
            for (int y = agents[NUM_AGENTS].y - AGENT_X_SIZE; y <= agents[NUM_AGENTS].y + AGENT_X_SIZE; y++) {
                int nx = x - agents[NUM_AGENTS].x;
                int ny = y - agents[NUM_AGENTS].y;

                // Check if the point (x, y) is within the circle's boundaries
                if (nx * nx + ny * ny <= AGENT_X_SIZE * AGENT_X_SIZE) {
                    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                        gray[x][y] = 255;
                    }
                }
            }
        }
    }

    if(IMAGE_SUBTRACTION !=0 && (INCREMENT%IMAGE_SUBTRACTION == 0)){
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = 0; y < GRID_HEIGHT; y++){
                if(gray[x][y] >= SUB_VAL){
                    gray[x][y] -= SUB_VAL;
                }else{
                    gray[x][y] = 0;
                }  
            }
        }
    }

    if(IMAGE_DIFFUSION !=0 && (INCREMENT%IMAGE_DIFFUSION == 0)){
        //Smooth out the matrix values
        averaging_filter();
    }
}

void averaging_filter(){
    //Smooth out the matrix values
    int count = 0;
    int div = 0;

    for (int x = 0; x < GRID_WIDTH; x++){
        for (int y = 0; y < GRID_HEIGHT; y++){
            for (int i = -FILTER_DIM; i <= FILTER_DIM; i++){
                for (int j = -FILTER_DIM; j <= FILTER_DIM; j++){

                    count += gray[(x + i + GRID_WIDTH) % GRID_WIDTH][(y + j + GRID_HEIGHT) % GRID_HEIGHT];
                    div++;
                    if(i == 0 && j== 0){
                        count += CENTER_WEIGHT * gray[(x + i + GRID_WIDTH) % GRID_WIDTH][(y + j + GRID_HEIGHT) % GRID_HEIGHT];
                        div += CENTER_WEIGHT;
                    }
                }
            }
            gray[x][y] = count/(div);
            count = 0; 
            div = 0;
        }
    } 
}

// Used in render.c in process_event()
void move_image(char key){
    if (key == 'u'){
        //Shift the grid downward by MOVE rows
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = GRID_HEIGHT - 1; y >= MOVE; y--){
                gray[x][y] = gray[x][y - MOVE];
            }
        }
        //Fill the newly empty rows at the top with zeros
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = 0; y < MOVE; y++){
                gray[x][y] = 0;
            }
        }
    }
    if (key == 'd'){
        //Shift the grid upward by MOVE rows
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = 0; y < GRID_HEIGHT - MOVE; y++){
                gray[x][y] = gray[x][y + MOVE];
            }
        }
        //Fill the newly empty rows at the bottom with zeros
        for (int x = 0; x < GRID_WIDTH; x++){
            for (int y = GRID_HEIGHT - MOVE; y < GRID_HEIGHT; y++){
                gray[x][y] = 0;
            }
        }
    }
    if (key == 'l'){
        //Shift the grid rightward by MOVE columns
        for (int y = 0; y < GRID_HEIGHT; y++){
            for (int x = GRID_WIDTH - 1; x >= MOVE; x--){
                gray[x][y] = gray[x - MOVE][y];
            }
        }
        //Fill the newly empty columns on the left with zeros
        for (int y = 0; y < GRID_HEIGHT; y++){
            for (int x = 0; x < MOVE; x++){
                gray[x][y] = 0;
            }
        }
    }
    if (key == 'r'){
        //Shift the grid leftward by MOVE columns
        for (int y = 0; y < GRID_HEIGHT; y++){
            for (int x = 0; x < GRID_WIDTH - MOVE; x++){
                gray[x][y] = gray[x + MOVE][y];
            }
        }
        //Fill the newly empty columns on the right with zeros
        for (int y = 0; y < GRID_HEIGHT; y++){
            for (int x = GRID_WIDTH - MOVE; x < GRID_WIDTH; x++){
                gray[x][y] = 0;
            }
        }
    }
}

void rgba_convert(){
    Uint8* pixel = NULL;

    // Calculate the angle using INCREMENT, mapping it from 0 to 2*pi
    float angle = (INCREMENT/(CALC_TO_RENDER*255.0)) * 2 * M_PI;

    // Calculate the radius using a sine function to achieve the desired effect
    float min_radius = 20; // Minimum radius
    float max_radius = 80; // Maximum radius
    float period = 4;

    // Use the sine function to smoothly transition the radius from min to max and back to min
    float radius = max_radius + min_radius + (max_radius - min_radius) * cos(angle/period + M_PI) ;

    // Convert grayscale values in the red matrix to RGBA format
     for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y <= GRID_HEIGHT; y++) {
            Uint8 gray_val = gray[x][y];
            pixel = &rgba[     (x + y*GRID_WIDTH) * 4];
            HSV.s = 1.0;
            HSV.v = 1.0;

            int delta_x = x - agents[NUM_AGENTS].x;
            int delta_y = y - agents[NUM_AGENTS].y;
            int distance = sqrt(delta_x * delta_x + delta_y * delta_y);
            float fat;
            if(FAT_ONE)
                fat = (float)1/((distance/70)+1); //1 when near, 0 when far, 0.5 in 100
            
            

            // NO BACKGROUND
            if      (COLOR == 10){   // void
                if(gray_val == 0){
                    HSV.v = 0;
                }else{
                    HSV.h =  ( MIN((sqrt(gray_val*0.05 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
                } 
            }else if(COLOR == 11){   // supnov
                if(gray_val == 0){
                    HSV.v = 0;
                }else{
                    HSV.h = -fat + ( MIN((sqrt(gray_val*0.05 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
                } 
            }else if(COLOR == 12){   // firac
                if(gray_val == 0){
                    HSV.v = 0;
                }else{
                    HSV.h = fat + ( MIN((sqrt(gray_val*0.5 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
                }     
            }else if(COLOR == 13){   // spagh
                if(gray_val == 0){
                    HSV.v = 0;
                }else{
                    HSV.h = fat -( MIN((sqrt(gray_val*0.5 -1) * 0.02), 0.71 + 0.01* cos(( 0.1 * gray_val))));
                } 
            }else if(COLOR == 14){   // green

                if(gray_val == 0){
                    HSV.h = 0.08;
                    HSV.v = 0.1; // 1 nothing 0 black
                    HSV.s = 0.85; // 1 nothing 0 white

                }else{
                    HSV.h =  fat + (0.25*(float)(gray_val*gray_val)/(255*255)) - 0.1*sin(0.05*gray_val + 3*angle/period + M_PI) +0.1;
                    HSV.v = 0.3 + 0.7*(float)gray_val/255;
                    HSV.s = 0.5 + 0.5*(float)gray_val/255;
                }
            }else if(COLOR == 15){   // test

                if(gray_val == 0){
                    HSV.h = 0.65;
                    HSV.v = 0.1; // 1 nothing 0 black
                    HSV.s = 0.85; // 1 nothing 0 white

                }else{
                    HSV.h =  fat*0.8 - ((float)0.2*gray_val/255) - 0.1*sin(0.05*gray_val + 3*angle/period + M_PI);
                    HSV.s = 1; // 1 nothing 0 white
                }
            }else if(COLOR == 16){   
                if(gray_val == 0){
                    HSV.h = 0.9;
                    HSV.v = 0.9; // 1 nothing 0 black
                    HSV.s = 0.1; // 1 nothing 0 white
                }else{
                    HSV.h = 0.5 - 0.003*gray_val * sin(0.004*gray_val);
                }


            //  WITH BACKGROUND
            }else if(COLOR == 20){// void 
                HSV.h = ( MIN((sqrt(gray_val*0.05 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
            }else if(COLOR == 21){// Supernova  
                HSV.h = ( MIN((sqrt(gray_val*0.05 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
            }else if(COLOR == 22){// Fire acid   
                HSV.h = ( MIN((sqrt(gray_val*0.5 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
            }else if(COLOR == 23){// 20test   
                HSV.h = ( MIN((sqrt(gray_val*0.5 -1) * 0.05), 0.71 + 0.01* cos(( 0.1 * gray_val))));
            }

            if(COLOR > 9)
                hsvToRgb(HSV.h, HSV.s, HSV.v, &color.r, &color.g, &color.b);
           
            if(COLOR == 0){       // b&w
                    pixel[0] = 255;      // Alpha (fully opaque)
                    pixel[1] = gray_val; // Blue
                    pixel[2] = gray_val; // Green
                    pixel[3] = gray_val; // Red               
            }else if(COLOR == 1){ // brain
                    pixel[0] = 255;            
                    pixel[1] = 158 - gray_val; 
                    pixel[2] = 137 - gray_val; 
                    pixel[3] = 255;                                       
            }else if(COLOR == 2){ // azzu
                    pixel[0] = 255;      
                    pixel[1] =  gray_val - 0.3 * gray_val + 0.05 * gray_val * gray_val ;                  
                    pixel[2] =  gray_val - 0.1 * gray_val + 0.05 * gray_val * gray_val ;         
                    pixel[3] =  gray_val;           
            }else if(COLOR == 3){ // test
                    pixel[0] = 255;      
                    pixel[1] =  0.1*gray_val- 0.3 * gray_val + 0.05 * gray_val * gray_val;                  
                    pixel[2] =  gray_val;         
                    pixel[3] =  0.1*gray_val- 0.1 * gray_val + 0.05 * gray_val * gray_val;
            }else if(COLOR == 9){ // test
                    pixel[0] = 255;      
                    pixel[1] =  gray_val;                  
                    pixel[2] =  gray_val;         
                    pixel[3] =  gray_val;   
            }else if(COLOR == 10){// void
                    pixel[0] = 255;                 
                    pixel[1] =color.b;
                    pixel[2] =color.g;
                    pixel[3] =color.r;
            }else if(COLOR == 11){// supnov
                    pixel[0] = 255;                 
                    pixel[1] =color.b;
                    pixel[2] =color.g;
                    pixel[3] =color.r;  
            }else if(COLOR == 12){ // firac
                    pixel[0] = 255;                 
                    pixel[1] =color.b;
                    pixel[2] =color.g;
                    pixel[3] =color.r;  
            }else if(COLOR == 13){// spagh
                    pixel[0] = 255; 
                    pixel[1] = color.b + gray_val;
                    pixel[2] = color.g + gray_val;
                    pixel[3] = color.r;
            }else if(COLOR == 14){// green
                    pixel[0] = 255;                 
                    pixel[1] =color.b;
                    pixel[2] =color.g;
                    pixel[3] =color.r; 
            }else if(COLOR == 15){// test
                    pixel[0] = 255; 
                    pixel[1] = color.b;//+ 4*gray_val;
                    pixel[2] = color.g;//+ 2*gray_val;
                    pixel[3] = color.r;//+ gray_val;
            }else if(COLOR == 16){ //Blue Fire
                    pixel[0] = 255;                 
                    pixel[1] =255 - (color.b*0.8 + color.g*0.2);
                    pixel[2] =255 - (color.g - color.b);
                    pixel[3] =255 - (color.r*0.8 + color.g*0.2); 
            }else if(COLOR == 20){// 20 void
                    pixel[0] = 255;     
                    pixel[1] = color.b+ gray_val + ((x -GRID_WIDTH/2)*(x-GRID_WIDTH/2) + (y-GRID_HEIGHT/2)*(y-GRID_HEIGHT/2))/radius + INCREMENT;
                    pixel[2] = color.g;  
                    pixel[3] = color.r; 
            }else if(COLOR == 21){// 20 supnov
                    pixel[0] = 255;     
                    pixel[1] = color.b+ gray_val + ((x -GRID_WIDTH/2)*(x-GRID_WIDTH/2) + (y-GRID_HEIGHT/2)*(y-GRID_HEIGHT/2))/radius + INCREMENT;
                    pixel[2] = color.g;  
                    pixel[3] = color.r;            
            }else if(COLOR == 22){// firac
                    pixel[0] = 255; 
                    pixel[1] = color.b + gray_val + ((x -GRID_WIDTH/2)*(x-GRID_WIDTH/2) + (y-GRID_HEIGHT/2)*(y-GRID_HEIGHT/2))/radius + INCREMENT;
                    pixel[2] = color.g + gray_val + ((x -GRID_WIDTH/2)*(x-GRID_WIDTH/2) + (y-GRID_HEIGHT/2)*(y-GRID_HEIGHT/2))/radius + INCREMENT;
                    pixel[3] = color.r;             
            }else if(COLOR == 23){// 20test
                    pixel[0] = 255; 
                    pixel[1] =color.b+ gray_val + ((x -GRID_WIDTH/2)*(x-GRID_WIDTH/2) + (y-GRID_HEIGHT/2)*(y-GRID_HEIGHT/2))/radius + INCREMENT;
                    pixel[2] =color.g+ gray_val + ((x -GRID_WIDTH/2)*(x-GRID_WIDTH/2) + (y-GRID_HEIGHT/2)*(y-GRID_HEIGHT/2))/radius + INCREMENT;
                    pixel[3] =color.r;             
            }else{
                    pixel[0] = 255;
                    pixel[1] = gray_val;
                    pixel[2] = gray_val;
                    pixel[3] = gray_val;     
            }              
        }   
    }
}  




