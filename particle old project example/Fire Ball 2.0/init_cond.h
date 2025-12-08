#ifndef INCOND_H
#define INCOND_H//"-O3",

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define NAME "Fire Ball"
#define CALC_TO_RENDER 20

// INIT
#define IN_DINAM 1              // 1 to start with dinamic particle
#define INITIALIZE_DINAM 'c'    // Particles position, 'r' random, 'c' center...
#define INITIALIZE_OPTION 'z'   // The option displayed in the first frame: 'r' random...
#define SPEED_MOD_RANDOMNESS 0  // Add randomnes to the speed module
#define SPEED_ANG_RANDOMNESS 0

#define INIT_SCALE 1            // GRID:WINDOW  ==>  1-> 1:1, 2-> 1:2, ... 

#define COLOR 15               // 0 b&w, 1 brain, 2 azzur, 3 green, 9 test 
                                // 10 void, 11 supnov, 12 firac, 13 spagh, 14 green, 15 test, 16 Bluefire
                                // 20 void, 21 supnov, 22 firac, 23 20test
#define FAT_ONE 1

#define IN_MOUSE_CHAR 'D'       //'V' Vortex, 'C' Circle, 'c' Circles, 'A' Attract, 'D' Deattract, 'N' New_Experiments 


// PARTICELLE FINI SI SEGUONO BENE

// DINAM
#define NUM_AGENTS 5000        // number of particle
#define AGENT_SIZE 0           // Size of the agents
#define IN_SPEED   10         // Speed of particle, 10 == 1

// SENSOR
#define IN_SENSE 2                   // Activate the sensor with 1, activate the avoiders with 2
#define SENSE_2  1                   // Every SENSE_2 paticle that follows the others, one will avoid others
#define SENSOR_RADIUS 1           // Sensor radius..
#define SENSOR_MODULE 7           // Sensors distance from agent
#define SENSOR_ANGLE M_PI/4       // Angle between sensors
#define SPEED_ANGLE M_PI/6       // Change of angle in the speed when something is detected
#define SPEED_ANGLE_RANDOMNESS 1  // Add randomnes to the speed angle

// IMAGE
#define IMAGE_SUBTRACTION 20    // Subrtact every time INCREMENT % IMAGE_SUBTRACTION == 1
#define SUB_VAL 1// Value that get subtracted
#define IMAGE_DIFFUSION 0       // Diffuse every time INCREMENT % IMAGE_DIFFUSION == 1
#define FILTER_DIM 1            // Set the dimension of the averaging filter
#define CENTER_WEIGHT 1         // Set how much the center weight in the final color of the pixel


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MOUSE
#define IN_MOUSE_ACTION 1       // 1 to have mouse action
#define MOVE 5       // 1 to have mouse action
#define DELAY 0                 // Delay between frames

#define SAVE_FRAME 0            // Will save the frames as .png in the folder 'frames'
                                //ffmpeg -framerate 30 -i frames/frame%d.png -c:v libx264 -s 1920x1080 -pix_fmt yuv420p -vf "fps=30,format=yuv420p" -r 60 -crf 18 -preset slow -c:a aac -strict experimental -y THE_GAME_OF_LIFE.mp4
                                // ffmpeg -framerate 60 -i frames/frame%d.png -c:v libx264 -pix_fmt yuv420p calls.mp4
#endif