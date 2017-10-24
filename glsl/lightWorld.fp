layout (binding = 0) uniform sampler2D		u_Diffuse;
layout (binding = 1) uniform sampler2D		u_NormalMap;
layout (binding = 2) uniform samplerCube	u_CubeFilterMap;
layout (binding = 3) uniform sampler2D		u_Caustics;
layout (binding = 4) uniform sampler2D		u_RghMap;

uniform float	u_ColorModulate;
uniform float	u_specularScale;
uniform float	u_roughnessScale;
uniform vec4 	u_LightColor;
uniform float 	u_LightRadius;
uniform int		u_fog;
uniform float	u_fogDensity;
uniform float	u_CausticsModulate; 
uniform int		u_isCaustics;
uniform int		u_isAmbient;
uniform int		u_isRgh;
uniform int		u_autoBump;
uniform int		u_spotLight;
uniform vec3	u_spotParams;
uniform vec2	u_autoBumpParams; // x - bump scale y - specular scale

in vec3			v_positionVS;
in vec3			v_viewVecTS;
in vec3			v_lightVec;
in vec2			v_texCoord;
in vec4			v_CubeCoord;
in vec4			v_lightCoord;
in vec3			v_lightAtten;
in vec3			v_lightSpot;

#include lighting.inc
#include parallax.inc

void main (void) {

	float attenMap = PointAttenuation(v_lightAtten, 2.0);

	if(u_spotLight == 1)
		attenMap *= ConeAttenuation(v_lightSpot, u_spotParams);
  
	if(attenMap <= CUTOFF_EPSILON){
		discard;
		return;
	}

	vec3	V = normalize(v_viewVecTS);
	vec3	L = normalize(v_lightVec);

	vec4 diffuseMap;
	vec4 normalMap;
	vec2 texCoord = v_texCoord;

	if(u_autoBump == 0){
		texCoord = CalcParallaxOffset(u_Diffuse, v_texCoord, V);
		diffuseMap = texture(u_Diffuse,  texCoord);
		normalMap.rgb =  normalize(texture(u_NormalMap, texCoord).rgb * 2.0 - 1.0);

	}

	if(u_autoBump == 1){
		diffuseMap = texture(u_Diffuse,  v_texCoord);	
		normalMap = Height2Normal(v_texCoord, u_Diffuse, diffuseMap.rgb, u_autoBumpParams.x, u_autoBumpParams.y);
	}

	// light filter
	vec4 cubeFilter = texture(u_CubeFilterMap, v_CubeCoord.xyz) * 2.0;

	if (u_isCaustics == 1) {
		vec4 causticsMap = texture(u_Caustics, texCoord);
		vec4 tmp = causticsMap * diffuseMap;
		tmp *= u_CausticsModulate;
		diffuseMap = tmp + diffuseMap;
	}

	diffuseMap *= u_ColorModulate;

	if(u_isAmbient == 1) {
		fragData = diffuseMap * LambertLighting(normalMap.xyz, L) * u_LightColor * attenMap;
		return;
	}

	if(u_isAmbient == 0) {
		
		float roughness;
		float specular = texture(u_NormalMap, texCoord).a * u_specularScale;
    
		if(u_isRgh == 1){
			roughness = texture(u_RghMap, texCoord).r * u_roughnessScale;
		}
    if(u_isRgh != 1)
		{
		roughness = 1.0 - diffuseMap.r;
		roughness = clamp(roughness, 0.1, 1.0);
		}

		vec3 brdf =  Lighting_BRDF(diffuseMap.rgb, vec3(specular), roughness, normalMap.xyz, L, V);
		vec3 brdfColor = brdf * u_LightColor.rgb;
          
		if(u_fog == 1) {  
			float fogCoord = abs(gl_FragCoord.z / gl_FragCoord.w); // = gl_FragCoord.z / gl_FragCoord.w;
			float fogFactor = exp(-u_fogDensity * fogCoord); //exp1

			fragData = mix(u_LightColor, vec4(brdfColor, 1.0), fogFactor) * attenMap;  // u_LightColor == fogColor
			return;
		}
     
		if(u_fog == 0) { 
		fragData.rgb =  brdfColor  * cubeFilter.rgb * attenMap; 
		fragData.a = 1.0;
     }
  }	
}