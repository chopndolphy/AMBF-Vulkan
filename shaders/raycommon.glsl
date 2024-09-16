struct hitPayload
{
    vec3 hitValue;
};

struct PushConstantRay
{
    vec4 clearColor;
    vec3 lightPosition[2];
    float lightIntensity;
};
