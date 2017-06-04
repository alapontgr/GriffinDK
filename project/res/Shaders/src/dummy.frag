#version 450

layout(binding = 0) uniform UniformBufferObject
{
	vec4 colour;
} ubo; 

layout(location = 0) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = ubo.colour * v_Color;
}