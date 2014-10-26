
uniform sampler2D		u_Diffuse;
uniform sampler2D		u_NormalMap;
uniform samplerCube 	u_CubeFilterMap;
uniform sampler3D	 	u_attenMap;
uniform sampler2D		u_Caustics;

uniform float			u_ColorModulate;
uniform float			u_specularScale;
uniform float			u_specularExp;
uniform float			u_toksvigFactor;
uniform vec4 			u_LightColor;
uniform float 			u_LightRadius;
uniform int				u_fog;
uniform float       	u_CausticsModulate; 
uniform int				u_isCaustics;

varying vec3			v_viewVecTS;
varying vec3			v_lightVec;
varying vec2			v_texCoord;
varying vec4			v_CubeCoord;
varying vec4			v_lightCoord;
varying vec3			v_AttenCoord;
varying float			v_fogFactor;

#include lighting.inc
#include parallax.inc

void main ()
{

float attenMap = texture3D(u_attenMap, v_AttenCoord).r;

if(attenMap <= CUTOFF_EPSILON){
	discard;
		return;
}
 
vec3 V = normalize(v_viewVecTS);
vec3 L = normalize(v_lightVec);

#ifdef PARALLAX
vec2	P = CalcParallaxOffset(u_Diffuse, v_texCoord.xy, V);
vec4	diffuseMap = texture2D(u_Diffuse, P);
vec3	normalMap =  normalize(texture2D(u_NormalMap, P).rgb * 2.0 - 1.0);
float	specTmp = texture2D(u_NormalMap, P).a;
vec4	causticsMap = texture2D(u_Caustics, P);
#else
vec4	diffuseMap = texture2D(u_Diffuse,  v_texCoord.xy);
vec3	normalMap =  normalize(texture2D(u_NormalMap, v_texCoord.xy).rgb * 2.0 - 1.0);
float	specTmp = texture2D(u_NormalMap, v_texCoord).a;
vec4	causticsMap = texture2D(u_Caustics, v_texCoord.xy);
#endif

float specular = specTmp * u_specularScale;
specular /= mix(u_toksvigFactor, 1.0, specular);

// light filter
vec4 cubeFilter = textureCube(u_CubeFilterMap, v_CubeCoord.xyz);
cubeFilter *= 2;

if (u_isCaustics == 1){
vec4 tmp = causticsMap * diffuseMap;
tmp *= u_CausticsModulate;
diffuseMap = tmp + diffuseMap;
}

#ifdef AMBIENT   

gl_FragColor = diffuseMap * LambertLighting(normalMap, L) * u_LightColor * attenMap;

#else

vec2 Es = PhongLighting(normalMap, L, V, u_specularExp);

if(u_fog == 1){

vec4 color = (Es.x * diffuseMap + Es.y * specular) * cubeFilter * attenMap;
gl_FragColor = mix(u_LightColor, color, v_fogFactor) * attenMap;
}
else
gl_FragColor = (Es.x * diffuseMap + Es.y * specular) * u_LightColor * cubeFilter * attenMap;

#endif 
}
