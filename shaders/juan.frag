#version 450

#extension GL_GOOGLE_include_directive : require
#include "input_structures.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inWorldPos;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalTex, inUV).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(inWorldPos);
    vec3 Q2  = dFdy(inWorldPos);
    vec2 st1 = dFdx(inUV);
    vec2 st2 = dFdy(inUV);

    vec3 N   = normalize(inNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
vec4 shade(vec3 p, vec3 v, vec3 n)
{
    vec4 texSample = texture(colorTex, inUV);
    
    vec3 lightPosition = vec3(0.0, 1000.0, 0.0);
    vec3 albedo     = pow(texSample.rgb, vec3(2.2)) * materialData.colorFactors.xyz;
    float metallic  = materialData.metal_rough_factors.x;
    float roughness = materialData.metal_rough_factors.y;
    float ao = 1.0f;

     vec3 Ia = sceneData.ambientColor.xyz;
     vec3 Il = vec3(0.0);

	 vec3 l = normalize(lightPosition - p);
	 vec3 h = normalize(l + v);
	 vec3 r = reflect(-l, n);

	 float s_m = 2/pow(roughness, 4) - 2;
	 float cosNL = max(dot(n, l), 0.0);
	 float cosNH = max(dot(v, r), 0.0);

	 float intensity = 1.0f;

	 vec3 texColor = albedo;

	 vec3 Iambient = sceneData.sunlightColor.rgb * 0.03;

	 vec3 Idiffuse = cosNL * sceneData.sunlightColor.rgb;

	 vec3 Ispecular = pow(cosNH, s_m) * vec3(1.0);

	 Iambient *= texColor * intensity;

	 Idiffuse *= texColor * intensity;

	 Ispecular *= texColor;

	 vec3 phong = Iambient + Idiffuse + Ispecular;

	 Il += phong;
	 Il = clamp(Il, 0.0, 1.0);

     float alpha = texSample.a;
     return vec4(Ia + Il, alpha);
}
vec4 learnOpenGL(vec3 p, vec3 v, vec3 n)
{
    vec4 texSample = texture(colorTex, inUV);
    vec3 lightPosition = vec3(0.0, 1000.0, 0.0);

    vec3 albedo     = pow(texSample.rgb, vec3(2.2)) * materialData.colorFactors.xyz;
    float roughness = materialData.metal_rough_factors.y;

    vec3 lightDir = normalize(lightPosition - inWorldPos);
    // diffuse shading
    float diff = max(dot(n, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, n);
    float spec = pow(max(dot(v, reflectDir), 0.0), 2/pow(roughness, 4) - 2);
    // combine results
    vec3 ambient  = vec3(0.1) * albedo;
    vec3 diffuse  = vec3(1.0)  * diff * albedo;
    vec3 specular = vec3(1.0) * spec * vec3(1.0 - roughness);
    return vec4(ambient + diffuse + specular, texSample.a);
}
// ----------------------------------------------------------------------------
void main()
{	
//    vec3 N = getNormalFromMap();
    vec3 N = inNormal;
    vec3 V = normalize(sceneData.cameraPos.xyz - inWorldPos);

//    vec4 color = shade(inWorldPos, V, N);
    vec4 color = learnOpenGL(inWorldPos, V, N);

    outFragColor = color;
}