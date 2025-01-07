#version 450 core
layout(location = 0) in vec2 aPos; // Posizione particella
uniform vec2 uScreenSize;          // Dimensione schermo

void main() {
    vec2 normalizedPos = (aPos / uScreenSize) * 2.0 - 1.0; // Normalizza in [-1, 1]
    gl_Position = vec4(normalizedPos, 0.0, 1.0);
    gl_PointSize = 2.0; // Dimensione punti traccia
}
