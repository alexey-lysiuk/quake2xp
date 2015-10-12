uniform sampler2D		u_Diffuse;
uniform sampler2D		u_NormalMap;
uniform samplerCube		u_CubeFilterMap;
uniform sampler3D	 	u_attenMap;
uniform sampler2D		u_Caustics;
uniform sampler2D		u_csmMap;

uniform float			u_ColorModulate;
uniform float			u_specularScale;
uniform float			u_specularExp;
uniform vec4 			u_LightColor;
uniform float 			u_LightRadius;
uniform int				u_fog;
uniform float			u_fogDensity;
uniform float			u_CausticsModulate; 
uniform int				u_isCaustics;
uniform int				u_isAmbient;

varying vec3			v_tbn[3];
varying vec3			v_positionVS;
varying vec3			v_viewVecTS;
varying vec3			v_lightVec;
varying vec2			v_texCoord;
varying vec4			v_CubeCoord;
varying vec4			v_lightCoord;
varying vec4			v_AttenCoord;

#include lighting.inc
#include parallax.inc

void main (void) {
	float attenMap = texture3D(u_attenMap, v_AttenCoord.xyz).r;

	if(attenMap <= CUTOFF_EPSILON){
		discard;
		return;
	}
 
	vec3	V = normalize(v_viewVecTS);
	vec3	L = normalize(v_lightVec);
	vec2  P = CalcParallaxOffset(u_Diffuse, v_texCoord, V);

	vec4 diffuseMap = texture2D(u_Diffuse,  P);
	vec4 normalMap =  texture2D(u_NormalMap, P);
	normalMap.xyz *= 2.0;
	normalMap.xyz -= 1.0;

	vec4 causticsMap = texture2D(u_Caustics, P);

	// light filter
	vec4 cubeFilter = textureCube(u_CubeFilterMap, v_CubeCoord.xyz) * 2.0;

	if (u_isCaustics == 1) {
		vec4 tmp = causticsMap * diffuseMap;
		tmp *= u_CausticsModulate;
		diffuseMap = tmp + diffuseMap;
	}

	if(u_isAmbient == 1) {
		gl_FragColor = diffuseMap * LambertLighting(normalize(normalMap.xyz), L) * u_LightColor * attenMap;
		return;
	}

	if(u_isAmbient == 0) {
		float specular = normalMap.a * u_specularScale;
		vec2 Es = PhongLighting(normalize(normalMap.xyz), L, V, u_specularExp);

		if(u_fog == 1) {
			float fogCoord = abs(gl_FragCoord.z / gl_FragCoord.w); // = gl_FragCoord.z / gl_FragCoord.w;
			float fogFactor = exp(-u_fogDensity * fogCoord); //exp1
			//float fogFactor = exp(-pow(u_fogDensity * fogCoord, 2.0)); //exp2

			vec4 color = (Es.x * diffuseMap + Es.y * specular) * u_LightColor * cubeFilter * attenMap;
			gl_FragColor = mix(u_LightColor, color, fogFactor) * attenMap;  // u_LightColor == fogColor
			return;
		}

		if(u_fog == 0)
			gl_FragColor = (Es.x * diffuseMap + Es.y * specular) * u_LightColor * cubeFilter * attenMap;
	} 
}