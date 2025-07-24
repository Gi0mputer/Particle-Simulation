#version 450 core

in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;

uniform usampler2D uTrail;

vec3 hsv2rgb(vec3 c){
    vec3 rgb = clamp( abs(mod(c.x*6.0 + vec3(0,4,2),6.0)-3.0)-1.0, 0.0, 1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}

void main(){
    float v = uintBitsToFloat(texture(uTrail, vTexCoord).r);
    v = clamp(v, 0.0, 1.0);
    vec3 col = hsv2rgb(vec3(0.66 - v*0.66, 1.0, v));
    FragColor = vec4(col, 1.0);
}
