layout (binding = 0) uniform sampler2D		u_bumpMap;
layout (binding = 1) uniform sampler2D		u_diffuseMap;
layout (binding = 2) uniform sampler2D		u_causticMap;
layout (binding = 3) uniform samplerCube	u_CubeFilterMap;

uniform float	u_LightRadius;
uniform float	u_specularScale;
uniform float	u_CausticsModulate;
uniform float	u_ColorModulate;
uniform vec4	u_LightColor;
uniform int		u_fog;
uniform float	u_fogDensity;
uniform int		u_isCaustics;
uniform int		u_isAmbient;

in vec2			v_texCoord;
in vec3			v_viewVec;
in vec3			v_lightVec;
in vec4			v_CubeCoord;
in vec4			v_positionVS;
in vec3			v_lightAtten;
in vec3			v_lightDirection;

#include lighting.inc

void main (void) {

	float attenMap = PointAttenuation(v_lightAtten, 2.0);

	if(attenMap <= CUTOFF_EPSILON){
		discard;
		return;
	}

	vec3 L = normalize(v_lightVec);
	vec3 V = normalize(v_viewVec);

	vec4 diffuseMap  = texture(u_diffuseMap, v_texCoord);
	vec4 normalMap =  texture(u_bumpMap, v_texCoord);
	normalMap.xyz *= 2.0;
	normalMap.xyz -= 1.0;
	
	float SSS = diffuseMap.a;

	vec4 cubeFilter = texture(u_CubeFilterMap, v_CubeCoord.xyz) * 2.0;

	if (u_isCaustics == 1){
		vec4 causticsMap = texture(u_causticMap, v_texCoord);
		vec4 tmp = causticsMap * diffuseMap * u_CausticsModulate;
		diffuseMap += tmp;
	}

	diffuseMap *= u_ColorModulate;

	if (u_isAmbient == 1) {
		fragData = diffuseMap * LambertLighting(normalize(normalMap.xyz), V) * u_LightColor * attenMap;
		return;
	}
	
	if (u_isAmbient == 0) {

  float specular = normalMap.a * u_specularScale;
  float roughness = diffuseMap.r * 0.45; 
  roughness = 1.0 - roughness;
  vec3 brdf =  Lighting_BRDF(diffuseMap.rgb, vec3(specular), roughness, normalize(normalMap.xyz), L, V);
  vec3 brdfColor = brdf * u_LightColor.rgb * cubeFilter.rgb;

		if(u_fog == 1) {  
			float fogCoord = abs(gl_FragCoord.z / gl_FragCoord.w);
			float fogFactor = exp(-u_fogDensity * fogCoord); //exp1
			//float fogFactor = exp(-pow(u_fogDensity * fogCoord, 2.0)); //exp2

			fragData = mix(u_LightColor, vec4(brdfColor, 1.0), fogFactor) * attenMap; // u_LightColor == fogColor

			return;
		}

		if(u_fog == 0) {

			if(SSS <= 0.00392){
					fragData = subScatterFS(V, L, normalize(normalMap.xyz), u_LightColor.rgb, diffuseMap * 0.5, attenMap, specular * 0.25) * cubeFilter;
					return;
			}
			else{	
				fragData.rgb = fragData.rgb =  brdfColor * attenMap; 
				fragData.a = 1.0;
			}
		}
	}        
}