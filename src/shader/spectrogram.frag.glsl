#version 120

varying vec2 tex_coord;

uniform sampler2D texture;
uniform sampler1D palette;

void main(void)
{
    vec2 texcoord = tex_coord;

    // TODO: Proper log scaling!
    texcoord.x = pow(texcoord.x, 2.0);

    // TODO: Is this check really correct?
    if (texcoord.x >= 1.0)
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
