#ifndef RENDER_H
#define RENDER_H

///////// RENDER /////////
extern SDL_Renderer *renderer;
extern SDL_Window *window;
extern SDL_Texture* texture;
extern SDL_Color color;

// Hsv color
typedef struct {
    double h;
    double s;
    double v;
} Hsv_Struct;
extern Hsv_Struct HSV;

// Input
extern SDL_Event event;
extern SDL_Keycode keyPressed;
extern int WINDOW_CHANGE;
extern int WINDOW_FULL;
extern int MOUSE_X, MOUSE_Y;
extern Uint32 MOUSE_STATE;
extern Uint8 MOUSE_CLICKED;

// Arrow pos
typedef struct {
    int x;
    int y;
} Arrow_Pos_Struct;
extern Arrow_Pos_Struct ARROW_POS;

// Conditions
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int FRAME;
extern Uint8 SAVE;
extern Uint8 RUNNING;
extern Uint8 PAUSE;

////////// FUNCTION DEFINITIONS //////////
void initialize_render();
void initialize_save();
void process_event();
void draw_render();
void cleanup_render();
void resize_texture();

void saveFrame(int);

void drawFilledCircle(int centerX, int centerY, int radius, SDL_Color color);
void hsvToRgb(double, double, double, Uint8* , Uint8*, Uint8*);

#endif