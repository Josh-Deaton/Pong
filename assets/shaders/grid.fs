#version 330 core
out vec4 FragColor;

uniform vec4 COLOR;
uniform vec2 OFFSET;

void main()
{
    float gridSpacing = 32.0;
    float lineWidth = 1.0;

    vec2 coord = gl_FragCoord.xy - OFFSET;
    vec2 m = abs(mod(coord, gridSpacing) - gridSpacing * 0.5);
    float dist = min(m.x, m.y);

    float line = 1.0 - smoothstep(lineWidth, lineWidth + 1.0, dist);

    vec4 bg = vec4(0.2, 0.3, 0.3, 1.0);
    FragColor = mix(bg, COLOR, line);
}
