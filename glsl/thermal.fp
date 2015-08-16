#version 120
uniform sampler2DRect u_screenTex;

const vec4 colors[3] = vec4[3](vec4(0.0, 0.0, 1.0, 1.0),
                               vec4(1.0, 1.0, 0.0, 1.0),
                               vec4(1.0, 0.0, 0.0, 1.0));

void main()
{
 vec2 ds = gl_FragCoord.xy * 2.0; 
 vec4 pixcol = texture2DRect(u_screenTex, ds);
 pixcol += texture2DRect( u_screenTex, ds + vec2(1.0, 0.0)); 
 pixcol += texture2DRect( u_screenTex, ds + vec2(0.0, 1.0)); 
 pixcol += texture2DRect( u_screenTex, ds + vec2(1.0, 1.0)); 
 pixcol *= 0.25;

 float lum = (pixcol.r  +pixcol.g + pixcol.b)/3.0;

 int ix = (lum < 0.5) ? 0 : 1;

 gl_FragColor = mix(colors[ix], colors[ix+1], (lum-float(ix) * 0.5) * 2.0);

}
