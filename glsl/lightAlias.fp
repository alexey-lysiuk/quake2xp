uniform sampler2D		u_bumpMap;
uniform sampler2D		u_diffuseMap;
uniform samplerCube 	u_CubeFilterMap;
uniform sampler3D	 	u_attenMap;

uniform float           u_LightRadius;
uniform vec3			u_LightColor;

varying vec2			v_texCoord;
varying vec3			v_viewVec;
varying vec3			v_lightVec;
varying vec4			v_CubeCoord;
varying vec3			v_AttenCoord;

#include lighting.inc

void main(){

// compute the light vector
vec3 L = normalize(v_lightVec);
// compute the view vector
vec3 V = normalize(v_viewVec);

float density = 0.008;
const float LOG2 = 1.442695;

float z = gl_FragCoord.z / gl_FragCoord.w;
float fogFactor = exp2(-density * density * z * z * LOG2);

vec3 N =  normalize(texture2D(u_bumpMap, v_texCoord).rgb * 2.0 - 1.0);
float tmp = texture2D(u_bumpMap,   v_texCoord.xy).a;
vec4 specular = vec4(tmp);
vec4 diffuse  = texture2D(u_diffuseMap,  v_texCoord.xy);
vec4 cubeFilter = textureCube(u_CubeFilterMap, v_CubeCoord.xyz);
cubeFilter *= 2;

vec4 u_attenMap = texture3D(u_attenMap ,v_AttenCoord);

#ifdef AMBIENT

vec4 ambient = u_attenMap * (N.z * N.z);
vec4 temp = diffuse * ambient * vec4(u_LightColor, 1.0);

gl_FragColor = diffuse * ambient * vec4(u_LightColor, 1);

gl_FragColor = (mix(vec4(u_LightColor, 1), temp, fogFactor)) *u_attenMap;
#else

vec2 E = PhongLighting(N, L, V, 16.0);
gl_FragColor = (E.x * diffuse + E.y * specular) * cubeFilter * u_attenMap * vec4(u_LightColor, 1);

#endif
}
