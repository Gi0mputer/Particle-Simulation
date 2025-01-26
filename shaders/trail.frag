#version 450 core

in vec2 vTexCoord;
out vec4 FragColor;

// Parametri
uniform sampler2D uTexture;      // input texture (frame precedente)
uniform vec2 uResolution;        // dimensioni in pixel (simWidth, simHeight)
uniform float uBlurStrength;     // 0..1, quantita' di blur
uniform float uFade;             // 0..1, quanto resta del colore
uniform vec4 uBgColor;           // colore sfondo

void main()
{
    // 1) Calcolo del blur 3x3
    vec2 pixelSize = 1.0 / uResolution;
    vec4 sum = vec4(0.0);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            sum += texture(uTexture, vTexCoord + pixelSize * vec2(i,j));
        }
    }
    vec4 blurColor = sum / 9.0;

    // 2) Recupero il colore originale (pixel center)
    vec4 originalColor = texture(uTexture, vTexCoord);

    // 3) Interpola tra colore originale e blur 
    // (uBlurStrength=1 -> 100% blur, 0 -> 100% original)
    vec4 mixBlur = mix(originalColor, blurColor, uBlurStrength);

    // 4) Fade verso background
    // (uFade=0 -> subito sfondo, uFade=1 -> rimane mixBlur)
    vec4 finalColor = mix(uBgColor, mixBlur, uFade);

    FragColor = finalColor;
}
