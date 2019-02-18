layout (binding = 0) uniform	sampler2DRect	u_DNMiniMap;
layout (binding = 1) uniform	sampler2D		u_randomNormalMap;

layout (location = U_PARAM_VEC2_0)	uniform vec2	u_ssaoParms;	// intensity, scale
layout (location = U_SCREEN_SIZE)	uniform vec2	u_viewport;

#define HQ

#ifdef HQ
#define HQ_SCALE		0.5
#endif

#define FARCLIP			4096.0		// Q2 specific


void main (void) {
	// define kernel
	const float step = 1.0 - 1.0 / 8.0;
	const float fScale = 0.025; 
	float n = 0.0;

	vec3 kernel[8];
	kernel[0] = normalize(vec3( 1.0, 1.0, 1.0))*fScale*(n+=step);
	kernel[1] = normalize(vec3(-1.0,-1.0,-1.0))*fScale*(n+=step);
	kernel[2] = normalize(vec3(-1.0,-1.0, 1.0))*fScale*(n+=step);
	kernel[3] = normalize(vec3(-1.0, 1.0,-1.0))*fScale*(n+=step);
	kernel[4] = normalize(vec3(-1.0, 1.0 ,1.0))*fScale*(n+=step);
	kernel[5] = normalize(vec3( 1.0,-1.0,-1.0))*fScale*(n+=step);
	kernel[6] = normalize(vec3( 1.0,-1.0, 1.0))*fScale*(n+=step);
	kernel[7] = normalize(vec3( 1.0, 1.0,-1.0))*fScale*(n+=step);


	// create random rotation matrix
	vec3 randomNormal = texture(u_randomNormalMap, gl_FragCoord.xy * (1.0 / 4.0)).xyz * 2.0 - 1.0;

	// get fragment depth
	float centerDepth = texture2DRect(u_DNMiniMap, gl_FragCoord.xy).x;

	// compute sample scale
	vec3 scale = vec3(u_ssaoParms.y *
		min(centerDepth / 212.0, 1.0) *		// make area smaller if distance is less than 5 meters
		(1.0 + centerDepth / 320.0));		// make area bigger if distance is more than 32 meters

	float depthRangeScale = FARCLIP / scale.z * 0.85;

	// convert from inches to pixels
	scale.xy *= 1.0 / centerDepth;
	scale.z *= 2.0;
	scale.xy *= u_viewport;

	float depthTestSoftness = 64.0 * FARCLIP / scale.z;

	vec3 bias;
	vec4 depths[2];
	vec4 sum = vec4(0.0);
	
	for (int i = 0; i < 2; i++) {
		bias = reflect(kernel[i*4+0], randomNormal) * scale;

		depths[0].x = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#ifdef HQ
			bias *= HQ_SCALE;
			depths[1].x = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#endif

		bias = reflect(kernel[i*4+1], randomNormal) * scale;
		depths[0].y = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#ifdef HQ
			bias *= HQ_SCALE;
			depths[1].y = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#endif

		bias = reflect(kernel[i*4+2], randomNormal) * scale;
		depths[0].z = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#ifdef HQ
			bias *= HQ_SCALE;
			depths[1].z = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#endif

		bias = reflect(kernel[i*4+3], randomNormal) * scale;
		depths[0].w = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#ifdef HQ
			bias *= HQ_SCALE;
			depths[1].w = texture2DRect(u_DNMiniMap, gl_FragCoord.xy + bias.xy).x + bias.z;
		#endif

		#ifdef HQ
			for (int j = 0; j < 2; j++) {
		#else
			for (int j = 0; j < 1; j++) {
		#endif
				vec4 diff = (vec4(centerDepth) - depths[j]) * (1.0 / FARCLIP);
				vec4 diffScaled = diff * depthRangeScale;
				vec4 rangeIsInvalid = (min(abs(diffScaled), 1.0) + clamp(diffScaled, 0.0, 1.0)) * 0.5;

				sum += mix(clamp(-diff * depthTestSoftness, 0.0, 1.0), vec4(0.55), rangeIsInvalid);
			}
	}

	#ifdef HQ
		float occ = dot(sum, vec4(2.0 / 16.0)) - 0.075;
	#else
		float occ = dot(sum, vec4(2.0 / 8.0)) - 0.075;
	#endif

	fragData = vec4(min(mix(1.0, occ, u_ssaoParms.x), 1.0));
}