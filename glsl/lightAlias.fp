uniform sampler2D		u_bumpMap;
uniform sampler2D		u_diffuseMap;
uniform sampler2D       u_causticMap;
uniform samplerCube 	u_CubeFilterMap;
uniform sampler3D	 	u_attenMap;

uniform float           u_LightRadius;
uniform float			u_specularScale;
uniform float			u_toksvigFactor;
uniform float			u_CausticsModulate;
uniform vec4			u_LightColor;
uniform int				u_fog;
uniform int				u_isCaustics;

varying vec2			v_texCoord;
varying vec3			v_viewVec;
varying vec3			v_lightVec;
varying vec4			v_CubeCoord;
varying vec3			v_AttenCoord;
varying float			v_fogFactor;

#include lighting.inc

void main(){

float attenMap = texture3D(u_attenMap, v_AttenCoord).r;

if(attenMap <= CUTOFF_EPSILON){
	discard;
		return;
}

// compute the light vector
vec3 L = normalize(v_lightVec);
// compute the view vector
vec3 V = normalize(v_viewVec);

vec3 normalMap =  normalize(texture2D(u_bumpMap, v_texCoord).rgb * 2.0 - 1.0);
float tmp = texture2D(u_bumpMap, v_texCoord.xy).a;

float specular = tmp * u_specularScale;
specular /= mix(u_toksvigFactor, 1.0, specular);

vec4 diffuseMap  = texture2D(u_diffuseMap, v_texCoord.xy);
vec4 cubeFilter = textureCube(u_CubeFilterMap, v_CubeCoord.xyz);
cubeFilter *= 2;

if (u_isCaustics == 1){
vec4 causticsMap = texture2D(u_causticMap, v_texCoord.xy);
vec4 tmp = causticsMap * diffuseMap;
tmp *= u_CausticsModulate;
diffuseMap = tmp + diffuseMap;
}

#ifdef AMBIENT
gl_FragColor = diffuseMap * LambertLighting(normalMap, V) * u_LightColor * attenMap;

#else

vec2 Es = PhongLighting(normalMap, L, V, 16.0);

if(u_fog == 1){

vec4 color = (Es.x * diffuseMap + Es.y * specular) * cubeFilter * attenMap;
gl_FragColor = mix(u_LightColor, color, v_fogFactor) * attenMap;
}
else
gl_FragColor = (Es.x * diffuseMap + Es.y * specular) * u_LightColor * cubeFilter * attenMap;

#endif
}
