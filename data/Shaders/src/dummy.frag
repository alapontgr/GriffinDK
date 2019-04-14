#version 450

layout(set = 0, binding=0) uniform sampler2D Albedo;

layout(location = 0) in vec2 uvs;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(Albedo, uvs);
}