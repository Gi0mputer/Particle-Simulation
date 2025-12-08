#ifndef IMAGE_H
#define IMAGE_H
 
extern uint8_t** gray;
extern Uint8* rgba;

extern float DT;
extern int INCREMENT;

extern float PROPORTION;
extern float REST; 

extern Uint8 SCALE;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;

////////// IMAGE FUNCTION //////////
void averaging_filter();
void initialize_image();
void resize_image();
void resize_image2();
void cleanup_image();

void rgba_convert();

// Image Style
void averaging_filter();

void update_image();

void move_image(char);

void add_to_image(char);

#endif