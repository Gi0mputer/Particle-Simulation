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


void initialize_dinam(char c){
         
    for (int i = 0; i <= NUM_AGENTS; i++) {
        
        // Initialize agents positions
        if(c == 'c'){ //center
            agents[i].x = GRID_WIDTH/2  + ((double)rand() / RAND_MAX);
            agents[i].y = GRID_HEIGHT/2 + ((double)rand() / RAND_MAX);

        }else if(c == 'r'){ //random
            agents[i].x = ((double)rand() / RAND_MAX) * (GRID_WIDTH-1);
            agents[i].y = ((double)rand() / RAND_MAX) * (GRID_HEIGHT-1);
        }
        // Initialize agents velocities
        agents[i].speed_ang = (double)rand() / RAND_MAX * 2*M_PI;
        agents[i].speed_mod =(float)IN_SPEED/10;
        
        if(SPEED_MOD_RANDOMNESS)
            agents[i].speed_mod *= MAX((double)rand()/RAND_MAX, 0.2 );
    }
}


void update_position(){

    for (int i = 0; i < NUM_AGENTS; i++) {                       
        // Set the angle between 0 and 2*M_PI
        if (agents[i].speed_ang >= 2 * M_PI) {
            agents[i].speed_ang = fmod(agents[i].speed_ang, 2 * M_PI);
        }
        while (agents[i].speed_ang < 0) {
            agents[i].speed_ang += 2 * M_PI;
        }
        // Set the angle between -M_PI and M_PI
        if(agents[i].speed_ang > M_PI){
            agents[i].speed_ang -= 2*M_PI;
        }

        // Update position adding the speed vector
        agents[i].x = agents[i].x + agents[i].speed_mod*cos(agents[i].speed_ang);
        agents[i].y = agents[i].y + agents[i].speed_mod*sin(agents[i].speed_ang);
        
        if(SPEED_ANG_RANDOMNESS != 0)
            agents[i].speed_ang += random_sign() * 0.1;
        
        border(&agents[i]);
        
        if(SENSE == 1){
                sense(&agents[i]);

        }else if(SENSE == 2){
            if(i%SENSE_2 == 0){
                sense2(&agents[i]);
            }else
                sense(&agents[i]);
        }      
    }   


    // SINGLE AGENT 
    agents[NUM_AGENTS].x = agents[NUM_AGENTS].x + 1.5*agents[NUM_AGENTS].speed_mod*cos(agents[NUM_AGENTS].speed_ang);
    agents[NUM_AGENTS].y = agents[NUM_AGENTS].y + 1.5*agents[NUM_AGENTS].speed_mod*sin(agents[NUM_AGENTS].speed_ang);
    agents[NUM_AGENTS].speed_ang += random_sign() * 0.05;


        if(MOUSE_CLICKED){
            float delta_x = MOUSE_X - agents[NUM_AGENTS].x;
            float delta_y = MOUSE_Y - agents[NUM_AGENTS].y;
            float distance = sqrt(delta_x * delta_x + delta_y * delta_y);

            float force_ang = atan2(delta_y, delta_x)  ;
            float force_mod = 3/sqrt(distance);
            float ang_diff = agents[NUM_AGENTS].speed_ang - force_ang;
            
            // Set the angle between -M_PI and M_PI
            if (ang_diff >= 2 * M_PI) {
                ang_diff = fmod(agents[NUM_AGENTS].speed_ang, 2 * M_PI);
            }
            while (ang_diff < 0) {
                ang_diff += 2 * M_PI;
            }
            if(ang_diff > M_PI){
                ang_diff -= 2*M_PI;
            }

            // Calculate the perpendicular force for orbiting
            agents[NUM_AGENTS].speed_ang = agents[NUM_AGENTS].speed_ang - 0.002*ang_diff;
            agents[NUM_AGENTS].speed_mod = MAX(0.1, agents[NUM_AGENTS].speed_mod * force_mod);
        }
        
        bounce(&agents[NUM_AGENTS]);

}

void update_speed(char C){

    double delta_x;
    double delta_y;
    double distance;
    
    if(C == 'V'){ // Vortex
        for (int i = 0; i < NUM_AGENTS+1; i++) {
            delta_x = MOUSE_X - agents[i].x;
            delta_y = MOUSE_Y - agents[i].y;
            distance = sqrt(delta_x * delta_x + delta_y * delta_y);

            float force_ang = atan2(delta_y, delta_x)  ;
            //float force_mod =  MIN(5,MAX(1, agents[i].speed_mod * 500/distance));
        

            // Calculate the perpendicular force for orbiting
            agents[i].speed_ang +=  (M_PI/2 + force_ang - agents[i].speed_ang) ;
            //agents[i].speed_mod = force_mod; //agents[i].speed_mod * force_mod;
        }
    }if(C == 'C'){  // Circle
        for (int i = 0; i < NUM_AGENTS; i++) {
            delta_x = MOUSE_X - agents[i].x;
            delta_y = MOUSE_Y - agents[i].y;
            distance = sqrt(delta_x * delta_x + delta_y * delta_y);

            float force_ang = atan2(delta_y, delta_x)  ;
            float force_mod = 1/distance;
            float ang_diff = agents[i].speed_ang - force_ang;
            
            // Set the angle between -M_PI and M_PI
            if (ang_diff >= 2 * M_PI) {
                ang_diff = fmod(agents[i].speed_ang, 2 * M_PI);
            }
            while (ang_diff < 0) {
                ang_diff += 2 * M_PI;
            }
            if(ang_diff > M_PI){
                ang_diff -= 2*M_PI;
            }

            // Calculate the perpendicular force for orbiting
            if(distance > 100){
                agents[i].speed_ang = agents[i].speed_ang - 0.02*ang_diff;
            }else{
                agents[i].speed_ang = agents[i].speed_ang + 0.2*ang_diff;
            }

            agents[i].speed_mod += 0.05*force_mod;
        }
    }if(C == 'c'){  // Circles
        for (int i = 0; i < NUM_AGENTS; i++) {
            delta_x = MOUSE_X - agents[i].x;
            delta_y = MOUSE_Y - agents[i].y;
            distance = sqrt(delta_x * delta_x + delta_y * delta_y);
            // Calculate the perpendicular force for orbiting
            agents[i].speed_ang = agents[i].speed_ang + 0.001 * distance;
            agents[i].speed_mod = agents[i].speed_mod;
        }
    }if(C == 'A'){  // Attractor
        for (int i = 0; i < NUM_AGENTS+1; i++) {
            delta_x = MOUSE_X - agents[i].x;
            delta_y = MOUSE_Y - agents[i].y;
            distance = sqrt(delta_x * delta_x + delta_y * delta_y);

            float force_ang = atan2(delta_y, delta_x)  ;
            float force_mod = 3/distance;
            float ang_diff = agents[i].speed_ang - force_ang;
            
            // Set the angle between -M_PI and M_PI
            if (ang_diff >= 2 * M_PI) {
                ang_diff = fmod(agents[i].speed_ang, 2 * M_PI);
            }
            while (ang_diff < 0) {
                ang_diff += 2 * M_PI;
            }
            if(ang_diff > M_PI){
                ang_diff -= 2*M_PI;
            }

            // Calculate the perpendicular force for orbiting
            agents[i].speed_ang = agents[i].speed_ang - 0.02*ang_diff;
            agents[i].speed_mod = MAX(0.1, agents[i].speed_mod * force_mod);
        }
    }if(C == 'D'){  // Deattractor
        for (int i = 0; i < NUM_AGENTS; i++) {
            delta_x = MOUSE_X - agents[i].x;
            delta_y = MOUSE_Y - agents[i].y;

            if(delta_x > GRID_WIDTH/2){
                delta_x = GRID_WIDTH-1 - delta_x;
            }else if(delta_x < -GRID_WIDTH/2){
                delta_x = GRID_WIDTH-1 + delta_x;
            }
            if(delta_y > GRID_HEIGHT/2){
                delta_y = GRID_HEIGHT-1 - delta_y;
            }else if(delta_y < - GRID_HEIGHT/2){
                delta_y = GRID_HEIGHT-1 + delta_y;
            }

            distance = sqrt(delta_x * delta_x + delta_y * delta_y);

            float force_ang = atan2(delta_y, delta_x)  ;
            float force_mod = 5/distance;
            float ang_diff = agents[i].speed_ang - force_ang;
            
            // Set the angle between -M_PI and M_PI
            if (ang_diff >= 2 * M_PI) {
                ang_diff = fmod(agents[i].speed_ang, 2 * M_PI);
            }
            while (ang_diff < 0) {
                ang_diff += 2 * M_PI;
            }
            if(ang_diff > M_PI){
                ang_diff -= 2*M_PI;
            }
            // Calculate the perpendicular force for orbiting
            agents[i].speed_ang = agents[i].speed_ang + 0.1*ang_diff;
            agents[i].speed_mod = MAX(0.5, force_mod);
        }
    }    
}

void border(Agent_Struct *agents){
    // X border
    if((agents->x > GRID_WIDTH-1)){
        agents->x = 0;
        if(sin(agents->speed_ang) == 0) 
            agents->speed_ang++;
    }else if(agents->x < 0){
        agents->x = GRID_WIDTH-1;
        if(sin(agents->speed_ang) == 0) 
            agents->speed_ang--;
    }
    // Y border
    if(agents->y > GRID_HEIGHT-1){
        agents->y = 0;
        if(cos(agents->speed_ang) == 0) 
            agents->speed_ang++;
    }
    else if(agents->y < 0){
        agents->y = GRID_HEIGHT-1;
        if(cos(agents->speed_ang) == 0) 
            agents->speed_ang--;
    }
}

void bounce(Agent_Struct *agents){
    // X bounce
    if(agents->x >= GRID_WIDTH-1){
        agents->x = GRID_WIDTH-1;
        agents->speed_ang = -agents->speed_ang + M_PI;
    }else if(agents->x <= 0){
        agents->x = 0;
        agents->speed_ang = -agents->speed_ang + M_PI;
    }
    // Y bounce
    if(agents->y >= GRID_HEIGHT-1){
        agents->y = GRID_HEIGHT-1;
        agents->speed_ang = -agents->speed_ang;
    }else if(agents->y <= 0){
        agents->y = 0;
        agents->speed_ang = -agents->speed_ang;
    }
}

double random_sign(){
    int sign = rand() % 2;  // 0 or 1
    double random_value = (double)rand() / RAND_MAX; //[0.0, 1.0)
    if (sign == 0) {
        random_value = -random_value;  // Make it negative
    }
    return random_value;
}

void resize_dinam(int new_width, int new_height){
    
    for (int i = 0; i < NUM_AGENTS+1; i++) {                       
        // Set the angle between 0 and 2*M_PI
        agents[i].x = (agents[i].x * new_width)/ GRID_WIDTH;                 
        agents[i].y = (agents[i].y * new_height)/ GRID_HEIGHT;         
    }   
}

void play_dinam(){

    update_position();
    if(MOUSE_CLICKED)
        update_speed(IN_MOUSE_CHAR);
}


void sense(Agent_Struct *agents){

    int LEFT_SENSOR = 0;
    int CENTER_SENSOR = 0;
    int RIGHT_SENSOR = 0;
    int sensor_max = SENSOR_RADIUS * SENSOR_RADIUS * 255;

    if (agents->speed_mod == 0) {
        agents->speed_mod = (double)rand() / RAND_MAX * IN_SPEED/10;
    }

    Sensor_Struct sensor;
    sensor.SL_x = SENSOR_MODULE * cos(agents->speed_ang + SENSOR_ANGLE);
    sensor.SL_y = SENSOR_MODULE * sin(agents->speed_ang + SENSOR_ANGLE);
    sensor.SC_x = SENSOR_MODULE * cos(agents->speed_ang);
    sensor.SC_y = SENSOR_MODULE * sin(agents->speed_ang);
    sensor.SR_x = SENSOR_MODULE * cos(agents->speed_ang - SENSOR_ANGLE);
    sensor.SR_y = SENSOR_MODULE * sin(agents->speed_ang - SENSOR_ANGLE);
    
    // Read in the sensors 
    for (int x = agents->x -SENSOR_RADIUS; x <= agents->x +SENSOR_RADIUS; x++) {
        for (int y = agents->y -SENSOR_RADIUS; y <= agents->y +SENSOR_RADIUS; y++) {

                LEFT_SENSOR   += gray[((int)(x + sensor.SL_x) + GRID_WIDTH -1) % (GRID_WIDTH-1)][((int)(y + sensor.SL_y) + GRID_WIDTH-1) % (GRID_WIDTH-1)];
                CENTER_SENSOR += gray[((int)(x + sensor.SC_x) + GRID_WIDTH -1) % (GRID_WIDTH-1)][((int)(y + sensor.SC_y) + GRID_WIDTH-1) % (GRID_WIDTH-1)];
                RIGHT_SENSOR  += gray[((int)(x + sensor.SR_x) + GRID_WIDTH -1) % (GRID_WIDTH-1)][((int)(y + sensor.SR_y) + GRID_WIDTH-1) % (GRID_WIDTH-1)];
         }
    }

    // Calculate the new speed components

    if(SPEED_ANGLE_RANDOMNESS){
        if(CENTER_SENSOR < RIGHT_SENSOR && CENTER_SENSOR < LEFT_SENSOR){
            if(rand()%2 == 0){
                 agents->speed_ang -= SPEED_ANGLE * ((double)rand() / RAND_MAX); 
            }else{
                agents->speed_ang += SPEED_ANGLE * ((double)rand() / RAND_MAX); 
            }
        }else if(LEFT_SENSOR < RIGHT_SENSOR){
            agents->speed_ang -= SPEED_ANGLE * ((double)rand() / RAND_MAX);    
        }else if(LEFT_SENSOR > RIGHT_SENSOR){
            agents->speed_ang += SPEED_ANGLE * ((double)rand() / RAND_MAX);     
        }   
    }else{
        if(CENTER_SENSOR < RIGHT_SENSOR && CENTER_SENSOR < LEFT_SENSOR){
            if(rand()%2 == 0){
                 agents->speed_ang -= SPEED_ANGLE; 
            }else{
                agents->speed_ang += SPEED_ANGLE; 
            }
        }else if(LEFT_SENSOR < RIGHT_SENSOR){
            agents->speed_ang -= SPEED_ANGLE;    
        }else if(LEFT_SENSOR > RIGHT_SENSOR){
            agents->speed_ang += SPEED_ANGLE;     
        }
    }

    agents->speed_mod = MIN(1,
                        0.1* agents->speed_mod 
                        +0.1*(CENTER_SENSOR/sensor_max) * agents->speed_mod  
                        +0.1*(LEFT_SENSOR/sensor_max) * agents->speed_mod
                        +0.1*(RIGHT_SENSOR/sensor_max) * agents->speed_mod
                        +1.1*(((float)IN_SPEED/10)-(agents->speed_mod))); 

}

void sense2(Agent_Struct *agents){

    int LEFT_SENSOR = 0;
    int CENTER_SENSOR = 0;
    int RIGHT_SENSOR = 0;
    int sensor_max = SENSOR_RADIUS * SENSOR_RADIUS * 255;

    if (agents->speed_mod == 0) {
        agents->speed_mod = (double)rand() / RAND_MAX * IN_SPEED/10/10;
    }

    Sensor_Struct sensor;
    sensor.SL_x = SENSOR_MODULE * cos(agents->speed_ang + SENSOR_ANGLE);
    sensor.SL_y = SENSOR_MODULE * sin(agents->speed_ang + SENSOR_ANGLE);
    sensor.SC_x = SENSOR_MODULE * cos(agents->speed_ang);
    sensor.SC_y = SENSOR_MODULE * sin(agents->speed_ang);
    sensor.SR_x = SENSOR_MODULE * cos(agents->speed_ang - SENSOR_ANGLE);
    sensor.SR_y = SENSOR_MODULE * sin(agents->speed_ang - SENSOR_ANGLE);
    
    // Read in the sensors 
    for (int x = agents->x -SENSOR_RADIUS; x <= agents->x +SENSOR_RADIUS; x++) {
        for (int y = agents->y -SENSOR_RADIUS; y <= agents->y +SENSOR_RADIUS; y++) {

                LEFT_SENSOR   += gray[((int)(x + sensor.SL_x) + GRID_WIDTH -1) % (GRID_WIDTH-1)][((int)(y + sensor.SL_y) + GRID_WIDTH-1) % (GRID_WIDTH-1)];
                CENTER_SENSOR += gray[((int)(x + sensor.SC_x) + GRID_WIDTH -1) % (GRID_WIDTH-1)][((int)(y + sensor.SC_y) + GRID_WIDTH-1) % (GRID_WIDTH-1)];
                RIGHT_SENSOR  += gray[((int)(x + sensor.SR_x) + GRID_WIDTH -1) % (GRID_WIDTH-1)][((int)(y + sensor.SR_y) + GRID_WIDTH-1) % (GRID_WIDTH-1)];
         }
    }

    // Calculate the new speed components
 
        if(LEFT_SENSOR > CENTER_SENSOR){
            agents->speed_ang -= SPEED_ANGLE * LEFT_SENSOR/sensor_max;    
        }else if(RIGHT_SENSOR > CENTER_SENSOR){
            agents->speed_ang += SPEED_ANGLE * RIGHT_SENSOR/sensor_max;    
        }else{
            //agents->speed_ang -= SPEED_ANGLE * CENTER_SENSOR/sensor_max;
        }

    //
    agents->speed_mod = MIN(3,
                        0.3* agents->speed_mod 
                        +0.1*(CENTER_SENSOR/sensor_max) * agents->speed_mod  
                        +0.1*(LEFT_SENSOR/sensor_max) * agents->speed_mod
                        +0.1*(RIGHT_SENSOR/sensor_max) * agents->speed_mod
                        +1.3*(((float)IN_SPEED/10/8)-(agents->speed_mod))); 
} 