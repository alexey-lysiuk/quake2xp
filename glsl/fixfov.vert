layout(location = 0) in vec3	att_position;

layout(location = U_PARAM_VEC4_0) uniform vec4 u_params;
layout(location = U_ORTHO_MATRIX) uniform mat4 u_orthoMatrix;
  
out vec3 vUV;                 // output to interpolate over screen
out vec2 vUVDot;              // output to interpolate over screen
 
void main() {

	gl_Position = u_orthoMatrix * vec4(att_position, 1.0);
	vec2 uv = (gl_Position.xy / gl_Position.w) * (0.5) + vec2(0.5);
        
    float strength            = u_params.x;
    float height              = u_params.y;
    float aspectRatio         = u_params.z;
    float cylindricalRatio    = u_params.w;
        
    float scaledHeight = strength * height;
    float cylAspectRatio = aspectRatio * cylindricalRatio;
    float aspectDiagSq = aspectRatio * aspectRatio + 1.0;
    float diagSq = scaledHeight * scaledHeight * aspectDiagSq;
    vec2 signedUV = (2.0 * uv + vec2(-1.0, -1.0));
 
    float z = 0.5 * sqrt(diagSq + 1.0) + 0.5;
    float ny = (z - 1.0) / (cylAspectRatio * cylAspectRatio + 1.0);
 
    vUVDot = sqrt(ny) * vec2(cylAspectRatio, 1.0) * signedUV;
    vUV = vec3(0.5, 0.5, 1.0) * z + vec3(-0.5, -0.5, 0.0);
    vUV.xy += uv.xy;
}