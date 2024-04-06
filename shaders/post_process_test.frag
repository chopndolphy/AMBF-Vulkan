#version 460

 layout(location = 0) in INTERFACE {
	vec2 uv; ///< UV coordinates.
} In ;

layout(set = 0, binding = 0) uniform texture2D screenTexture; ///< Image to filter.
layout(set = 0, binding = 1) uniform sampler sClampLinear;

layout(set = 0, binding = 2) uniform UniformBlock {
	vec2 inverseScreenSize; ///< Size of one-pixel in UV space.
};

// Settings for FXAA.
#define EDGE_THRESHOLD_MIN 0.0312
#define EDGE_THRESHOLD_MAX 0.125
#define QUALITY(q) ((q) < 5 ? 1.0 : ((q) > 5 ? ((q) < 10 ? 2.0 : ((q) < 11 ? 4.0 : 8.0)) : 1.5))
#define ITERATIONS 12
#define SUBPIXEL_QUALITY 0.75

// Output: the fragment color
layout(location = 0) out vec3 fragColor; ///< Color.

float rgb2luma(vec3 rgb){
	return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

void main() { 
	fragColor = texture(sampler2D(screenTexture, sClampLinear), In.uv).rgb;

}
