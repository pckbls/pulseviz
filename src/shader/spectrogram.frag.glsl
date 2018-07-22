#version 120

varying vec2 tex_coord;

uniform sampler2D texture;
uniform sampler1D palette;

uniform float coord_coeff; // TODO: Find a better name.
uniform float freq_min, freq_max;

void main(void)
{
    vec2 texcoord = tex_coord;

    texcoord.x = exp(texcoord.x * coord_coeff) * freq_min;
    texcoord.x = (texcoord.x - freq_min) / (freq_max - freq_min);

    // TODO: Is this check really correct?
    if (texcoord.x >= 1.0 || texcoord.x < 0.0)
    {
        // This should not happen!
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);

        // gl_FragColor = texture1D(palette, texture2D(texture, texcoord));

        float magnitude = texture2D(texture, texcoord).x;
        if (magnitude < 0.0 || magnitude > 1.0)
        {
            // This should not happen
            gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
        else
            gl_FragColor = texture1D(palette, magnitude);
    }
}
