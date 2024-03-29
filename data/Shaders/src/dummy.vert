#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
} GfScene;

layout(location = 0) out vec2 uvs;

vec2 positions[6] = vec2[](
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, -0.5),

    vec2(0.5, -0.5),
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5)    
);

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),

    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 1.0, 0.0)
);

vec2 pUVs[6] = vec2[](
    vec2(0.0, 2.0),
    vec2(2.0, 2.0),
    vec2(2.0, 0.0),

    vec2(2.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 2.0)
);

void main() {
    gl_Position = GfScene.ViewProjection * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    uvs = pUVs[gl_VertexIndex];//colors[gl_VertexIndex];
}