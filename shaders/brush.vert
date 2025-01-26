#version 450 core

layout(location = 0) in vec2 aPos;   // posizione (x,y) in [0..simWidth, 0..simHeight]
uniform vec2 uSimSize;              // dimensioni della simulazione

void main()
{
    // Converti (x,y) da [0..simWidth, 0..simHeight] in clip-space [-1..+1]
    float nx = (aPos.x / uSimSize.x) * 2.0 - 1.0;
    float ny = (aPos.y / uSimSize.y) * 2.0 - 1.0;
    gl_Position = vec4(nx, ny, 0.0, 1.0);

    // Per disegnare punti più grandi (se vuoi), ad es:
    gl_PointSize = 0.1;
}
