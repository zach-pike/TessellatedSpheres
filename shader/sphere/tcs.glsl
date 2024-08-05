#version 410 core

layout(vertices = 4) out;

in vec4 sphereData[];

patch out vec4 sphInfo;

void main() {
    // per vertex code
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Same for all verts in a patch
    gl_TessLevelOuter[0] = 4.0;
    gl_TessLevelOuter[1] = 4.0;
    gl_TessLevelOuter[2] = 4.0;
    gl_TessLevelOuter[3] = 4.0;

    gl_TessLevelInner[0] = 4.0;
    gl_TessLevelInner[1] = 4.0;

    sphInfo = sphereData[gl_InvocationID];
}