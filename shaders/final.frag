#version 450 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uTime;
uniform int   uColorMode; // 0=Original, 1=Legacy, 2=Neon, 3=Bismuth, 4=Psycho
uniform vec3  uColor1;
uniform vec3  uColor2;
uniform float uNeonSpeed;
uniform float uNeonRange;

// Helper: HSV to RGB
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// IQ Palette
vec3 palette( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d ) {
    return a + b*cos( 6.28318*(c*t+d) );
}

void main()
{
    vec4 texVal = texture(uTexture, vTexCoord);
    
    // Global Density Estimation (using Red channel)
    float density = texVal.r; 
    
    if (uColorMode == 0) {
        // Standard
        FragColor = texVal;
    } 
    else if (uColorMode == 1) {
        // Legacy Supernova (Fire Ball 2.0)
        float g = density * 255.0; 
        if (g < 1.0) {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        } else {
            float term1 = sqrt(max(g * 0.05 - 1.0, 0.0)) * 0.05;
            float term2 = 0.71 + 0.01 * cos(0.1 * g + uTime * uNeonSpeed);
            float h = min(term1, term2);
            vec3 rgb = hsv2rgb(vec3(h, 1.0, 1.0));
            FragColor = vec4(rgb, 1.0);
        }
    }
    else if (uColorMode == 2) {
        // Neon Thermal (Parametric)
        float speed = uNeonSpeed * 3.0; // Scale speed for neon
        float range = uNeonRange * 50.0; // Scale range
        
        float rings = sin(density * range - uTime * speed);
        vec3 base = mix(uColor1, uColor2, density);
        vec3 col = base + rings * 0.2;
        
        // Intensity mapping
        col *= (density * 5.0); 
        FragColor = vec4(col, 1.0);
    }
    else if (uColorMode == 3) {
        // Bismuth / Iridescent
        // Uses IQ palette based on density + time
        float t = density + uTime * uNeonSpeed * 0.1;
        
        // Palette: Rainbow-ish
        vec3 a = vec3(0.5, 0.5, 0.5);
        vec3 b = vec3(0.5, 0.5, 0.5);
        vec3 c = vec3(1.0, 1.0, 1.0);
        vec3 d = vec3(0.00, 0.33, 0.67);
        
        vec3 col = palette(t * uNeonRange * 5.0, a, b, c, d);
        FragColor = vec4(col * density, 1.0);
    }
     else if (uColorMode == 4) {
        // Psycho (Overflow)
        // Simulate int8 overflow cyclic wrapping
        float val = density * 255.0;
        float cyclic = fract((val + gl_FragCoord.x * 0.1 + uTime * uNeonSpeed * 100.0) / 255.0 * uNeonRange * 5.0);
        vec3 col = hsv2rgb(vec3(cyclic, 1.0, 1.0));
        FragColor = vec4(col * density, 1.0);
    }
    else {
        FragColor = texVal;
    }
}
