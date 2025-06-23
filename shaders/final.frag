#version 450 core

in vec2 vTexCoord;
out vec4 FragColor;

// Sampler per la texture finale (unità 0)
uniform sampler2D uTexture;

void main()
{
    // Semplice campionamento della texture finale
    FragColor = texture(uTexture, vTexCoord);
}
