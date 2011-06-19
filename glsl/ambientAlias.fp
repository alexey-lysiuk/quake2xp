uniform sampler2D 	u_Diffuse;
uniform sampler2D 	u_Add;

#ifdef CAUSTICS
uniform sampler2D  u_Caustics;
uniform float      u_CausticsModulate;
#endif

uniform float       u_ColorModulate;  
uniform float       u_AddShift; 


void main ()
{

vec4 r0 = texture2D(u_Diffuse,  gl_TexCoord[0].xy);
vec4 r1 = texture2D(u_Add,      gl_TexCoord[0].xy);

#ifdef CAUSTICS
vec4 r2 = texture2D(u_Caustics, gl_TexCoord[0].xy);
vec4 tmp;
#endif

vec4 color;

r0 *= gl_Color;


r1 *= u_AddShift;

color = r0+r1;

#ifdef CAUSTICS
tmp = r2 * color;
tmp *= u_CausticsModulate;
color = tmp + color;
#endif

gl_FragColor = color * u_ColorModulate;

}
