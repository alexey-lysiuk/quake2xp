//!#include "include/global.inc"
layout(location = 0) in vec4	att_position;

layout(location = U_MVP_MATRIX) uniform mat4	u_modelViewProjectionMatrix;
layout(location = U_LIGHT_POS)	uniform vec3    u_lightOrg;

void main (void) {

if (att_position.w == 0){ 
    vec4 vPos =  att_position - vec4(u_lightOrg, 0.0);
 	vPos = ( vPos.wwww * vec4(u_lightOrg, 0.0) ) + vPos;
    gl_Position = u_modelViewProjectionMatrix * vPos;
}
    
if (att_position.w == 1)                     
	 gl_Position = u_modelViewProjectionMatrix * att_position;
}