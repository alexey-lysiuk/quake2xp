layout	(location = 0) in vec3	att_position;
layout	(location = 5) in vec2	att_texCoordDiffuse;

layout	(location = U_ORTHO_MATRIX) uniform mat4 u_orthoMatrix;

out vec2 v_texCoord;

void main() 
{
	gl_Position = u_orthoMatrix * vec4(att_position, 1.0);
	v_texCoord = att_texCoordDiffuse;
}