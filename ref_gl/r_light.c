/*
Copyright (C) 1997-2001 Id Software, Inc.

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
// r_light.c

#include "r_local.h"

int r_dlightframecount;

/*
 =================
 BoundsAndSphereIntersect
 =================
*/
qboolean BoundsAndSphereIntersect (const vec3_t mins, const vec3_t maxs, const vec3_t origin, float radius){

	if (mins[0] > origin[0] + radius || mins[1] > origin[1] + radius || mins[2] > origin[2] + radius)
		return false;
	if (maxs[0] < origin[0] - radius || maxs[1] < origin[1] - radius || maxs[2] < origin[2] - radius)
		return false;

	return true;
}

/*
=============
R_MarkLights
=============
*/

void R_MarkLights(dlight_t * light, int bit, mnode_t * node)
{
	cplane_t *splitplane;
	float dist;
	msurface_t *surf;
	int i, sidebit;

	if (node->contents != -1)
		return;

	splitplane = node->plane;
	dist =
		DotProduct(light->origin, splitplane->normal) - splitplane->dist;

	if (dist > light->intensity) {
		R_MarkLights(light, bit, node->children[0]);
		return;
	}
	if (dist < -light->intensity) {
		R_MarkLights(light, bit, node->children[1]);
		return;
	}
// mark the polygons
	surf = r_worldmodel->surfaces + node->firstsurface;
	for (i = 0; i < node->numsurfaces; i++, surf++) {
	
	if (!BoundsAndSphereIntersect(surf->mins, surf->maxs, light->origin, light->intensity))
			continue;		// No intersection


		dist = DotProduct(light->origin, surf->plane->normal) - surf->plane->dist;
		if (dist >= 0)
			sidebit = 0;
		else
		sidebit = SURF_PLANEBACK;

		if ((surf->flags & SURF_PLANEBACK) != sidebit)	// Discoloda
			continue;			

		if (surf->dlightframe != r_dlightframecount) {
			surf->dlightbits = 0;
			surf->dlightframe = r_dlightframecount;
		}
		surf->dlightbits |= bit;
				
	}

	R_MarkLights(light, bit, node->children[0]);
	R_MarkLights(light, bit, node->children[1]);

}




/*
=============
R_PushDlights
=============
*/
void R_PushDlights(void)
{
	int i;
	dlight_t *l;

	r_dlightframecount = r_framecount + 1;	// because the count hasn't
	// advanced yet for this frame
	l = r_newrefdef.dlights;
	for (i = 0; i < r_newrefdef.num_dlights; i++, l++)
		R_MarkLights(l, 1 << i, r_worldmodel->nodes);
}


/*
=============================================================================

LIGHT SAMPLING

=============================================================================
*/

vec3_t pointcolor;
cplane_t *lightplane;			// used as shadow plane
vec3_t lightspot;

int RecursiveLightPoint(mnode_t * node, vec3_t start, vec3_t end)
{
	float front, back, frac;
	int side;
	cplane_t *plane;
	vec3_t mid;
	msurface_t *surf;
	int s, t, ds, dt;
	int i;
	mtexinfo_t *tex;
	byte *lightmap;
	int maps;
	int r;

	if (node->contents != -1)
		return -1;				// didn't hit anything

// calculate mid point

// FIXME: optimize for axial

	plane = node->plane;
	front = DotProduct(start, plane->normal) - plane->dist;
	back = DotProduct(end, plane->normal) - plane->dist;
	side = front < 0;

	if ((back < 0) == side)
		return RecursiveLightPoint(node->children[side], start, end);


	frac = front / (front - back);
	mid[0] = start[0] + (end[0] - start[0]) * frac;
	mid[1] = start[1] + (end[1] - start[1]) * frac;
	mid[2] = start[2] + (end[2] - start[2]) * frac;

// go down front side   
	r = RecursiveLightPoint(node->children[side], start, mid);
	if (r >= 0)
		return r;				// hit something

	if ((back < 0) == side)
		return -1;				// didn't hit anuthing

// check for impact on this node
	VectorCopy(mid, lightspot);
	lightplane = plane;

	surf = r_worldmodel->surfaces + node->firstsurface;

	for (i = 0; i < node->numsurfaces; i++, surf++) {
		if (surf->flags & (SURF_DRAWTURB | SURF_DRAWSKY))
			continue;			// no lightmaps

		tex = surf->texinfo;

		s = DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3];
		t = DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3];;

		if (s < surf->texturemins[0] || t < surf->texturemins[1])
			continue;

		ds = s - surf->texturemins[0];
		dt = t - surf->texturemins[1];

		if (ds > surf->extents[0] || dt > surf->extents[1])
			continue;

		if (!surf->samples)
			return 0;

		ds /= r_worldmodel->lightmap_scale;
		dt /= r_worldmodel->lightmap_scale;

		lightmap = surf->samples;
		VectorCopy(vec3_origin, pointcolor);
		if (lightmap) {
			vec3_t scale;

			lightmap += 3 * (dt * ((surf->extents[0] / r_worldmodel->lightmap_scale) + 1) + ds);
			for (maps = 0;
				 maps < MAXLIGHTMAPS && surf->styles[maps] != 255;
				 maps++) {
				for (i = 0; i < 3; i++)
					scale[i] = r_newrefdef.lightstyles[surf->styles[maps]].rgb[i];

				pointcolor[0] +=
					lightmap[0] * scale[0] *
					0.003921568627450980392156862745098;
				pointcolor[1] +=
					lightmap[1] * scale[1] *
					0.003921568627450980392156862745098;
				pointcolor[2] +=
					lightmap[2] * scale[2] *
					0.003921568627450980392156862745098;
				lightmap +=
					3 * ((surf->extents[0] / r_worldmodel->lightmap_scale) +
						 1) * ((surf->extents[1] / r_worldmodel->lightmap_scale) + 1);
			}
		}

		return 1;
	}

// go down back side
	return RecursiveLightPoint(node->children[!side], mid, end);
}

/*
===============
R_LightPoint
===============
*/

void R_LightPoint(vec3_t p, vec3_t color, qboolean bump)
{
	vec3_t end;
	float r;
	int i;
	dlight_t *dl;
	vec3_t dir;
	float add, dst;
	trace_t trace;

	if ((r_worldmodel && !r_worldmodel->lightdata) || !r_worldmodel)
	{
          color[0] = color[1] = color[2] = 1.0;
          return;
     }
	
	if (!r_newrefdef.areabits)
		return;

	end[0] = p[0];
	end[1] = p[1];
	end[2] = p[2] - 8192;

	r = RecursiveLightPoint(r_worldmodel->nodes, p, end);
	

	if (r == -1) {
		VectorCopy(vec3_origin, color);
	} else {
		VectorCopy(pointcolor, color);
	}

	// this catches too bright modulated color
	for (i = 0; i < 3; i++)
		if (color[i] > 1)
			color[i] = 1;
	
	if(!bump){
	
	dl = r_newrefdef.dlights;
	for (i=0; i<r_newrefdef.num_dlights; i++, dl++){
	
	VectorSubtract(dl->origin, p, dir);
	dst = VectorLength(dir);
	
	if (!dst || dst > dl->intensity)
		continue;

	// dlight behind the wall
	if (r_newrefdef.areabits){
			trace = CM_BoxTrace(dl->origin, p, vec3_origin, vec3_origin, r_worldmodel->firstnode, MASK_OPAQUE);
			if(trace.fraction != 1.0)
			continue;
	}

	add = (dl->intensity - dst) * (1.0/255);
	VectorMA(color, add, dl->color, color);
	}
	}
}


#define LIGHTGRID_STEP 128
#define LIGHTGRID_NUM_STEPS (8192/LIGHTGRID_STEP)	// 64

byte r_lightgrid[LIGHTGRID_NUM_STEPS * LIGHTGRID_NUM_STEPS *
				 LIGHTGRID_NUM_STEPS][3];

//___________________________________________________________
void R_InitLightgrid(void)
{
	int i, x, y, z;
	vec3_t p, end;
	float r;
	byte *b;

	memset(r_lightgrid, 0, sizeof(r_lightgrid));

	if (!r_worldmodel->lightdata)
		return;

	b = &r_lightgrid[0][0];
	// Huh ?
	for (x = 0; x < 8192; x += LIGHTGRID_STEP)
		for (y = 0; y < 8192; y += LIGHTGRID_STEP)
			for (z = 0; z < 8192; z += LIGHTGRID_STEP) {
				end[0] = p[0] = x - 4096;
				end[1] = p[1] = y - 4096;
				end[2] = (p[2] = z - 4096) - 2048;
				r = RecursiveLightPoint(r_worldmodel->nodes, p, end);
				if (r != -1) {
					for (i = 0; i < 3; i++) {
						float mu = pointcolor[i];
						float f = mu * (2 * (1 - mu) + mu);
						*b++ = 255.0 * f;
					}
				} else {
					b += 3;
				}
			}
}

//___________________________________________________________
void R_LightColor(vec3_t org, vec3_t color)
{
	byte *b[8];
	int i, lnum;
	dlight_t *dl;
	float f, light, add;
	vec3_t dist;
	float x = (4096 + org[0]) / LIGHTGRID_STEP;
	float y = (4096 + org[1]) / LIGHTGRID_STEP;
	float z = (4096 + org[2]) / LIGHTGRID_STEP;
	int s = x;
	int t = y;
	int r = z;

	x = x - s;
	y = y - t;
	z = z - r;

	b[0] =
		&r_lightgrid[(((s * LIGHTGRID_NUM_STEPS) +
					   t) * LIGHTGRID_NUM_STEPS) + r][0];
	b[1] =
		&r_lightgrid[(((s * LIGHTGRID_NUM_STEPS) +
					   t) * LIGHTGRID_NUM_STEPS) + r + 1][0];
	b[2] =
		&r_lightgrid[(((s * LIGHTGRID_NUM_STEPS) + t +
					   1) * LIGHTGRID_NUM_STEPS) + r][0];
	b[3] =
		&r_lightgrid[(((s * LIGHTGRID_NUM_STEPS) + t +
					   1) * LIGHTGRID_NUM_STEPS) + r + 1][0];
	b[4] =
		&r_lightgrid[((((s + 1) * LIGHTGRID_NUM_STEPS) +
					   t) * LIGHTGRID_NUM_STEPS) + r][0];
	b[5] =
		&r_lightgrid[((((s + 1) * LIGHTGRID_NUM_STEPS) +
					   t) * LIGHTGRID_NUM_STEPS) + r + 1][0];
	b[6] =
		&r_lightgrid[((((s + 1) * LIGHTGRID_NUM_STEPS) + t +
					   1) * LIGHTGRID_NUM_STEPS) + r][0];
	b[7] =
		&r_lightgrid[((((s + 1) * LIGHTGRID_NUM_STEPS) + t +
					   1) * LIGHTGRID_NUM_STEPS) + r + 1][0];

	f = ((float) 4 / (float) (255));

	if (!(b[0][0] && b[0][1] && b[0][2]))
		f -= (1.0 / (510.0)) * (1 - x) * (1 - y) * (1 - z);
	if (!(b[1][0] && b[1][1] && b[1][2]))
		f -= (1.0 / (510.0)) * (1 - x) * (1 - y) * (z);
	if (!(b[2][0] && b[2][1] && b[2][2]))
		f -= (1.0 / (510.0)) * (1 - x) * (y) * (1 - z);
	if (!(b[3][0] && b[3][1] && b[3][2]))
		f -= (1.0 / (510.0)) * (1 - x) * (y) * (z);
	if (!(b[4][0] && b[4][1] && b[4][2]))
		f -= (1.0 / (510.0)) * (x) * (1 - y) * (1 - z);
	if (!(b[5][0] && b[5][1] && b[5][2]))
		f -= (1.0 / (510.0)) * (x) * (1 - y) * (z);
	if (!(b[6][0] && b[6][1] && b[6][2]))
		f -= (1.0 / (510.0)) * (x) * (y) * (1 - z);
	if (!(b[7][0] && b[7][1] && b[7][2]))
		f -= (1.0 / (510.0)) * (x) * (y) * (z);

	for (i = 0; i < 3; i++) {	// hahaha slap me silly
		color[i] =
			(1 - x) * ((1 - y) * ((1 - z) * b[0][i] + (z) * b[1][i]) +
		y * ((1 - z) * b[2][i] + (z) * b[3][i]))
	  + x * ((1 - y) * ((1 - z) * b[4][i] + (z) * b[5][i]) +
	    y * ((1 - z) * b[6][i] + (z) * b[7][i]));
		color[i] *= f;

		if(color[i] <= 0.35)
			color[i] = 0.35;


	}

	// add dynamic light
	light = 0;
	dl = r_newrefdef.dlights;
	for (lnum = 0; lnum < r_newrefdef.num_dlights; lnum++, dl++) {
		VectorSubtract(org, dl->origin, dist);
		f = ((dl->intensity * 2) -
			 VectorLength(dist)) / (dl->intensity * 2);
		if (f <= 0)
			continue;
		add = f * (0.2 + (0.8 * f));
		if (add > 0.01) {
			VectorMA(color, add, dl->color, color);
		}
	}
}

/*==============================

Per Pixel Lighting Light Manager

==============================*/

worldShadowLight_t *shadowLight_static = NULL, *shadowLight_frame = NULL;
static worldShadowLight_t shadowLightsBlock[MAX_WORLD_SHADOW_LIHGTS];
static int num_dlits;
static int numCulledLights;

qboolean EntityInLightSphere(worldShadowLight_t *light) {

	vec3_t dst;

	VectorSubtract (light->origin, currententity->origin, dst);
	return
		(VectorLength (dst) < (light->radius + currentmodel->radius));

		
		
}

qboolean R_CullLight(worldShadowLight_t *light) {
	
	float c;
	vec3_t mins, maxs, none = {1, 1, 1};
	int sidebit;
	float viewplane;

	if (r_newrefdef.areabits){
		if (!(r_newrefdef.areabits[light->area >> 3] & (1 << (light->area & 7)))){
			return true;
		}
	}
	
	if (!HasSharedLeafs (light->vis, viewvis))
		return true;

	if(light->surf){

	viewplane = DotProduct(currententity->origin, light->surf->plane->normal) - light->surf->plane->dist;
			
	if (viewplane >= 0)
		sidebit = 0;
	else
		sidebit = SURF_PLANEBACK;

	if ((light->surf->flags & SURF_PLANEBACK) != sidebit)
		return true;
	}

	c = (light->sColor[0] + light->sColor[1] + light->sColor[2]) * light->radius*(1.0/3.0);
		if(c < 0.1)
			return true;
	
	VectorMA(light->origin,  light->radius, none, maxs);
	VectorMA(light->origin, -light->radius, none, mins);

	if(R_CullBox(mins, maxs))
		return true;

	return false;
	}

void R_AddDynamicLight(dlight_t *dl) {
	
	worldShadowLight_t *light;
	vec3_t mins, maxs, none = {1, 1, 1};
	float c;

	c = (dl->color[0] + dl->color[1] + dl->color[2]) * dl->intensity*(1.0/3.0);
	if(c < 0.1)
		return;

	VectorMA(dl->origin, dl->intensity, none, maxs);
	VectorMA(dl->origin, -dl->intensity, none, mins);

	if(R_CullBox(mins, maxs)){
		numCulledLights++;
		return;
	}

	light = &shadowLightsBlock[num_dlits++];
	memset(light, 0, sizeof(worldShadowLight_t));
	light->next = shadowLight_frame;
	shadowLight_frame = light;

	VectorCopy(dl->origin, light->origin);
	VectorCopy(dl->color, light->sColor);
	light->radius = dl->intensity;
	light->isStatic = false;
}


void R_PrepareShadowLightFrame(void) {
	
	int i;
	worldShadowLight_t *light;
	num_dlits = 0;
	shadowLight_frame = NULL;

	numCulledLights = 0;
	// add pre computed lights
	if(shadowLight_static) {
		for(light = shadowLight_static; light; light = light->s_next) {
			if(R_CullLight(light)){
				numCulledLights++;
				continue;
			}
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

	Com_DPrintf("%i lights was culled\n", numCulledLights);
	
	if(!shadowLight_frame) 
		return;

	for(light = shadowLight_frame; light; light = light->next) {

		VectorCopy(light->sColor, light->color);

	if(r_newrefdef.areabits)
		{
		light->color[0] *= r_newrefdef.lightstyles[light->style].rgb[0];
		light->color[1] *= r_newrefdef.lightstyles[light->style].rgb[1];
		light->color[2] *= r_newrefdef.lightstyles[light->style].rgb[2];
		}

		light->mins[0] = light->origin[0] - light->radius;
		light->mins[1] = light->origin[1] - light->radius;
		light->mins[2] = light->origin[2] - light->radius;
		light->maxs[0] = light->origin[0] + light->radius;
		light->maxs[1] = light->origin[1] + light->radius;
		light->maxs[2] = light->origin[2] + light->radius;
	}

}


worldShadowLight_t *R_AddNewWorldLight(vec3_t origin, vec3_t color, float radius, int style, qboolean isStatic, qboolean isShadow, msurface_t *surf) {
	
	worldShadowLight_t *light;
	int leafnum;
	int cluster;
	
	light = (worldShadowLight_t*)malloc(sizeof(worldShadowLight_t));
	light->s_next = shadowLight_static;
	shadowLight_static = light;

	VectorCopy(origin, light->origin);
	VectorCopy(color, light->sColor);
	light->radius = radius;
	light->isStatic = isStatic;
	light->isShadow = isShadow;
	light->ignore = false;
	light->next = NULL;
	light->style = style;

	// cull info
	light->surf = surf;
	leafnum = CM_PointLeafnum(light->origin);
	cluster = CM_LeafCluster(leafnum);
	light->area = CM_LeafArea(leafnum);
	Q_memcpy(light->vis, CM_ClusterPVS(cluster), (CM_NumClusters() + 7) >> 3);

	if(!light->style)
		r_numWorlsShadowLights++;
	return light;
}

int Load_BspLights(void) {
	
	int addLight, style, numlights, addLight_mine;
	char *c, *token, key[256], *value;
	float color[3], origin[3], radius;

	if(!r_worldmodel) {
		Com_Printf(PRINT_ALL, "No map loaded.\n");
		return 0;
	}

	c = CM_EntityString();
	numlights = 0;

	while(1) {
		token = COM_Parse(&c);
		if(!c)
			break;

		color[0] = 0.5;
		color[1] = 0.5;
		color[2] = 0.5;
		radius = 300;
		origin[0] = 0;
		origin[1] = 0;
		origin[2] = 0;
		addLight = false;
		addLight_mine = false;
		style = 0;

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
					addLight_mine = true;
				}
				if(!Q_stricmp(value, "light_mine2")){
					addLight = true;
					addLight_mine = true;
				}
			}

			if(!Q_stricmp(key, "light"))
				radius = atof(value);
			else if(!Q_stricmp(key, "origin"))
				sscanf(value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
			else if(!Q_stricmp(key, "_color"))
				sscanf(value, "%f %f %f", &color[0], &color[1], &color[2]);
			else if(!Q_stricmp(key, "style"))
				style = atoi(value);
		}

		if(addLight) {
			if((style > 0 && style < 12) || addLight_mine){
			R_AddNewWorldLight(origin, color, radius, style, true, true, NULL);
			numlights++;	
			}
		}
	}
	Com_DPrintf("loaded %i bsp lights with styles\n",numlights);
	return numlights;
}

void CleanDuplicateLights(void){

	worldShadowLight_t *light1, *light2;
	vec3_t tmp;
	
	for(light1 = shadowLight_static; light1; light1 = light1->s_next) {

	for(light2 = light1->s_next; light2; light2 = light2->s_next) {

		VectorSubtract(light2->origin, light1->origin, tmp);
  
		if (VectorLength(tmp) < r_lightsWeldThreshold->value){
	
			light2->ignore = true;  
			VectorAdd(light1->origin, light2->origin, tmp);
			VectorScale(tmp, 0.5f, light1->origin);
			r_numIgnoreLights++;
		}
	}
}

Com_DPrintf("loaded %i world lights, %i lights ignored\n", r_numWorlsShadowLights, r_numIgnoreLights);
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
	r_numIgnoreLights = 0;

}