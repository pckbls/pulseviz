#version 120

varying float magnitude;

uniform sampler1D palette;

void main(void)
{
    // gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);
    // gl_FragColor = texture1D(palette, texture2D(texture, texcoord));

    if (magnitude < 0.0 || magnitude > 1.0)
    {
        // This should not happen
        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else
        gl_FragColor = texture1D(palette, magnitude);
}
