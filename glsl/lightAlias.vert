
layout(location = 0) in vec3 att_position;
layout(location = 1) in vec3 att_normal;
layout(location = 2) in vec3 att_tangent;
layout(location = 3) in vec3 att_binormal;
layout(location = 5) in vec2 att_texCoordDiffuse;

layout(location = U_LIGHT_POS)		uniform vec3	u_LightOrg;
layout(location = U_VIEW_POS)		uniform vec3	u_ViewOrigin; 		
layout(location = U_ATTEN_MATRIX)	uniform mat4	u_attenMatrix;
layout(location = U_SPOT_MATRIX)	uniform mat4	u_spotMatrix;
layout(location = U_CUBE_MATRIX)	uniform mat4	u_cubeMatrix;
layout(location = U_MVP_MATRIX)		uniform mat4	u_modelViewProjectionMatrix;
layout(location = U_MODELVIEW_MATRIX)	uniform mat4	u_modelViewMatrix; 

out vec2		v_texCoord;
out vec4		v_CubeCoord;
out vec4		v_AttenCoord;
out vec3		v_viewVec;
out vec3		v_lightVec;
out vec3		v_lightAtten;
out vec3		v_lightSpot;
out vec3		v_tangent;
out mat3		v_tangentToView;

void main (void) {
	
v_texCoord			= att_texCoordDiffuse; 
v_CubeCoord			= u_cubeMatrix		* vec4(att_position, 1.0);
v_lightAtten		= (u_attenMatrix	* vec4(att_position, 1.0)).xyz;
v_lightSpot			= (u_spotMatrix		* vec4(att_position, 1.0)).xyz;
v_tangent			= att_tangent;

vec3 LV = u_LightOrg - att_position;
v_lightVec.x = dot(att_tangent, LV);
v_lightVec.y = dot(att_binormal, LV);
v_lightVec.z = dot(att_normal, LV); 

vec3 VV = u_ViewOrigin - att_position;
v_viewVec.x = dot(att_tangent, VV);
v_viewVec.y = dot(att_binormal, VV);
v_viewVec.z = dot(att_normal, VV); 

mat3 m = mat3(u_modelViewMatrix);
v_tangent = m * att_tangent;

// calculate tangent to view space transform
v_tangentToView[0] = m * att_tangent;
v_tangentToView[1] = m * att_binormal;
v_tangentToView[2] = m * att_normal;

gl_Position = u_modelViewProjectionMatrix * vec4(att_position, 1.0);
}