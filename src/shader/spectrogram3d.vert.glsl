#version 120

uniform sampler2D texture;
attribute int is_spectrogram_vertex;
varying float magnitude;

void main()
{
    vec2 tex_coord = gl_MultiTexCoord0.xy;

    // TODO: Proper log scaling!
    tex_coord.x = pow(tex_coord.x, 3.0);

    vec4 position = gl_Vertex;
    //if (is_spectrogram_vertex > 0)
    if (true)
    {
        magnitude = texture2D(texture, tex_coord).x;
        position.z = magnitude * 1.3;
    }

    gl_Position = gl_ModelViewProjectionMatrix * position;
}
