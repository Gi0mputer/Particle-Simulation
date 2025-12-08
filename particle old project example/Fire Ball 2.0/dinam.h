#ifndef DINAM_H
#define DINAM_H

#define M_PI (3.14159265358979323846264338327950288)

// Dinamic agent
typedef struct {
    float x;
    float y;
    float speed_ang;
    float speed_mod;
} Agent_Struct;

typedef struct {
    float SL_x;
    float SL_y;
    float SC_x;
    float SC_y;
    float SR_x;
    float SR_y;
} Sensor_Struct;

// Flag to play this
extern Uint8 DINAM;
extern Uint8 SENSE;

// Dinam
extern Agent_Struct agents [];
extern int STARTING;

////////// DINAM FUNCTION //////////

void initialize_dinam(char);
void play_dinam();
void resize_dinam(int, int);
void border(Agent_Struct*);
void border1(Agent_Struct*);
void bounce(Agent_Struct*);
void sense(Agent_Struct*);
void sense2(Agent_Struct*);
void update_position();
void update_speed(char);

double random_sign();

#endif