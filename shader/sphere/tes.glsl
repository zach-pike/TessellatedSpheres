#version 410 core

layout(quads) in;

out vec2 tesscoord;

uniform mat4 MVP;

patch in vec4 sphInfo;

vec4 interpolate(vec4 p0, vec4 p1, vec4 p2, vec4 p3, vec2 uv) {
    // Perform bilinear interpolation
    vec4 interp0 = mix(p0, p1, uv.x);
    vec4 interp1 = mix(p2, p3, uv.x);
    vec4 result = mix(interp0, interp1, uv.y);

    return result;
}

void main() {
    vec3 origin = sphInfo.xyz;
    float scale = sphInfo.w;

    vec4 pos = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.xy) + vec4(origin, 0);
    tesscoord = gl_TessCoord.xy;
    
    // Hack
    gl_Position = MVP * vec4(normalize(pos.xyz - origin) * scale + origin, 1);
}