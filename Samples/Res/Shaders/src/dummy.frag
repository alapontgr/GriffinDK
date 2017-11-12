#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
	mat4 View;
	mat4 Projection;
	mat4 ViewProjection;
	vec4 colour;
} FDSceneParams; 


layout(location = 0) in vec4 v_Color;
layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = v_Color + vec4(0.5f, 0.5f, 0.5f, 1.0f);
}