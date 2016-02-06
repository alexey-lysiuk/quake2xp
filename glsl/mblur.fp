layout (binding = 0) uniform sampler2DRect	 	u_ScreenTex;
layout (binding = 1) uniform sampler2DRect		u_MaskTex;
uniform vec3				u_params; // x-velocity, y-velocity, num samples 

void main(void) 
{
	vec2 uv = gl_FragCoord.xy;
	//Get the initial color at this pixel.  
	vec4 color = texture2DRect(u_ScreenTex, uv);
	
	float mask = texture2DRect(u_MaskTex, gl_FragCoord.xy).a;
	if(mask == 0.0){
		fragData =  color;
		return;
	}
	vec2 velocity  = clamp(u_params.xy, -1.0, 1.0);
	uv += velocity.xy;  
	for(int i = 1; i < u_params.z; ++i)  
	{  
	//Sample the color buffer along the velocity vector.  
	vec4 accum = texture2DRect(u_ScreenTex, uv);  
	//Add the current color to our color sum.  
	color += accum;  
	uv += velocity;
	}
	//Average all of the samples to get the final blur color.  
	fragData =  color / u_params.z; 
}
