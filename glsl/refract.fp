layout (binding = 0) uniform sampler2D		u_deformMap;
layout (binding = 1) uniform sampler2D		u_colorMap;
layout (binding = 2) uniform sampler2DRect	g_colorBufferMap;
layout (binding = 3) uniform sampler2DRect	g_depthBufferMap;

layout(location = U_REFR_ALPHA)			uniform float	u_alpha;
layout(location = U_REFR_THICKNESS0)	uniform float	u_thickness0; //depth feather
layout(location = U_REFR_THICKNESS1)	uniform float	u_thickness1; //sprite softeness
layout(location = U_SCREEN_SIZE)		uniform vec2	u_viewport;
layout(location = U_DEPTH_PARAMS)		uniform vec2	u_depthParms;
layout(location = U_COLOR_MUL)			uniform float	u_ambientScale;
layout(location = U_REFR_MASK)			uniform vec2	u_mask;			//softeness
layout(location = U_REFR_ALPHA_MASK)	uniform int		u_ALPHAMASK;	//is sprite

in float	v_depth;
in float	v_depthS;
in vec2		v_deformMul;
in vec2		v_deformTexCoord;

#include depth.inc

void main (void) {

	vec2 N = texture(u_deformMap, v_deformTexCoord).xy * 2.0 - 1.0;
	vec4 diffuse  = texture(u_colorMap,  v_deformTexCoord.xy);

	// Z-feather
	float depth = DecodeDepth(texture2DRect(g_depthBufferMap, gl_FragCoord.xy).x, u_depthParms);
	N *= clamp((depth - v_depth) / u_thickness0, 0.0, 1.0);
	// scale by the deform multiplier and the viewport size
	N *= v_deformMul * u_viewport.xy;
	
	if(u_ALPHAMASK == 1){
		float A = texture(u_deformMap, v_deformTexCoord).a;
		float softness = clamp((depth - v_depthS) / u_thickness1, 0.0, 1.0);
		// refracted sprites with soft edges
		if (A <= 0.01) {
			discard;
				return;
			}
		N *= A;
		N *= softness;
		vec3 deform = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N).xyz;
		diffuse *= A;
		fragData = (vec4(deform, 1.0) + diffuse) * A;
		fragData *= mix(vec4(1.0), vec4(softness), u_mask.xxxy);
	
		fragData.r = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N * 0.425).r;
		fragData.g = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N * 0.5).g;
		fragData.b = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N * 0.575).b;
	return;
	}

	// world refracted surfaces
	// chromatic aberration approximation

	fragData.r = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N * 0.85).r;
	fragData.g = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N * 1.00).g;
	fragData.b = texture2DRect(g_colorBufferMap, gl_FragCoord.xy + N * 1.15).b;
	// blend glass texture
	diffuse.rgb *= u_ambientScale;
	fragData.xyz += diffuse.xyz * u_alpha;
  fragData.w = 1.0;
}
