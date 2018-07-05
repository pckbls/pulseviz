#version 120

varying float magnitude;

void main()
{
    magnitude = gl_MultiTexCoord0.x;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
