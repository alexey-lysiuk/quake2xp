//!#include "include/global.inc"
layout(location = 0) in vec3 att_position;
layout(location = 1) in vec3 att_normal;
layout(location = 2) in vec3 att_tangent;
layout(location = 3) in vec3 att_binormal;
layout(location = 4) in vec4 att_color4f;
layout(location = 5) in vec2 att_texCoordDiffuse;

layout(location = U_WATER_DEFORM_MUL)	uniform float	u_deformMul;
layout(location = U_WATER_TRANS)		uniform int		u_transSurf;
layout(location = U_MVP_MATRIX)			uniform mat4	u_modelViewProjectionMatrix;
layout(location = U_MODELVIEW_MATRIX)	uniform mat4	u_modelViewMatrix; 
layout(location = U_PROJ_MATRIX)		uniform mat4	u_projectionMatrix;

out vec2		v_deformTexCoord;
out vec2		v_diffuseTexCoord;
out vec2		v_deformMul;
out vec3		v_positionVS;
out mat3		v_tangentToView;
out vec4		v_color;
out vec3		v_viewVecTS;

void main (void) {
	v_diffuseTexCoord = att_texCoordDiffuse;
	v_deformTexCoord = att_texCoordDiffuse;
	v_deformTexCoord *= 0.25;

	vec4 pos = u_modelViewMatrix * vec4(att_position, 1.0);
	v_positionVS = pos.xyz;

	vec3 view;
	view.x =  u_modelViewMatrix[3][0];
	view.y =  u_modelViewMatrix[3][1];
	view.z =  u_modelViewMatrix[3][2];
	vec3 tmp = view - att_position;
	v_viewVecTS.x = dot(tmp, att_tangent);
	v_viewVecTS.y = dot(tmp, att_binormal);
	v_viewVecTS.z = dot(tmp, att_normal);
	
	// compute view space depth
	pos = vec4(1.0, 0.0, pos.z, 1.0);
	// compute the deform strength
	vec2 d = (u_projectionMatrix * pos).xw;
	d.x /= max(d.y, 1.0);
	v_deformMul = min(d.xx, 0.02) * u_deformMul;
	v_color = att_color4f;

	// calculate tangent to view space transform
	mat3 m = mat3(u_modelViewMatrix);
	v_tangentToView[0] = m * att_tangent;
	v_tangentToView[1] = m * att_binormal;
	v_tangentToView[2] = m * att_normal;

	gl_Position = u_modelViewProjectionMatrix * vec4(att_position, 1.0);
}