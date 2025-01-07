#version 450 core
layout(location = 0) in vec2 aPos; // Posizione delle particelle
uniform vec2 uScreenSize;          // Dimensione dello schermo

void main() {
    vec2 normalizedPos = (aPos / uScreenSize) * 2.0 - 1.0; // Mappa in [-1, 1]
    gl_Position = vec4(normalizedPos, 0.0, 1.0); // Mappa in [-1, 1]
    gl_PointSize = 1.0;
}
