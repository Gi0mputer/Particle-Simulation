#version 450 core
out vec4 FragColor;
uniform sampler2D uTexture; // Texture precedente per blending

void main() {
    // Debug: Cambia il colore per verificare il funzionamento
    FragColor = vec4(gl_PointCoord, 0.5, 1.0); 
}
