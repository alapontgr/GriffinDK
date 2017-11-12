#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
	vec4 colour;
} test_cb; 


layout(location = 0) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = test_cb.colour * v_Color;
}