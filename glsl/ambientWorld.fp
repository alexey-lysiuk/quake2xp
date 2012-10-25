
uniform sampler2D		u_Diffuse;
uniform sampler2D		u_LightMap;
uniform sampler2D		u_Add;
uniform sampler2D		u_NormalMap;
uniform sampler2D		u_deluxMap;
uniform sampler2D		u_Caustics;
uniform float       		u_ColorModulate;
uniform float       		u_ambientScale;    
uniform float       		u_CausticsModulate; 
uniform int			u_isCaustics;
uniform float			u_specularScale;
uniform float			u_specularExp;

varying vec3			v_viewVecTS;
varying vec3			t, b, n;
varying vec2			v_wTexCoord;
varying vec2			v_lTexCoord;
varying vec4			v_color;

 
#include parallax.inc
#include lighting.inc

void main ()
{
vec3 V = normalize(v_viewVecTS);
vec4 lightMap = texture2D(u_LightMap, v_lTexCoord.xy); 
vec3 wDelux = normalize(texture2D(u_deluxMap, v_lTexCoord).rgb - 0.5);
vec4 diffuseMap;
vec4 glowMap;
vec4 causticsMap;
vec3 normalMap;
float specTmp;
vec3 tbnDelux;
vec4 bumpLight;

#ifdef PARALLAX
vec2 P = CalcParallaxOffset(u_Diffuse, v_wTexCoord.xy, V);
diffuseMap = texture2D(u_Diffuse, P);
glowMap = texture2D(u_Add, P);
causticsMap = texture2D(u_Caustics, P);
normalMap =  normalize(texture2D(u_NormalMap, P.xy).rgb - 0.5);
specTmp = texture2D(u_NormalMap,   P.xy).a;

#else

diffuseMap = texture2D(u_Diffuse,  v_wTexCoord.xy);
glowMap = texture2D(u_Add,  v_wTexCoord.xy);
causticsMap = texture2D(u_Caustics, v_wTexCoord.xy);
normalMap =  normalize(texture2D(u_NormalMap, v_wTexCoord.xy).rgb - 0.5);
specTmp = texture2D(u_NormalMap, v_wTexCoord).a;

#endif 

vec4 specular = vec4(specTmp) * u_specularScale;


#ifdef VERTEXLIGHT
diffuseMap *= clamp(v_color, 0.0, 0.666);
#endif

// Bump World 
#ifdef BUMP

#ifdef VERTEXLIGHT
tbnDelux.x = abs(dot(n, t));
tbnDelux.y = abs(dot(n, b));
tbnDelux.z = 1.0;
vec2 Es = PhongLighting(normalMap, tbnDelux, V, u_specularExp);

#else

//Put delux into tangent space
tbnDelux.x = dot(wDelux, t);
tbnDelux.y = dot(wDelux, b);
tbnDelux.z = abs(dot(wDelux, n));

tbnDelux = clamp(tbnDelux, 0.4, 1.0);
vec2 Es = PhongLighting(normalMap, tbnDelux, V, u_specularExp);

#endif

#ifdef LIGHTMAP
bumpLight = (Es.x * diffuseMap) + (Es.y * specular * lightMap);
#endif

#ifdef VERTEXLIGHT
bumpLight = (Es.x * diffuseMap * v_color) + (Es.y * specular * v_color); //via lava surfaces 
#endif

diffuseMap *= u_ambientScale;
diffuseMap += bumpLight;

#ifdef LIGHTMAP
diffuseMap *= lightMap;
#endif

vec4 finalColor = diffuseMap + glowMap;

if (u_isCaustics == 1){
vec4 tmp;
tmp = causticsMap * finalColor;
tmp *= u_CausticsModulate;
finalColor = tmp + finalColor;
}

#else
// Non bump World
#ifdef LIGHTMAP
diffuseMap *= lightMap;
#endif

vec4 finalColor = diffuseMap + glowMap;

if (u_isCaustics == 1){
vec4 tmp;
tmp = causticsMap * finalColor;
tmp *= u_CausticsModulate;
finalColor = tmp + finalColor;
}

#endif

gl_FragColor = vec4(finalColor.rgb, 1.0) * u_ColorModulate;

}
