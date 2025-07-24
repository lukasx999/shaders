#version 330 core

uniform float u_time;
uniform vec2 u_resolution;

float plot(vec2 pos, float thickness, float wanted_y) {
    return smoothstep(wanted_y-thickness, wanted_y, pos.y) -
    smoothstep(wanted_y, wanted_y+thickness, pos.y);
}

void main() {
    vec2 pos = gl_FragCoord.xy / u_resolution;

    // vec3 color_line = vec3(1-pos.x*pos.y, 0, pos.x*pos.y);
    vec3 color_line = mix(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), pos.x*pos.y);

    float y = smoothstep(0.1, 0.9, pos.x);
    // float y = pow(pos.x, 5);
    // float y = (sin(pos.x*50)+1)/2 * (sin(u_time)+1)/2;

    float point = plot(pos, 0.05, y);

    // vec3 color_bg = vec3(y);
    vec3 color_bg = vec3(fract(pos.x*5)*fract(pos.y*5));
    vec3 color = (1-point)*color_bg + point*color_line;

    gl_FragColor = vec4(color, 1.0f);
}
