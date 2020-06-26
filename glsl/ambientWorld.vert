//!#include "include/global.inc"
layout(location = 0) in vec3	att_position;
layout(location = 1) in vec3	att_normal;
layout(location = 2) in vec3	att_tangent;
layout(location = 3) in vec3	att_binormal;
layout(location = 5) in vec2	att_texCoordDiffuse;
layout(location = 6) in vec2	att_texCoordLightMap;

layout (location = U_VIEW_POS)		uniform vec3		u_viewOrg;
layout (location = U_SCROLL)		uniform float		u_scroll;
layout (location = U_PARALLAX_TYPE)	uniform int			u_parallaxType;
layout (location = U_MVP_MATRIX)	uniform mat4		u_modelViewProjectionMatrix;

out vec3		v_viewVecTS;
out vec2		v_wTexCoord;
out vec2		v_lTexCoord;

void main (void) {
	// setup tex coords
	v_wTexCoord = att_texCoordDiffuse;  // diffuse, additive, caustics
	v_wTexCoord.x += u_scroll;

	v_lTexCoord = att_texCoordLightMap; // lightmap only

	// calculate tangent space view vector for parallax
	vec3 tmp = u_viewOrg - att_position;
	
	v_viewVecTS.x = dot(tmp, att_tangent);
	v_viewVecTS.y = dot(tmp, att_binormal);
	v_viewVecTS.z = dot(tmp, att_normal);

	gl_Position = u_modelViewProjectionMatrix * vec4(att_position, 1.0);
}
