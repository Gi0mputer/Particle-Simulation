#version 450 core

layout(location = 0) in vec2 aPos;

// In “modalità semplice” assumiamo che le coordinate delle particelle
// siano già “in clip space” (ovvero nell’intervallo [-1..1] in X e Y).
// Quindi NON facciamo trasformazioni, le passiamo direttamente:

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
}
