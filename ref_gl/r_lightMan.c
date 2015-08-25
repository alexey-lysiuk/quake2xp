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
qboolean R_MarkLightLeaves (worldShadowLight_t *light);
void R_DrawBspModelVolumes (qboolean precalc, worldShadowLight_t *light);
void R_LightFlareOutLine ();

qboolean R_AddLightToFrame (worldShadowLight_t *light, qboolean weapon) {

	if (light->startColor[0] <= 0.01 && light->startColor[1] <= 0.01 && light->startColor[0] <= 0.01 && !r_lightEditor->value)
		return qfalse;

	if (light->spherical) {

		if (R_CullSphere(light->origin, light->radius[0]))
			return qfalse;
	}
	else {
		if (R_CullBox(light->mins, light->maxs))
			return qfalse;
	}
	if (weapon) {
		if (!BoundsAndSphereIntersect (light->mins, light->maxs, r_origin, currententity->model->radius))
			return qfalse;
	}

	if (r_newrefdef.areabits) {
		if (!(r_newrefdef.areabits[light->area >> 3] & (1 << (light->area & 7)))) {
			return qfalse;
		}
	}
	if (!HasSharedLeafs(light->vis, viewvis))
		return qfalse;
	
	return qtrue;
}

void UpdateLightBounds (worldShadowLight_t *light) {
	int i;
	mat3_t lightAxis;
	mat4_t temp;
	vec3_t tmp;

	if (light->radius[0] == light->radius[1] && light->radius[0] == light->radius[2])
		light->spherical = qtrue;
	else
		light->spherical = qfalse;

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - light->radius[i];
		light->maxs[i] = light->origin[i] + light->radius[i];
	}

	if (light->spherical) 
		light->maxRad = light->radius[0];
	else 
		light->maxRad = max(max(light->radius[0], light->radius[1]), light->radius[2]);
	
	for (i = 0; i < 8; i++) {
		tmp[0] = (i & 1) ? -light->radius[0] : light->radius[0];
		tmp[1] = (i & 2) ? -light->radius[1] : light->radius[1];
		tmp[2] = (i & 4) ? -light->radius[2] : light->radius[2];

		AnglesToMat3 (light->angles, lightAxis);
		Mat3_TransposeMultiplyVector (lightAxis, tmp, light->corners[i]);
		VectorAdd (light->corners[i], light->origin, light->corners[i]);
	}

	Mat4_Identity (temp);
	Mat4_Translate (temp, 0.5, 0.5, 0.5);
	Mat4_Scale (temp, 0.5 / light->radius[0], 0.5 / light->radius[1], 0.5 / light->radius[2]);
	Mat4_Translate (temp, -light->origin[0], -light->origin[1], -light->origin[2]);
	Mat4_Copy (temp, light->attenMapMatrix);
}


void R_AddDynamicLight (dlight_t *dl) {

	worldShadowLight_t *light;
	mat3_t lightAxis;
	mat4_t temp;
	vec3_t tmp;
	int i;

	if (R_CullSphere (dl->origin, dl->intensity))
		return;

	light = &shadowLightsBlock[num_dlits++];
	memset (light, 0, sizeof(worldShadowLight_t));
	light->next = shadowLight_frame;
	shadowLight_frame = light;

	VectorCopy (dl->origin, light->origin);
	VectorCopy (dl->color, light->startColor);
	VectorCopy (dl->angles, light->angles);
	VectorSet (light->radius, dl->intensity, dl->intensity, dl->intensity);

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
	light->spherical = qtrue;
	light->maxRad = dl->intensity;

	for (i = 0; i < 8; i++) {
		tmp[0] = (i & 1) ? -light->radius[0] : light->radius[0];
		tmp[1] = (i & 2) ? -light->radius[1] : light->radius[1];
		tmp[2] = (i & 4) ? -light->radius[2] : light->radius[2];

		AnglesToMat3(light->angles, lightAxis);
		Mat3_TransposeMultiplyVector(lightAxis, tmp, light->corners[i]);
		VectorAdd(light->corners[i], light->origin, light->corners[i]);
	}

	Mat4_Identity(temp);
	Mat4_Translate(temp, 0.5, 0.5, 0.5);
	Mat4_Scale(temp, 0.5 / light->radius[0], 0.5 / light->radius[1], 0.5 / light->radius[2]);
	Mat4_Translate(temp, -light->origin[0], -light->origin[1], -light->origin[2]);
	Mat4_Copy(temp, light->attenMapMatrix);
}

void R_AddNoWorldModelLight () {

	worldShadowLight_t *light;
	mat4_t temp;
	int i;

	light = &shadowLightsBlock[num_nwmLights++];
	memset (light, 0, sizeof(worldShadowLight_t));
	light->next = shadowLight_frame;
	shadowLight_frame = light;
	VectorSet (light->origin, r_newrefdef.vieworg[0]-500.0, r_newrefdef.vieworg[1], r_newrefdef.vieworg[2] + 256);
	VectorSet (light->startColor, 1.0, 1.0, 0.666);
	VectorSet (light->angles, 0, 0, 0);
	VectorSet (light->radius, 1500, 1500, 1500);

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - 256;
		light->maxs[i] = light->origin[i] + 256;
	}

	light->style = 0;
	light->filter = 0;
	light->isStatic = 1;
	light->isShadow = 0;
	light->_cone = 0;
	light->isNoWorldModel = 1;
	light->flare = 0;
	light->isAmbient = 0;
	light->spherical = qtrue;
	light->maxRad = light->radius[0];

	Mat4_Identity(temp);
	Mat4_Translate(temp, 0.5, 0.5, 0.5);
	Mat4_Scale(temp, 0.5 / light->radius[0], 0.5 / light->radius[1], 0.5 / light->radius[2]);
	Mat4_Translate(temp, -light->origin[0], -light->origin[1], -light->origin[2]);
	Mat4_Copy(temp, light->attenMapMatrix);
}


void R_PrepareShadowLightFrame (qboolean weapon) {

	int i;
	worldShadowLight_t *light;

	num_dlits = 0;
	num_nwmLights = 0;
	shadowLight_frame = NULL;

	// add pre computed lights
	if (shadowLight_static) {
		for (light = shadowLight_static; light; light = light->s_next) {

			if (!R_AddLightToFrame (light, weapon))
				continue;
			light->next = shadowLight_frame;
			shadowLight_frame = light;
		}
	}

	// add tempory lights
	for (i = 0; i<r_newrefdef.num_dlights; i++) {

		if (num_dlits > MAX_WORLD_SHADOW_LIHGTS)
			break;

		R_AddDynamicLight (&r_newrefdef.dlights[i]);
	}

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		R_AddNoWorldModelLight ();

	if (!shadowLight_frame)
		return;

	for (light = shadowLight_frame; light; light = light->next) {

		if (!light->isStatic)
		if (!R_MarkLightLeaves (light))
			return;

		MakeFrustum4Light (light, qtrue);

		if (CL_PMpointcontents (light->origin) & MASK_WATER)
			light->castCaustics = qfalse;
		else
			light->castCaustics = qtrue;

		VectorCopy (light->startColor, light->color);

		if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
			continue;

		light->color[0] *= r_newrefdef.lightstyles[light->style].rgb[0];
		light->color[1] *= r_newrefdef.lightstyles[light->style].rgb[1];
		light->color[2] *= r_newrefdef.lightstyles[light->style].rgb[2];
	}


}

void R_SaveLights_f (void) {

	char	name[MAX_QPATH], path[MAX_QPATH];
	FILE	*f;
	int		i = 0;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!shadowLight_static)
		return;

	FS_StripExtension (r_worldmodel->name, name, sizeof (name));
	Com_sprintf (path, sizeof(path), "%s/%s.xplit", FS_Gamedir (), name);
	remove (path); //remove prev version

	f = fopen (path, "w");
	if (!f) {
		Com_Printf ("Could not open %s.\n", path);
		return;
	}

	fprintf (f, "//ReLight for %s\n//Generated by quake2xp\n\n", r_worldmodel->name);

	for (currentShadowLight = shadowLight_static; currentShadowLight; currentShadowLight = currentShadowLight->s_next) {

		fprintf (f, "//Light %i\n", i);
		fprintf (f, "{\n");
		fprintf (f, "\"classname\" \"light\"\n");
		fprintf (f, "\"origin\" \"%i %i %i\"\n", (int)currentShadowLight->origin[0], (int)currentShadowLight->origin[1], (int)currentShadowLight->origin[2]);
		fprintf (f, "\"radius\" \"%i %i %i\"\n", (int)currentShadowLight->radius[0], (int)currentShadowLight->radius[1], (int)currentShadowLight->radius[2]);
		fprintf (f, "\"color\" \"%.3f %.3f %.3f\"\n", currentShadowLight->startColor[0], currentShadowLight->startColor[1], currentShadowLight->startColor[2]);
		fprintf (f, "\"style\" \"%i\"\n", (int)currentShadowLight->style);
		fprintf (f, "\"filter\" \"%i\"\n", (int)currentShadowLight->filter);
		fprintf (f, "\"angles\" \"%i %i %i\"\n", (int)currentShadowLight->angles[0], (int)currentShadowLight->angles[1], (int)currentShadowLight->angles[2]);
		fprintf (f, "\"speed\" \"%.3f %.3f %.3f\"\n", currentShadowLight->speed[0], currentShadowLight->speed[1], currentShadowLight->speed[2]);
		fprintf (f, "\"shadow\" \"%i\"\n", currentShadowLight->isShadow);
		fprintf (f, "\"ambient\" \"%i\"\n", currentShadowLight->isAmbient);
		fprintf (f, "\"_cone\" \"%.1f\"\n", currentShadowLight->_cone);
		if (currentShadowLight->targetname[0])
			fprintf (f, "\"targetname\" \"%s\"\n", currentShadowLight->targetname);
		fprintf (f, "\"spawnflags\" \"%i\"\n", currentShadowLight->start_off);
		fprintf (f, "\"flareSize\" \"%i\"\n", (int)currentShadowLight->flareSize);
		fprintf (f, "\"flareOrigin\" \"%i %i %i\"\n", (int)currentShadowLight->flareOrigin[0], (int)currentShadowLight->flareOrigin[1], (int)currentShadowLight->flareOrigin[2]);
		fprintf (f, "\"flare\" \"%i\"\n", currentShadowLight->flare);
		if (currentShadowLight->isFog) {
			fprintf (f, "\"fogLight\" \"%i\"\n", currentShadowLight->isFog);
			fprintf (f, "\"fogDensity\" \"%.6f\"\n", currentShadowLight->fogDensity);
		}
		fprintf (f, "}\n");
		i++;
	}

	fclose (f);

	Com_Printf (""S_COLOR_MAGENTA"SaveLights_f: "S_COLOR_WHITE"Save lights to "S_COLOR_GREEN"%s.xplit\n"S_COLOR_WHITE"Save "S_COLOR_GREEN"%i"S_COLOR_WHITE" lights\n", name, i);

}

static void DeleteCurrentLight (worldShadowLight_t *l) {
	worldShadowLight_t *light;

	if (l == shadowLight_static) {
		shadowLight_static = l->s_next;
	}
	else {
		for (light = shadowLight_static; light; light = light->s_next) {
			if (light->s_next == l) {
				light->s_next = l->s_next;
				break;
			}
		}
	}
	free (l);
}

void R_Light_Spawn_f (void) {
	vec3_t	color = { 1.0, 1.0, 1.0 }, end, spawn, radius = { 300, 300, 300 };
	trace_t trace;
	char	target[MAX_QPATH];

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	memset (target, 0, sizeof(target));

	VectorMA (player_org, 1024, v_forward, end);
	trace = CL_PMTraceWorld (player_org, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);

	if (trace.fraction != 1.0) {
		VectorMA (trace.endpos, -10, v_forward, spawn);
		R_AddNewWorldLight (spawn, color, radius, 0, 0, vec3_origin, vec3_origin, qtrue, 1, 0, 0, qtrue, 0, spawn, 10.0, target, 0, 0, 0.0);
	}
}

void R_Light_SpawnToCamera_f (void) {
	vec3_t	color = { 1.0, 1.0, 1.0 }, radius = { 300, 300, 300 };
	char	target[MAX_QPATH];

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}
	memset (target, 0, sizeof(target));
	R_AddNewWorldLight (player_org, color, radius, 0, 0, vec3_origin, vec3_origin, qtrue, 1, 0, 0, qtrue, 0, player_org, 10.0, target, 0, 0, 0.0);
}

void R_Light_Clone_f (void) {
	vec3_t	color, spawn, origin, angles,
		speed, radius, flareOrg, end;
	float	_cone, flareSize, fogDensity;
	int		style, filter, shadow, ambient,
		flare, flag, fog;
	char	target[MAX_QPATH];
	trace_t trace;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	VectorCopy (selectedShadowLight->origin, origin);
	VectorCopy (selectedShadowLight->color, color);
	VectorCopy (selectedShadowLight->angles, angles);
	VectorCopy (selectedShadowLight->speed, speed);
	VectorCopy (selectedShadowLight->radius, radius);
	VectorCopy (selectedShadowLight->flareOrigin, flareOrg);

	memset (target, 0, sizeof(target));
	if (selectedShadowLight->targetname[0])
		memcpy (target, selectedShadowLight->targetname, sizeof(target));

	style = selectedShadowLight->style;
	filter = selectedShadowLight->filter;
	shadow = selectedShadowLight->isShadow;
	ambient = selectedShadowLight->isAmbient;
	_cone = selectedShadowLight->_cone;
	flare = selectedShadowLight->flare;
	flareSize = selectedShadowLight->flareSize;
	flag = selectedShadowLight->start_off;
	fog = selectedShadowLight->isFog;
	fogDensity = selectedShadowLight->fogDensity;
	VectorMA (player_org, 1024, v_forward, end);

	trace = CL_PMTraceWorld (player_org, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction != 1.0) {
		VectorMA (trace.endpos, -10, v_forward, spawn);
		selectedShadowLight = R_AddNewWorldLight (spawn, color, radius, style, filter, angles, vec3_origin, qtrue, shadow, ambient, _cone, qtrue, flare, flareOrg, flareSize, target, flag, fog, fogDensity);
	}
}

/*===========================
Copy - Paste Light Properties
===========================*/

typedef struct {
	float	angles[3];
	float	speed[3];
	float	color[3];
	float	radius[3];
	float	fogDensity;
	float	flareSize;
	int		flare;
	int		style;
	float	cone;
	int		filter;
	int		isShadow;
	int		isAmbient;
	int		isFog;
	int		start_off;
	char	targetname[MAX_QPATH];
}lightClipboard_t;

lightClipboard_t lightClipBoard;

void R_Copy_Light_Properties_f (void) {

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	memset (&lightClipBoard, 0, sizeof(lightClipboard_t));

	VectorCopy (selectedShadowLight->color, lightClipBoard.color);
	VectorCopy (selectedShadowLight->angles, lightClipBoard.angles);
	VectorCopy (selectedShadowLight->speed, lightClipBoard.speed);
	VectorCopy (selectedShadowLight->radius, lightClipBoard.radius);

	memset (lightClipBoard.targetname, 0, sizeof(lightClipBoard.targetname));
	if (selectedShadowLight->targetname[0])
		memcpy (lightClipBoard.targetname, selectedShadowLight->targetname, sizeof(lightClipBoard.targetname));

	lightClipBoard.style = selectedShadowLight->style;
	lightClipBoard.filter = selectedShadowLight->filter;
	lightClipBoard.isShadow = selectedShadowLight->isShadow;
	lightClipBoard.isAmbient = selectedShadowLight->isAmbient;
	lightClipBoard.cone = selectedShadowLight->_cone;
	lightClipBoard.flare = selectedShadowLight->flare;
	lightClipBoard.flareSize = selectedShadowLight->flareSize;
	lightClipBoard.start_off = selectedShadowLight->start_off;
	lightClipBoard.isFog = selectedShadowLight->isFog;
	lightClipBoard.fogDensity = selectedShadowLight->fogDensity;

	Com_Printf ("Copy light properties to clipboard.\n");
}

void R_Paste_Light_Properties_f (void) {

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!lightClipBoard.radius[0]) {
		Com_Printf ("No light clipboard data.\n");
		return;
	}

	VectorCopy (lightClipBoard.color, selectedShadowLight->startColor);
	VectorCopy (lightClipBoard.angles, selectedShadowLight->angles);
	VectorCopy (lightClipBoard.speed, selectedShadowLight->speed);
	VectorCopy (lightClipBoard.radius, selectedShadowLight->radius);

	if (lightClipBoard.targetname[0])
		memcpy (selectedShadowLight->targetname, lightClipBoard.targetname, sizeof(selectedShadowLight->targetname));

	selectedShadowLight->style = lightClipBoard.style;
	selectedShadowLight->filter = lightClipBoard.filter;
	selectedShadowLight->isShadow = lightClipBoard.isShadow;
	selectedShadowLight->isAmbient = lightClipBoard.isAmbient;
	selectedShadowLight->_cone = lightClipBoard.cone;
	selectedShadowLight->flare = lightClipBoard.flare;
	selectedShadowLight->flareSize = lightClipBoard.flareSize;
	selectedShadowLight->start_off = lightClipBoard.start_off;
	selectedShadowLight->isFog = lightClipBoard.isFog;
	selectedShadowLight->fogDensity = lightClipBoard.fogDensity;

	UpdateLightBounds (selectedShadowLight);
	R_MarkLightLeaves (selectedShadowLight);
	R_DrawBspModelVolumes (qtrue, selectedShadowLight);

	Com_Printf ("Paste light properties from clipboard.\n");
}

void R_EditSelectedLight_f (void) {

	vec3_t	color, origin, angles,
		speed, radius, fOrg;
	float	_cone, fSize, fogDensity;
	int		style, filter, shadow,
		ambient, flare, start_off, fog;
	char	target[MAX_QPATH];

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	VectorCopy (selectedShadowLight->origin, origin);
	VectorCopy (selectedShadowLight->color, color);
	VectorCopy (selectedShadowLight->angles, angles);
	VectorCopy (selectedShadowLight->radius, radius);
	VectorCopy (selectedShadowLight->flareOrigin, fOrg);

	if (selectedShadowLight->targetname[0]) {
		memset (target, 0, sizeof(target));
		memcpy (target, selectedShadowLight->targetname, sizeof(target));
	}

	style = selectedShadowLight->style;
	filter = selectedShadowLight->filter;
	shadow = selectedShadowLight->isShadow;
	ambient = selectedShadowLight->isAmbient;
	_cone = selectedShadowLight->_cone;
	flare = selectedShadowLight->flare;
	fSize = selectedShadowLight->flareSize;
	start_off = selectedShadowLight->start_off;
	fog = selectedShadowLight->isFog;
	fogDensity = selectedShadowLight->fogDensity;

	if (!strcmp (Cmd_Argv (1), "origin")) {
		if (Cmd_Argc () != 5) {
			Com_Printf ("usage: editLight: %s X Y Z\nCurrent Origin: %.4f %.4f %.4f\n", Cmd_Argv (0),
				selectedShadowLight->origin[0],
				selectedShadowLight->origin[1],
				selectedShadowLight->origin[2]);
			return;
		}
		origin[0] = atof (Cmd_Argv (2));
		origin[1] = atof (Cmd_Argv (3));
		origin[2] = atof (Cmd_Argv (4));
		VectorCopy (origin, selectedShadowLight->origin);
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}
	else
	if (!strcmp (Cmd_Argv (1), "color")) {
		if (Cmd_Argc () != 5) {
			Com_Printf ("usage: editLight: %s Red Green Blue\nCurrent Color: %.4f %.4f %.4f\n", Cmd_Argv (0),
				selectedShadowLight->color[0],
				selectedShadowLight->color[1],
				selectedShadowLight->color[2]);
			return;
		}
		color[0] = atof (Cmd_Argv (2));
		color[1] = atof (Cmd_Argv (3));
		color[2] = atof (Cmd_Argv (4));
		VectorCopy (color, selectedShadowLight->startColor);
	}
	else
	if (!strcmp (Cmd_Argv (1), "speed")) {
		if (Cmd_Argc () != 5) {
			Com_Printf ("usage: editLight: %s X rotate speed Y rotate speed Z rotate speed\nCurrent speed rotations: %.4f %.4f %.4f\n", Cmd_Argv (0),
				selectedShadowLight->speed[0],
				selectedShadowLight->speed[1],
				selectedShadowLight->speed[2]);
			return;
		}
		speed[0] = atof (Cmd_Argv (2));
		speed[1] = atof (Cmd_Argv (3));
		speed[2] = atof (Cmd_Argv (4));
		VectorCopy (speed, selectedShadowLight->speed);
	}
	else
	if (!strcmp (Cmd_Argv (1), "radius")) {
		if (Cmd_Argc () != 5) {
			Com_Printf ("usage: editLight: %s value\nCurrent Radius: %.1f %.1f %.1f\n", Cmd_Argv (0),
				selectedShadowLight->radius[0], selectedShadowLight->radius[1], selectedShadowLight->radius[2]);
			return;
		}
		radius[0] = atof (Cmd_Argv (2));
		radius[1] = atof (Cmd_Argv (3));
		radius[2] = atof (Cmd_Argv (4));
		VectorCopy (radius, selectedShadowLight->radius);
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}
	else
	if (!strcmp (Cmd_Argv (1), "cone")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight: %s value\nCurrent Light Cone: %.1f\n", Cmd_Argv (0),
				selectedShadowLight->_cone);
			return;
		}
		_cone = atof (Cmd_Argv (2));
		selectedShadowLight->_cone = _cone;
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}
	else
	if (!strcmp (Cmd_Argv (1), "style")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Style %i\n", Cmd_Argv (0),
				selectedShadowLight->style);
			return;
		}
		style = atoi (Cmd_Argv (2));
		selectedShadowLight->style = style;
	}
	else
	if (!strcmp (Cmd_Argv (1), "filter")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Cube Filter %i\n", Cmd_Argv (0),
				selectedShadowLight->filter);
			return;
		}
		filter = atoi (Cmd_Argv (2));
		selectedShadowLight->filter = filter;
	}
	else
	if (!strcmp (Cmd_Argv (1), "angles")) {
		if (Cmd_Argc () != 5) {
			Com_Printf ("usage: editLight: %s X Y Z \nCurrent Angles: %.4f %.4f %.4f\n", Cmd_Argv (0),
				selectedShadowLight->angles[0],
				selectedShadowLight->angles[1],
				selectedShadowLight->angles[2]);
			return;
		}
		angles[0] = atof (Cmd_Argv (2));
		angles[1] = atof (Cmd_Argv (3));
		angles[2] = atof (Cmd_Argv (4));
		VectorCopy (angles, selectedShadowLight->angles);
	}
	else
	if (!strcmp (Cmd_Argv (1), "shadow")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Shadow Flag is %i\n", Cmd_Argv (0),
				selectedShadowLight->isShadow);
			return;
		}
		shadow = atoi (Cmd_Argv (2));
		selectedShadowLight->isShadow = shadow;
	}
	else
	if (!strcmp (Cmd_Argv (1), "ambient")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Ambient Flag is %i\n", Cmd_Argv (0),
				selectedShadowLight->isAmbient);
			return;
		}
		ambient = atoi (Cmd_Argv (2));
		selectedShadowLight->isAmbient = ambient;
	}
	else
	if (!strcmp (Cmd_Argv (1), "flare")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Flare Flag is %i\n", Cmd_Argv (0),
				selectedShadowLight->flare);
			return;
		}
		flare = atoi (Cmd_Argv (2));
		selectedShadowLight->flare = flare;
		VectorCopy (selectedShadowLight->origin, selectedShadowLight->flareOrigin);
	}
	else
	if (!strcmp (Cmd_Argv (1), "flareSize")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Flare Size is %i\n", Cmd_Argv (0),
				selectedShadowLight->flareSize);
			return;
		}
		fSize = atoi (Cmd_Argv (2));
		selectedShadowLight->flareSize = fSize;
	}
	else
	if (!strcmp (Cmd_Argv (1), "flareOrigin")) {
		if (Cmd_Argc () != 5) {
			Com_Printf ("usage: editLight: %s X Y Z\nCurrent Flare Origin: %.4f %.4f %.4f\n", Cmd_Argv (0),
				selectedShadowLight->flareOrigin[0],
				selectedShadowLight->flareOrigin[1],
				selectedShadowLight->flareOrigin[2]);
			return;
		}
		fOrg[0] = atof (Cmd_Argv (2));
		fOrg[1] = atof (Cmd_Argv (3));
		fOrg[2] = atof (Cmd_Argv (4));
		VectorCopy (fOrg, selectedShadowLight->flareOrigin);
	}
	else
	if (!strcmp (Cmd_Argv (1), "target")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight: %s value\nCurrent Light Target: %s\n", Cmd_Argv (0),
				selectedShadowLight->targetname);
			return;
		}
		memcpy (selectedShadowLight->targetname, Cmd_Argv (2), sizeof(selectedShadowLight->targetname));
	}
	else
	if (!strcmp (Cmd_Argv (1), "start_off")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Start Off Flag is %i\n", Cmd_Argv (0),
				selectedShadowLight->start_off);
			return;
		}
		start_off = atoi (Cmd_Argv (2));
		selectedShadowLight->start_off = start_off;
	}
	else
	if (!strcmp (Cmd_Argv (1), "fog")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Fog Light Flag is %i\n", Cmd_Argv (0),
				selectedShadowLight->isFog);
			return;
		}
		fog = atoi (Cmd_Argv (2));
		selectedShadowLight->isFog = fog;
	}
	else
	if (!strcmp (Cmd_Argv (1), "fogDensity")) {
		if (Cmd_Argc () != 3) {
			Com_Printf ("usage: editLight %s value\nCurrent Fog Light Flag is %f\n", Cmd_Argv (0),
				selectedShadowLight->fogDensity);
			return;
		}
		fogDensity = atof (Cmd_Argv (2));
		selectedShadowLight->fogDensity = fogDensity;
	}
}

qboolean flareEdit;

void R_FlareEdit_f (void) {
	int mode;

	if (Cmd_Argc () != 2) {
		Com_Printf ("Toggle Flare Editing Mode.\nUsage: editFlare: 0 or 1\n");
		return;
	}
	mode = atoi (Cmd_Argv (1));
	if (mode > 0)
		flareEdit = qtrue;
	else
		flareEdit = qfalse;
}

void R_ResetFlarePos_f (void) {

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	VectorCopy (selectedShadowLight->origin, selectedShadowLight->flareOrigin);

}

void R_MoveLightToRight_f (void) {

	vec3_t	origin;
	float	offset;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	if (Cmd_Argc () != 2) {
		Com_Printf ("Usage: moveLight_right: <value>\n");
		return;
	}

	offset = atof (Cmd_Argv (1));

	if (flareEdit)
		VectorCopy (selectedShadowLight->flareOrigin, origin);
	else
		VectorCopy (selectedShadowLight->origin, origin);

	if (r_CameraSpaceLightMove->value)
		VectorMA (origin, offset, v_right, origin);
	else
		origin[0] += offset;

	if (flareEdit)
		VectorCopy (origin, selectedShadowLight->flareOrigin);
	else {
		VectorCopy (origin, selectedShadowLight->origin);
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}
}

void R_MoveLightForward_f (void) {

	vec3_t origin;
	float  offset, fix;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	if (Cmd_Argc () != 2) {
		Com_Printf ("Usage: moveLight_forward: <value>\n");
		return;
	}

	offset = atof (Cmd_Argv (1));

	if (flareEdit)
		VectorCopy (selectedShadowLight->flareOrigin, origin);
	else
		VectorCopy (selectedShadowLight->origin, origin);

	if (r_CameraSpaceLightMove->value) {
		fix = origin[2];
		VectorMA (origin, offset, v_forward, origin);
		origin[2] = fix;
	}
	else
		origin[1] += offset;

	if (flareEdit)
		VectorCopy (origin, selectedShadowLight->flareOrigin);
	else {
		VectorCopy (origin, selectedShadowLight->origin);
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}
}

void R_MoveLightUpDown_f (void) {

	vec3_t	origin;
	float	offset;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}


	if (Cmd_Argc () != 2) {
		Com_Printf ("Usage: moveSelectedLight_right: <value>\n");
		return;
	}

	if (flareEdit)
		VectorCopy (selectedShadowLight->flareOrigin, origin);
	else
		VectorCopy (selectedShadowLight->origin, origin);

	offset = atof (Cmd_Argv (1));
	origin[2] += offset;

	if (flareEdit)
		VectorCopy (origin, selectedShadowLight->flareOrigin);
	else {
		VectorCopy (origin, selectedShadowLight->origin);
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}
}

void R_ChangeLightRadius_f (void) {

	float	offset, fRad;
	vec3_t	rad;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	if (Cmd_Argc () != 2) {
		Com_Printf ("Usage: ajustSelectedLightRadius: X Y Z\n");
		return;
	}
	if (flareEdit) {
		fRad = selectedShadowLight->flareSize;
		offset = atof (Cmd_Argv (1));
		fRad += offset;
		selectedShadowLight->flareSize = fRad;
	}
	else {
		VectorCopy (selectedShadowLight->radius, rad);

		offset = atof (Cmd_Argv (1));

		rad[0] += offset;
		rad[1] += offset;
		rad[2] += offset;

		if (rad[0] < 10)
			rad[0] = 10;

		if (rad[1] < 10)
			rad[1] = 10;

		if (rad[2] < 10)
			rad[2] = 10;

		VectorCopy (rad, selectedShadowLight->radius);
		UpdateLightBounds (selectedShadowLight);
		R_MarkLightLeaves (selectedShadowLight);
		R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	}

}

void R_ChangeLightCone_f (void) {

	float cone, offset;

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	if (Cmd_Argc () != 2) {
		Com_Printf ("Usage: ajustSelectedLightRadius: <value>\n");
		return;
	}

	cone = selectedShadowLight->_cone;

	offset = atof (Cmd_Argv (1));

	cone += offset;
	if (cone > 999999)
		cone = 999999;

	if (cone < 0)
		cone = 0;

	selectedShadowLight->_cone = cone;
	R_DrawBspModelVolumes (qtrue, selectedShadowLight);
	R_MarkLightLeaves (selectedShadowLight);
	UpdateLightBounds (selectedShadowLight);
}



void R_Light_Delete_f (void) {

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
		return;
	}

	DeleteCurrentLight (selectedShadowLight);
	selectedShadowLight = NULL;
}

void R_Light_UnSelect_f (void) {

	if (!r_lightEditor->value) {
		Com_Printf ("Type r_lightEditor 1 to enable light editing.\n");
		return;
	}

	if (!selectedShadowLight) {
		Com_Printf ("No selected light.\n");
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
char buff14[128];
char buff15[128];


vec3_t	vCache[MAX_VERTEX_ARRAY];

void UpdateLightEditor (void) {

	vec3_t		end_trace, mins = { -5.0f, -5.0f, -5.0f },
		maxs = { 5.0f, 5.0f, 5.0f };
	vec3_t		corners[8], tmp;
	int			j;
	float		fraction = 1.0;
	vec3_t		v[8];
	trace_t		trace_light, trace_bsp;
	unsigned	headNode;
	vec3_t		tmpOrg, tmpRad;

	if (!r_lightEditor->value)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (!currentShadowLight->isStatic)
		return;

	GL_Disable (GL_SCISSOR_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable (GL_DEPTH_BOUNDS_TEST_EXT);

	GL_Disable (GL_BLEND);
	GL_Disable (GL_STENCIL_TEST);
	GL_Disable (GL_TEXTURE_2D);
	GL_Disable (GL_CULL_FACE);

	// stupid player camera and angles corruption, fixed
	VectorCopy (r_origin, player_org);
	AngleVectors (r_newrefdef.viewangles, v_forward, v_right, v_up);

	// create a temp hull from bounding box
	headNode = CM_HeadnodeForBox (mins, maxs);
	VectorMA (r_origin, 1024, v_forward, end_trace);

	trace_bsp = CL_PMTraceWorld (r_origin, vec3_origin, vec3_origin, end_trace, MASK_SOLID, qfalse); //bsp collision with bmodels

	// light in focus?
	trace_light = CM_TransformedBoxTrace (r_origin, trace_bsp.endpos, vec3_origin, vec3_origin, headNode, MASK_ALL,
		currentShadowLight->origin, vec3_origin); // find light

	if (trace_light.fraction < fraction) {
		selectedShadowLight = currentShadowLight;
		fraction = trace_light.fraction;
	}

	// setup program
	GL_BindProgram (genericProgram, 0);
	qglUniform1i (gen_attribColors, 0);
	qglUniform1i (gen_attribConsole, 0);
	qglUniform1i (gen_sky, 0);

	qglEnableVertexAttribArray (ATRB_POSITION);
	qglVertexAttribPointer (ATRB_POSITION, 3, GL_FLOAT, qfalse, 0, vCache);

	if (currentShadowLight != selectedShadowLight) {

		for (j = 0; j < 8; j++) {
			tmp[0] = (j & 1) ? -5.0f : 5.0f;
			tmp[1] = (j & 2) ? -5.0f : 5.0f;
			tmp[2] = (j & 4) ? -5.0f : 5.0f;

			VectorAdd (tmp, currentShadowLight->origin, corners[j]);
		}
		qglUniform4f (gen_color, currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2], 1.0);
		qglEnable (GL_LINE_SMOOTH);
		qglLineWidth (3.0);

		// top quad
		VA_SetElem3 (vCache[0], corners[0][0], corners[0][1], corners[0][2]);
		VA_SetElem3 (vCache[1], corners[1][0], corners[1][1], corners[1][2]);
		VA_SetElem3 (vCache[2], corners[2][0], corners[2][1], corners[2][2]);
		VA_SetElem3 (vCache[3], corners[3][0], corners[3][1], corners[3][2]);
		VA_SetElem3 (vCache[4], corners[3][0], corners[3][1], corners[3][2]);
		VA_SetElem3 (vCache[5], corners[1][0], corners[1][1], corners[1][2]);
		VA_SetElem3 (vCache[6], corners[2][0], corners[2][1], corners[2][2]);
		VA_SetElem3 (vCache[7], corners[0][0], corners[0][1], corners[0][2]);
		// bottom quad
		VA_SetElem3 (vCache[8], corners[4][0], corners[4][1], corners[4][2]);
		VA_SetElem3 (vCache[9], corners[5][0], corners[5][1], corners[5][2]);
		VA_SetElem3 (vCache[10], corners[6][0], corners[6][1], corners[6][2]);
		VA_SetElem3 (vCache[11], corners[7][0], corners[7][1], corners[7][2]);
		VA_SetElem3 (vCache[12], corners[7][0], corners[7][1], corners[7][2]);
		VA_SetElem3 (vCache[13], corners[5][0], corners[5][1], corners[5][2]);
		VA_SetElem3 (vCache[14], corners[6][0], corners[6][1], corners[6][2]);
		VA_SetElem3 (vCache[15], corners[4][0], corners[4][1], corners[4][2]);
		// connectors
		VA_SetElem3 (vCache[16], corners[0][0], corners[0][1], corners[0][2]);
		VA_SetElem3 (vCache[17], corners[4][0], corners[4][1], corners[4][2]);
		VA_SetElem3 (vCache[18], corners[1][0], corners[1][1], corners[1][2]);
		VA_SetElem3 (vCache[19], corners[5][0], corners[5][1], corners[5][2]);
		VA_SetElem3 (vCache[20], corners[2][0], corners[2][1], corners[2][2]);
		VA_SetElem3 (vCache[21], corners[6][0], corners[6][1], corners[6][2]);
		VA_SetElem3 (vCache[22], corners[3][0], corners[3][1], corners[3][2]);
		VA_SetElem3 (vCache[23], corners[7][0], corners[7][1], corners[7][2]);
		qglDrawArrays (GL_LINES, 0, 24);

		qglDisable (GL_LINE_SMOOTH);
	}

	if (selectedShadowLight) {

		qglUniform4f (gen_color, selectedShadowLight->color[0], selectedShadowLight->color[1], selectedShadowLight->color[2], 1.0);

		qglPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
		qglEnable (GL_LINE_SMOOTH);
		qglLineWidth (3.0);
		VectorCopy (selectedShadowLight->origin, tmpOrg);
		VectorCopy (selectedShadowLight->radius, tmpRad);

		sprintf (buff0, "Origin: %i %i %i", (int)selectedShadowLight->origin[0],
			(int)selectedShadowLight->origin[1],
			(int)selectedShadowLight->origin[2]);
		sprintf (buff1, "Color: %.3f %.3f %.3f", selectedShadowLight->color[0],
			selectedShadowLight->color[1],
			selectedShadowLight->color[2]);
		sprintf (buff2, "Radius: %i %i %i", (int)selectedShadowLight->radius[0],
			(int)selectedShadowLight->radius[1],
			(int)selectedShadowLight->radius[2]);
		sprintf (buff3, "Style: %i", selectedShadowLight->style);
		sprintf (buff4, "Filter: %i", selectedShadowLight->filter);
		sprintf (buff5, "Angles: %i %i %i", (int)selectedShadowLight->angles[0],
			(int)selectedShadowLight->angles[1],
			(int)selectedShadowLight->angles[2]);
		sprintf (buff6, "Speed: %.3f %.3f %.3f", selectedShadowLight->speed[0],
			selectedShadowLight->speed[1],
			selectedShadowLight->speed[2]);
		sprintf (buff7, "Shadow: %i", selectedShadowLight->isShadow);
		sprintf (buff8, "Ambient: %i", selectedShadowLight->isAmbient);
		sprintf (buff9, "Cone: %.2f", selectedShadowLight->_cone);
		sprintf (buff10, "Flare: %i; Flare Editing is %i",
			selectedShadowLight->flare, (int)flareEdit);
		sprintf (buff11, "Flare Size: %i", (int)selectedShadowLight->flareSize);
		sprintf (buff12, "Target Name: %s", selectedShadowLight->targetname);
		sprintf (buff13, "Start Off: %i", selectedShadowLight->start_off);
		sprintf (buff14, "Fog Light: %i", selectedShadowLight->isFog);
		sprintf (buff15, "Fog Density: %5f", selectedShadowLight->fogDensity);

		VectorSet (v[0], tmpOrg[0] - tmpRad[0], tmpOrg[1] - tmpRad[1], tmpOrg[2] - tmpRad[2]);
		VectorSet (v[1], tmpOrg[0] - tmpRad[0], tmpOrg[1] - tmpRad[1], tmpOrg[2] + tmpRad[2]);
		VectorSet (v[2], tmpOrg[0] - tmpRad[0], tmpOrg[1] + tmpRad[1], tmpOrg[2] - tmpRad[2]);
		VectorSet (v[3], tmpOrg[0] - tmpRad[0], tmpOrg[1] + tmpRad[1], tmpOrg[2] + tmpRad[2]);
		VectorSet (v[4], tmpOrg[0] + tmpRad[0], tmpOrg[1] - tmpRad[1], tmpOrg[2] - tmpRad[2]);
		VectorSet (v[5], tmpOrg[0] + tmpRad[0], tmpOrg[1] - tmpRad[1], tmpOrg[2] + tmpRad[2]);
		VectorSet (v[6], tmpOrg[0] + tmpRad[0], tmpOrg[1] + tmpRad[1], tmpOrg[2] - tmpRad[2]);
		VectorSet (v[7], tmpOrg[0] + tmpRad[0], tmpOrg[1] + tmpRad[1], tmpOrg[2] + tmpRad[2]);

		//front
		VA_SetElem3 (vCache[0], v[0][0], v[0][1], v[0][2]);
		VA_SetElem3 (vCache[1], v[1][0], v[1][1], v[1][2]);
		VA_SetElem3 (vCache[2], v[3][0], v[3][1], v[3][2]);
		VA_SetElem3 (vCache[3], v[0][0], v[0][1], v[0][2]);
		VA_SetElem3 (vCache[4], v[2][0], v[2][1], v[2][2]);
		VA_SetElem3 (vCache[5], v[3][0], v[3][1], v[3][2]);
		//right
		VA_SetElem3 (vCache[6], v[0][0], v[0][1], v[0][2]);
		VA_SetElem3 (vCache[7], v[4][0], v[4][1], v[4][2]);
		VA_SetElem3 (vCache[8], v[5][0], v[5][1], v[5][2]);
		VA_SetElem3 (vCache[9], v[0][0], v[0][1], v[0][2]);
		VA_SetElem3 (vCache[10], v[1][0], v[1][1], v[1][2]);
		VA_SetElem3 (vCache[11], v[5][0], v[5][1], v[5][2]);
		//bottom
		VA_SetElem3 (vCache[12], v[0][0], v[0][1], v[0][2]);
		VA_SetElem3 (vCache[13], v[4][0], v[4][1], v[4][2]);
		VA_SetElem3 (vCache[14], v[6][0], v[6][1], v[6][2]);
		VA_SetElem3 (vCache[15], v[0][0], v[0][1], v[0][2]);
		VA_SetElem3 (vCache[16], v[2][0], v[2][1], v[2][2]);
		VA_SetElem3 (vCache[17], v[6][0], v[6][1], v[6][2]);
		//top
		VA_SetElem3 (vCache[18], v[1][0], v[1][1], v[1][2]);
		VA_SetElem3 (vCache[19], v[5][0], v[5][1], v[5][2]);
		VA_SetElem3 (vCache[20], v[7][0], v[7][1], v[7][2]);
		VA_SetElem3 (vCache[21], v[1][0], v[1][1], v[1][2]);
		VA_SetElem3 (vCache[22], v[3][0], v[3][1], v[3][2]);
		VA_SetElem3 (vCache[23], v[7][0], v[7][1], v[7][2]);
		//left
		VA_SetElem3 (vCache[24], v[2][0], v[2][1], v[2][2]);
		VA_SetElem3 (vCache[25], v[3][0], v[3][1], v[3][2]);
		VA_SetElem3 (vCache[26], v[7][0], v[7][1], v[7][2]);
		VA_SetElem3 (vCache[27], v[2][0], v[2][1], v[2][2]);
		VA_SetElem3 (vCache[28], v[6][0], v[6][1], v[6][2]);
		VA_SetElem3 (vCache[29], v[7][0], v[7][1], v[7][2]);
		//back
		VA_SetElem3 (vCache[30], v[4][0], v[4][1], v[4][2]);
		VA_SetElem3 (vCache[31], v[5][0], v[5][1], v[5][2]);
		VA_SetElem3 (vCache[32], v[7][0], v[7][1], v[7][2]);
		VA_SetElem3 (vCache[33], v[4][0], v[4][1], v[4][2]);
		VA_SetElem3 (vCache[34], v[6][0], v[6][1], v[6][2]);
		VA_SetElem3 (vCache[35], v[7][0], v[7][1], v[7][2]);

		qglDrawArrays (GL_TRIANGLES, 0, 36);

		qglPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		qglDisable (GL_LINE_SMOOTH);

		if (!flareEdit || !selectedShadowLight->flare) { // skip filled box in flare editing mode

			// draw small light box
			VectorSet (v[0], tmpOrg[0] - 5, tmpOrg[1] - 5, tmpOrg[2] - 5);
			VectorSet (v[1], tmpOrg[0] - 5, tmpOrg[1] - 5, tmpOrg[2] + 5);
			VectorSet (v[2], tmpOrg[0] - 5, tmpOrg[1] + 5, tmpOrg[2] - 5);
			VectorSet (v[3], tmpOrg[0] - 5, tmpOrg[1] + 5, tmpOrg[2] + 5);
			VectorSet (v[4], tmpOrg[0] + 5, tmpOrg[1] - 5, tmpOrg[2] - 5);
			VectorSet (v[5], tmpOrg[0] + 5, tmpOrg[1] - 5, tmpOrg[2] + 5);
			VectorSet (v[6], tmpOrg[0] + 5, tmpOrg[1] + 5, tmpOrg[2] - 5);
			VectorSet (v[7], tmpOrg[0] + 5, tmpOrg[1] + 5, tmpOrg[2] + 5);

			//front
			VA_SetElem3 (vCache[0], v[0][0], v[0][1], v[0][2]);
			VA_SetElem3 (vCache[1], v[1][0], v[1][1], v[1][2]);
			VA_SetElem3 (vCache[2], v[3][0], v[3][1], v[3][2]);
			VA_SetElem3 (vCache[3], v[0][0], v[0][1], v[0][2]);
			VA_SetElem3 (vCache[4], v[2][0], v[2][1], v[2][2]);
			VA_SetElem3 (vCache[5], v[3][0], v[3][1], v[3][2]);
			//right
			VA_SetElem3 (vCache[6], v[0][0], v[0][1], v[0][2]);
			VA_SetElem3 (vCache[7], v[4][0], v[4][1], v[4][2]);
			VA_SetElem3 (vCache[8], v[5][0], v[5][1], v[5][2]);
			VA_SetElem3 (vCache[9], v[0][0], v[0][1], v[0][2]);
			VA_SetElem3 (vCache[10], v[1][0], v[1][1], v[1][2]);
			VA_SetElem3 (vCache[11], v[5][0], v[5][1], v[5][2]);
			//bottom
			VA_SetElem3 (vCache[12], v[0][0], v[0][1], v[0][2]);
			VA_SetElem3 (vCache[13], v[4][0], v[4][1], v[4][2]);
			VA_SetElem3 (vCache[14], v[6][0], v[6][1], v[6][2]);
			VA_SetElem3 (vCache[15], v[0][0], v[0][1], v[0][2]);
			VA_SetElem3 (vCache[16], v[2][0], v[2][1], v[2][2]);
			VA_SetElem3 (vCache[17], v[6][0], v[6][1], v[6][2]);
			//top
			VA_SetElem3 (vCache[18], v[1][0], v[1][1], v[1][2]);
			VA_SetElem3 (vCache[19], v[5][0], v[5][1], v[5][2]);
			VA_SetElem3 (vCache[20], v[7][0], v[7][1], v[7][2]);
			VA_SetElem3 (vCache[21], v[1][0], v[1][1], v[1][2]);
			VA_SetElem3 (vCache[22], v[3][0], v[3][1], v[3][2]);
			VA_SetElem3 (vCache[23], v[7][0], v[7][1], v[7][2]);
			//left
			VA_SetElem3 (vCache[24], v[2][0], v[2][1], v[2][2]);
			VA_SetElem3 (vCache[25], v[3][0], v[3][1], v[3][2]);
			VA_SetElem3 (vCache[26], v[7][0], v[7][1], v[7][2]);
			VA_SetElem3 (vCache[27], v[2][0], v[2][1], v[2][2]);
			VA_SetElem3 (vCache[28], v[6][0], v[6][1], v[6][2]);
			VA_SetElem3 (vCache[29], v[7][0], v[7][1], v[7][2]);
			//back
			VA_SetElem3 (vCache[30], v[4][0], v[4][1], v[4][2]);
			VA_SetElem3 (vCache[31], v[5][0], v[5][1], v[5][2]);
			VA_SetElem3 (vCache[32], v[7][0], v[7][1], v[7][2]);
			VA_SetElem3 (vCache[33], v[4][0], v[4][1], v[4][2]);
			VA_SetElem3 (vCache[34], v[6][0], v[6][1], v[6][2]);
			VA_SetElem3 (vCache[35], v[7][0], v[7][1], v[7][2]);

			qglDrawArrays (GL_TRIANGLES, 0, 36);
		}
	}

	qglDisableVertexAttribArray (ATRB_POSITION);
	GL_BindNullProgram ();
	GL_Enable (GL_TEXTURE_2D);
	GL_Enable (GL_CULL_FACE);
	GL_Enable (GL_BLEND);

	if (r_shadows->value)
		GL_Enable (GL_STENCIL_TEST);

	if (r_useLightScissors->value)
		GL_Enable (GL_SCISSOR_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable (GL_DEPTH_BOUNDS_TEST_EXT);

	R_LightFlareOutLine ();
}

void Clamp2RGB (vec3_t color) {
	if (color[0] > 1.0)
		color[0] = 1.0;

	if (color[1] > 1.0)
		color[1] = 1.0;

	if (color[2] > 1.0)
		color[2] = 1.0;
}



void CreateNormal (vec3_t dst, vec3_t xyz0, vec3_t xyz1, vec3_t xyz2) {
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

	VectorNormalize (dst);
}


void MakeFrustum4Light (worldShadowLight_t *light, qboolean ingame) {
	vec3_t		v0, v1, v2, v3, v4;
	vec3_t		forward, right, up;
	vec3_t		angles, rspeed;

	if (!light->_cone)
		return;	// �����, ���� ������� �� ���� ������������ (�� ����� �������� _cone)

	if (ingame)
		VectorCopy (light->speed, rspeed);
	else
		VectorClear (rspeed);

	angles[0] = light->angles[0] + rspeed[0] * r_newrefdef.time * 1000;
	angles[1] = light->angles[1] + rspeed[1] * r_newrefdef.time * 1000;
	angles[2] = light->angles[2] + rspeed[2] * r_newrefdef.time * 1000;

	AngleVectors (angles, forward, right, up);

	VectorScale (right, light->_cone, right);
	VectorScale (up, light->_cone, up);
	VectorCopy (light->origin, v0);

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

	CreateNormal (light->frust[0].normal, v1, v2, v0);
	light->frust[0].dist = DotProduct (light->frust[0].normal, v0);

	CreateNormal (light->frust[1].normal, v2, v3, v0);
	light->frust[1].dist = DotProduct (light->frust[1].normal, v0);

	CreateNormal (light->frust[2].normal, v3, v4, v0);
	light->frust[2].dist = DotProduct (light->frust[2].normal, v0);

	CreateNormal (light->frust[3].normal, v4, v1, v0);
	light->frust[3].dist = DotProduct (light->frust[3].normal, v0);
}


worldShadowLight_t *R_AddNewWorldLight (vec3_t origin, vec3_t color, float radius[3], int style,
	int filter, vec3_t angles, vec3_t speed, qboolean isStatic,
	int isShadow, int isAmbient, float cone, qboolean ingame,
	int flare, vec3_t flareOrg, float flareSize, char target[MAX_QPATH],
	int flags, int fog, float fogDensity) {

	worldShadowLight_t	*light;
	cplane_t			*plane;
	int					i, leafnum, cluster;
	vec3_t				tmp;
	mat4_t				temp;

	light = (worldShadowLight_t*)malloc (sizeof(worldShadowLight_t));
	light->s_next = shadowLight_static;
	shadowLight_static = light;

	VectorClear (tmp);
	VectorClear (light->frust[0].normal);
	VectorClear (light->origin);
	VectorClear (light->startColor);
	VectorClear (light->angles);
	VectorClear (light->speed);
	VectorClear (light->radius);
	VectorClear (light->flareOrigin);
	memset (light->targetname, 0, sizeof(light->targetname));
	light->start_off = 0;

	for (i = 0; i < 8; i++)
		VectorClear (light->corners[i]);

	VectorCopy (origin, light->origin);
	VectorCopy (color, light->startColor);
	VectorCopy (angles, light->angles);
	VectorCopy (speed, light->speed);
	VectorCopy (radius, light->radius);
	VectorCopy (flareOrg, light->flareOrigin);

	if (light->radius[0] == light->radius[1] && light->radius[0] == light->radius[2])
		light->spherical = qtrue;
	else
		light->spherical = qfalse;

	Clamp2RGB (light->startColor);

	light->_cone = cone;
	light->isStatic = isStatic;
	light->isShadow = isShadow;
	light->isAmbient = isAmbient;
	light->isFog = fog;
	light->fogDensity = fogDensity;
	light->isNoWorldModel = 0;
	light->next = NULL;
	light->style = style;
	light->filter = filter;
	light->flareSize = flareSize;
	light->flare = flare;
	light->vboId = light->iboId = light->iboNumIndices = 0;
	light->depthBounds[0] = 0.0;
	light->depthBounds[1] = 1.0;
	light->maxRad = 0;

	memcpy (light->targetname, target, sizeof(light->targetname));

	for (i = 0; i < 3; i++) {
		light->mins[i] = light->origin[i] - light->radius[i];
		light->maxs[i] = light->origin[i] + light->radius[i];
	}

	if (light->spherical)
		light->maxRad = light->radius[0];
	else 
		light->maxRad = max(max(light->radius[0], light->radius[1]), light->radius[2]);
	
	for (i = 0; i < 8; i++) {
		tmp[0] = (i & 1) ? -light->radius[0] : light->radius[0];
		tmp[1] = (i & 2) ? -light->radius[1] : light->radius[1];
		tmp[2] = (i & 4) ? -light->radius[2] : light->radius[2];

		AnglesToMat3 (light->angles, light->axis);
		Mat3_TransposeMultiplyVector (light->axis, tmp, light->corners[i]);
		VectorAdd (light->corners[i], light->origin, light->corners[i]);
	}


	// compute the frustum planes for non cone lights
	if (!light->_cone) {
		for (i = 0, plane = light->frust; i < 6; i++, plane++) {
			float d = DotProduct (light->origin, light->axis[i >> 1]);

			if (i & 1) {
				VectorNegate (light->axis[i >> 1], plane->normal);
				plane->dist = -d - light->radius[i >> 1];
			}
			else {
				VectorCopy (light->axis[i >> 1], plane->normal);
				plane->dist = d - light->radius[i >> 1];
			}

			SetPlaneType (plane);
			SetPlaneSignBits (plane);
		}
	}

	MakeFrustum4Light (light, qfalse);

	if (ingame) {
		R_MarkLightLeaves (light);
		R_DrawBspModelVolumes (qtrue, light);
	}
	else {
		//// simple cull info for new light 
		leafnum = CM_PointLeafnum (light->origin);
		cluster = CM_LeafCluster (leafnum);
		light->area = CM_LeafArea (leafnum);
		Q_memcpy (light->vis, CM_ClusterPVS (cluster), (CM_NumClusters () + 7) >> 3);
	}
	light->occQ = numLightQ;
	numLightQ++;

#define START_OFF	1
	light->start_off = (flags & START_OFF);

	// calc atten tex coord
	Mat4_Identity (temp);
	Mat4_Translate (temp, 0.5, 0.5, 0.5);
	Mat4_Scale (temp, 0.5 / light->radius[0], 0.5 / light->radius[1], 0.5 / light->radius[2]);
	Mat4_Translate (temp, -light->origin[0], -light->origin[1], -light->origin[2]);
	Mat4_Copy (temp, light->attenMapMatrix);

	r_numWorlsShadowLights++;
	return light;
}

model_t *loadmodel;

void R_SetLightPlanes () {

	vec4_t	clipPlane[6];
	int		i;

	for (i = 0; i < 6; i++) {
		clipPlane[i][0] = -DotProduct (r_newrefdef.axis[1], currentShadowLight->frust[i].normal);
		clipPlane[i][1] = DotProduct (r_newrefdef.axis[2], currentShadowLight->frust[i].normal);
		clipPlane[i][2] = -DotProduct (r_newrefdef.axis[0], currentShadowLight->frust[i].normal);
		clipPlane[i][3] = DotProduct (r_newrefdef.vieworg, currentShadowLight->frust[i].normal) - currentShadowLight->frust[i].dist;

		qglClipPlane (GL_CLIP_PLANE0 + i, (double*)clipPlane[i]);
	}
}

void Load_BspLights () {

	int addLight, style, numlights, flag;
	char *c, *token, key[256], *value, target[MAX_QPATH];
	float color[3], origin[3], radius[3], cone;

	if (!loadmodel) {
		Com_Printf ("No map loaded.\n");
		return;
	}

	c = CM_EntityString ();
	numlights = 0;

	while (1) {
		token = COM_Parse (&c);
		if (!c)
			break;

		memset (target, 0, sizeof(target));
		VectorClear (origin);
		VectorSet (color, 1, 1, 1);
		VectorClear (radius);
		style = 0;
		cone = 0;
		flag = 0;

		addLight = qfalse;

		while (1) {
			token = COM_Parse (&c);
			if (token[0] == '}')
				break;

			strncpy (key, token, sizeof(key)-1);

			value = COM_Parse (&c);
			if (!Q_stricmp (key, "classname")) {
				if (!Q_stricmp (value, "light"))
					addLight = qtrue;
				if (!Q_stricmp (value, "light_mine1")) {
					addLight = qtrue;
				}
				if (!Q_stricmp (value, "light_mine2")) {
					addLight = qtrue;
				}
			}

			if (!Q_stricmp (key, "light"))
				radius[0] = atoi (value);
			if (!Q_stricmp (key, "origin"))
				sscanf (value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
			if (!Q_stricmp (key, "_color"))
				sscanf (value, "%f %f %f", &color[0], &color[1], &color[2]);
			if (!Q_stricmp (key, "style"))
				style = atoi (value);
			if (!Q_stricmp (key, "_cone"))
				cone = atof (value);
			if (!Q_stricmp (key, "targetname"))
				Q_strncpyz (target, value, sizeof(target));
			if (!Q_stricmp (key, "spawnflags"))
				flag = atoi (value);
		}

		if (addLight && style > 0) {
			VectorSet (radius, radius[0], radius[0], radius[0]);
			R_AddNewWorldLight (origin, color, radius, style, 0, vec3_origin, vec3_origin, qtrue, 1, 0, cone, qfalse, 0, origin, 10.0, target, flag, 0, 0.0);
			numlights++;
		}
	}
	Com_Printf (""S_COLOR_MAGENTA"Loaded "S_COLOR_GREEN"%i"S_COLOR_WHITE" bsp lights\n", numlights);

}

void Load_LightFile () {

	int		style, numLights = 0, filter, shadow, ambient, flare, flag, fog;
	vec3_t	angles, speed, color, origin, lOrigin, fOrg;
	char	*c, *token, key[256], *value, target[MAX_QPATH];
	float	radius[3], cone, fSize, fogDensity;
	char	name[MAX_QPATH], path[MAX_QPATH];

	if (!r_worldmodel) {
		Com_Printf ("No map loaded.\n");
		return;
	}

	FS_StripExtension (r_worldmodel->name, name, sizeof (name));
	Com_sprintf (path, sizeof(path), "%s.xplit", name);
	FS_LoadFile (path, (void **)&c);

	if (!c) {
		Load_BspLights ();
		return;
	}

	Com_Printf ("Loaded lights from "S_COLOR_GREEN"%s"S_COLOR_WHITE".\n", path);

	while (1) {
		token = COM_Parse (&c);

		if (!c) {
			break;
		}

		style = 0;
		filter = 0;
		shadow = 1;
		ambient = 0;
		cone = 0;
		fSize = 0;
		flare = 0;
		flag = 0;
		fog = 0;
		fogDensity = 0.0;

		memset (target, 0, sizeof(target));
		VectorClear (radius);
		VectorClear (angles);
		VectorClear (speed);
		VectorClear (origin);
		VectorClear (lOrigin);
		VectorClear (color);
		VectorClear (fOrg);

		while (1) {
			token = COM_Parse (&c);
			if (token[0] == '}')
				break;

			strncpy (key, token, sizeof(key)-1);

			value = COM_Parse (&c);

			if (!Q_stricmp (key, "radius"))
				sscanf (value, "%f %f %f", &radius[0], &radius[1], &radius[2]);
			else if (!Q_stricmp (key, "origin"))
				sscanf (value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
			else if (!Q_stricmp (key, "color"))
				sscanf (value, "%f %f %f", &color[0], &color[1], &color[2]);
			else if (!Q_stricmp (key, "style"))
				style = atoi (value);
			else if (!Q_stricmp (key, "filter"))
				filter = atoi (value);
			else if (!Q_stricmp (key, "angles"))
				sscanf (value, "%f %f %f", &angles[0], &angles[1], &angles[2]);
			else if (!Q_stricmp (key, "speed"))
				sscanf (value, "%f %f %f", &speed[0], &speed[1], &speed[2]);
			else if (!Q_stricmp (key, "shadow"))
				shadow = atoi (value);
			else if (!Q_stricmp (key, "ambient"))
				ambient = atoi (value);
			else if (!Q_stricmp (key, "_cone"))
				cone = atoi (value);
			else if (!Q_stricmp (key, "flare"))
				flare = atoi (value);
			else if (!Q_stricmp (key, "flareOrigin"))
				sscanf (value, "%f %f %f", &fOrg[0], &fOrg[1], &fOrg[2]);
			else if (!Q_stricmp (key, "flareSize"))
				fSize = atoi (value);
			else if (!Q_stricmp (key, "targetname"))
				Q_strncpyz (target, value, sizeof(target));
			else if (!Q_stricmp (key, "spawnflags"))
				flag = atoi (value);
			else if (!Q_stricmp (key, "fogLight"))
				fog = atof (value);
			else if (!Q_stricmp (key, "fogDensity"))
				fogDensity = atof (value);

		}

		R_AddNewWorldLight (origin, color, radius, style, filter, angles, speed, qtrue, shadow, ambient, cone, qfalse, flare, fOrg, fSize, target, flag, fog, fogDensity);
		numLights++;
	}
	Com_Printf (""S_COLOR_MAGENTA"Load_LightFile:"S_COLOR_WHITE" add "S_COLOR_GREEN"%i"S_COLOR_WHITE" world lights\n", numLights);
}

/*
===============
R_MarkLightLeaves

Marks nodes from the light, this is used for
gross culling during svbsp creation.
===============
*/
qboolean R_MarkLightLeaves (worldShadowLight_t *light) {
	int contents, leafnum, cluster;
	int		leafs[MAX_MAP_LEAFS];
	int		i, count;
	vec3_t	mins, maxs;
	byte	vis[MAX_MAP_LEAFS / 8];

	contents = CL_PMpointcontents (light->origin);
	if (contents & CONTENTS_SOLID)
		goto skip;

	leafnum = CM_PointLeafnum (light->origin);
	cluster = CM_LeafCluster (leafnum);
	light->area = CM_LeafArea (leafnum);

	if (!light->area) {
	skip:	Com_DPrintf ("Out of BSP, rejected light at %f %f %f\n", light->origin[0], light->origin[1], light->origin[2]);
		return qfalse;
	}

	// build vis-data
	memcpy (&light->vis, CM_ClusterPVS (cluster), (((CM_NumClusters () + 31) >> 5) << 2));

	for (i = 0; i < 3; i++) {
		mins[i] = light->origin[i] - light->radius[i];
		maxs[i] = light->origin[i] + light->radius[i];
	}

	count = CM_BoxLeafnums (mins, maxs, leafs, r_worldmodel->numLeafs, NULL);
	if (count < 1)
		Com_Error (ERR_FATAL, "R_MarkLightLeaves: count < 1");

	// convert leafs to clusters
	for (i = 0; i < count; i++)
		leafs[i] = CM_LeafCluster (leafs[i]);

	memset (&vis, 0, (((r_worldmodel->numLeafs + 31) >> 5) << 2));
	for (i = 0; i < count; i++)
		vis[leafs[i] >> 3] |= (1 << (leafs[i] & 7));

	for (i = 0; i < ((r_worldmodel->numLeafs + 31) >> 5); i++)
		((long *)light->vis)[i] &= ((long *)vis)[i];

	return qtrue;
}
extern qboolean cinServer;

qboolean InLightVISEntity () {
	int		leafs[MAX_MAP_LEAFS];
	int		i, count;
	int		longs;
	vec3_t	mins, maxs;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return qtrue;
	
	if (cinServer)
		return qtrue;

	if (!r_worldmodel)
		return qfalse;
	
	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2]) {
		for (i = 0; i < 3; i++) {
			mins[i] = currententity->origin[i] - currentmodel->radius;
			maxs[i] = currententity->origin[i] + currentmodel->radius;
		}
	}
		else {
			VectorAdd (currententity->origin, currententity->model->maxs, maxs);
			VectorAdd (currententity->origin, currententity->model->mins, mins);
		}

		count = CM_BoxLeafnums (mins, maxs, leafs, r_worldmodel->numLeafs, NULL);
		if (count < 1)
			Com_Error (ERR_FATAL, "InLightVISEntity: count < 1");
		longs = (CM_NumClusters () + 31) >> 5;

		// convert leafs to clusters
		for (i = 0; i < count; i++) {
			leafs[i] = CM_LeafCluster (leafs[i]);
		}
		memset (&currententity->vis, 0, (((r_worldmodel->numLeafs + 31) >> 5) << 2));
		for (i = 0; i < count; i++)
			currententity->vis[leafs[i] >> 3] |= (1 << (leafs[i] & 7));

	return HasSharedLeafs (currentShadowLight->vis, currententity->vis);

}


void R_PreCalcLightData (void) {
	worldShadowLight_t *light;

	for (light = shadowLight_static; light; light = light->s_next) {

		if (!R_MarkLightLeaves (light))
			continue;

		R_DrawBspModelVolumes (qtrue, light);
	}
	Com_Printf (""S_COLOR_MAGENTA"R_PreCalcLightData: "S_COLOR_GREEN"%i"S_COLOR_WHITE" lights\n", numPreCachedLights);
}

void DeleteShadowVertexBuffers (void) {
	worldShadowLight_t *light;

	for (light = shadowLight_static; light; light = light->s_next) {

		qglDeleteBuffers (1, &light->vboId);
		qglDeleteBuffers (1, &light->iboId);
		light->vboId = light->iboId = light->iboNumIndices = 0;
	}
	numPreCachedLights = 0;
}

void R_ClearWorldLights (void) {
	worldShadowLight_t *light, *next;

	if (shadowLight_static) {
		for (light = shadowLight_static; light; light = next) {
			next = light->s_next;
			free (light);
		}
		shadowLight_static = NULL;
	}

	memset (shadowLightsBlock, 0, sizeof(worldShadowLight_t)* MAX_WORLD_SHADOW_LIHGTS);

	r_numWorlsShadowLights = 0;
}

/*
=============
R_CalcCubeMapMatrix

Loads the current matrix with a tranformation used for light filters
=============
*/
void R_CalcCubeMapMatrix (qboolean model) {
	float   a, b, c;
	mat4_t  m;

	a = currentShadowLight->angles[2] + (currentShadowLight->speed[2] * r_newrefdef.time * 1000);
	b = currentShadowLight->angles[0] + (currentShadowLight->speed[0] * r_newrefdef.time * 1000);
	c = currentShadowLight->angles[1] + (currentShadowLight->speed[1] * r_newrefdef.time * 1000);

	Mat4_Identity (m);

	if (a)
		Mat4_Rotate (m, -a, 1.f, 0.f, 0.f);
	if (b)
		Mat4_Rotate (m, -b, 0.f, 1.f, 0.f);
	if (c)
		Mat4_Rotate (m, -c, 0.f, 0.f, 1.f);

	if (model) {
		if (currententity->angles[1])
			Mat4_Rotate (m, currententity->angles[1], 0, 0, 1);
		if (currententity->angles[0])
			Mat4_Rotate (m, currententity->angles[0], 0, 1, 0);
		if (currententity->angles[2])
			Mat4_Rotate (m, currententity->angles[2], 1, 0, 0);
	}

	Mat4_Translate (m, -currentShadowLight->origin[0], -currentShadowLight->origin[1], -currentShadowLight->origin[2]);
	Mat4_Copy (m, currentShadowLight->cubeMapMatrix);
}


void R_DrawLightOccluders () {
	vec3_t		v[8];
	vec3_t		tmpOrg;
	vec3_t		radius;

	if (!r_useLightOccluders->value || currentShadowLight->isNoWorldModel || !currentShadowLight->isStatic)
		return;

	// setup program
	GL_BindProgram (nullProgram, 0);
	qglEnableVertexAttribArray (ATRB_POSITION);
	qglVertexAttribPointer (ATRB_POSITION, 3, GL_FLOAT, qfalse, 0, vCache);

	GL_ColorMask (0, 0, 0, 0);
	GL_Disable (GL_TEXTURE_2D);
	GL_Disable (GL_CULL_FACE);
	GL_Disable (GL_BLEND);
	GL_Disable(GL_SCISSOR_TEST);
	GL_Disable(GL_STENCIL_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable(GL_DEPTH_BOUNDS_TEST_EXT);


	VectorCopy (currentShadowLight->origin, tmpOrg);
	VectorScale (currentShadowLight->radius, r_occLightBoundsSize->value, radius);

	VectorSet (v[0], tmpOrg[0] - radius[0], tmpOrg[1] - radius[1], tmpOrg[2] - radius[2]);
	VectorSet (v[1], tmpOrg[0] - radius[0], tmpOrg[1] - radius[1], tmpOrg[2] + radius[2]);
	VectorSet (v[2], tmpOrg[0] - radius[0], tmpOrg[1] + radius[1], tmpOrg[2] - radius[2]);
	VectorSet (v[3], tmpOrg[0] - radius[0], tmpOrg[1] + radius[1], tmpOrg[2] + radius[2]);
	VectorSet (v[4], tmpOrg[0] + radius[0], tmpOrg[1] - radius[1], tmpOrg[2] - radius[2]);
	VectorSet (v[5], tmpOrg[0] + radius[0], tmpOrg[1] - radius[1], tmpOrg[2] + radius[2]);
	VectorSet (v[6], tmpOrg[0] + radius[0], tmpOrg[1] + radius[1], tmpOrg[2] - radius[2]);
	VectorSet (v[7], tmpOrg[0] + radius[0], tmpOrg[1] + radius[1], tmpOrg[2] + radius[2]);

	qglBeginQueryARB (gl_state.query_passed, lightsQueries[currentShadowLight->occQ]);

	//front
	VA_SetElem3 (vCache[0], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[1], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[2], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3 (vCache[3], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[4], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[5], v[3][0], v[3][1], v[3][2]);
	//right
	VA_SetElem3 (vCache[6], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[7], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[8], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3 (vCache[9], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[10], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[11], v[5][0], v[5][1], v[5][2]);
	//bottom
	VA_SetElem3 (vCache[12], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[13], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[14], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3 (vCache[15], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[16], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[17], v[6][0], v[6][1], v[6][2]);
	//top
	VA_SetElem3 (vCache[18], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[19], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3 (vCache[20], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3 (vCache[21], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[22], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3 (vCache[23], v[7][0], v[7][1], v[7][2]);
	//left
	VA_SetElem3 (vCache[24], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[25], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3 (vCache[26], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3 (vCache[27], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[28], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3 (vCache[29], v[7][0], v[7][1], v[7][2]);
	//back
	VA_SetElem3 (vCache[30], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[31], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3 (vCache[32], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3 (vCache[33], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[34], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3 (vCache[35], v[7][0], v[7][1], v[7][2]);

	qglDrawArrays (GL_TRIANGLES, 0, 36);

	qglEndQueryARB (gl_state.query_passed);

	GL_BindNullProgram();
	qglDisableVertexAttribArray(ATRB_POSITION);

	GL_ColorMask (1, 1, 1, 1);
	GL_Enable (GL_TEXTURE_2D);
	GL_Enable (GL_CULL_FACE);
	GL_Enable (GL_BLEND);
	if (r_shadows->value)
		GL_Enable(GL_STENCIL_TEST);

	if (r_useLightScissors->value)
		GL_Enable(GL_SCISSOR_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable(GL_DEPTH_BOUNDS_TEST_EXT);

	if (BoundsAndSphereIntersect(currentShadowLight->mins, currentShadowLight->maxs, r_origin, 25))
		glBeginConditionalRender(lightsQueries[currentShadowLight->occQ], GL_QUERY_NO_WAIT);
	else
		glBeginConditionalRender(lightsQueries[currentShadowLight->occQ], GL_QUERY_WAIT);
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
		dist = DotProduct (point, plane->normal) - plane->dist;

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

			Mat4_MultiplyVector (mvpMatrix, in, out);

			scale = 1.0f;

			point[0] = out[0] * scale;
			point[1] = out[1] * scale;
			point[2] = out[2] * scale;

			AddPointToBounds (point, mins, maxs);
		}

		return;
	}

	if (numPoints > MAX_LIGHT_PLANE_VERTICES - 2)
		Com_Error (qfalse, "R_ClipLightPlane: MAX_LIGHT_PLANE_VERTICES hit");

	// determine sides for each point
	front = qfalse;
	back = qfalse;

	for (i = 0; i < numPoints; i++) {
		dists[i] = DotProduct (points[i], frustum[stage].normal) - frustum[stage].dist;

		if (dists[i] > ON_EPSILON) {
			sides[i] = SIDE_FRONT;
			front = qtrue;
			continue;
		}

		if (dists[i] < -ON_EPSILON) {
			sides[i] = SIDE_BACK;
			back = qtrue;
			continue;
		}

		sides[i] = SIDE_ON;
	}

	if (!front)
		return;		// not clipped

	if (!back) {
		// continue
		R_ClipLightPlane (mvpMatrix, mins, maxs, numPoints, points, stage + 1);
		return;
	}

	// handle wraparound case
	VectorCopy (points[0], points[i]);

	dists[i] = dists[0];
	sides[i] = sides[0];

	// clip it
	numClipped = 0;

	for (i = 0; i < numPoints; i++) {
		if (sides[i] == SIDE_ON) {
			VectorCopy (points[i], clipped[numClipped]);
			numClipped++;
			continue;
		}

		if (sides[i] == SIDE_FRONT) {
			VectorCopy (points[i], clipped[numClipped]);
			numClipped++;
		}

		if (sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
			continue;

		if (dists[i] == dists[i + 1]) {
			VectorCopy (points[i], clipped[numClipped]);
			numClipped++;
		}
		else
			VectorLerp (points[i], points[i + 1], dists[i] / (dists[i] - dists[i + 1]), clipped[numClipped++]);
	}

	// continue
	R_ClipLightPlane (mvpMatrix, mins, maxs, numClipped, clipped, stage + 1);
}

void R_SetViewLightScreenBounds () {
	int			i, scissor[4],
				cornerIndices[6][4] = { { 3, 2, 6, 7 }, { 0, 1, 5, 4 }, { 2, 3, 1, 0 }, { 4, 5, 7, 6 }, { 1, 3, 7, 5 }, { 2, 0, 4, 6 } };
	vec3_t		mins = { Q_INFINITY, Q_INFINITY, Q_INFINITY },
				maxs = { -Q_INFINITY, -Q_INFINITY, -Q_INFINITY },
				points[5];
	float		depth[2];

	currentShadowLight->scissor[0] = r_newrefdef.viewport[0];
	currentShadowLight->scissor[1] = r_newrefdef.viewport[1];
	currentShadowLight->scissor[2] = r_newrefdef.viewport[2];
	currentShadowLight->scissor[3] = r_newrefdef.viewport[3];

	currentShadowLight->depthBounds[0] = 0.0f;
	currentShadowLight->depthBounds[1] = 1.0f;
	
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (r_useLightScissors->value || (gl_state.depthBoundsTest && r_useDepthBounds->value)) {
		// copy the corner points of each plane and clip to the frustum
		for (i = 0; i < 6; i++) {
			VectorCopy (currentShadowLight->corners[cornerIndices[i][0]], points[0]);
			VectorCopy (currentShadowLight->corners[cornerIndices[i][1]], points[1]);
			VectorCopy (currentShadowLight->corners[cornerIndices[i][2]], points[2]);
			VectorCopy (currentShadowLight->corners[cornerIndices[i][3]], points[3]);

			R_ClipLightPlane(r_newrefdef.modelViewProjectionMatrixTranspose, mins, maxs, 4, points, 0);
		}

		// check if any corner point is not in front of the near plane
		for (i = 0; i < 8; i++) {
			if (PointOnPlaneSide (currentShadowLight->corners[i], &frustum[4]) != SIDE_FRONT) {
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
		scissor[0] = max (Q_ftol (floor (mins[0])), r_newrefdef.viewport[0]);
		scissor[1] = max (Q_ftol (floor (mins[1])), r_newrefdef.viewport[1]);
		scissor[2] = min (Q_ftol (ceil (maxs[0])), r_newrefdef.viewport[0] + r_newrefdef.viewport[2]);
		scissor[3] = min (Q_ftol (ceil (maxs[1])), r_newrefdef.viewport[1] + r_newrefdef.viewport[3]);

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

	// set the depth bounds
	if (r_useDepthBounds->value) {
		depth[0] = max (mins[2], 0.0f);
		depth[1] = min (maxs[2], 1.0f);

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


void R_DrawLightFlare () {

	float		dist, dist2, scale;
	vec3_t		v, tmp;
	int			id;
	vec3_t		vert_array[MAX_FLARES_VERTEX];
	vec2_t		tex_array[MAX_FLARES_VERTEX];
	vec4_t		color_array[MAX_FLARES_VERTEX];

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (!currentShadowLight->flare)
		return;

	if (currentShadowLight->isNoWorldModel)
		return;

	if (currentShadowLight->isAmbient)
		return;

	if (!r_drawFlares->value)
		return;

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable (GL_DEPTH_BOUNDS_TEST_EXT);
	
	if (r_useLightScissors->value)
		GL_Disable (GL_SCISSOR_TEST);

	qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ibo_quadTris);
	qglEnableVertexAttribArray (ATRB_POSITION);
	qglEnableVertexAttribArray (ATRB_TEX0);
	qglEnableVertexAttribArray (ATRB_COLOR);

	qglVertexAttribPointer (ATRB_POSITION, 3, GL_FLOAT, qfalse, 0, vert_array);
	qglVertexAttribPointer (ATRB_TEX0, 2, GL_FLOAT, qfalse, 0, tex_array);
	qglVertexAttribPointer (ATRB_COLOR, 4, GL_FLOAT, qfalse, 0, color_array);

	GL_BindProgram (particlesProgram, 0);
	id = particlesProgram->id[0];

	GL_MBind (GL_TEXTURE0_ARB, r_flare->texnum);
	qglUniform1i (qglGetUniformLocation (id, "u_map0"), 0);

	GL_SelectTexture (GL_TEXTURE1_ARB);
	GL_BindRect (depthMap->texnum);
	qglUniform1i (qglGetUniformLocation (id, "u_depthBufferMap"), 1);
	qglUniform2f (qglGetUniformLocation (id, "u_depthParms"), r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
	qglUniform2f (qglGetUniformLocation (id, "u_mask"), 1.0, 0.0);
	qglUniform1f (qglGetUniformLocation (id, "u_colorScale"), 1.0);
	qglUniform1f (qglGetUniformLocation (id, "u_thickness"), currentShadowLight->flareSize * 1.5);

	// Color Fade
	VectorSubtract (currentShadowLight->flareOrigin, r_origin, v);
	dist = VectorLength (v) * (currentShadowLight->flareSize * 0.01);
	dist2 = VectorLength (v);

	scale = ((1024 - dist2) / 1024) * 0.5;

	VectorScale (currentShadowLight->color, scale, tmp);

	VectorMA (currentShadowLight->flareOrigin, -1 - dist, vup, vert_array[0]);
	VectorMA (vert_array[0], 1 + dist, vright, vert_array[0]);
	VA_SetElem2 (tex_array[0], 0, 1);
	VA_SetElem4 (color_array[0], tmp[0], tmp[1], tmp[2], 1);

	VectorMA (currentShadowLight->flareOrigin, -1 - dist, vup, vert_array[1]);
	VectorMA (vert_array[1], -1 - dist, vright, vert_array[1]);
	VA_SetElem2 (tex_array[1], 0, 0);
	VA_SetElem4 (color_array[1], tmp[0], tmp[1], tmp[2], 1);

	VectorMA (currentShadowLight->flareOrigin, 1 + dist, vup, vert_array[2]);
	VectorMA (vert_array[2], -1 - dist, vright, vert_array[2]);
	VA_SetElem2 (tex_array[2], 1, 0);
	VA_SetElem4 (color_array[2], tmp[0], tmp[1], tmp[2], 1);

	VectorMA (currentShadowLight->flareOrigin, 1 + dist, vup, vert_array[3]);
	VectorMA (vert_array[3], 1 + dist, vright, vert_array[3]);
	VA_SetElem2 (tex_array[3], 1, 1);
	VA_SetElem4 (color_array[3], tmp[0], tmp[1], tmp[2], 1);

	qglDrawElements	(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

	GL_BindNullProgram ();

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable (GL_DEPTH_BOUNDS_TEST_EXT);
	
	if (r_useLightScissors->value)
		GL_Enable(GL_SCISSOR_TEST);

	qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	qglDisableVertexAttribArray (ATRB_POSITION);
	qglDisableVertexAttribArray (ATRB_TEX0);
	qglDisableVertexAttribArray (ATRB_COLOR);
}

void R_LightFlareOutLine () { //flare editing highlights

	vec3_t		v[8], tmpOrg;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (!currentShadowLight->flare)
		return;

	if (!flareEdit)
		return;

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable (GL_DEPTH_BOUNDS_TEST_EXT);


	GL_Disable (GL_SCISSOR_TEST);
	GL_Disable (GL_STENCIL_TEST);
	GL_Disable (GL_TEXTURE_2D);
	GL_Disable (GL_CULL_FACE);

	qglEnableVertexAttribArray (ATRB_POSITION);
	qglVertexAttribPointer (ATRB_POSITION, 3, GL_FLOAT, qfalse, 0, vCache);

	// setup program
	GL_BindProgram (genericProgram, 0);
	qglUniform1i (gen_attribColors, 0);
	qglUniform1i (gen_attribConsole, 0);
	qglUniform1i (gen_sky, 0);
	qglUniform4f (gen_color, currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2], 1.0);
	
	// draw light to flare connector
	qglEnable(GL_LINE_SMOOTH);
	qglLineWidth(3.0);

	VA_SetElem3 (vCache[0], currentShadowLight->origin[0], currentShadowLight->origin[1], currentShadowLight->origin[2]);
	VA_SetElem3 (vCache[1], currentShadowLight->flareOrigin[0], currentShadowLight->flareOrigin[1], currentShadowLight->flareOrigin[2]);

	qglDrawArrays (GL_LINES, 0, 2);

	qglDisable (GL_LINE_SMOOTH);

	// draw center of flare
	VectorCopy (currentShadowLight->flareOrigin, tmpOrg);
	VectorSet (v[0], tmpOrg[0] - 1, tmpOrg[1] - 1, tmpOrg[2] - 1);
	VectorSet (v[1], tmpOrg[0] - 1, tmpOrg[1] - 1, tmpOrg[2] + 1);
	VectorSet (v[2], tmpOrg[0] - 1, tmpOrg[1] + 1, tmpOrg[2] - 1);
	VectorSet (v[3], tmpOrg[0] - 1, tmpOrg[1] + 1, tmpOrg[2] + 1);
	VectorSet (v[4], tmpOrg[0] + 1, tmpOrg[1] - 1, tmpOrg[2] - 1);
	VectorSet (v[5], tmpOrg[0] + 1, tmpOrg[1] - 1, tmpOrg[2] + 1);
	VectorSet (v[6], tmpOrg[0] + 1, tmpOrg[1] + 1, tmpOrg[2] - 1);
	VectorSet (v[7], tmpOrg[0] + 1, tmpOrg[1] + 1, tmpOrg[2] + 1);


	//front
	VA_SetElem3 (vCache[0], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[1], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[2], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3 (vCache[3], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[4], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[5], v[3][0], v[3][1], v[3][2]);
	//right
	VA_SetElem3 (vCache[6], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[7], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[8], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3 (vCache[9], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[10], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[11], v[5][0], v[5][1], v[5][2]);
	//bottom
	VA_SetElem3 (vCache[12], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[13], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[14], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3 (vCache[15], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3 (vCache[16], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[17], v[6][0], v[6][1], v[6][2]);
	//top
	VA_SetElem3 (vCache[18], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[19], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3 (vCache[20], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3 (vCache[21], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3 (vCache[22], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3 (vCache[23], v[7][0], v[7][1], v[7][2]);
	//left
	VA_SetElem3 (vCache[24], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[25], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3 (vCache[26], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3 (vCache[27], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3 (vCache[28], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3 (vCache[29], v[7][0], v[7][1], v[7][2]);
	//back
	VA_SetElem3 (vCache[30], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[31], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3 (vCache[32], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3 (vCache[33], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3 (vCache[34], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3 (vCache[35], v[7][0], v[7][1], v[7][2]);

	qglDrawArrays (GL_TRIANGLES, 0, 36);

	GL_BindNullProgram ();

	if (r_useLightScissors->value)
		GL_Enable (GL_SCISSOR_TEST);
	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable (GL_DEPTH_BOUNDS_TEST_EXT);
	GL_Enable (GL_STENCIL_TEST);
	GL_Enable (GL_TEXTURE_2D);
	GL_Enable (GL_CULL_FACE);
	qglDisableVertexAttribArray (ATRB_POSITION);
}


void R_DrawLightBounds(void) {

	vec3_t		v[8];
	vec3_t		tmpOrg;

	if (!r_debugLights->value)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (!currentShadowLight->isStatic)
		return;

	GL_Disable(GL_SCISSOR_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable(GL_DEPTH_BOUNDS_TEST_EXT);

	GL_Disable(GL_BLEND);
	GL_Disable(GL_STENCIL_TEST);
	GL_Disable(GL_TEXTURE_2D);
	GL_Disable(GL_CULL_FACE);

	if (r_debugLights->value == 2)
		GL_Disable(GL_DEPTH_TEST);

	// setup program
	GL_BindProgram(genericProgram, 0);
	qglUniform1i(gen_attribColors, 0);
	qglUniform1i(gen_attribConsole, 0);
	qglUniform1i(gen_sky, 0);
	qglUniform4f(gen_color, currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2], 1.0);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, qfalse, 0, vCache);
		
	VectorCopy(currentShadowLight->origin, tmpOrg);


	VectorSet(v[0], tmpOrg[0] - 5, tmpOrg[1] - 5, tmpOrg[2] - 5);
	VectorSet(v[1], tmpOrg[0] - 5, tmpOrg[1] - 5, tmpOrg[2] + 5);
	VectorSet(v[2], tmpOrg[0] - 5, tmpOrg[1] + 5, tmpOrg[2] - 5);
	VectorSet(v[3], tmpOrg[0] - 5, tmpOrg[1] + 5, tmpOrg[2] + 5);
	VectorSet(v[4], tmpOrg[0] + 5, tmpOrg[1] - 5, tmpOrg[2] - 5);
	VectorSet(v[5], tmpOrg[0] + 5, tmpOrg[1] - 5, tmpOrg[2] + 5);
	VectorSet(v[6], tmpOrg[0] + 5, tmpOrg[1] + 5, tmpOrg[2] - 5);
	VectorSet(v[7], tmpOrg[0] + 5, tmpOrg[1] + 5, tmpOrg[2] + 5);

	//front
	VA_SetElem3(vCache[0], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3(vCache[1], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3(vCache[2], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3(vCache[3], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3(vCache[4], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3(vCache[5], v[3][0], v[3][1], v[3][2]);
	//right
	VA_SetElem3(vCache[6], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3(vCache[7], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3(vCache[8], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3(vCache[9], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3(vCache[10], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3(vCache[11], v[5][0], v[5][1], v[5][2]);
	//bottom
	VA_SetElem3(vCache[12], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3(vCache[13], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3(vCache[14], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3(vCache[15], v[0][0], v[0][1], v[0][2]);
	VA_SetElem3(vCache[16], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3(vCache[17], v[6][0], v[6][1], v[6][2]);
	//top
	VA_SetElem3(vCache[18], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3(vCache[19], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3(vCache[20], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3(vCache[21], v[1][0], v[1][1], v[1][2]);
	VA_SetElem3(vCache[22], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3(vCache[23], v[7][0], v[7][1], v[7][2]);
	//left
	VA_SetElem3(vCache[24], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3(vCache[25], v[3][0], v[3][1], v[3][2]);
	VA_SetElem3(vCache[26], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3(vCache[27], v[2][0], v[2][1], v[2][2]);
	VA_SetElem3(vCache[28], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3(vCache[29], v[7][0], v[7][1], v[7][2]);
	//back
	VA_SetElem3(vCache[30], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3(vCache[31], v[5][0], v[5][1], v[5][2]);
	VA_SetElem3(vCache[32], v[7][0], v[7][1], v[7][2]);
	VA_SetElem3(vCache[33], v[4][0], v[4][1], v[4][2]);
	VA_SetElem3(vCache[34], v[6][0], v[6][1], v[6][2]);
	VA_SetElem3(vCache[35], v[7][0], v[7][1], v[7][2]);

	qglDrawArrays(GL_TRIANGLES, 0, 36);

	qglDisableVertexAttribArray(ATRB_POSITION);
	GL_BindNullProgram();
	GL_Enable(GL_TEXTURE_2D);
	GL_Enable(GL_CULL_FACE);
	GL_Enable(GL_BLEND);

	if (r_debugLights->value == 2)
		GL_Enable(GL_DEPTH_TEST);

	if (r_shadows->value)
		GL_Enable(GL_STENCIL_TEST);

	if (r_useLightScissors->value)
		GL_Enable(GL_SCISSOR_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable(GL_DEPTH_BOUNDS_TEST_EXT);
}