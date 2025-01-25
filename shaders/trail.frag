#version 450 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;   // frame precedente
uniform float uDecay;         // fattore di dissolvenza (es. 0.95, 0.99, 1.0 = no fade)

void main()
{
    vec4 col = texture(uTexture, vTexCoord);
    // Se vuoi scurire leggermente per creare “scia”
    col *= uDecay; 
    FragColor = col;
}
