varying vec2 v_texCoord;
varying vec2 v_lineOffset;
uniform sampler2D u_cinMap;

const float brightness = 0.85;
const vec2 sine_comp = vec2(0.0005, 0.35);

void main ()
{
	vec4 cin = texture2D(u_cinMap, v_texCoord.xy);
	vec4 scanline = cin * (brightness + dot(sine_comp * sin(v_texCoord.xy * v_lineOffset), vec2(1.0)));
	gl_FragColor = clamp(scanline, 0.0, 1.0);
	gl_FragColor *=1.5;
}