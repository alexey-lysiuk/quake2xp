/*
Copyright (C) 2004-2013 Quake2xp Team.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// r_lightMan.c
// Per-pixel light manager and editor basic idea based on Discoloda work

#include "r_local.h"

worldShadowLight_t *currentShadowLight;
worldShadowLight_t *shadowLight_static = NULL, *shadowLight_frame = NULL, *selectedShadowLight = NULL;
worldShadowLight_t	shadowLightsBlock[MAX_WORLD_SHADOW_LIHGTS];

static int num_dlits;
int num_nwmLights;
int num_visLights;
int numLightQ;

vec3_t player_org, v_forward, v_right, v_up;
trace_t CL_PMTraceWorld(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int mask);
qboolean R_MarkLightLeaves (worldShadowLight_t *light);
void R_DrawBspModelVolumes(qboolean precalc, worldShadowLight_t *light);
void R_LightFlareOutLine();

qboolean R_AddLightToFrame(worldShadowLight_t *light) {

	if (r_newrefdef.areabits){
		if (!(r_newrefdef.areabits[light->area >> 3] & (1 << (light->area & 7)))){
			return false;
		}
	}

	if(!HasSharedLeafs(light->vis, viewvis))
		return false;

	if(light->spherical){
		
	if(!SphereInFrustum(light->origin, light->radius[0]))
		 return false;
	}
	else
	{
	if(R_CullBox(light->mins, light->maxs))
		return false;
	}
	 return true;
}

void UpdateLightBounds(worldShadowLight_t *light)
{
	int i;
	mat3_t lightAxis;
	vec3_t tmp;
				
	if (light->radius[0] == light->radius[1] && light->radius[0] == light->radius[2])
		light->spherical = true;
	else
		light->spherical = false;

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - light->radius[i];
		light->maxs[i] = light->origin[i] + light->radius[i];
	}
	
	if(light->spherical){
		light->len = light->radius[0];
	}
	else
	{
		VectorSubtract(light->origin, light->maxs, tmp);
		light->len = VectorLength(tmp);
	}

	for (i=0; i<8; i++) {
	tmp[0] = (i & 1) ? -light->radius[0] : light->radius[0];
	tmp[1] = (i & 2) ? -light->radius[1] : light->radius[1];
	tmp[2] = (i & 4) ? -light->radius[2] : light->radius[2];

	AnglesToMat3(light->angles, lightAxis);
	Mat3_TransposeMultiplyVector(lightAxis, tmp, light->corners[i]);
	VectorAdd(light->corners[i], light->origin, light->corners[i]);
	}
}


void R_AddDynamicLight(dlight_t *dl) {
	
	worldShadowLight_t *light;
	int i;

	if(!SphereInFrustum(dl->origin, dl->intensity))
		return;
	
	light = &shadowLightsBlock[num_dlits++];
	memset(light, 0, sizeof(worldShadowLight_t));
	light->next = shadowLight_frame;
	shadowLight_frame = light;

	VectorCopy(dl->origin, light->origin);
	VectorCopy(dl->color, light->startColor);
	VectorCopy(dl->angles, light->angles);
	VectorSet(light->radius, dl->intensity, dl->intensity, dl->intensity);

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - dl->intensity;
		light->maxs[i] = light->origin[i] + dl->intensity;
	}

	light->style = 0;
	light->filter = dl->filter;
	light->isStatic = 0;
	light->_cone = dl->_cone;
	light->isNoWorldModel = 0;
	light->isShadow = 1;
	light->spherical = true;
	UpdateLightBounds(light);
}

void R_AddNoWorldModelLight() {
	
	worldShadowLight_t *light;
	int i;

	light = &shadowLightsBlock[num_nwmLights++];
	memset(light, 0, sizeof(worldShadowLight_t));
	light->next = shadowLight_frame;
	shadowLight_frame = light;
	VectorSet(light->origin, -100, 0, 25);
	VectorSet(light->startColor, 1.0, 1.0, 1.0);
	VectorSet(light->angles, 0, 0, 0);
	VectorSet(light->radius, 600, 600, 600);

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - 600;
		light->maxs[i] = light->origin[i] + 600;
	}

	light->style = 0;
	light->filter = 0;
	light->isStatic = 1;
	light->isShadow = 0;
	light->_cone = 0;
	light->isNoWorldModel = 1;
	light->spherical = true;
	UpdateLightBounds(light);
}


void R_PrepareShadowLightFrame(void) {
	
	int i;
	worldShadowLight_t *light;
	
	num_dlits = 0;
	num_nwmLights = 0;
	shadowLight_frame = NULL;

	// add pre computed lights
	if(shadowLight_static) {
		for(light = shadowLight_static; light; light = light->s_next) {

			if(!R_AddLightToFrame(light))
				continue;

			light->next = shadowLight_frame;
			shadowLight_frame = light;
		}
	}

	// add tempory lights
	for(i=0;i<r_newrefdef.num_dlights;i++) {

		if(num_dlits > MAX_WORLD_SHADOW_LIHGTS)
			break;

		R_AddDynamicLight(&r_newrefdef.dlights[i]);
	}


	if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		R_AddNoWorldModelLight();
	

	if(!shadowLight_frame) 
		return;
		
	for(light = shadowLight_frame; light; light = light->next) {
	
	if(!light->isStatic)
		if(!R_MarkLightLeaves(light))
			return;

	MakeFrustum4Light(light, true);

	VectorCopy(light->startColor, light->color);
		
	if(!(r_newrefdef.rdflags & RDF_NOWORLDMODEL)){
		light->color[0] *= r_newrefdef.lightstyles[light->style].rgb[0];
		light->color[1] *= r_newrefdef.lightstyles[light->style].rgb[1];
		light->color[2] *= r_newrefdef.lightstyles[light->style].rgb[2];
		}

	}

}

void FS_StripExtension (const char *in, char *out, size_t size_out)
{
	char *last = NULL;

	if (size_out == 0)
		return;

	while (*in && size_out > 1)
	{
		if (*in == '.')
			last = out;
		else if (*in == '/' || *in == '\\' || *in == ':')
			last = NULL;
		*out++ = *in++;
		size_out--;
	}
	if (last)
		*last = 0;
	else
		*out = 0;
}

void R_SaveLights_f(void) {
	
	char				name[MAX_QPATH], path[MAX_QPATH];
	FILE				*f;
	int					i=0;
	
	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!shadowLight_static) 
		return;

	FS_StripExtension(r_worldmodel->name, name, sizeof (name));
	Com_sprintf(path, sizeof(path),"%s/%s.xplit", FS_Gamedir(), name);
	remove(path); //remove prev version

	f = fopen(path, "w");
	if(!f) {
		Com_Printf("Could not open %s.\n", path);
		return;
	}
	
	fprintf(f, "//ReLight configuration file\n//Generated by quake2xp\n\n");

	for(currentShadowLight = shadowLight_static; currentShadowLight; currentShadowLight = currentShadowLight->s_next){

		fprintf(f, "//Light %i\n", i);
		fprintf(f, "{\n");
		fprintf(f, "\"classname\" \"light\"\n");
		fprintf(f, "\"origin\" \"%i %i %i\"\n",			(int)currentShadowLight->origin[0], (int)currentShadowLight->origin[1], (int)currentShadowLight->origin[2]);
		fprintf(f, "\"radius\" \"%i %i %i\"\n",			(int)currentShadowLight->radius[0], (int)currentShadowLight->radius[1], (int)currentShadowLight->radius[2]);
		fprintf(f, "\"color\" \"%.3f %.3f %.3f\"\n",		 currentShadowLight->startColor[0],	 currentShadowLight->startColor[1],	 currentShadowLight->startColor[2]);
		fprintf(f, "\"style\" \"%i\"\n",				(int)currentShadowLight->style);
		fprintf(f, "\"filter\" \"%i\"\n",				(int)currentShadowLight->filter);
		fprintf(f, "\"angles\" \"%i %i %i\"\n",			(int)currentShadowLight->angles[0], (int)currentShadowLight->angles[1], (int)currentShadowLight->angles[2]);
		fprintf(f, "\"speed\" \"%.3f %.3f %.3f\"\n",		currentShadowLight->speed[0],		 currentShadowLight->speed[1],	     currentShadowLight->speed[2]);
		fprintf(f, "\"shadow\" \"%i\"\n",					currentShadowLight->isShadow);
		fprintf(f, "\"ambient\" \"%i\"\n",					currentShadowLight->isAmbient);
		fprintf(f, "\"_cone\" \"%.1f\"\n",					currentShadowLight->_cone);
		if(currentShadowLight->targetname[0])
			fprintf(f, "\"targetname\" \"%s\"\n",			currentShadowLight->targetname);
		fprintf(f, "\"spawnflags\" \"%i\"\n",				currentShadowLight->start_off);
		fprintf(f, "\"flareSize\" \"%i\"\n",			(int)currentShadowLight->flareSize);
		fprintf(f, "\"flareOrigin\" \"%i %i %i\"\n",	(int)currentShadowLight->flareOrigin[0], (int)currentShadowLight->flareOrigin[1], (int)currentShadowLight->flareOrigin[2]);
		fprintf(f, "\"flare\" \"%i\"\n",					currentShadowLight->flare);
		fprintf(f, "}\n");
		i++;
	}

	fclose(f);
	
		Com_Printf(""S_COLOR_MAGENTA"SaveLights_f: "S_COLOR_WHITE"Save lights to "S_COLOR_GREEN"%s.xplit\n"S_COLOR_WHITE"Save "S_COLOR_GREEN"%i"S_COLOR_WHITE" lights\n", name, i);

}

static void DeleteCurrentLight(worldShadowLight_t *l) {
	worldShadowLight_t *light;

	if(l == shadowLight_static) {
		shadowLight_static = l->s_next;
	}
	else {
		for(light = shadowLight_static; light; light = light->s_next) {
			if(light->s_next == l) {
				light->s_next = l->s_next;
				break;
			}
		}
	}
	free(l);
}

void R_Light_Spawn_f(void) {
	vec3_t color = {1.0, 1.0, 1.0}, end, spawn, radius={300,300,300};
	trace_t trace;
	char target[MAX_QPATH];

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	memset(target, 0, sizeof(target));

	VectorMA(player_org, 1024, v_forward, end);
	trace = CL_PMTraceWorld(player_org, vec3_origin, vec3_origin, end, MASK_SOLID);
	
	if (trace.fraction != 1.0){
		VectorMA(trace.endpos, -10, v_forward, spawn);
		R_AddNewWorldLight(spawn, color, radius, 0, 0, vec3_origin, vec3_origin, true, 1, 0, 0, true, 0, spawn, 10.0, target, 0);
		}
}

void R_Light_SpawnToCamera_f(void) {
	vec3_t color = {1.0, 1.0, 1.0}, radius={300,300,300};
	char target[MAX_QPATH];

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}
	memset(target, 0, sizeof(target));
	R_AddNewWorldLight(player_org, color, radius, 0, 0, vec3_origin, vec3_origin, true, 1, 0, 0, true, 0, player_org, 10.0, target, 0);
}

void R_Light_Copy_f(void) {
	vec3_t color, spawn, origin, angles, speed, radius, flareOrg;
	float _cone, flareSize;
	int style, filter, shadow, ambient, flare, flag;
	char target[MAX_QPATH];

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}
	
	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	VectorCopy(selectedShadowLight->origin, origin);
	VectorCopy(selectedShadowLight->color, color);
	VectorCopy(selectedShadowLight->angles, angles);
	VectorCopy(selectedShadowLight->speed, speed);
	VectorCopy(selectedShadowLight->radius, radius);
	VectorCopy(selectedShadowLight->flareOrigin, flareOrg);
	
	memset(target, 0, sizeof(target));
	if(selectedShadowLight->targetname[0])
		memcpy(target, selectedShadowLight->targetname, sizeof(target));

	style = selectedShadowLight->style;
	filter = selectedShadowLight->filter;
	shadow = selectedShadowLight->isShadow;
	ambient = selectedShadowLight->isAmbient;
	_cone = selectedShadowLight->_cone;
	flare = selectedShadowLight->flare;
	flareSize = selectedShadowLight->flareSize;
	flag = selectedShadowLight->start_off;

	VectorMA(origin, -50, v_forward, spawn);
	selectedShadowLight = R_AddNewWorldLight(spawn, color, radius, style, filter, angles, vec3_origin, true, shadow, ambient, _cone, true, flare, flareOrg, flareSize, target, flag);
}

void R_EditSelectedLight_f(void) {
	
	vec3_t color, origin, angles, speed, radius, fOrg;
	float _cone, fSize;
	int style, filter, shadow, ambient, flare, start_off;
	char *target[MAX_QPATH];

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}
	
	VectorCopy(selectedShadowLight->origin, origin);
	VectorCopy(selectedShadowLight->color, color);
	VectorCopy(selectedShadowLight->angles, angles);
	VectorCopy(selectedShadowLight->radius, radius);
	VectorCopy(selectedShadowLight->flareOrigin, fOrg);

	if(selectedShadowLight->targetname[0]){
	memset(target, 0, sizeof(target));
	memcpy(target, selectedShadowLight->targetname, sizeof(target));
	}

	style = selectedShadowLight->style;
	filter = selectedShadowLight->filter;
	shadow = selectedShadowLight->isShadow;
	ambient = selectedShadowLight->isAmbient;
	_cone = selectedShadowLight->_cone;
	flare = selectedShadowLight->flare;
	fSize = selectedShadowLight->flareSize;
	start_off = selectedShadowLight->start_off;

	if (!strcmp(Cmd_Argv(1), "origin")) {
		if(Cmd_Argc() != 5) {
			Com_Printf("usage: editLight: %s X Y Z\nCurrent Origin: %.4f %.4f %.4f\n", Cmd_Argv(0),
				selectedShadowLight->origin[0], 
				selectedShadowLight->origin[1], 
				selectedShadowLight->origin[2]);
			return;
		}
		origin[0] = atof(Cmd_Argv(2));
		origin[1] = atof(Cmd_Argv(3));
		origin[2] = atof(Cmd_Argv(4));
		VectorCopy(origin, selectedShadowLight->origin);
		UpdateLightBounds(selectedShadowLight);
		R_MarkLightLeaves(selectedShadowLight);
		R_DrawBspModelVolumes(true, selectedShadowLight);
	} 
	else
	if (!strcmp(Cmd_Argv(1), "color")) {
		if(Cmd_Argc() != 5) {
			Com_Printf("usage: editLight: %s Red Green Blue\nCurrent Color: %.4f %.4f %.4f\n", Cmd_Argv(0),
			selectedShadowLight->color[0], 
			selectedShadowLight->color[1], 
			selectedShadowLight->color[2]);
			return;
		}
		color[0] = atof(Cmd_Argv(2));
		color[1] = atof(Cmd_Argv(3));
		color[2] = atof(Cmd_Argv(4));
		VectorCopy(color, selectedShadowLight->startColor);
	} 
	else
	if (!strcmp(Cmd_Argv(1), "speed")) {
		if(Cmd_Argc() != 5) {
			Com_Printf("usage: editLight: %s X rotate speed Y rotate speed Z rotate speed\nCurrent speed rotations: %.4f %.4f %.4f\n", Cmd_Argv(0),
			selectedShadowLight->speed[0], 
			selectedShadowLight->speed[1], 
			selectedShadowLight->speed[2]);
			return;
		}
		speed[0] = atof(Cmd_Argv(2));
		speed[1] = atof(Cmd_Argv(3));
		speed[2] = atof(Cmd_Argv(4));
		VectorCopy(speed, selectedShadowLight->speed);
	} 
	else
	if (!strcmp(Cmd_Argv(1), "radius")) {
		if(Cmd_Argc() != 5) {
			Com_Printf("usage: editLight: %s value\nCurrent Radius: %.1f %.1f %.1f\n", Cmd_Argv(0), 
			selectedShadowLight->radius[0], selectedShadowLight->radius[1], selectedShadowLight->radius[2]);
			return;
		}
		radius[0] = atof(Cmd_Argv(2));
		radius[1] = atof(Cmd_Argv(3));
		radius[2] = atof(Cmd_Argv(4));
		VectorCopy(radius, selectedShadowLight->radius);
		UpdateLightBounds(selectedShadowLight);
		R_MarkLightLeaves(selectedShadowLight);
		R_DrawBspModelVolumes(true, selectedShadowLight);
	 } 
	else
	if (!strcmp(Cmd_Argv(1), "cone")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight: %s value\nCurrent Light Cone: %.1f\n", Cmd_Argv(0), 
				selectedShadowLight->_cone);
			return;
		}
		_cone = atof(Cmd_Argv(2));
		selectedShadowLight->_cone = _cone;
		UpdateLightBounds(selectedShadowLight);
		R_MarkLightLeaves(selectedShadowLight);
		R_DrawBspModelVolumes(true, selectedShadowLight);
	} 
	else
	 if (!strcmp(Cmd_Argv(1), "style")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Style %i\n", Cmd_Argv(0),
			selectedShadowLight->style);
			return;
		}
		style = atoi(Cmd_Argv(2)); 
		selectedShadowLight->style = style;
	 }
	 else
		if (!strcmp(Cmd_Argv(1), "filter")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Cube Filter %i\n", Cmd_Argv(0),
			selectedShadowLight->filter);
			return;
		}
		filter = atoi(Cmd_Argv(2)); 
		selectedShadowLight->filter = filter;
	 }
	 else
		if (!strcmp(Cmd_Argv(1), "angles")) {
		if(Cmd_Argc() != 5) {
			Com_Printf("usage: editLight: %s X Y Z \nCurrent Angles: %.4f %.4f %.4f\n", Cmd_Argv(0),
			selectedShadowLight->angles[0], 
			selectedShadowLight->angles[1], 
			selectedShadowLight->angles[2]);
			return;
		}
		angles[0] = atof(Cmd_Argv(2));
		angles[1] = atof(Cmd_Argv(3));
		angles[2] = atof(Cmd_Argv(4));
		VectorCopy (angles, selectedShadowLight->angles);
	} 
	else
		if (!strcmp(Cmd_Argv(1), "shadow")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Shadow Flag is %i\n", Cmd_Argv(0),
			selectedShadowLight->isShadow);
			return;
		}
		shadow = atoi(Cmd_Argv(2)); 
		selectedShadowLight->isShadow = shadow;
	 }
	else
		if (!strcmp(Cmd_Argv(1), "ambient")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Ambient Flag is %i\n", Cmd_Argv(0),
				selectedShadowLight->isAmbient);
			return;
		}
		ambient = atoi(Cmd_Argv(2)); 
		selectedShadowLight->isAmbient = ambient;
	 }
	else
		if (!strcmp(Cmd_Argv(1), "flare")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Flare Flag is %i\n", Cmd_Argv(0),
				selectedShadowLight->flare);
			return;
		}
		flare = atoi(Cmd_Argv(2)); 
		selectedShadowLight->flare = flare;
		VectorCopy(selectedShadowLight->origin, selectedShadowLight->flareOrigin);
	 }
	else
		if (!strcmp(Cmd_Argv(1), "flareSize")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Flare Size is %i\n", Cmd_Argv(0),
				selectedShadowLight->flareSize);
			return;
		}
		fSize = atoi(Cmd_Argv(2)); 
		selectedShadowLight->flareSize = fSize;
	 }
	else
	if (!strcmp(Cmd_Argv(1), "flareOrigin")) {
		if(Cmd_Argc() != 5) {
			Com_Printf("usage: editLight: %s X Y Z\nCurrent Flare Origin: %.4f %.4f %.4f\n", Cmd_Argv(0),
				selectedShadowLight->flareOrigin[0], 
				selectedShadowLight->flareOrigin[1], 
				selectedShadowLight->flareOrigin[2]);
			return;
		}
		fOrg[0] = atof(Cmd_Argv(2));
		fOrg[1] = atof(Cmd_Argv(3));
		fOrg[2] = atof(Cmd_Argv(4));
		VectorCopy(fOrg, selectedShadowLight->flareOrigin);
	} 
	else
	if (!strcmp(Cmd_Argv(1), "target")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight: %s value\nCurrent Light Target: %s\n", Cmd_Argv(0), 
				selectedShadowLight->targetname);
			return;
		}
		memcpy(selectedShadowLight->targetname, Cmd_Argv(2), sizeof(selectedShadowLight->targetname));
	 } else
		if (!strcmp(Cmd_Argv(1), "start_off")) {
		if(Cmd_Argc() != 3) {
			Com_Printf("usage: editLight %s value\nCurrent Start Off Flag is %i\n", Cmd_Argv(0),
				selectedShadowLight->start_off);
			return;
		}
		start_off = atoi(Cmd_Argv(2)); 
		selectedShadowLight->start_off = start_off;
	 }
	{
		 Com_Printf("Light Properties: Origin: %.4f %.4f %.4f\nColor: %.4f %.4f %.4f\nRadius %.1f %.1f %.1f\nStyle %i\nFilter Cube %i\nAngles: %.4f %.4f %.4f\nSpeed: %.4f %.4f %.4f\nShadows %i\nAmbient %i\nCone %f\nFlare %i\nFlare Size %f\nTargetname %s\nStart Off %i\n",
		 selectedShadowLight->origin[0],	selectedShadowLight->origin[1], selectedShadowLight->origin[2],
		 selectedShadowLight->color[0],		selectedShadowLight->color[1], selectedShadowLight->color[2], 
		 selectedShadowLight->radius[0],	selectedShadowLight->radius[1], selectedShadowLight->radius[2],
		 selectedShadowLight->style,
		 selectedShadowLight->filter,
		 selectedShadowLight->angles[0],	selectedShadowLight->angles[1], selectedShadowLight->angles[2],
		 selectedShadowLight->speed[0],		selectedShadowLight->speed[1], selectedShadowLight->speed[2],
		 selectedShadowLight->isShadow, 
		 selectedShadowLight->isAmbient,
		 selectedShadowLight->_cone,
		 selectedShadowLight->flare,
		 selectedShadowLight->flareSize,
		 selectedShadowLight->targetname,
		 selectedShadowLight->start_off);

	}

}

qboolean flareEdit;

void R_FlareEdit_f(void) {
	int mode;

	if( Cmd_Argc() != 2 )
	{
		Com_Printf("Toggle Flare Editing Mode.\nUsage: editFlare: 0 or 1\n");
		return;
	}
	mode = atoi(Cmd_Argv(1)); 
	if(mode >0)
		flareEdit = true;
	else
		flareEdit = false;
}

void R_ResetFlarePos_f(void){

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}
	
	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	VectorCopy(selectedShadowLight->origin, selectedShadowLight->flareOrigin);

}

void R_MoveLightToRight_f(void) {
	
	vec3_t origin;
	float offset;
	
	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	if( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: moveLight_right: <value>\n");
		return;
	}

	offset = atof(Cmd_Argv(1)); 
	
	if(flareEdit)
		VectorCopy(selectedShadowLight->flareOrigin, origin);
	else
		VectorCopy(selectedShadowLight->origin, origin);

	if(r_CameraSpaceLightMove->value)
		VectorMA(origin, offset, v_right, origin);
	else
		origin[0] += offset;

	if(flareEdit)
		VectorCopy(origin, selectedShadowLight->flareOrigin);
	else
	{
	VectorCopy(origin, selectedShadowLight->origin);
	UpdateLightBounds(selectedShadowLight);
	R_MarkLightLeaves(selectedShadowLight);
	R_DrawBspModelVolumes(true, selectedShadowLight);
	}
}

void R_MoveLightForward_f(void) {
	
	vec3_t origin;
	float  offset, fix;
	
	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	if( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: moveLight_forward: <value>\n");
		return;
	}

	offset = atof(Cmd_Argv(1)); 

	if(flareEdit)
		VectorCopy(selectedShadowLight->flareOrigin, origin);
	else
		VectorCopy(selectedShadowLight->origin, origin);
	
	if(r_CameraSpaceLightMove->value){
		fix = origin[2];
		VectorMA(origin, offset, v_forward, origin);
		origin[2] = fix;
	}
	else
		origin[1] += offset;
	
	if(flareEdit)
		VectorCopy(origin, selectedShadowLight->flareOrigin);
	else
	{
	VectorCopy(origin, selectedShadowLight->origin);
	UpdateLightBounds(selectedShadowLight);
	R_MarkLightLeaves(selectedShadowLight);
	R_DrawBspModelVolumes(true, selectedShadowLight);
	}
}

void R_MoveLightUpDown_f(void) {
	
	vec3_t origin;
	float offset;
	
	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}


	if( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: moveSelectedLight_right: <value>\n");
		return;
	}

	if(flareEdit)
		VectorCopy(selectedShadowLight->flareOrigin, origin);
	else
		VectorCopy(selectedShadowLight->origin, origin);

	offset = atof(Cmd_Argv(1)); 
	origin[2] += offset;
	
	if(flareEdit)
		VectorCopy(origin, selectedShadowLight->flareOrigin);
	else
	{
	VectorCopy(origin, selectedShadowLight->origin);
	UpdateLightBounds(selectedShadowLight);
	R_MarkLightLeaves(selectedShadowLight);
	R_DrawBspModelVolumes(true, selectedShadowLight);
	}
}

void R_ChangeLightRadius_f(void) {
	
	float offset, fRad;
	vec3_t rad;

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	if( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: ajustSelectedLightRadius: X Y Z\n");
		return;
	}
	if(flareEdit){
		fRad = selectedShadowLight->flareSize;
		offset = atof(Cmd_Argv(1));
		fRad += offset;
		selectedShadowLight->flareSize = fRad;
	}
	else{
	VectorCopy(selectedShadowLight->radius, rad);

	offset = atof(Cmd_Argv(1));
	
	rad[0] += offset;
	rad[1] += offset;
	rad[2] += offset;
	
	if(rad[0] <10)
		rad[0] = 10;

	if(rad[1] <10)
		rad[1] = 10;

	if(rad[2] <10)
		rad[2] = 10;
	
	VectorCopy(rad, selectedShadowLight->radius);
	UpdateLightBounds(selectedShadowLight);
	R_MarkLightLeaves(selectedShadowLight);
	R_DrawBspModelVolumes(true, selectedShadowLight);
	}

}

void R_ChangeLightCone_f(void) {
	
	float cone, offset;
	
	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	if( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: ajustSelectedLightRadius: <value>\n");
		return;
	}

	cone = selectedShadowLight->_cone;

	offset = atof(Cmd_Argv(1));
	
	cone += offset;
	if(cone >999999)
		cone = 999999;

	if(cone < 0)
		cone = 0;

	selectedShadowLight->_cone = cone;
	R_DrawBspModelVolumes(true, selectedShadowLight);
	R_MarkLightLeaves(selectedShadowLight);
	UpdateLightBounds(selectedShadowLight);
}



void R_Light_Delete_f(void) {

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}
	
	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}

	DeleteCurrentLight(selectedShadowLight);
	selectedShadowLight = NULL;
}

void R_Light_UnSelect_f(void) {

	if(!r_lightEditor->value){
		Com_Printf("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}
	
	if(!selectedShadowLight)
	{
	Com_Printf("No selected light.\n");
		return;
	}
	selectedShadowLight = NULL;
}

char buff0[128];
char buff1[128];
char buff2[128];
char buff3[128];
char buff4[128];
char buff5[128];
char buff6[128];
char buff7[128];
char buff8[128];
char buff9[128];
char buff10[128];
char buff11[128];
char buff12[128];
char buff13[128];

void UpdateLightEditor(void){

	vec3_t		end_trace, mins = { -5.0f, -5.0f, -5.0f }, maxs = { 5.0f, 5.0f, 5.0f };
	vec3_t		corners[8], tmp;
	int			j;
	float		fraction = 1.0;
	vec3_t		v[8];
	trace_t		trace_light, trace_bsp;
	unsigned	headNode;
	vec3_t		tmpOrg, tmpRad;
	int			id;
	unsigned	defBits = 0;

	if(!r_lightEditor->value)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if(!currentShadowLight->isStatic)
		return;

	qglDisable(GL_SCISSOR_TEST);
	
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		qglDisable(GL_DEPTH_BOUNDS_TEST_EXT);

	qglDisable(GL_BLEND);
	qglDisable(GL_STENCIL_TEST);
	qglDisable(GL_TEXTURE_2D);
	qglDisable(GL_CULL_FACE);
	qglColor4f(1,1,1,1);

	// stupid player camera and angles corruption, fixed
	VectorCopy(r_origin, player_org);
	AngleVectors(r_newrefdef.viewangles, v_forward, v_right, v_up);

	// create a temp hull from bounding box
	headNode = CM_HeadnodeForBox (mins, maxs);
	VectorMA(r_origin, 1024, v_forward, end_trace);

	trace_bsp = CL_PMTraceWorld(r_origin, vec3_origin, vec3_origin, end_trace, MASK_SOLID); //bsp collision with bmodels

	// light in focus?
	trace_light = CM_TransformedBoxTrace(	r_origin, trace_bsp.endpos, vec3_origin, vec3_origin, headNode, MASK_ALL, 
											currentShadowLight->origin, vec3_origin); // find light
	
	if(trace_light.surface->name && trace_light.surface->name[0])
		if((int)trace_light.ent>1)
		Com_Printf("bmodel nane is \n");

	if(trace_light.fraction  < fraction){
		selectedShadowLight = currentShadowLight;
		fraction = trace_light.fraction;
	}

	// setup program
	GL_BindProgram(genericProgram, defBits);
	id = genericProgram->id[defBits];

	if(currentShadowLight != selectedShadowLight){

	for (j=0; j<8; j++) {
		tmp[0] = (j & 1) ? -5.0f : 5.0f;
		tmp[1] = (j & 2) ? -5.0f : 5.0f;
		tmp[2] = (j & 4) ? -5.0f : 5.0f;

	VectorAdd(tmp, currentShadowLight->origin, corners[j]);
	}
	qglUniform3f(qglGetUniformLocation(id, "u_color"),	currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2]);
	qglEnable(GL_LINE_SMOOTH);
	qglLineWidth(3.0);

	qglBegin(GL_LINE_LOOP);
	qglVertex3fv(corners[0]);
	qglVertex3fv(corners[2]);
	qglVertex3fv(corners[3]);
	qglVertex3fv(corners[1]);
	qglEnd();

	qglBegin(GL_LINE_LOOP);
	qglVertex3fv(corners[4]);
	qglVertex3fv(corners[6]);
	qglVertex3fv(corners[7]);
	qglVertex3fv(corners[5]);
	qglEnd();

	qglBegin(GL_LINES);
	qglVertex3fv(corners[0]);
	qglVertex3fv(corners[4]);
	qglVertex3fv(corners[1]);
	qglVertex3fv(corners[5]);
	qglVertex3fv(corners[2]);
	qglVertex3fv(corners[6]);
	qglVertex3fv(corners[3]);
	qglVertex3fv(corners[7]);
	qglEnd();
	qglDisable(GL_LINE_SMOOTH);
	}

	if(selectedShadowLight){	
	
	qglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	qglEnable(GL_LINE_SMOOTH);
	qglLineWidth(3.0);
	VectorCopy(selectedShadowLight->origin, tmpOrg);
	VectorCopy(selectedShadowLight->radius, tmpRad);

	qglUniform3f(qglGetUniformLocation(id, "u_color"),	selectedShadowLight->color[0], selectedShadowLight->color[1], selectedShadowLight->color[2]);

	sprintf(buff0,	"Origin: %i %i %i",	(int)selectedShadowLight->origin[0], 
												(int)selectedShadowLight->origin[1], 
												(int)selectedShadowLight->origin[2]);
	sprintf(buff1,	"Color: %.3f %.3f %.3f",	selectedShadowLight->color[0], 
												selectedShadowLight->color[1], 
												selectedShadowLight->color[2]);
	sprintf(buff2,	"Radius: %i %i %i",			(int)selectedShadowLight->radius[0],
												(int)selectedShadowLight->radius[1],
												(int)selectedShadowLight->radius[2]);		
	sprintf(buff3,	"Style: %i",				selectedShadowLight->style);
	sprintf(buff4,	"Filter: %i",				selectedShadowLight->filter);
	sprintf(buff5,	"Angles: %i %i %i",			(int)selectedShadowLight->angles[0], 
												(int)selectedShadowLight->angles[1], 
												(int)selectedShadowLight->angles[2]);
	sprintf(buff6,	"Speed: %.3f %.3f %.3f",	selectedShadowLight->speed[0], 
												selectedShadowLight->speed[1], 
												selectedShadowLight->speed[2]);
	sprintf(buff7,	"Shadow: %i",				selectedShadowLight->isShadow);
	sprintf(buff8,	"Ambient: %i",				selectedShadowLight->isAmbient);
	sprintf(buff9,	"Cone: %.2f",				selectedShadowLight->_cone);
	sprintf(buff10,	"Flare: %i; Flare Editing is %i",				
												selectedShadowLight->flare, (int)flareEdit );
	sprintf(buff11,	"Flare Size: %i",			(int)selectedShadowLight->flareSize);
	sprintf(buff12,	"Target Name: %s",			selectedShadowLight->targetname);
	sprintf(buff13,	"Start Off: %i",			selectedShadowLight->start_off);

	VectorSet(v[0], tmpOrg[0]-tmpRad[0], tmpOrg[1]-tmpRad[1], tmpOrg[2]-tmpRad[2]);
	VectorSet(v[1], tmpOrg[0]-tmpRad[0], tmpOrg[1]-tmpRad[1], tmpOrg[2]+tmpRad[2]);
	VectorSet(v[2], tmpOrg[0]-tmpRad[0], tmpOrg[1]+tmpRad[1], tmpOrg[2]-tmpRad[2]);
	VectorSet(v[3], tmpOrg[0]-tmpRad[0], tmpOrg[1]+tmpRad[1], tmpOrg[2]+tmpRad[2]);
	VectorSet(v[4], tmpOrg[0]+tmpRad[0], tmpOrg[1]-tmpRad[1], tmpOrg[2]-tmpRad[2]);
	VectorSet(v[5], tmpOrg[0]+tmpRad[0], tmpOrg[1]-tmpRad[1], tmpOrg[2]+tmpRad[2]);
	VectorSet(v[6], tmpOrg[0]+tmpRad[0], tmpOrg[1]+tmpRad[1], tmpOrg[2]-tmpRad[2]);
	VectorSet(v[7], tmpOrg[0]+tmpRad[0], tmpOrg[1]+tmpRad[1], tmpOrg[2]+tmpRad[2]);


	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[4]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[3]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();

	qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	qglDisable(GL_LINE_SMOOTH);
	
	if(!flareEdit || !selectedShadowLight->flare){ // skip filled box in flare editing mode

	// draw small light box
	VectorSet(v[0], tmpOrg[0]-5, tmpOrg[1]-5, tmpOrg[2]-5);
	VectorSet(v[1], tmpOrg[0]-5, tmpOrg[1]-5, tmpOrg[2]+5);
	VectorSet(v[2], tmpOrg[0]-5, tmpOrg[1]+5, tmpOrg[2]-5);
	VectorSet(v[3], tmpOrg[0]-5, tmpOrg[1]+5, tmpOrg[2]+5);
	VectorSet(v[4], tmpOrg[0]+5, tmpOrg[1]-5, tmpOrg[2]-5);
	VectorSet(v[5], tmpOrg[0]+5, tmpOrg[1]-5, tmpOrg[2]+5);
	VectorSet(v[6], tmpOrg[0]+5, tmpOrg[1]+5, tmpOrg[2]-5);
	VectorSet(v[7], tmpOrg[0]+5, tmpOrg[1]+5, tmpOrg[2]+5);


	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[4]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[3]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();
	}

}
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_CULL_FACE);
	qglEnable(GL_BLEND);

	if(r_shadows->value)
		qglEnable(GL_STENCIL_TEST);

	if(r_useLightScissors->value)
		qglEnable(GL_SCISSOR_TEST);
	
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		qglEnable(GL_DEPTH_BOUNDS_TEST_EXT);

	GL_BindNullProgram();

	R_LightFlareOutLine();
}

void Clamp2RGB(vec3_t color)
{
	if (color[0] > 1.0)
		color[0] = 1.0;
	if (color[0] < 0.0)
		color[0] = 0.0;

	if (color[1] > 1.0)
		color[1] = 1.0;
	if (color[1] < 0.0)
		color[1] = 0.0;

	if (color[2] > 1.0)
		color[2] = 1.0;
	if (color[2] < 0.0)
		color[2] = 0.0;
}



void CreateNormal(vec3_t dst, vec3_t xyz0, vec3_t xyz1, vec3_t xyz2)
{
	float x10, y10, z10;
	float x20, y20, z20;

	x10 = xyz1[0] - xyz0[0];
	x20 = xyz2[0] - xyz0[0];
	y10 = xyz1[1] - xyz0[1];
	y20 = xyz2[1] - xyz0[1];
	z10 = xyz1[2] - xyz0[2];
	z20 = xyz2[2] - xyz0[2];

	dst[0] = y10 * z20 - y20 * z10;
	dst[1] = z10 * x20 - z20 * x10;
	dst[2] = x10 * y20 - x20 * y10;
	
	VectorNormalize(dst);
}


void MakeFrustum4Light(worldShadowLight_t *light, qboolean ingame)
{
	vec3_t		v0, v1, v2, v3, v4;
	vec3_t		forward, right, up;
	vec3_t		angles, rspeed;

	if (!light->_cone)
		return;	// �����, ���� ������� �� ���� ������������ (�� ����� �������� _cone)

	if(ingame)
		VectorCopy(light->speed, rspeed);
	else
		VectorClear(rspeed);

	angles[0] = light->angles[0] + rspeed[0] * r_newrefdef.time * 1000;
	angles[1] = light->angles[1] + rspeed[1] * r_newrefdef.time * 1000;
	angles[2] = light->angles[2] + rspeed[2] * r_newrefdef.time * 1000;

	AngleVectors (angles, forward, right, up);

	VectorScale(right, light->_cone, right);
	VectorScale(up, light->_cone, up);
	VectorCopy(light->origin, v0);

	v1[0] = v0[0] + (forward[0] - right[0] - up[0]);
	v1[1] = v0[1] + (forward[1] - right[1] - up[1]);
	v1[2] = v0[2] + (forward[2] - right[2] - up[2]);

	v2[0] = v0[0] + (forward[0] - right[0] + up[0]);
	v2[1] = v0[1] + (forward[1] - right[1] + up[1]);
	v2[2] = v0[2] + (forward[2] - right[2] + up[2]);

	v3[0] = v0[0] + (forward[0] + right[0] + up[0]);
	v3[1] = v0[1] + (forward[1] + right[1] + up[1]);
	v3[2] = v0[2] + (forward[2] + right[2] + up[2]);

	v4[0] = v0[0] + (forward[0] + right[0] - up[0]);
	v4[1] = v0[1] + (forward[1] + right[1] - up[1]);
	v4[2] = v0[2] + (forward[2] + right[2] - up[2]);

	CreateNormal(light->frust[0].normal, v1, v2, v0);
	light->frust[0].dist = DotProduct(light->frust[0].normal, v0);

	CreateNormal(light->frust[1].normal, v2, v3, v0);
	light->frust[1].dist = DotProduct(light->frust[1].normal, v0);

	CreateNormal(light->frust[2].normal, v3, v4, v0);
	light->frust[2].dist = DotProduct(light->frust[2].normal, v0);

	CreateNormal(light->frust[3].normal, v4, v1, v0);
	light->frust[3].dist = DotProduct(light->frust[3].normal, v0);
}


worldShadowLight_t *R_AddNewWorldLight(vec3_t origin, vec3_t color, float radius[3], int style, 
									   int filter, vec3_t angles, vec3_t speed, qboolean isStatic, 
									   int isShadow, int isAmbient, float cone, qboolean ingame, int flare, vec3_t flareOrg, float flareSize, char target[MAX_QPATH], int flags) {
	
	worldShadowLight_t *light;
	int i, leafnum, cluster;
	mat3_t lightAxis;
	vec3_t tmp;

	light = (worldShadowLight_t*)malloc(sizeof(worldShadowLight_t));
	light->s_next = shadowLight_static;
	shadowLight_static = light;

	VectorClear(tmp);
	VectorClear(light->frust[0].normal);
	VectorClear(light->origin);
	VectorClear(light->startColor);
	VectorClear(light->angles);
	VectorClear(light->speed);
	VectorClear(light->radius);
	VectorClear(light->flareOrigin);
	memset(light->targetname, 0, sizeof(light->targetname));
	light->start_off = 0;

	for (i=0; i<8; i++)
		VectorClear(light->corners[i]);

	VectorCopy(origin, light->origin);
	VectorCopy(color, light->startColor);
	VectorCopy(angles, light->angles);
	VectorCopy(speed, light->speed);
	VectorCopy(radius, light->radius);
	VectorCopy(flareOrg, light->flareOrigin);

	if (light->radius[0] == light->radius[1] && light->radius[0] == light->radius[2])
		light->spherical = true;
	else
		light->spherical = false;

	Clamp2RGB(light->color);
	
	light->_cone = cone;
	light->isStatic = isStatic;
	light->isShadow = isShadow;
	light->isAmbient = isAmbient;
	light->isNoWorldModel = 0;
	light->next = NULL;
	light->style = style;
	light->filter = filter;
	light->flareSize = flareSize;
	light->flare = flare;
	light->vboId = light->iboId = light->iboNumIndices = 0;
	light->depthBounds[0] = 0.0;
	light->depthBounds[1] = 1.0;
	light->len = 0;
	memcpy(light->targetname, target, sizeof(light->targetname));

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - light->radius[i];
		light->maxs[i] = light->origin[i] + light->radius[i];
	}

	if(light->spherical){
		light->len = light->radius[0];
	}else
	{
		VectorSubtract(light->origin, light->maxs, tmp);
		light->len = VectorLength(tmp);
	}

	 for (i=0; i<8; i++) {
		tmp[0] = (i & 1) ? -light->radius[0] : light->radius[0];
		tmp[1] = (i & 2) ? -light->radius[1] : light->radius[1];
		tmp[2] = (i & 4) ? -light->radius[2] : light->radius[2];

		AnglesToMat3(light->angles, lightAxis);
		Mat3_TransposeMultiplyVector(lightAxis, tmp, light->corners[i]);
		VectorAdd(light->corners[i], light->origin, light->corners[i]);
	}

	MakeFrustum4Light(light, false);

	if(ingame){
	R_MarkLightLeaves(light);
	R_DrawBspModelVolumes(true, light);
	}
	else
	{
	//// simple cull info for new light 
	leafnum = CM_PointLeafnum(light->origin);
	cluster = CM_LeafCluster(leafnum);
	light->area = CM_LeafArea(leafnum);
	Q_memcpy(light->vis, CM_ClusterPVS(cluster), (CM_NumClusters() + 7) >> 3);
	}
	light->occQ = numLightQ;
	numLightQ++;

	#define START_OFF	1
	light->start_off = (flags & START_OFF);

	r_numWorlsShadowLights++;
	return light;
}

model_t *loadmodel;

void Load_BspLights() {
	
	int addLight, style, numlights, flag;
	char *c, *token, key[256], *value, target[MAX_QPATH];
	float color[3], origin[3], radius[3], cone;

	if(!loadmodel) {
		Com_Printf("No map loaded.\n");
		return;
	}

	c = CM_EntityString();
	numlights = 0;

	while(1) {
		token = COM_Parse(&c);
		if(!c)
			break;
		
		memset(target, 0, sizeof(target));
		VectorClear(origin);
		VectorSet(color, 1,1,1);
		VectorClear(radius);
		style = 0;
		cone = 0;
		flag = 0;

		addLight = false;

		while(1) {
			token = COM_Parse(&c);
			if(token[0] == '}')
				break;

			strncpy(key, token, sizeof(key)-1);

			value = COM_Parse(&c);
			if(!Q_stricmp(key, "classname")) {
				if(!Q_stricmp(value, "light"))
					addLight = true;
				if(!Q_stricmp(value, "light_mine1")){
					addLight = true;
				}
				if(!Q_stricmp(value, "light_mine2")){
					addLight = true;
				}
			}

			if(!Q_stricmp(key, "light"))
				radius[0] = atoi(value);
			if(!Q_stricmp(key, "origin"))
				sscanf(value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
			if(!Q_stricmp(key, "_color"))
				sscanf(value, "%f %f %f", &color[0], &color[1], &color[2]);
			if(!Q_stricmp(key, "style"))
				style = atoi(value);
			if(!Q_stricmp(key, "_cone"))
				cone = atoi(value);
			if(!Q_stricmp(key, "targetname"))
				Q_strncpyz(target, value, sizeof(target));
			if(!Q_stricmp(key, "spawnflags"))
				flag = atoi(value);
		}

		if(addLight && style > 0) {
			VectorSet(radius, radius[0], radius[0], radius[0]);
			R_AddNewWorldLight(origin, color, radius, style, 0, vec3_origin, vec3_origin, true, 1, 0, cone, false, 0, origin, 10.0, target, flag);
			numlights++;
		}
	}
	Com_Printf(""S_COLOR_MAGENTA"Loaded "S_COLOR_GREEN"%i"S_COLOR_WHITE" bsp lights\n", numlights);

}

void Load_LightFile() {
	
	int		style, numLights = 0, filter, shadow, ambient, flare, flag;
	vec3_t	angles, speed, color, origin, lOrigin, fOrg;
	char	*c, *token, key[256], *value, target[MAX_QPATH];
	float	radius[3], cone, fSize;
	char	name[MAX_QPATH], path[MAX_QPATH];

	if(!r_worldmodel) {
		Com_Printf("No map loaded.\n");
		return;
	}

	FS_StripExtension(r_worldmodel->name, name, sizeof (name));
	Com_sprintf(path, sizeof(path),"%s.xplit", name);
	FS_LoadFile (path, (void **)&c);

	if(!c){
		Load_BspLights();
		return;
	}
	
	Com_Printf("Load lights from "S_COLOR_GREEN"%s"S_COLOR_WHITE".\n", path);

	while(1) {
		token = COM_Parse(&c);
		
		if(!c){
		break;
		}

		style = 0;
		filter = 0;
		shadow = 0;
		ambient = 0;
		cone = 0;
		fSize = 0;
		flare = 0;
		flag = 0;

		memset(target, 0, sizeof(target));
		VectorClear(radius);
		VectorClear(angles);
		VectorClear(speed);
		VectorClear(origin);
		VectorClear(lOrigin);
		VectorClear(color);
		VectorClear(fOrg);

		while(1) {
			token = COM_Parse(&c);
			if(token[0] == '}')
				break;

			strncpy(key, token, sizeof(key)-1);

			value = COM_Parse(&c);
			
			if(!Q_stricmp(key, "radius"))
				sscanf(value, "%f %f %f", &radius[0], &radius[1], &radius[2]);
			else if(!Q_stricmp(key, "origin"))
				sscanf(value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
			else if(!Q_stricmp(key, "color"))
				sscanf(value, "%f %f %f", &color[0], &color[1], &color[2]);
			else if(!Q_stricmp(key, "style"))
				style = atoi(value);
			else if(!Q_stricmp(key, "filter"))
				filter = atoi(value);
			else if(!Q_stricmp(key, "angles"))
				sscanf(value, "%f %f %f", &angles[0], &angles[1], &angles[2]);
			else if(!Q_stricmp(key, "speed"))
				sscanf(value, "%f %f %f", &speed[0], &speed[1], &speed[2]);
			else if(!Q_stricmp(key, "shadow"))
				shadow = atoi(value);
			else if(!Q_stricmp(key, "ambient"))
				ambient = atoi(value);
			else if(!Q_stricmp(key, "_cone"))
				cone = atoi(value);
			else if(!Q_stricmp(key, "flare"))
				flare = atoi(value);
			else if(!Q_stricmp(key, "flareOrigin"))
				sscanf(value, "%f %f %f", &fOrg[0], &fOrg[1], &fOrg[2]);
			else if(!Q_stricmp(key, "flareSize"))
				fSize = atoi(value);
			else if(!Q_stricmp(key, "targetname"))
				Q_strncpyz(target, value, sizeof(target));
			else if(!Q_stricmp(key, "spawnflags"))
				flag = atoi(value);

		}
	
		R_AddNewWorldLight(origin, color, radius, style, filter, angles, speed, true, shadow, ambient, cone, false, flare, fOrg, fSize, target, flag);
		numLights++;
		}
	Com_Printf(""S_COLOR_MAGENTA"Load_LightFile:"S_COLOR_WHITE" add "S_COLOR_GREEN"%i"S_COLOR_WHITE" world lights\n", numLights);
}

/*
===============
R_MarkLightLeaves

Marks nodes from the light, this is used for
gross culling during svbsp creation.
===============
*/
qboolean R_MarkLightLeaves (worldShadowLight_t *light)
{
	int contents, leafnum, cluster;
	int		leafs[MAX_MAP_LEAFS];
	int		i, count;
	vec3_t	mins, maxs;
	byte	vis[MAX_MAP_LEAFS/8];

	contents = CL_PMpointcontents(light->origin);
	if (contents & CONTENTS_SOLID)
		goto skip;

	leafnum = CM_PointLeafnum (light->origin);
	cluster = CM_LeafCluster (leafnum);
	light->area = CM_LeafArea (leafnum);

	if(!light->area)
	{
skip:	Com_DPrintf("Out of BSP, rejected light at %f %f %f\n", light->origin[0], light->origin[1], light->origin[2]);
		return false;
	}

	// build vis-data
	memcpy (&light->vis, CM_ClusterPVS(cluster), (((CM_NumClusters()+31)>>5)<<2));

	for (i=0 ; i<3 ; i++)
	{
		mins[i] = light->origin[i] - light->radius[i];
		maxs[i] = light->origin[i] + light->radius[i];
	}

	count = CM_BoxLeafnums (mins, maxs, leafs, r_worldmodel->numleafs, NULL);
	if (count < 1)
		Com_Error (ERR_FATAL, "R_MarkLightLeaves: count < 1");

	// convert leafs to clusters
	for (i=0 ; i<count ; i++)
		leafs[i] = CM_LeafCluster(leafs[i]);

	memset(&vis, 0, (((r_worldmodel->numleafs+31)>>5)<<2));
	for (i=0 ; i<count ; i++)
		vis[leafs[i]>>3] |= (1<<(leafs[i]&7));

	for (i=0 ; i<((r_worldmodel->numleafs+31)>>5); i++)
		((long *)light->vis)[i] &= ((long *)vis)[i];

	return true;
}

qboolean InLightVISEntity()
{
	int		leafs[MAX_MAP_LEAFS];
	int		i, count;
	int		longs;
	vec3_t	mins, maxs;
		
	if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return true;

	if (!r_worldmodel)
		return false;
	
	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2]) {
		for (i = 0; i < 3; i++) {
			mins[i] = currententity->origin[i] - currentmodel->radius;
			maxs[i] = currententity->origin[i] + currentmodel->radius;
		}
	}
	else
	{
	VectorAdd(currententity->origin, currententity->model->maxs, maxs);
	VectorAdd(currententity->origin, currententity->model->mins, mins);
	}

	count = CM_BoxLeafnums (mins, maxs, leafs, r_worldmodel->numleafs, NULL);
	if (count < 1)
		Com_Error (ERR_FATAL, "InLightVISEntity: count < 1");
	longs = (CM_NumClusters()+31)>>5;

	// convert leafs to clusters
	for (i=0 ; i<count ; i++){
		leafs[i] = CM_LeafCluster(leafs[i]);
	}
	memset(&currententity->vis, 0, (((r_worldmodel->numleafs+31)>>5)<<2));
	for (i=0 ; i<count ; i++)
		currententity->vis[leafs[i]>>3] |= (1<<(leafs[i]&7));

	return HasSharedLeafs (currentShadowLight->vis, currententity->vis);
}


void R_PreCalcLightData(void){
	worldShadowLight_t *light;

		for(light = shadowLight_static; light; light = light->s_next) {

			if(!R_MarkLightLeaves(light))
				continue;
			
			R_DrawBspModelVolumes(true, light);
		}
	Com_Printf(""S_COLOR_MAGENTA"R_PreCalcLightData: "S_COLOR_GREEN"%i"S_COLOR_WHITE" lights\n", numPreCachedLights);
}

void DeleteShadowVertexBuffers(void){
	worldShadowLight_t *light;

		for(light = shadowLight_static; light; light = light->s_next) {
			
			qglDeleteBuffers(1, &light->vboId);
			qglDeleteBuffers(1, &light->iboId);
			light->vboId = light->iboId = light->iboNumIndices = 0;
		}
	numPreCachedLights = 0;
}

extern qboolean relightMap;

void CleanDuplicateLights(void){

	worldShadowLight_t *light1, *light2, *plug;
	vec3_t tmp;

	if(!relightMap)
		return;

	for(light1 = shadowLight_static; light1; light1 = light1->s_next) {
	
		
	for(light2 = light1->s_next; light2; light2 = plug) {
		
		plug = light2->s_next;
				
		VectorSubtract(light2->origin, light1->origin, tmp);
  
		if ((VectorLength(tmp) < r_lightsWeldThreshold->value)){
						
			VectorAdd(light1->origin, light2->origin, tmp);
			VectorScale(tmp, 0.5f, light1->origin);
			DeleteCurrentLight(light2);
			
		}
	}
}
}


void R_ClearWorldLights(void)
{
	worldShadowLight_t *light, *next;

	if(shadowLight_static) {
		for(light = shadowLight_static; light; light = next) {
			next = light->s_next;
			free(light);
		}
		shadowLight_static = NULL;
	}

	memset(shadowLightsBlock, 0, sizeof(worldShadowLight_t) * MAX_WORLD_SHADOW_LIHGTS);

	r_numWorlsShadowLights = 0;
}

/*
=============
GL_SetupCubeMapMatrix

Loads the current matrix with a tranformation used for light filters
=============
*/
void GL_SetupCubeMapMatrix(qboolean model)
{
	float	a, b, c;

	qglMatrixMode(GL_TEXTURE);
	qglLoadIdentity();

	a = currentShadowLight->angles[2] + (currentShadowLight->speed[2] * r_newrefdef.time * 1000);
	b = currentShadowLight->angles[0] + (currentShadowLight->speed[0] * r_newrefdef.time * 1000);
	c = currentShadowLight->angles[1] + (currentShadowLight->speed[1] * r_newrefdef.time * 1000);

	if (a)
		qglRotatef ( -a,  1, 0, 0);
	if (b)
		qglRotatef ( -b,  0, 1, 0);
	if (c)
		qglRotatef ( -c,  0, 0, 1);

	if (model){

	if (currententity->angles[1])
		qglRotatef (currententity->angles[1],  0, 0, 1);
	if (currententity->angles[0])
		qglRotatef (currententity->angles[0],  0, 1, 0);		/// fixed "stupid quake bug" ;)
	if (currententity->angles[2])
		qglRotatef (currententity->angles[2],  1, 0, 0);
	
	}
	qglTranslatef(-currentShadowLight->origin[0], -currentShadowLight->origin[1], -currentShadowLight->origin[2]);
	qglMatrixMode(GL_MODELVIEW);
}

/*
extern int	occ_framecount;

qboolean R_DrawLightOccluders()
{
	vec3_t		v[8];
	vec3_t		tmpOrg;
	vec3_t		radius;
	int			sampleCount;
	int			id;
	unsigned	defBits = 0;

	if(!r_useLightOccluders->value)
		return true;
	
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return true;
	
	if(!currentShadowLight->isStatic)
		return true;
	
	if(BoundsAndSphereIntersect (currentShadowLight->mins, currentShadowLight->maxs, r_origin, 0))
		return true;
	
	// setup program
	GL_BindProgram(nullProgram, defBits);
	id = nullProgram->id[defBits];

	qglColorMask(0,0,0,0);
	qglDisable(GL_TEXTURE_2D);
	qglDisable(GL_CULL_FACE);
	qglDisable(GL_BLEND);
	qglDisable(GL_STENCIL_TEST);
	
	currentShadowLight->framecount = occ_framecount;

	VectorCopy(currentShadowLight->origin, tmpOrg);
	VectorScale(currentShadowLight->radius, r_occLightBoundsSize->value, radius);

	VectorSet(v[0], tmpOrg[0]-radius[0], tmpOrg[1]-radius[1], tmpOrg[2]-radius[2]);
	VectorSet(v[1], tmpOrg[0]-radius[0], tmpOrg[1]-radius[1], tmpOrg[2]+radius[2]);
	VectorSet(v[2], tmpOrg[0]-radius[0], tmpOrg[1]+radius[1], tmpOrg[2]-radius[2]);
	VectorSet(v[3], tmpOrg[0]-radius[0], tmpOrg[1]+radius[1], tmpOrg[2]+radius[2]);
	VectorSet(v[4], tmpOrg[0]+radius[0], tmpOrg[1]-radius[1], tmpOrg[2]-radius[2]);
	VectorSet(v[5], tmpOrg[0]+radius[0], tmpOrg[1]-radius[1], tmpOrg[2]+radius[2]);
	VectorSet(v[6], tmpOrg[0]+radius[0], tmpOrg[1]+radius[1], tmpOrg[2]-radius[2]);
	VectorSet(v[7], tmpOrg[0]+radius[0], tmpOrg[1]+radius[1], tmpOrg[2]+radius[2]);

	qglBeginQueryARB(gl_state.query_passed, lightsQueries[currentShadowLight->occQ]);

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[4]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[3]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();
	
	qglEndQueryARB(gl_state.query_passed);

	qglColorMask(1,1,1,1);
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_CULL_FACE);
	qglEnable(GL_BLEND);
	qglEnable(GL_STENCIL_TEST);
	GL_BindNullProgram();

//	if (currentShadowLight->framecount == occ_framecount-1)
		qglGetQueryObjectivARB(lightsQueries[currentShadowLight->occQ], GL_QUERY_RESULT_ARB, &sampleCount);

	if (!sampleCount) 
		return false;
	else 
		return true;

}
*/

void R_LightScale(void) {
	float	val;
	int		i;

	if (!r_lightScale->value || !r_pplWorld->value)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
         return;

	qglLoadIdentity();

	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();
	qglOrtho(0, 1, 0, 1, -1, 1);

	GL_MBind(GL_TEXTURE0, 0);

	qglEnable(GL_BLEND);
	qglBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);	// multiply by two every pass

	qglColorMask(1, 1, 1, 0);
	qglDepthMask(0);

	val = max(r_lightScale->value, 0.0);

	qglColor3f(1, 1, 1);

	qglBegin(GL_QUADS);

	for(i=1; i<val; i<<=1) {
		qglVertex2f(0, 0);
		qglVertex2f(0, 1);
		qglVertex2f(1, 1);
		qglVertex2f(1, 0);
	}
	val /= (float)(i << 1);
	qglColor3f(val, val, val);

	qglVertex2f(0, 0);
	qglVertex2f(0, 1);
	qglVertex2f(1, 1);
	qglVertex2f(1, 0);

	qglEnd();

	qglPopMatrix();
	qglMatrixMode(GL_MODELVIEW);

	qglColor3f(1, 1, 1);
	qglColorMask(1, 1, 1, 1);
	qglDepthMask(1);
	qglDisable(GL_BLEND);
}

/*
==================
R_ClipLightPlane

==================
*/
#define MAX_LIGHT_PLANE_VERTICES 64
#define	ON_EPSILON			0.1	// point on plane side epsilon

void VectorLerp (const vec3_t from, const vec3_t to, float frac, vec3_t out) {
	if (frac <= 0.0f) {
		out[0] = from[0];
		out[1] = from[1];
		out[2] = from[2];
		return;
	}

	if (frac >= 1.0f) {
		out[0] = to[0];
		out[1] = to[1];
		out[2] = to[2];
		return;
	}

	out[0] = from[0] + (to[0] - from[0]) * frac;
	out[1] = from[1] + (to[1] - from[1]) * frac;
	out[2] = from[2] + (to[2] - from[2]) * frac;
}

/*
=============
PointOnPlaneSide

=============
*/
int PointOnPlaneSide (const vec3_t point, struct cplane_s *plane) {
	float	dist;

	if (plane->type < 3)
		dist = point[plane->type] - plane->dist;
	else
		dist = DotProduct(point, plane->normal) - plane->dist;

	if (dist > ON_EPSILON)
		return SIDE_FRONT;
	if (dist < -ON_EPSILON)
		return SIDE_BACK;

	return SIDE_ON;
}

static void R_ClipLightPlane (const mat4_t mvpMatrix, vec3_t mins, vec3_t maxs, int numPoints, vec3_t *points, int stage) {
	vec3_t	clipped[MAX_LIGHT_PLANE_VERTICES];
	float	dists[MAX_LIGHT_PLANE_VERTICES];
	int		sides[MAX_LIGHT_PLANE_VERTICES];
	qboolean	front, back;
	vec4_t	in, out;
	vec3_t	point;
	float	scale;
	int		i, numClipped;

	if (stage == 5) {
		// fully clipped, so add points in normalized device coordinates
		for (i = 0; i < numPoints; i++) {
			in[0] = points[i][0];
			in[1] = points[i][1];
			in[2] = points[i][2];
			in[3] = 1.0f;

			Mat4_MultiplyVector(mvpMatrix, in, out);

			scale = 1.0f;

			point[0] = out[0] * scale;
			point[1] = out[1] * scale;
			point[2] = out[2] * scale;

			AddPointToBounds(point, mins, maxs);
		}

		return;
	}

	if (numPoints > MAX_LIGHT_PLANE_VERTICES - 2)
		Com_Error(false, "R_ClipLightPlane: MAX_LIGHT_PLANE_VERTICES hit");

	// determine sides for each point
	front = false;
	back = false;

	for (i = 0; i < numPoints; i++) {
		dists[i] = DotProduct(points[i], frustum[stage].normal) - frustum[stage].dist;

		if (dists[i] > ON_EPSILON) {
			sides[i] = SIDE_FRONT;
			front = true;
			continue;
		}

		if (dists[i] < -ON_EPSILON) {
			sides[i] = SIDE_BACK;
			back = true;
			continue;
		}

		sides[i] = SIDE_ON;
	}

	if (!front)
		return;		// not clipped

	if (!back) {
		// continue
		R_ClipLightPlane(mvpMatrix, mins, maxs, numPoints, points, stage+1);
		return;
	}

	// handle wraparound case
	VectorCopy(points[0], points[i]);

	dists[i] = dists[0];
	sides[i] = sides[0];

	// clip it
	numClipped = 0;

	for (i = 0; i < numPoints; i++) {
		if (sides[i] == SIDE_ON) {
			VectorCopy(points[i], clipped[numClipped]);
			numClipped++;
			continue;
		}

		if (sides[i] == SIDE_FRONT) {
			VectorCopy(points[i], clipped[numClipped]);
			numClipped++;
		}

		if (sides[i+1] == SIDE_ON || sides[i+1] == sides[i])
			continue;

		if (dists[i] == dists[i+1]) {
			VectorCopy(points[i], clipped[numClipped]);
			numClipped++;
		}
		else
			VectorLerp(points[i], points[i+1], dists[i] / (dists[i] - dists[i+1]), clipped[numClipped++]);
	}

	// continue
	R_ClipLightPlane(mvpMatrix, mins, maxs, numClipped, clipped, stage+1);
}

void R_SetViewLightScreenBounds () {
	int			i, scissor[4], 
				cornerIndices[6][4] = {{3, 2, 6, 7}, {0, 1, 5, 4}, {2, 3, 1, 0}, {4, 5, 7, 6}, {1, 3, 7, 5}, {2, 0, 4, 6}};
	vec3_t		mins = {Q_INFINITY, Q_INFINITY, Q_INFINITY},
				maxs = {-Q_INFINITY, -Q_INFINITY, -Q_INFINITY},
				points[5];
	mat4_t		tmpMatrix, mvpMatrix;
	float		depth[2];

/*
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL){

		currentShadowLight->scissor[0] = r_newrefdef.viewport[0];
		currentShadowLight->scissor[1] = r_newrefdef.viewport[1];
		currentShadowLight->scissor[2] = r_newrefdef.viewport[2];
		currentShadowLight->scissor[3] = r_newrefdef.viewport[3];
		currentShadowLight->depthBounds[0] = 0.0f;
		currentShadowLight->depthBounds[1] = 1.0f;
		return;
	}
*/
	if (!r_useLightScissors->value) {

		currentShadowLight->scissor[0] = r_newrefdef.viewport[0];
		currentShadowLight->scissor[1] = r_newrefdef.viewport[1];
		currentShadowLight->scissor[2] = r_newrefdef.viewport[2];
		currentShadowLight->scissor[3] = r_newrefdef.viewport[3];
	}

	if(!(gl_state.depthBoundsTest || r_useDepthBounds->value)){
		currentShadowLight->depthBounds[0] = 0.0f;
		currentShadowLight->depthBounds[1] = 1.0f;
	}

	if(r_useLightScissors->value || (gl_state.depthBoundsTest && r_useDepthBounds->value)){
	// compute modelview-projection matrix
	Mat4_Multiply(r_newrefdef.modelViewMatrix, r_newrefdef.projectionMatrix, tmpMatrix);
	Mat4_Transpose(tmpMatrix, mvpMatrix);

	// copy the corner points of each plane and clip to the frustum
	for (i = 0; i < 6; i++) {
		VectorCopy(currentShadowLight->corners[cornerIndices[i][0]], points[0]);
		VectorCopy(currentShadowLight->corners[cornerIndices[i][1]], points[1]);
		VectorCopy(currentShadowLight->corners[cornerIndices[i][2]], points[2]);
		VectorCopy(currentShadowLight->corners[cornerIndices[i][3]], points[3]);

		R_ClipLightPlane(mvpMatrix, mins, maxs, 4, points, 0);
	}

	// check if any corner point is not in front of the near plane
	for (i = 0; i < 8; i++) {
		if (PointOnPlaneSide(currentShadowLight->corners[i], &frustum[4]) != SIDE_FRONT) {
			mins[2] = -1.0f;
			break;
		}
	}

	// transform into screen space
	mins[0] = (0.5f + 0.5f * mins[0]) * r_newrefdef.viewport[2] + r_newrefdef.viewport[0];
	mins[1] = (0.5f + 0.5f * mins[1]) * r_newrefdef.viewport[3] + r_newrefdef.viewport[1];
	mins[2] = (0.5f + 0.5f * mins[2]);
	maxs[0] = (0.5f + 0.5f * maxs[0]) * r_newrefdef.viewport[2] + r_newrefdef.viewport[0];
	maxs[1] = (0.5f + 0.5f * maxs[1]) * r_newrefdef.viewport[3] + r_newrefdef.viewport[1];
	maxs[2] = (0.5f + 0.5f * maxs[2]);
	
	}

	// set the scissor rectangle
	if (r_useLightScissors->value) {
		scissor[0] = max(Q_ftol(floor(mins[0])), r_newrefdef.viewport[0]);
		scissor[1] = max(Q_ftol(floor(mins[1])), r_newrefdef.viewport[1]);
		scissor[2] = min(Q_ftol(ceil(maxs[0])), r_newrefdef.viewport[0] + r_newrefdef.viewport[2]);
		scissor[3] = min(Q_ftol(ceil(maxs[1])), r_newrefdef.viewport[1] + r_newrefdef.viewport[3]);

		if (scissor[0] > scissor[2] || scissor[1] > scissor[3]) {
			currentShadowLight->scissor[0] = r_newrefdef.viewport[0];
			currentShadowLight->scissor[1] = r_newrefdef.viewport[1];
			currentShadowLight->scissor[2] = r_newrefdef.viewport[2];
			currentShadowLight->scissor[3] = r_newrefdef.viewport[3];
		}
		else {
			currentShadowLight->scissor[0] = scissor[0];
			currentShadowLight->scissor[1] = scissor[1];
			currentShadowLight->scissor[2] = scissor[2] - scissor[0];
			currentShadowLight->scissor[3] = scissor[3] - scissor[1];
		}
	}
	else {
		currentShadowLight->scissor[0] = r_newrefdef.viewport[0];
		currentShadowLight->scissor[1] = r_newrefdef.viewport[1];
		currentShadowLight->scissor[2] = r_newrefdef.viewport[2];
		currentShadowLight->scissor[3] = r_newrefdef.viewport[3];
	}

	// set the depth bounds
	if(r_useDepthBounds->value){
		depth[0] = max(mins[2], 0.0f);
		depth[1] = min(maxs[2], 1.0f);

		if (depth[0] > depth[1]) {
			currentShadowLight->depthBounds[0] = 0.0f;
			currentShadowLight->depthBounds[1] = 1.0f;
		}
		else {
			currentShadowLight->depthBounds[0] = depth[0];
			currentShadowLight->depthBounds[1] = depth[1];
		}
	}
}

void R_DebugScissors(void)
{
	int			id;
	unsigned	defBits = 0;
	
	if(!r_debugLightScissors->value)
		return;

	// setup program
	GL_BindProgram(genericProgram, defBits);
	id = genericProgram->id[defBits];
	qglUniform3f(qglGetUniformLocation(id, "u_color"),	currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2]);
	qglEnable(GL_LINE_SMOOTH);
	qglLineWidth(5.0);
	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();
	qglOrtho(r_newrefdef.viewport[0], r_newrefdef.viewport[0] + r_newrefdef.viewport[2], r_newrefdef.viewport[1], r_newrefdef.viewport[1] +r_newrefdef.viewport[3], -1.0, 1.0);
	qglMatrixMode(GL_MODELVIEW);
	qglPushMatrix();
	qglLoadIdentity();
	
	qglDisable(GL_DEPTH_TEST);
	qglBegin(GL_LINE_LOOP);
	qglVertex2i(currentShadowLight->scissor[0],		currentShadowLight->scissor[1]);
	qglVertex2i(currentShadowLight->scissor[0] +	currentShadowLight->scissor[2],		currentShadowLight->scissor[1]);
	qglVertex2i(currentShadowLight->scissor[0] +	currentShadowLight->scissor[2],		currentShadowLight->scissor[1] + currentShadowLight->scissor[3]);
	qglVertex2i(currentShadowLight->scissor[0],		currentShadowLight->scissor[1] +	currentShadowLight->scissor[3]);
	qglEnd();
	qglEnable(GL_DEPTH_TEST);

	qglMatrixMode(GL_PROJECTION);
	qglPopMatrix();
	qglMatrixMode(GL_MODELVIEW);
	qglPopMatrix();
	qglDisable(GL_LINE_SMOOTH);
	GL_BindNullProgram();
}


void R_DrawLightFlare(){
	
	float		dist, dist2, scale;
	vec3_t		v, tmp;
	unsigned	flareIndex[MAX_INDICES], defBits = 0;
	int			flareVert=0, index=0, id;
	vec3_t		vert_array[MAX_FLARES_VERTEX];
	vec2_t		tex_array[MAX_FLARES_VERTEX];
	vec4_t		color_array[MAX_FLARES_VERTEX];

	if (!currentShadowLight->flare)
		return;
	
	if ( r_newrefdef.rdflags & RDF_IRGOGGLES)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;
	
	if (!r_drawFlares->value)
		return;

	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		qglDisable(GL_DEPTH_BOUNDS_TEST_EXT);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_COLOR);

	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, vert_array);
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, tex_array);
	qglVertexAttribPointer(ATRB_COLOR, 4, GL_FLOAT, false, 0, color_array);

	GL_BindProgram(particlesProgram, defBits);
	id = particlesProgram->id[defBits];

	GL_MBind				(GL_TEXTURE0_ARB, r_flare->texnum);
	qglUniform1i			(qglGetUniformLocation(id, "u_map0"), 0);

	GL_SelectTexture		(GL_TEXTURE1_ARB);	
	GL_BindRect				(depthMap->texnum);
    qglUniform1i			(qglGetUniformLocation(id, "u_depthBufferMap"), 1);
	qglUniform2f			(qglGetUniformLocation(id, "u_depthParms"), r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
	qglUniform2f			(qglGetUniformLocation(id, "u_mask"), 1.0, 0.0);
	qglUniform1f			(qglGetUniformLocation(id, "u_colorScale"), 1.0);
	qglUniform1f			(qglGetUniformLocation(id, "u_thickness"), currentShadowLight->flareSize * 1.5);
	
	// Color Fade
	VectorSubtract(currentShadowLight->flareOrigin, r_origin, v);
	dist = VectorLength(v) * (currentShadowLight->flareSize * 0.01);
	dist2 = VectorLength(v);

	scale = ((1024 - dist2) / 1024) * 0.5;
	if(r_lightScale->value)
		scale /= r_lightScale->value;

	VectorScale(currentShadowLight->color, scale, tmp);

	VectorMA (currentShadowLight->flareOrigin, -1-dist, vup, vert_array[flareVert+0]);
	VectorMA (vert_array[flareVert+0], 1+dist, vright, vert_array[flareVert+0]);
	VA_SetElem2(tex_array[flareVert+0], 0, 1);
	VA_SetElem4(color_array[flareVert+0], tmp[0],tmp[1],tmp[2], 1);

	VectorMA (currentShadowLight->flareOrigin, -1-dist, vup, vert_array[flareVert+1]);
	VectorMA (vert_array[flareVert+1], -1-dist, vright, vert_array[flareVert+1]);
    VA_SetElem2(tex_array[flareVert+1], 0, 0);
	VA_SetElem4(color_array[flareVert+1], tmp[0],tmp[1],tmp[2], 1);

    VectorMA (currentShadowLight->flareOrigin, 1+dist, vup, vert_array[flareVert+2]);
	VectorMA (vert_array[flareVert+2], -1-dist, vright, vert_array[flareVert+2]);
    VA_SetElem2(tex_array[flareVert+2], 1, 0);
	VA_SetElem4(color_array[flareVert+2], tmp[0],tmp[1],tmp[2], 1);

	VectorMA (currentShadowLight->flareOrigin, 1+dist, vup, vert_array[flareVert+3]);
	VectorMA (vert_array[flareVert+3], 1+dist, vright, vert_array[flareVert+3]);
    VA_SetElem2(tex_array[flareVert+3], 1, 1);
	VA_SetElem4(color_array[flareVert+3], tmp[0],tmp[1],tmp[2], 1);
	
	flareIndex[index++] = flareVert+0;
	flareIndex[index++] = flareVert+1;
	flareIndex[index++] = flareVert+3;
	flareIndex[index++] = flareVert+3;
	flareIndex[index++] = flareVert+1;
	flareIndex[index++] = flareVert+2;
			
	flareVert+=4;
		
	if(flareVert)
	{
		if(gl_state.DrawRangeElements && r_DrawRangeElements->value)
			qglDrawRangeElementsEXT(GL_TRIANGLES, 0, flareVert, index, GL_UNSIGNED_INT, flareIndex);
		else
			qglDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, flareIndex);
	
	flareVert = 0;
	index = 0;
	}

	GL_BindNullProgram();
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		qglEnable(GL_DEPTH_BOUNDS_TEST_EXT);

	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
    qglDisableVertexAttribArray(ATRB_COLOR);
	GL_SelectTexture(GL_TEXTURE0_ARB);
}

void R_LightFlareOutLine(){ //flare editing highlights

	unsigned	flareIndex[MAX_INDICES], defBits = 0;
	int			flareVert=0, index=0, id;
	vec3_t		vert_array[MAX_FLARES_VERTEX], v[8], tmpOrg;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (!currentShadowLight->flare)
		return;

	if(!flareEdit)
		return;
		
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
	qglDisable(GL_DEPTH_BOUNDS_TEST_EXT);
	qglDisable(GL_SCISSOR_TEST);
	qglDisable(GL_STENCIL_TEST);
	qglDisable(GL_TEXTURE_2D);
	qglDisable(GL_CULL_FACE);
	qglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	qglEnable(GL_LINE_SMOOTH);
	qglLineWidth(3.0);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, vert_array);

	// setup program
	GL_BindProgram(genericProgram, defBits);
	id = genericProgram->id[defBits];
	qglUniform3f(qglGetUniformLocation(id, "u_color"),	currentShadowLight->color[0],
														currentShadowLight->color[1], 
														currentShadowLight->color[2]);
	
	// draw flare polygon
	VectorMA (currentShadowLight->flareOrigin, -1-currentShadowLight->flareSize, vup, vert_array[flareVert+0]);
	VectorMA (vert_array[flareVert+0], 1+currentShadowLight->flareSize, vright, vert_array[flareVert+0]);

	VectorMA (currentShadowLight->flareOrigin, -1-currentShadowLight->flareSize, vup, vert_array[flareVert+1]);
	VectorMA (vert_array[flareVert+1], -1-currentShadowLight->flareSize, vright, vert_array[flareVert+1]);

    VectorMA (currentShadowLight->flareOrigin, 1+currentShadowLight->flareSize, vup, vert_array[flareVert+2]);
	VectorMA (vert_array[flareVert+2], -1-currentShadowLight->flareSize, vright, vert_array[flareVert+2]);

	VectorMA (currentShadowLight->flareOrigin, 1+currentShadowLight->flareSize, vup, vert_array[flareVert+3]);
	VectorMA (vert_array[flareVert+3], 1+currentShadowLight->flareSize, vright, vert_array[flareVert+3]);
	
	flareIndex[index++] = flareVert+0;
	flareIndex[index++] = flareVert+1;
	flareIndex[index++] = flareVert+3;
	flareIndex[index++] = flareVert+3;
	flareIndex[index++] = flareVert+1;
	flareIndex[index++] = flareVert+2;
			
	flareVert+=4;
	
	if(flareVert)
	{
	if(gl_state.DrawRangeElements && r_DrawRangeElements->value)
		qglDrawRangeElementsEXT(GL_TRIANGLES, 0, flareVert, index, GL_UNSIGNED_INT, flareIndex);
	else
		qglDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, flareIndex);

	flareVert = 0;
	index = 0;
	}
	// draw light to flare connector
	VA_SetElem3(vert_array[0],currentShadowLight->origin[0], currentShadowLight->origin[1], currentShadowLight->origin[2]);
	VA_SetElem3(vert_array[1],currentShadowLight->flareOrigin[0], currentShadowLight->flareOrigin[1], currentShadowLight->flareOrigin[2]);
	qglDrawArrays (GL_LINES, 0, 2);

	qglDisable(GL_LINE_SMOOTH);
	qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// draw center of flare
	VectorCopy(currentShadowLight->flareOrigin, tmpOrg);
	VectorSet(v[0], tmpOrg[0]-1, tmpOrg[1]-1, tmpOrg[2]-1);
	VectorSet(v[1], tmpOrg[0]-1, tmpOrg[1]-1, tmpOrg[2]+1);
	VectorSet(v[2], tmpOrg[0]-1, tmpOrg[1]+1, tmpOrg[2]-1);
	VectorSet(v[3], tmpOrg[0]-1, tmpOrg[1]+1, tmpOrg[2]+1);
	VectorSet(v[4], tmpOrg[0]+1, tmpOrg[1]-1, tmpOrg[2]-1);
	VectorSet(v[5], tmpOrg[0]+1, tmpOrg[1]-1, tmpOrg[2]+1);
	VectorSet(v[6], tmpOrg[0]+1, tmpOrg[1]+1, tmpOrg[2]-1);
	VectorSet(v[7], tmpOrg[0]+1, tmpOrg[1]+1, tmpOrg[2]+1);


	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[4]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3fv(v[3]);
	qglVertex3fv(v[0]);
	qglVertex3fv(v[1]);
	qglVertex3fv(v[5]);
	qglVertex3fv(v[7]);
	qglVertex3fv(v[6]);
	qglVertex3fv(v[2]);
	qglVertex3fv(v[0]);
	qglEnd();

	GL_BindNullProgram();
	
	if(r_useLightScissors->value)
		qglEnable(GL_SCISSOR_TEST);
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		qglEnable(GL_DEPTH_BOUNDS_TEST_EXT);
	qglEnable(GL_STENCIL_TEST);
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_CULL_FACE);
	qglDisableVertexAttribArray(ATRB_POSITION);
}