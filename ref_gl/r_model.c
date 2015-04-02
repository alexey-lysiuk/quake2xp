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
// models.c -- model loading and caching

#include "r_local.h"

model_t *loadmodel;
int modfilelen;

void Mod_LoadSpriteModel (model_t * mod, void *buffer);
void Mod_LoadBrushModel (model_t * mod, void *buffer);
void Mod_LoadAliasModel (model_t * mod, void *buffer);

model_t *Mod_LoadModel (model_t * mod, qboolean crash);

byte mod_novis[MAX_MAP_LEAFS / 8];

#define	MAX_MOD_KNOWN	512
model_t mod_known[MAX_MOD_KNOWN];
int mod_numknown;

// the inline * models from the current map are kept seperate
model_t mod_inline[MAX_MOD_KNOWN];

int registration_sequence;
extern char map_entitystring[MAX_MAP_ENTSTRING];
extern qboolean relightMap;

byte Normal2Index (const vec3_t vec);

static float	r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};

int bspSize, aliasSize, spriteSize;
int numentitychars;
byte *mod_base;
int *mod_xplmOffsets;		// face light offsets from .xplm file, freed after level is loaded

/*
=================
Mod_LoadEntityString
=================
*/
void Mod_LoadEntityString (lump_t * l) {
	numentitychars = l->filelen;

	if (l->filelen > MAX_MAP_ENTSTRING)
		VID_Error (ERR_DROP, "Map has too large entity lump");

	Q_memcpy (map_entitystring, mod_base + l->fileofs, l->filelen);
}



static void R_ClearFlares (void) {
	memset (r_flares, 0, sizeof(r_flares));
	r_numflares = 0;
	r_numIgnoreflares = 0;

}

void GL_AddFlareSurface (msurface_t * surf) {
	int i, width, height, intens;
	glpoly_t *poly;
	byte *buffer;
	byte *p;
	float *v, surf_bound;
	vec3_t origin = { 0, 0, 0 }, color = { 1, 1, 1 }, tmp, rgbSum;
	vec3_t poly_center, mins, maxs, tmp1, lightOffset, radius;
	int leafnum;
	int cluster;
	char target[MAX_QPATH];

	if (surf->texInfo->
		flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_FLOWING |
		MSURF_DRAWTURB | SURF_WARP))
		return;

	if (!(surf->texInfo->flags & (SURF_LIGHT)))
		return;

	intens = surf->texInfo->value;

	if (r_numflares == MAX_FLARES)
		return;

	if (intens <= 1000)
		return;
	r_flares[r_numflares].lightIntens = intens;
	/*
	   =================== find poligon centre =================== */
	VectorSet (mins, 999999, 999999, 999999);
	VectorSet (maxs, -999999, -999999, -999999);

	for (poly = surf->polys; poly; poly = poly->chain) {
		for (i = 0, v = poly->verts[0]; i < poly->numVerts;
			i++, v += VERTEXSIZE) {

			if (v[0] > maxs[0])
				maxs[0] = v[0];
			if (v[1] > maxs[1])
				maxs[1] = v[1];
			if (v[2] > maxs[2])
				maxs[2] = v[2];

			if (v[0] < mins[0])
				mins[0] = v[0];
			if (v[1] < mins[1])
				mins[1] = v[1];
			if (v[2] < mins[2])
				mins[2] = v[2];
		}
	}


	poly_center[0] = (mins[0] + maxs[0]) / 2;
	poly_center[1] = (mins[1] + maxs[1]) / 2;
	poly_center[2] = (mins[2] + maxs[2]) / 2;
	VectorCopy (poly_center, origin);

	/* =======calc light surf bounds and flare size ========== */


	VectorSubtract (maxs, mins, tmp1);
	surf_bound = VectorLength (tmp1);
	if (surf_bound <= 25)
		r_flares[r_numflares].size = 10;
	else if (surf_bound <= 50)
		r_flares[r_numflares].size = 15;
	else if (surf_bound <= 100)
		r_flares[r_numflares].size = 20;
	else if (surf_bound <= 150)
		r_flares[r_numflares].size = 25;
	else if (surf_bound <= 200)
		r_flares[r_numflares].size = 30;
	else if (surf_bound <= 250)
		r_flares[r_numflares].size = 35;
	r_flares[r_numflares].sizefull = surf_bound;

	/* =================== calc texture color =================== */

	GL_Bind (surf->texInfo->image->texnum);
	width = surf->texInfo->image->upload_width;
	height = surf->texInfo->image->upload_height;

	buffer = (byte*)malloc (width * height * 3);
	qglGetTexImage (GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	VectorClear (rgbSum);

	for (i = 0, p = buffer; i < width * height; i++, p += 3) {
		rgbSum[0] += (float)p[0] * (1.0 / 255);
		rgbSum[1] += (float)p[1] * (1.0 / 255);
		rgbSum[2] += (float)p[2] * (1.0 / 255);
	}

	VectorScale (rgbSum, r_flaresIntens->value / (width * height), color);

	for (i = 0; i < 3; i++) {
		if (color[i] < 0.5)
			color[i] = color[i] * 0.5;
		else
			color[i] = color[i] * 0.5 + 0.5;
	}
	VectorCopy (color, r_flares[r_numflares].color);

	/* ============== move flare origin in to map bounds ============ */

	if (surf->flags & MSURF_PLANEBACK)
		VectorNegate (surf->plane->normal, tmp);
	else
		VectorCopy (surf->plane->normal, tmp);

	VectorMA (origin, 2, tmp, origin);
	VectorMA (origin, 10, tmp, lightOffset);

	VectorCopy (origin, r_flares[r_numflares].origin);
	VectorCopy (tmp, r_flares[r_numflares].lightsurf_origin);
	r_flares[r_numflares].surf = surf;
	r_flares[r_numflares].style = 0;
	r_flares[r_numflares].occId = numFlareOcc;
	numFlareOcc++;

	leafnum = CM_PointLeafnum (r_flares[r_numflares].origin);
	cluster = CM_LeafCluster (leafnum);
	r_flares[r_numflares].area = CM_LeafArea (leafnum);
	Q_memcpy (r_flares[r_numflares].vis, CM_ClusterPVS (cluster), (CM_NumClusters () + 7) >> 3);

	if (relightMap) {
		VectorSet (radius, r_flares[r_numflares].size * 10.0,
			r_flares[r_numflares].size * 10.0,
			r_flares[r_numflares].size * 10.0);

		memset (target, 0, sizeof(target));

		R_AddNewWorldLight (lightOffset, r_flares[r_numflares].color, radius, 0, 0, vec3_origin,
			vec3_origin, true, 1, 0, 0, false, 1, origin, r_flares[r_numflares].size, target, 0, 0, 0.0);
	}

	r_numflares++;
	free (buffer);

}

void CleanDuplicateFlares () {

	int i, j;
	flare_t *f1, *f2;
	vec3_t tmp;

	for (i = 0; i < r_numflares - 1; i++) {

		for (j = i + 1; j < r_numflares; j++) {

			f1 = &r_flares[i];
			f2 = &r_flares[j];

			if (f1->ignore || f2->ignore)
				continue;

			VectorSubtract (f2->origin, f1->origin, tmp);

			if (VectorLength (tmp) < r_flareWeldThreshold->value) {

				f2->ignore = true;
				VectorAdd (f1->origin, f2->origin, tmp);
				VectorScale (tmp, 0.5f, f1->origin);
				r_numIgnoreflares++;
			}
		}
	}
}


/*
=================
R_ModelBounds
=================
*/
void R_ModelBounds (model_t * model, vec3_t mins, vec3_t maxs) {
	if (model) {
		VectorCopy (model->mins, mins);
		VectorCopy (model->maxs, maxs);
	}
}

void R_ModelRadius (model_t * model, vec3_t rad) {
	if (model)
		rad[0] = model->radius;
}

void R_ModelCenter (model_t * model, vec3_t center) {
	if (model)
		VectorCopy (model->center, center);
}

void CalcSurfaceBounds (msurface_t *s) {
	glpoly_t	*p;
	float		*v;
	int			i, j;
	vec3_t		mins, maxs;

	if (s->polys) {
		mins[0] = mins[1] = mins[2] = 999999;
		maxs[0] = maxs[1] = maxs[2] = -999999;

		for (p = s->polys; p; p = p->next)
		for (i = 0, v = p->verts[0]; i < p->numVerts; i++, v += VERTEXSIZE)
		for (j = 0; j<3; j++) {
			if (mins[j] > v[j])
				mins[j] = v[j];
			if (maxs[j] < v[j])
				maxs[j] = v[j];
		}

		VectorCopy (mins, s->mins);
		VectorCopy (maxs, s->maxs);

	}
}




/*
===============
Mod_PointInLeaf
===============
*/
mleaf_t *Mod_PointInLeaf (vec3_t p, model_t * model) {
	mnode_t *node;
	float d;
	cplane_t *plane;

	if (!model || !model->nodes)
		VID_Error (ERR_DROP, "Mod_PointInLeaf: bad model");

	node = model->nodes;
	while (1) {
		if (node->contents != -1)
			return (mleaf_t *)node;
		plane = node->plane;
		d = DotProduct (p, plane->normal) - plane->dist;
		if (d > 0)
			node = node->children[0];
		else
			node = node->children[1];
	}

	return NULL;				// never reached
}


/*
===================
Mod_DecompressVis
===================
*/
byte *Mod_DecompressVis (byte * in, model_t * model) {
	static byte decompressed[MAX_MAP_LEAFS / 8];
	int c;
	byte *out;
	int row;

	row = (model->vis->numclusters + 7) >> 3;
	out = decompressed;

	if (!in) {					// no vis info, so make all visible
		while (row) {
			*out++ = 0xff;
			row--;
		}
		return decompressed;
	}

	do {
		if (*in) {
			*out++ = *in++;
			continue;
		}

		c = in[1];
		in += 2;
		while (c) {
			*out++ = 0;
			c--;
		}
	} while (out - decompressed < row);

	return decompressed;
}

/*
==============
Mod_ClusterPVS
==============
*/
byte *Mod_ClusterPVS (int cluster, model_t * model) {
	if (cluster == -1 || !model->vis)
		return mod_novis;
	return Mod_DecompressVis ((byte *)model->vis +
		model->vis->bitofs[cluster][DVIS_PVS], model);
}


//===============================================================================

/*
================
Mod_Modellist_f
================
*/
void Mod_Modellist_f (void) {
	int i;
	model_t *mod;
	int total;

	total = 0;
	Com_Printf ("Loaded models:\n");
	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++) {
		if (!mod->name[0])
			continue;
		Com_Printf ("%8i : %s\n", mod->extraDataSize, mod->name);
		total += mod->extraDataSize;
	}
	Com_Printf ("Total resident: %i\n", total);
}

/*
===============
Mod_Init
===============
*/
void CL_ClearDecals (void);

void Mod_Init (void) {
	memset (mod_novis, 0xff, sizeof(mod_novis));
	CL_ClearDecals ();
}



/*
==================
Mod_ForName

Loads in a model for the given name
==================
*/
model_t *Mod_ForName (char *name, qboolean crash) {
	model_t *mod;
	unsigned *buf;
	int i;

	if (!name[0])
		VID_Error (ERR_DROP, "Mod_ForName: NULL name");

	// 
	// inline models are grabbed only from worldmodel
	// 
	if (name[0] == '*') {
		i = atoi (name + 1);
		if (i < 1 || !r_worldmodel || i >= r_worldmodel->numSubModels)
			VID_Error (ERR_DROP, "bad inline model number");
		return &mod_inline[i];
	}
	// 
	// search the currently loaded models
	// 
	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++) {
		if (!mod->name[0])
			continue;
		if (!strcmp (mod->name, name)) {
			if (mod->type == mod_alias) {
				int i = 0;
				image_t *img;

				img = mod->skins[i];
			}

			return mod;
		}
	}

	// 
	// find a free model slot spot
	// 

	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++) {
		if (!mod->name[0])
			break;				// free spot
	}

	if (i == mod_numknown) {
		if (mod_numknown == MAX_MOD_KNOWN)
			VID_Error (ERR_DROP, "mod_numknown == MAX_MOD_KNOWN");

		mod_numknown++;
	}

	memset (mod, 0, sizeof(model_t));

	strcpy (mod->name, name);

	// 
	// load the file
	// 

	modfilelen = FS_LoadFile (mod->name, (void**)&buf);

	if (!buf) {
		if (crash)
			VID_Error (ERR_DROP, "Mod_ForName: %s not found", mod->name);

		memset (mod->name, 0, sizeof(mod->name));

		return NULL;
	}

	loadmodel = mod;

	// 
	// fill it in
	// 

	// call the apropriate loader

	switch (LittleLong (*(unsigned *)buf)) {
		case IDALIASHEADER:
			loadmodel->extraData = Hunk_Begin (hunk_model->value * 1048576, name);
			Mod_LoadAliasModel (mod, buf);
			break;

		case IDSPRITEHEADER:
			loadmodel->extraData = Hunk_Begin (hunk_sprite->value * 1048576, name);
			Mod_LoadSpriteModel (mod, buf);
			break;


		case IDBSPHEADER:
			loadmodel->extraData = Hunk_Begin (hunk_bsp->value * 1048576, name);
			Mod_LoadBrushModel (mod, buf);
			break;

		default:
			VID_Error (ERR_DROP, "Mod_NumForName: unknown fileid for %s", mod->name);
			break;
	}


	loadmodel->extraDataSize = Hunk_End ();

	FS_FreeFile (buf);

	return mod;
}

/*
===============================================================================

BRUSHMODEL LOADING

===============================================================================
*/

byte *mod_base;


/*
=================
Mod_LoadLighting
=================
*/
void Mod_LoadLighting (lump_t * l) {
	char *s, *c;

	loadmodel->useXPLM = false;

	if (!l->filelen) {
		loadmodel->lightData = NULL;
		loadmodel->lightmap_scale = 16;
		return;
	}
	loadmodel->lightData = (byte*)Hunk_Alloc (l->filelen);
	Q_memcpy (loadmodel->lightData, mod_base + l->fileofs, l->filelen);

	loadmodel->memorySize += l->filelen;

	loadmodel->lightmap_scale = -1;

	if ((s = strstr (CM_EntityString (), "\"lightmap_scale\""))) {  // resolve lightmap scale

		c = COM_Parse (&s);  // parse the string itself
		c = COM_Parse (&s);  // and then the value

		loadmodel->lightmap_scale = atoi (c);

		Com_DPrintf ("Resolved lightmap_scale: %i\n", loadmodel->lightmap_scale);
	}

	if (loadmodel->lightmap_scale == -1)  // ensure safe default
		loadmodel->lightmap_scale = 16;

}


/*
=================
Mod_LoadVisibility
=================
*/
void Mod_LoadVisibility (lump_t * l) {
	int i;

	if (!l->filelen) {
		loadmodel->vis = NULL;
		return;
	}
	loadmodel->vis = (dvis_t*)Hunk_Alloc (l->filelen);
	loadmodel->memorySize += l->filelen;
	Q_memcpy (loadmodel->vis, mod_base + l->fileofs, l->filelen);

	loadmodel->vis->numclusters = LittleLong (loadmodel->vis->numclusters);
	for (i = 0; i < loadmodel->vis->numclusters; i++) {
		loadmodel->vis->bitofs[i][0] =
			LittleLong (loadmodel->vis->bitofs[i][0]);
		loadmodel->vis->bitofs[i][1] =
			LittleLong (loadmodel->vis->bitofs[i][1]);
	}
}


/*
=================
Mod_LoadVertexes
=================
*/
void Mod_LoadVertexes (lump_t * l) {
	dvertex_t *in;
	mvertex_t *out;
	int i, count;

	in = (dvertex_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mvertex_t*)Hunk_Alloc (count * sizeof(*out));

	loadmodel->vertexes = out;
	loadmodel->numVertexes = count;

	loadmodel->memorySize += count * sizeof(*out);

	for (i = 0; i < count; i++, in++, out++) {
		out->position[0] = LittleFloat (in->point[0]);
		out->position[1] = LittleFloat (in->point[1]);
		out->position[2] = LittleFloat (in->point[2]);
	}
}

/*
=================
RadiusFromBounds
=================
*/
float RadiusFromBounds (vec3_t mins, vec3_t maxs) {
	int i;
	vec3_t corner;

	for (i = 0; i < 3; i++) {
		corner[i] =
			fabs (mins[i]) > fabs (maxs[i]) ? fabs (mins[i]) : fabs (maxs[i]);
	}

	return VectorLength (corner);
}


/*
=================
Mod_LoadsubModels
=================
*/
void Mod_LoadsubModels (lump_t * l) {
	dmodel_t *in;
	mmodel_t *out;
	int i, j, count;

	in = (dmodel_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mmodel_t*)Hunk_Alloc (count * sizeof(*out));

	loadmodel->subModels = out;
	loadmodel->numSubModels = count;

	loadmodel->memorySize += count * sizeof(*out);

	for (i = 0; i < count; i++, in++, out++) {
		for (j = 0; j < 3; j++) {	// spread the mins / maxs by a pixel
			out->mins[j] = LittleFloat (in->mins[j]) - 1;
			out->maxs[j] = LittleFloat (in->maxs[j]) + 1;
			out->origin[j] = LittleFloat (in->origin[j]);
		}
		out->radius = RadiusFromBounds (out->mins, out->maxs);
		out->headnode = LittleLong (in->headnode);
		out->firstface = LittleLong (in->firstface);
		out->numfaces = LittleLong (in->numfaces);
	}
}

/*
=================
Mod_LoadEdges
=================
*/
void Mod_LoadEdges (lump_t * l) {
	dedge_t *in;
	medge_t *out;
	int i, count;

	in = (dedge_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (medge_t*)Hunk_Alloc ((count + 1) * sizeof(*out));

	loadmodel->edges = out;
	loadmodel->numEdges = count;

	loadmodel->memorySize += count * sizeof(*out);

	for (i = 0; i < count; i++, in++, out++) {
		out->v[0] = (unsigned short)LittleShort (in->v[0]);
		out->v[1] = (unsigned short)LittleShort (in->v[1]);
	}
}

/*
=================
Mod_LoadTexinfo
=================
*/

void Mod_LoadTextureFx (image_t *tex, char *s) {

	char	*token;

	while (s) {
		token = COM_Parse (&s);

		if (!Q_strcasecmp (token, "parallaxScale")) {
			tex->parallaxScale = atof (COM_Parse (&s));
			continue;
		}

		if (!Q_strcasecmp (token, "specularScale")) {
			tex->specularScale = atof (COM_Parse (&s));
			continue;
		}

		if (!Q_strcasecmp (token, "specularExponent")) {
			tex->SpecularExp = atof (COM_Parse (&s));
			continue;
		}
		if (!Q_strcasecmp (token, "envScale")) {
			tex->envScale = atof (COM_Parse (&s));
			continue;
		}

		if (!Q_strcasecmp (token, "envMap")) {
			tex->envMap = true;
			continue;
		}
	}
}

void Mod_LoadTexinfo (lump_t * l) {
	texInfo_t  *in;
	mtexInfo_t *out, *step;
	image_t    *image;
	char       name[MAX_QPATH];
	char       *purename;
	int        count, next;
	int        i, j, k, x;
	char		*buff;

	in = (texInfo_t *)(mod_base + l->fileofs);

	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s", loadmodel->name);

	count = l->filelen / sizeof(*in);

	loadmodel->numTexInfo = count;
	loadmodel->texInfo = out = (mtexInfo_t*)Hunk_Alloc (count * sizeof(*out));
	loadmodel->memorySize += count * sizeof(*out);


	for (i = 0; i < count; i++, in++, out++) {
		for (j = 0; j < 2; j++)
		for (k = 0; k < 4; k++)
			out->vecs[j][k] = LittleFloat (in->vecs[j][k]);

		out->value = LittleLong (in->value);
		out->flags = LittleLong (in->flags);

		next = LittleLong (in->nexttexInfo);

		if (next > 0)
			out->next = loadmodel->texInfo + next;
		else
			out->next = NULL;

		// grab original texture size for override textures
		Com_sprintf (name, sizeof(name), "textures/%s.wal", in->texture);
		out->image = GL_FindImage (name, it_wall);

		if (!out->image) {
			// failed to load WAL, use default
			Com_Printf ("Couldn't load %s\n", name);

			out->image = GL_FindImage ("pics/noimage.tga", it_pic);

			if (!out->image)
				out->image = r_notexture;

			// no normal/glow map
			continue;
		}

		// get file name without path
		purename = COM_SkipPath (in->texture);

		// check if we have something to override with
		Com_sprintf (name, sizeof(name), "overrides/%s.tga", purename);
		image = GL_FindImage (name, it_wall);

		if (!image) {
			Com_sprintf (name, sizeof(name), "overrides/%s.dds", purename);
			image = GL_FindImage (name, it_wall);
		}

		// scale override texture size
		if (image) {
			image->width = out->image->width;
			image->height = out->image->height;

			// use override instead of WAL
			out->image = image;
		}

		//
		// Normal Maps Loading
		//

		Com_sprintf (name, sizeof(name), "overrides/%s_bump.tga", purename);
		out->normalmap = GL_FindImage (name, it_bump);

		if (!out->normalmap) {
			Com_sprintf (name, sizeof(name), "overrides/%s_bump.dds", purename);
			out->normalmap = GL_FindImage (name, it_bump);


			if (!out->normalmap) {
				Com_sprintf (name, sizeof(name), "textures/%s_bump.tga", in->texture);
				out->normalmap = GL_FindImage (name, it_bump);

				if (!out->normalmap) {
					Com_sprintf (name, sizeof(name), "textures/%s_bump.dds", in->texture);
					out->normalmap = GL_FindImage (name, it_bump);


					if (!out->normalmap)
						out->normalmap = r_defBump;
					// don't care if it's NULL
				}
			}
		}

		// Cone Step Maps
		Com_sprintf (name, sizeof(name), "overrides/%s_step.tga", purename);
		out->csmMap = GL_FindImage (name, it_bump);

		if (!out->csmMap) {
			Com_sprintf (name, sizeof(name), "overrides/%s_step.dds", purename);
			out->csmMap = GL_FindImage (name, it_bump);


			if (!out->csmMap) {
				Com_sprintf (name, sizeof(name), "textures/%s_step.tga", in->texture);
				out->csmMap = GL_FindImage (name, it_bump);

				if (!out->csmMap) {
					Com_sprintf (name, sizeof(name), "textures/%s_step.dds", in->texture);
					out->csmMap = GL_FindImage (name, it_bump);


					if (!out->csmMap)
						out->csmMap = r_notexture;
					// don't care if it's NULL
				}
			}
		}
		//
		// Glow Maps Loading
		//

		Com_sprintf (name, sizeof(name), "overrides/%s_light.tga", purename);
		out->addTexture = GL_FindImage (name, it_wall);

		if (!out->addTexture) {
			Com_sprintf (name, sizeof(name), "overrides/%s_light.dds", purename);
			out->addTexture = GL_FindImage (name, it_wall);

			if (!out->addTexture) {
				Com_sprintf (name, sizeof(name), "overrides/%s_light.jpg", purename);
				out->addTexture = GL_FindImage (name, it_wall);

				if (!out->addTexture) {
					Com_sprintf (name, sizeof(name), "textures/%s_light.tga", in->texture);
					out->addTexture = GL_FindImage (name, it_wall);

					if (!out->addTexture) {
						Com_sprintf (name, sizeof(name), "textures/%s_light.dds", in->texture);
						out->addTexture = GL_FindImage (name, it_wall);

						if (!out->addTexture) {
							Com_sprintf (name, sizeof(name), "textures/%s_light.jpg", in->texture);
							out->addTexture = GL_FindImage (name, it_wall);

							if (!out->addTexture)
								out->addTexture = r_notexture;
						}
					}
				}
			}
		}
		//
		// Env Maps Loading
		//

		Com_sprintf (name, sizeof(name), "overrides/%s_env.tga", purename);
		out->envTexture = GL_FindImage (name, it_wall);

		if (!out->envTexture) {
			Com_sprintf (name, sizeof(name), "overrides/%s_env.dds", purename);
			out->envTexture = GL_FindImage (name, it_wall);

			if (!out->envTexture) {
				Com_sprintf (name, sizeof(name), "overrides/%s_env.jpg", purename);
				out->envTexture = GL_FindImage (name, it_wall);

				if (!out->envTexture) {
					Com_sprintf (name, sizeof(name), "textures/%s_env.tga", in->texture);
					out->envTexture = GL_FindImage (name, it_wall);

					if (!out->envTexture) {
						Com_sprintf (name, sizeof(name), "textures/%s_env.dds", in->texture);
						out->envTexture = GL_FindImage (name, it_wall);

						if (!out->envTexture) {
							Com_sprintf (name, sizeof(name), "textures/%s_env.jpg", in->texture);
							out->envTexture = GL_FindImage (name, it_wall);

							if (!out->envTexture)
								out->envTexture = r_notexture;
						}
					}
				}
			}
		}

		// load texture configuration file
		Com_sprintf (name, sizeof(name), "materials/%s.mtr", purename);
		x = FS_LoadFile (name, (void **)&buff);
		if (buff) {

			char bak = buff[i];
			buff[x] = 0;
			Com_DPrintf ("Loading material for "S_COLOR_GREEN"%s\n", purename);
			Mod_LoadTextureFx (out->image, buff);
			buff[x] = bak;
			FS_FreeFile (buff);

		}
	}




	// count animation frames
	for (i = 0, out = loadmodel->texInfo; i < count; i++, out++) {
		out->numFrames = 1;

		for (step = out->next; step && step != out; step = step->next)
			out->numFrames++;
	}
}
/*
================
CalcSurfaceExtents

Fills in s->texturemins[] and s->extents[]
================
*/
void CalcSurfaceExtents (msurface_t * s) {
	float mins[2], maxs[2], val;
	int i, j, e;
	mvertex_t *v;
	mtexInfo_t *tex;
	int bmins[2], bmaxs[2];

	mins[0] = mins[1] = 999999;
	maxs[0] = maxs[1] = -99999;

	tex = s->texInfo;

	for (i = 0; i < s->numEdges; i++) {
		e = loadmodel->surfEdges[s->firstedge + i];
		if (e >= 0)
			v = &loadmodel->vertexes[loadmodel->edges[e].v[0]];
		else
			v = &loadmodel->vertexes[loadmodel->edges[-e].v[1]];

		for (j = 0; j < 2; j++) {
			val = v->position[0] * tex->vecs[j][0] +
				v->position[1] * tex->vecs[j][1] +
				v->position[2] * tex->vecs[j][2] + tex->vecs[j][3];
			if (val < mins[j])
				mins[j] = val;
			if (val > maxs[j])
				maxs[j] = val;
		}
	}

	for (i = 0; i < 2; i++) {
		bmins[i] = floor (mins[i] / loadmodel->lightmap_scale);
		bmaxs[i] = ceil (maxs[i] / loadmodel->lightmap_scale);

		s->texturemins[i] = bmins[i] * loadmodel->lightmap_scale;
		s->extents[i] = (bmaxs[i] - bmins[i]) *  loadmodel->lightmap_scale;

	}
}

void GL_CalcBspIndeces (msurface_t *surf) {
	int index, i;

	surf->numIndices = (surf->numVertices - 2) * 3;
	surf->indices = (index_t*)Hunk_Alloc (surf->numIndices * sizeof(int));

	for (i = 0, index = 2; i < surf->numIndices; i += 3, index++) {
		surf->indices[i + 0] = 0;
		surf->indices[i + 1] = index - 1;
		surf->indices[i + 2] = index;
	}

}

void GL_BuildPolygonFromSurface (msurface_t * fa);
void GL_CreateSurfaceLightmap (msurface_t * surf);
void GL_EndBuildingLightmaps (void);
void GL_BeginBuildingLightmaps (model_t * m);
void GL_BuildTBN (int count);

/*
================
SetupSurfaceNeighbors

Setup the neighour pointers of this surface's polygon.
================
*/
void BuildSurfaceNeighours (msurface_t *surf) {
	int				i, j, lindex;
	temp_connect_t	*tempEdge;

	if (surf->numEdges > MAX_POLY_VERT)
		Com_DPrintf ("BuildSurfaceNeighbors: too many edges %i\n", surf->numEdges);

	for (i = 0; i < surf->numEdges; i++) {
		lindex = currentmodel->surfEdges[surf->firstedge + i];
		tempEdge = tempEdges + abs (lindex);

		surf->polys->neighbours[i] = NULL;
		for (j = 0; j < tempEdge->used; j++)
		if (tempEdge->poly[j] != surf->polys)
			surf->polys->neighbours[i] = tempEdge->poly[j];
	}
}

/*
=================
Mod_LoadFaces
=================
*/

void Mod_BuildVertexCache () {
	msurface_t      *surf;
	int         i, vbo_size, vb;
	int         xyz_size, st_size, lm_size, nm_size, tg_size, bn_size;
	float   *buf;

	// calc vbo buffer size
	vb = 0;
	for (i = 0, surf = currentmodel->surfaces; i < currentmodel->numSurfaces; i++, surf++)
		vb += surf->polys->numVerts;

	// and offsets...
	gl_state.xyz_offset = 0;
	xyz_size = vb * sizeof(vec3_t);

	gl_state.st_offset = gl_state.xyz_offset + xyz_size;
	st_size = vb * sizeof(vec2_t);

	gl_state.lm_offset = gl_state.st_offset + st_size;
	lm_size = vb * sizeof(vec2_t);

	gl_state.nm_offset = gl_state.lm_offset + lm_size;
	nm_size = vb * sizeof(vec3_t);

	gl_state.tg_offset = gl_state.nm_offset + nm_size;
	tg_size = vb * sizeof(vec3_t);

	gl_state.bn_offset = gl_state.tg_offset + tg_size;
	bn_size = vb * sizeof(vec3_t);

	vbo_size = gl_state.bn_offset + bn_size;

	buf = (float*)malloc (vbo_size);
	if (!buf)
		Com_Error (ERR_DROP, ""S_COLOR_RED"Create vertex buffer - FALED!\n");   // wtf, man??? drop to console

	// fill vbo
	vb = 0;
	for (i = 0, surf = currentmodel->surfaces; i < currentmodel->numSurfaces; i++, surf++) {
		int                     jj, nv = surf->polys->numVerts;
		glpoly_t        *p = surf->polys;
		float           *v;

		v = p->verts[0];
		for (jj = 0; jj < nv; jj++, v += VERTEXSIZE, vb++) {
			// vertex data
			buf[gl_state.xyz_offset / 4 + vb * 3 + 0] = v[0];
			buf[gl_state.xyz_offset / 4 + vb * 3 + 1] = v[1];
			buf[gl_state.xyz_offset / 4 + vb * 3 + 2] = v[2];
			// st coords
			buf[gl_state.st_offset / 4 + vb * 2 + 0] = v[3];
			buf[gl_state.st_offset / 4 + vb * 2 + 1] = v[4];
			// lm coords
			buf[gl_state.lm_offset / 4 + vb * 2 + 0] = v[5];
			buf[gl_state.lm_offset / 4 + vb * 2 + 1] = v[6];
			// normals
			buf[gl_state.nm_offset / 4 + vb * 3 + 0] = v[7];
			buf[gl_state.nm_offset / 4 + vb * 3 + 1] = v[8];
			buf[gl_state.nm_offset / 4 + vb * 3 + 2] = v[9];
			// tangents
			buf[gl_state.tg_offset / 4 + vb * 3 + 0] = v[10];
			buf[gl_state.tg_offset / 4 + vb * 3 + 1] = v[11];
			buf[gl_state.tg_offset / 4 + vb * 3 + 2] = v[12];
			// binormals
			buf[gl_state.bn_offset / 4 + vb * 3 + 0] = v[13];
			buf[gl_state.bn_offset / 4 + vb * 3 + 1] = v[14];
			buf[gl_state.bn_offset / 4 + vb * 3 + 2] = v[15];
		}
	}

	qglGenBuffers (1, &gl_state.vbo_BSP);
	qglBindBuffer (GL_ARRAY_BUFFER_ARB, gl_state.vbo_BSP);
	qglBufferData (GL_ARRAY_BUFFER_ARB, vbo_size, buf, GL_STATIC_DRAW_ARB);
	qglBindBuffer (GL_ARRAY_BUFFER_ARB, 0);
	Com_DPrintf (""S_COLOR_GREEN"%d"S_COLOR_WHITE" kbytes of VBO vertex data\n", vbo_size / 1024);
	free (buf);
}

void Mod_LoadFaces (lump_t * l) {
	dface_t		*in;
	msurface_t *out;
	msurface_t	*surf;
	int			i, count, surfnum;

	in = (dface_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s", loadmodel->name);

	count = l->filelen / sizeof(*in);
	out = (msurface_t*)Hunk_Alloc (count * sizeof(*out));

	loadmodel->surfaces = out;
	loadmodel->numSurfaces = count;
	loadmodel->memorySize += count * sizeof(*out);

	currentmodel = loadmodel;
	surf = currentmodel->surfaces;

	tempEdges = (temp_connect_t *)Z_Malloc (currentmodel->numEdges * sizeof(temp_connect_t));

	GL_BeginBuildingLightmaps (loadmodel);

	for (surfnum = 0; surfnum < count; surfnum++, in++, out++) {
		int			ti;
		int			planenum, side;

		out->firstedge = LittleLong (in->firstedge);
		out->numEdges = LittleShort (in->numEdges);
		out->flags = 0;
		out->polys = NULL;

		planenum = LittleShort (in->planenum);
		side = LittleShort (in->side);
		if (side)
			out->flags |= MSURF_PLANEBACK;

		out->plane = loadmodel->planes + planenum;

		ti = LittleShort (in->texInfo);

		if (ti < 0 || ti >= loadmodel->numTexInfo)
			VID_Error (ERR_DROP, "Mod_LoadFaces(): bad texInfo number (%i).", ti);

		out->texInfo = loadmodel->texInfo + ti;

		CalcSurfaceExtents (out);

		// lighting info
		for (i = 0; i < MAXLIGHTMAPS; i++)
			out->styles[i] = in->styles[i];

		// KRIGSSVIN: .xplm light offsets
		i = loadmodel->useXPLM ? mod_xplmOffsets[surfnum] : LittleLong (in->lightofs);

		if (i == -1)
			out->samples = NULL;
		else
			out->samples = loadmodel->lightData + i;

		// set the drawing flags
		if (out->texInfo->flags & SURF_WARP)
			out->flags |= MSURF_DRAWTURB;

		// create lightmaps and polygons
		if (!(out->texInfo->flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)))
			GL_CreateSurfaceLightmap (out);

		GL_BuildPolygonFromSurface (out);
		GL_AddFlareSurface (out);
		CalcSurfaceBounds (out);
		GL_CalcBspIndeces (out);
	}

	// Build TBN for smoothing bump mapping (Berserker)
	GL_BuildTBN (count);
	GL_EndBuildingLightmaps ();

	// calc neighbours for shadow volumes
	for (surfnum = 0; surfnum < count; surfnum++, surf++) {
		if (surf->flags & (MSURF_DRAWTURB | MSURF_DRAWSKY))
			continue;
		BuildSurfaceNeighours (surf);
	}

	Z_Free (tempEdges);

	Mod_BuildVertexCache ();
}

// Mini cache abstraction, don't touch these varibles directly!

static byte *_cacheData;
static int _cachePos, _cacheSize;

static qboolean cache_Open (const char *name) {
	_cacheSize = FS_LoadFile (name, (void**)&_cacheData);
	_cachePos = 0;
	return (_cacheData != NULL);
}

static void cache_Close () {
	FS_FreeFile (_cacheData);
}

static qboolean cache_Fetch (void *dst, int size) {
	if (_cacheSize - _cachePos < size) {
		return false;
	}
	else {
		memcpy (dst, _cacheData + _cachePos, size);
		_cachePos += size;
		return true;
	}
}

// End mini cache

void GL_BuildTBN (int count) {
	int			ci, cj, i, j;
	float		*vi, *vj, threshold;
	msurface_t	*si, *sj;
	vec3_t		ni, nj;
	char		cacheName[MAX_QPATH];
	FILE		*cacheFile = NULL;
	int         smoothAng = (int)r_tbnSmoothAngle->value;

	threshold = cosf (DEG2RAD (r_tbnSmoothAngle->value));

	// Check for existing data
	Com_sprintf (cacheName, sizeof(cacheName), "cachexp/%s", currentmodel->name);
	if (cache_Open (cacheName)) {
		int angle;

		if (!cache_Fetch (&angle, sizeof(angle)) || angle != smoothAng) {
			Com_Printf (S_COLOR_RED "GL_BuildTBN: ignoring data for %s with angle %d (need %d)\n",
				cacheName, angle, smoothAng);
			cache_Close ();
			goto recreate;
		}

		for (i = 0; i < count; i++) {
			si = &currentmodel->surfaces[i];

			if (si->texInfo->flags & (SURF_SKY | /*SURF_TRANS33 | SURF_TRANS66 | */SURF_NODRAW))
				continue;

			vi = si->polys->verts[0];

			for (ci = 0; ci < si->numEdges; ci++, vi += VERTEXSIZE) {
				if (!cache_Fetch (vi + 7, 9 * sizeof(*vi))) {
					Com_Printf (S_COLOR_RED "GL_BuildTBN: insufficient data in %s\n", cacheName);
					cache_Close ();
					goto recreate;
				}
			}
		}
		Com_DPrintf (S_COLOR_GREEN "GL_BuildTBN: using cached data from %s\n", cacheName);
		cache_Close ();
		return;
	}

recreate:
	// Not found, so write it as we calculate it
	Com_sprintf (cacheName, sizeof(cacheName), "%s/cachexp/%s", FS_Gamedir (), currentmodel->name);
	FS_CreatePath (cacheName);
	cacheFile = fopen (cacheName, "wb");
	if (cacheFile == NULL)
		Com_Printf (S_COLOR_RED "GL_BuildTBN: could't open %s for writing\n", currentmodel->name);
	else {
		Com_Printf (S_COLOR_YELLOW "GL_BuildTBN: calculating %s, with angle %d\n",
			currentmodel->name, smoothAng);
		fwrite (&smoothAng, sizeof(smoothAng), 1, cacheFile);
	}

	for (i = 0; i < count; i++) {
		si = &currentmodel->surfaces[i];

		if (si->texInfo->flags & (SURF_SKY | /*SURF_TRANS33 | SURF_TRANS66 | */SURF_NODRAW))
			continue;

		vi = si->polys->verts[0];

		for (ci = 0; ci < si->numEdges; ci++, vi += VERTEXSIZE)
			vi[7] = vi[8] = vi[9] = vi[10] = vi[11] = vi[12] = vi[13] = vi[14] = vi[15] = 0;

		if (si->flags & MSURF_PLANEBACK)
			VectorNegate (si->plane->normal, ni);
		else
			VectorCopy (si->plane->normal, ni);

		for (j = 0; j < count; j++) {
			sj = &currentmodel->surfaces[j];

			if (!(sj->texInfo->flags & (SURF_SKY | /*SURF_TRANS33 | SURF_TRANS66 | */SURF_NODRAW))) {
				if (si->texInfo->image->texnum != sj->texInfo->image->texnum)
					continue;

				if (sj->flags & MSURF_PLANEBACK)
					VectorNegate (sj->plane->normal, nj);
				else
					VectorCopy (sj->plane->normal, nj);

				if (DotProduct (ni, nj) >= threshold) {
					vi = si->polys->verts[0];
					for (ci = 0; ci < si->numEdges; ci++, vi += VERTEXSIZE) {
						vj = sj->polys->verts[0];
						for (cj = 0; cj < sj->numEdges; cj++, vj += VERTEXSIZE) {

							if (VectorCompare (vi, vj)) {
								vi[7] += nj[0];
								vi[8] += nj[1];
								vi[9] += nj[2];
							}
						}
					}
				}
			}
		}

		vi = si->polys->verts[0];
		for (ci = 0; ci < si->numEdges; ci++, vi += VERTEXSIZE) {
			vec3_t normal, biTangent, tmp;
			VectorSet (normal, vi[7], vi[8], vi[9]);
			VectorNormalize (normal);

			if (DotProduct (normal, ni) < threshold) {
				vi[7] = normal[0] + ni[0];
				vi[8] = normal[1] + ni[1];
				vi[9] = normal[2] + ni[2];
			}
			else {
				vi[7] = normal[0];
				vi[8] = normal[1];
				vi[9] = normal[2];
			}

			CrossProduct (normal, si->texInfo->vecs[0], tmp);
			CrossProduct (normal, tmp, biTangent);
			VectorNormalize (biTangent);
			if (DotProduct (biTangent, si->texInfo->vecs[0]) < 0) {
				vi[10] = -biTangent[0];
				vi[11] = -biTangent[1];
				vi[12] = -biTangent[2];
			}
			else {
				vi[10] = biTangent[0];
				vi[11] = biTangent[1];
				vi[12] = biTangent[2];
			}

			CrossProduct (normal, si->texInfo->vecs[1], tmp);
			CrossProduct (normal, tmp, biTangent);
			VectorNormalize (biTangent);
			if (DotProduct (biTangent, si->texInfo->vecs[1]) < 0) {
				vi[13] = -biTangent[0];
				vi[14] = -biTangent[1];
				vi[15] = -biTangent[2];
			}
			else {
				vi[13] = biTangent[0];
				vi[14] = biTangent[1];
				vi[15] = biTangent[2];
			}

			if (cacheFile != NULL)
				fwrite (vi + 7, sizeof(*vi), 9, cacheFile);
		}
	}
	if (cacheFile != NULL)
		fclose (cacheFile);
}



/*
=================
Mod_SetParent
=================
*/
void Mod_SetParent (mnode_t * node, mnode_t * parent) {
	node->parent = parent;
	if (node->contents != -1)
		return;
	Mod_SetParent (node->children[0], node);
	Mod_SetParent (node->children[1], node);
}

/*
=================
Mod_LoadNodes
=================
*/
void Mod_LoadNodes (lump_t * l) {
	int i, j, count, p;
	dnode_t *in;
	mnode_t *out;

	in = (dnode_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mnode_t*)Hunk_Alloc (count * sizeof(*out));

	loadmodel->memorySize += count * sizeof(*out);

	loadmodel->nodes = out;
	loadmodel->numNodes = count;

	for (i = 0; i < count; i++, in++, out++) {
		for (j = 0; j < 3; j++) {
			out->minmaxs[j] = LittleShort (in->mins[j]);
			out->minmaxs[3 + j] = LittleShort (in->maxs[j]);
		}

		p = LittleLong (in->planenum);
		out->plane = loadmodel->planes + p;

		out->firstsurface = LittleShort (in->firstface);
		out->numsurfaces = LittleShort (in->numfaces);
		out->contents = -1;		// differentiate from leafs

		for (j = 0; j < 2; j++) {
			p = LittleLong (in->children[j]);
			if (p >= 0)
				out->children[j] = loadmodel->nodes + p;
			else
				out->children[j] =
				(mnode_t *)(loadmodel->leafs + (-1 - p));
		}
	}

	Mod_SetParent (loadmodel->nodes, NULL);	// sets nodes and leafs
}

/*
=================
Mod_LoadLeafs
=================
*/
void Mod_LoadLeafs (lump_t *l) {
	dleaf_t *in;
	mleaf_t *out;
	int i, j, count, p;
	glpoly_t *poly;

	in = (dleaf_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s", loadmodel->name);

	count = l->filelen / sizeof(*in);
	out = (mleaf_t *)Hunk_Alloc (count * sizeof(*out));

	loadmodel->memorySize += count * sizeof(*out);

	loadmodel->leafs = out;
	loadmodel->numLeafs = count;

	for (i = 0; i < count; i++, in++, out++) {
		for (j = 0; j < 3; j++) {
			out->minmaxs[j] = LittleShort (in->mins[j]);
			out->minmaxs[3 + j] = LittleShort (in->maxs[j]);
		}

		p = LittleLong (in->contents);
		out->contents = p;

		out->cluster = LittleShort (in->cluster);
		out->area = LittleShort (in->area);

		out->firstmarksurface = loadmodel->markSurfaces + LittleShort (in->firstleafface);
		out->numMarkSurfaces = LittleShort (in->numleaffaces);

		if (out->contents & MASK_WATER) {
			for (j = 0; j < out->numMarkSurfaces; j++) {
				out->firstmarksurface[j]->flags |= MSURF_UNDERWATER;

				for (poly = out->firstmarksurface[j]->polys; poly; poly = poly->next)
					poly->flags |= MSURF_UNDERWATER;

				if (out->contents & CONTENTS_LAVA) {
					out->firstmarksurface[j]->flags |= MSURF_LAVA;

					for (poly = out->firstmarksurface[j]->polys; poly; poly = poly->next)
						poly->flags |= MSURF_LAVA;
				}

				if (out->contents & CONTENTS_SLIME) {
					out->firstmarksurface[j]->flags |= MSURF_SLIME;

					for (poly = out->firstmarksurface[j]->polys; poly; poly = poly->next)
						poly->flags |= MSURF_SLIME;
				}

				if (out->contents & CONTENTS_WATER) {
					out->firstmarksurface[j]->flags |= MSURF_WATER;

					for (poly = out->firstmarksurface[j]->polys; poly; poly = poly->next)
						poly->flags |= MSURF_WATER;
				}
			}

		}
	}
}

/*
=================
Mod_LoadMarksurfaces
=================
*/
void Mod_LoadMarksurfaces (lump_t * l) {
	int i, j, count;
	short *in;
	msurface_t **out;

	in = (short *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (msurface_t**)Hunk_Alloc (count * sizeof(*out));

	loadmodel->markSurfaces = out;
	loadmodel->numMarkSurfaces = count;

	loadmodel->memorySize += count * sizeof(*out);

	for (i = 0; i < count; i++) {
		j = LittleShort (in[i]);
		if (j < 0 || j >= loadmodel->numSurfaces)
			VID_Error (ERR_DROP,
			"Mod_ParseMarksurfaces: bad surface number");
		out[i] = loadmodel->surfaces + j;
	}
}

/*
=================
Mod_LoadSurfedges
=================
*/
void Mod_LoadSurfedges (lump_t * l) {
	int i, count;
	int *in, *out;

	in = (int *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	if (count < 1 || count >= MAX_MAP_SURFEDGES)
		VID_Error (ERR_DROP,
		"MOD_LoadBmodel: bad surfEdges count in %s: %i",
		loadmodel->name, count);

	out = (int*)Hunk_Alloc (count * sizeof(*out));

	loadmodel->memorySize += count * sizeof(*out);

	loadmodel->surfEdges = out;
	loadmodel->numSurfEdges = count;

	for (i = 0; i < count; i++)
		out[i] = LittleLong (in[i]);
}


/*
=================
Mod_LoadPlanes
=================
*/
void Mod_LoadPlanes (lump_t * l) {
	int i, j;
	cplane_t *out;
	dplane_t *in;
	int count;
	int bits;

	in = (dplane_t *)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		VID_Error (ERR_DROP, "MOD_LoadBmodel: funny lump size in %s",
		loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (cplane_t*)Hunk_Alloc (count * 2 * sizeof(*out));

	loadmodel->planes = out;
	loadmodel->numPlanes = count;

	loadmodel->memorySize += count * sizeof(*out);

	for (i = 0; i < count; i++, in++, out++) {
		bits = 0;
		for (j = 0; j < 3; j++) {
			out->normal[j] = LittleFloat (in->normal[j]);
			if (out->normal[j] < 0)
				bits |= 1 << j;
		}

		out->dist = LittleFloat (in->dist);
		out->type = LittleLong (in->type);
		out->signbits = bits;
	}
}

void Mod_GenerateLights (model_t * mod) {
	r_worldmodel = mod;
	R_InitLightgrid ();
}

void R_PreCalcLightData (void);
void R_InitLightgrid2 (void);

void Mod_GenerateAmbientLights (model_t * mod) {
	r_worldmodel = mod;
	R_InitLightgrid2 ();
}


/*
=================
Mod_LoadBrushModel

=================
*/
static qboolean R_LoadXPLM (void) {
	char tmp[MAX_QPATH], name[MAX_QPATH];
	char *pB, *buf;
	int *pIB;
	int i, len, numFaces;

	FS_StripExtension (loadmodel->name, tmp, sizeof(tmp));
	Com_sprintf (name, sizeof(name), "%s.xplm", tmp);

	len = FS_LoadFile (name, (void**)&buf);

	if (!buf) {
		//		Com_DPrintf("R_LoadXPLM(): external lightmaps for '%s' not found.\n", loadmodel->name);
		return false;
	}

	//
	// face count & light offsets
	//

	pB = buf;
	pIB = (int *)buf;

	numFaces = LittleLong (*pIB++);
	pB += 4;
	len -= 4;

	mod_xplmOffsets = Z_Malloc (numFaces * 4);

	for (i = 0; i < numFaces; i++, pIB++)
		mod_xplmOffsets[i] = LittleLong (*pIB);

	pB += numFaces * 4;
	len -= numFaces * 4;

	//
	// face style counts
	//

	//
	// lightmap scale
	//

	loadmodel->lightmap_scale = *pB++;
	len--;

	//
	// lightmap data
	//

	//	Com_Printf("^1NUMFACES ^7%i ^1LM_DATA ^7%i ^1LM_SCALE ^7%i\n", numFaces, len, loadmodel->lightmap_scale);

	loadmodel->lightData = (byte *)Hunk_Alloc (len);
	Q_memcpy (loadmodel->lightData, pB, len);
	loadmodel->memorySize += len;
	loadmodel->useXPLM = true;

	FS_FreeFile (buf);

	Com_Printf ("Loaded lightmaps from "S_COLOR_GREEN"%s"S_COLOR_WHITE".\n", name);

	return true;
}

/*
=================
Mod_LoadBrushModel

=================
*/
void Mod_LoadBrushModel (model_t * mod, void *buffer) {
	int i;
	dheader_t *header;
	mmodel_t *bm;

	R_ClearFlares ();
	DeleteShadowVertexBuffers ();
	R_ClearWorldLights ();
	numLightQ = 0;
	numFlareOcc = 0;

	loadmodel->memorySize = 0;
	loadmodel->type = mod_brush;

	if (loadmodel != mod_known)
		VID_Error (ERR_DROP, "Loaded a brush model after the world");

	header = (dheader_t *)buffer;

	i = LittleLong (header->version);
	if (i != BSPVERSION)
		VID_Error (ERR_DROP,
		"Mod_LoadBrushModel: %s has wrong version number (%i should be %i)",
		mod->name, i, BSPVERSION);

	// swap all the lumps
	mod_base = (byte *)header;

	for (i = 0; i < sizeof(dheader_t)* 0.25; i++)
		((int *)header)[i] = LittleLong (((int *)header)[i]);


	// load into heap
	Mod_LoadEntityString (&header->lumps[LUMP_ENTITIES]);
	Mod_LoadVertexes (&header->lumps[LUMP_VERTEXES]);
	Mod_LoadEdges (&header->lumps[LUMP_EDGES]);
	Mod_LoadSurfedges (&header->lumps[LUMP_SURFEDGES]);

	if (!R_LoadXPLM ())
		Mod_LoadLighting (&header->lumps[LUMP_LIGHTING]);

	Mod_LoadPlanes (&header->lumps[LUMP_PLANES]);
	Mod_LoadTexinfo (&header->lumps[LUMP_TEXINFO]);

	Mod_LoadFaces (&header->lumps[LUMP_FACES]);

	if (loadmodel->useXPLM)
		Z_Free (mod_xplmOffsets);

	CleanDuplicateFlares ();

	Mod_LoadMarksurfaces (&header->lumps[LUMP_LEAFFACES]);
	Mod_LoadVisibility (&header->lumps[LUMP_VISIBILITY]);
	Mod_LoadLeafs (&header->lumps[LUMP_LEAFS]);
	Mod_LoadNodes (&header->lumps[LUMP_NODES]);
	Mod_LoadsubModels (&header->lumps[LUMP_MODELS]);
	Mod_GenerateLights (mod);

	Load_LightFile ();
	R_PreCalcLightData ();
	//	Mod_GenerateAmbientLights(mod);

	mod->numFrames = 2;			// regular and alternate animation

	//
	// set up the subModels
	//

	for (i = 0; i < mod->numSubModels; i++) {
		model_t *starmod;

		bm = &mod->subModels[i];
		starmod = &mod_inline[i];

		*starmod = *loadmodel;

		starmod->firstModelSurface = bm->firstface;
		starmod->numModelSurfaces = bm->numfaces;
		starmod->firstNode = bm->headnode;
		if (starmod->firstNode >= loadmodel->numNodes)
			VID_Error (ERR_DROP, "Inline model %i has bad firstNode", i);

		VectorCopy (bm->maxs, starmod->maxs);
		VectorCopy (bm->mins, starmod->mins);
		starmod->radius = bm->radius;

		if (i == 0)
			*loadmodel = *starmod;

		starmod->numLeafs = bm->visleafs;
	}

	bspSize += loadmodel->memorySize;
}

/*
==============================================================================

ALIAS MODELS

==============================================================================
*/

byte Normal2Index (const vec3_t vec) {
	int i, best;
	float d, bestd;

	bestd = best = 0;
	for (i = 0; i<NUMVERTEXNORMALS; i++) {
		d = DotProduct (vec, r_avertexnormals[i]);
		if (d > bestd) {
			bestd = d;
			best = i;
		}
	}

	return best;
}

/*
========================
Mod_FindTriangleWithEdge
Shadow volumes stuff
========================
*/
static int Mod_FindTriangleWithEdge (neighbors_t * neighbors, dtriangle_t * tris, int numtris, int triIndex, int edgeIndex) {


	int i, j, found = -1, foundj = 0;
	dtriangle_t *current = &tris[triIndex];
	qboolean dup = false;

	for (i = 0; i < numtris; i++) {
		if (i == triIndex)
			continue;

		for (j = 0; j < 3; j++) {
			if (((current->index_xyz[edgeIndex] == tris[i].index_xyz[j]) &&
				(current->index_xyz[(edgeIndex + 1) % 3] ==
				tris[i].index_xyz[(j + 1) % 3]))
				||
				((current->index_xyz[edgeIndex] ==
				tris[i].index_xyz[(j + 1) % 3])
				&& (current->index_xyz[(edgeIndex + 1) % 3] ==
				tris[i].index_xyz[j]))) {
				// no edge for this model found yet?
				if (found == -1) {
					found = i;
					foundj = j;
				}
				else
					dup = true;	// the three edges story
			}
		}
	}

	// normal edge, setup neighbour pointers
	if (!dup && found != -1) {	// / FIXED by BERSERKER: � Tenebrae ��
		// ����������� ������, ����� found == -1
		// -> ������ ������ ������!
		neighbors[found].n[foundj] = triIndex;
		return found;
	}
	// naughty egde let no-one have the neighbour
	return -1;
}

/*
===============
Mod_BuildTriangleNeighbors

===============
*/
static void Mod_BuildTriangleNeighbors (neighbors_t * neighbors,
	dtriangle_t * tris, int numtris) {
	int i, j;

	// set neighbours to -1
	for (i = 0; i < numtris; i++) {
		for (j = 0; j < 3; j++)
			neighbors[i].n[j] = -1;
	}

	// generate edges information (for shadow volumes)
	// NOTE: We do this with the original vertices not the reordered onces 
	// since reordering them
	// duplicates vertices and we only compare indices
	for (i = 0; i < numtris; i++) {
		for (j = 0; j < 3; j++) {
			if (neighbors[i].n[j] == -1)
				neighbors[i].n[j] = Mod_FindTriangleWithEdge (neighbors, tris, numtris, i, j);
		}
	}
}

/*
==================
Mod_LoadAliasModel
==================
*/

//see: http://members.rogers.com/deseric/tangentspace.htm
void VecsForTris (float *v0, float *v1, float *v2, float *st0, float *st1, float *st2, vec3_t Tangent, vec3_t Binormal) {
	vec3_t	vec1, vec2;
	vec3_t	planes[3];
	float	tmp;
	int		i;

	for (i = 0; i < 3; i++) {
		vec1[0] = v1[i] - v0[i];
		vec1[1] = st1[0] - st0[0];
		vec1[2] = st1[1] - st0[1];
		vec2[0] = v2[i] - v0[i];
		vec2[1] = st2[0] - st0[0];
		vec2[2] = st2[1] - st0[1];
		VectorNormalize (vec1);
		VectorNormalize (vec2);
		CrossProduct (vec1, vec2, planes[i]);
	}

	for (i = 0; i < 3; i++) {
		tmp = 1.0 / planes[i][0];
		Tangent[i] = -planes[i][1] * tmp;
		Binormal[i] = -planes[i][2] * tmp;
	}
	VectorNormalize (Tangent);
	VectorNormalize (Binormal);
}


void HACK_RecalcVertsLightNormalIdx (dmdl_t *pheader) {
	int				i, j, k, l;
	vec3_t			normal, triangle[3], v1, v2;
	dtriangle_t		*tris = (dtriangle_t *)((byte *)pheader + pheader->ofs_tris);
	vec3_t	normals_[MAX_VERTS];

	/// Berserker: �������� �� ��������� ���� �������� ������...
	//for all frames
	for (i = 0; i < pheader->num_frames; i++) {
		daliasframe_t	*frame;
		dtrivertx_t		*verts, *v;

		frame = (daliasframe_t *)((byte *)pheader + pheader->ofs_frames + i * pheader->framesize);
		verts = frame->verts;

		memset (normals_, 0, pheader->num_xyz*sizeof(vec3_t));

		//for all tris
		for (j = 0; j < pheader->num_tris; j++) {
			//make 3 vec3_t's of this triangle's vertices
			for (k = 0; k < 3; k++) {
				l = tris[j].index_xyz[k];
				v = &verts[l];
				for (l = 0; l < 3; l++)
					triangle[k][l] = v->v[l];
			}

			//calculate normal
			VectorSubtract (triangle[0], triangle[1], v1);
			VectorSubtract (triangle[2], triangle[1], v2);
			CrossProduct (v2, v1, normal);
			VectorScale (normal, -1.0 / VectorLength (normal), normal);

			for (k = 0; k < 3; k++) {
				l = tris[j].index_xyz[k];
				VectorAdd (normals_[l], normal, normals_[l]);
			}
		}

		/// Berserker
		// � ��������� ������� ������� ���������� ��������� �����, ������� ���������� ����������.
		// ��� ����� ������� ���������� �� ������� (���� ���� ����� ��������� ������� (�� 15 ��������)).
		for (j = 0; j < pheader->num_xyz; j++)
		for (k = j + 1; k < pheader->num_xyz; k++)
		if (verts[j].v[0] == verts[k].v[0] && verts[j].v[1] == verts[k].v[1] && verts[j].v[2] == verts[k].v[2]) {
			float *jnormal = r_avertexnormals[verts[j].lightnormalindex];
			float *knormal = r_avertexnormals[verts[k].lightnormalindex];
			if (DotProduct (jnormal, knormal) >= cos (DEG2RAD (45)))		/// smooth_cosine = cos(N), ��� �������� ���� ����� ��������� ����� ��� N ��������, �������, ��� ��� ���� �����
			{
				VectorAdd (normals_[j], normals_[k], normals_[j]);
				VectorCopy (normals_[j], normals_[k]);
			}
		}

		//normalize average
		for (j = 0; j < pheader->num_xyz; j++) {
			VectorNormalize (normals_[j]);
			verts[j].lightnormalindex = Normal2Index (normals_[j]);
		}
	}
}


void Mod_LoadAliasModelFx (model_t *mod, char *s) {

	char	*token;

	while (s) {
		token = COM_Parse (&s);

		if (!Q_strcasecmp (token, "monster")) {
			mod->flags |= RF_MONSTER;
			continue;
		}

		if (!Q_strcasecmp (token, "distort")) {
			mod->flags |= RF_DISTORT;
			continue;
		}

		if (!Q_strcasecmp (token, "noshadow")) {
			mod->flags |= RF_NOSHADOW;
			continue;
		}
		if (!Q_strcasecmp (token, "fullbright")) {
			mod->flags |= RF_FULLBRIGHT;
			continue;
		}

		if (!Q_strcasecmp (token, "glow")) {
			mod->glowCfg[0] = atof (COM_Parse (&s)); // alpha min
			mod->glowCfg[1] = atof (COM_Parse (&s)); // alpha max
			mod->glowCfg[2] = atof (COM_Parse (&s)); // time scale
			continue;
		}
		if (!Q_strcasecmp (token, "noSelfShadow")) {
			mod->noSelfShadow = true;
			continue;
		}
		if (!Q_strcasecmp (token, "envMap")) {
			mod->envMap = true;
			mod->envScale = atof (COM_Parse (&s));
			continue;
		}

		if (!Q_strcasecmp (token, "scale")) {
			mod->modelScale = atof (COM_Parse (&s));
			continue;
		}

	}
}

void Mod_LoadAliasModel (model_t * mod, void *buffer) {
	int				i, j, indexST;
	dmdl_t			*pinmodel, *pheader;
	fstvert_t		*poutst;
	fstvert_t		*st = NULL;
	dstvert_t		*pinst;
	dtriangle_t		*pintri, *pouttri, *tris;
	daliasframe_t	*pinframe, *poutframe;
	int				version;

	vec3_t			tempr, tempv;
	int				k, l;
	char			nam[MAX_OSPATH];
	char			*buff;

	daliasframe_t	*frame;
	dtrivertx_t		*verts;
	byte			*norms = NULL, *tangents = NULL, *binormals = NULL;
	float			s, t;
	float			iw, ih;
	byte			smooth;
	int				cx;
	vec3_t			binormals_[MAX_VERTS];
	vec3_t			tangents_[MAX_VERTS];
	char			cachename[MAX_OSPATH];
	FILE			*f;
	unsigned		checksum, cs_binormals, cs_tangents;
	qboolean		success = false, err = true;;

	mod->memorySize = 0;

	pinmodel = (dmdl_t *)buffer;

	version = LittleLong (pinmodel->version);
	if (version != ALIAS_VERSION)
		VID_Error (ERR_DROP, "%s has wrong version number (%i should be %i)", mod->name, version, ALIAS_VERSION);


	pheader = (dmdl_t*)Hunk_Alloc (LittleLong (pinmodel->ofs_end));

	mod->memorySize += LittleLong (pinmodel->ofs_end);

	aliasSize += mod->memorySize;

	// byte swap the header fields and sanity check
	for (i = 0; i < sizeof(dmdl_t)* 0.25; i++)
		((int *)pheader)[i] = LittleLong (((int *)buffer)[i]);

	if (pheader->num_xyz <= 0)
		VID_Error (ERR_DROP, "model %s has no vertices", mod->name);

	if (pheader->num_xyz > MAX_VERTS)
		VID_Error (ERR_DROP, "model %s has too many vertices", mod->name);

	if (pheader->num_st <= 0)
		VID_Error (ERR_DROP, "model %s has no st vertices", mod->name);

	if (pheader->num_tris <= 0)
		VID_Error (ERR_DROP, "model %s has no triangles", mod->name);

	if (pheader->num_frames <= 0)
		VID_Error (ERR_DROP, "model %s has no frames", mod->name);


	mod->flags = 0;

	// set default render fx values
	mod->glowCfg[0] = 0.3;
	mod->glowCfg[1] = 1.0;
	mod->glowCfg[2] = 5.666;
	mod->noSelfShadow = (qboolean)false;
	mod->modelScale = 1.0;
	mod->envMap = (qboolean)false;
	mod->envScale = 0.1;
	i = strlen (mod->name);
	memcpy (nam, mod->name, i);
	nam[i - 3] = 'r';
	nam[i - 2] = 'f';
	nam[i - 1] = 'x';
	nam[i] = 0;
	// load the .rfx
	i = FS_LoadFile (nam, (void **)&buff);
	if (buff) {
		char bak = buff[i];
		buff[i] = 0;
		Mod_LoadAliasModelFx (mod, buff);
		buff[i] = bak;
		FS_FreeFile (buff);
	}

	//
	// load triangle lists
	//
	pintri = (dtriangle_t *)((byte *)pinmodel + pheader->ofs_tris);
	pouttri = (dtriangle_t *)((byte *)pheader + pheader->ofs_tris);

	for (i = 0, tris = pouttri; i < pheader->num_tris; i++, tris++) {
		for (j = 0; j < 3; j++) {
			tris->index_xyz[j] = LittleShort (pintri[i].index_xyz[j]);
			tris->index_st[j] = LittleShort (pintri[i].index_st[j]);
		}
	}

	// find neighbours
	mod->neighbours = (neighbors_t*)malloc (pheader->num_tris * sizeof(neighbors_t));
	Mod_BuildTriangleNeighbors (mod->neighbours, pouttri, pheader->num_tris);
	mod->memorySize += pheader->num_tris * sizeof(neighbors_t);
	//
	// load the frames
	//
	for (i = 0; i < pheader->num_frames; i++) {
		pinframe = (daliasframe_t *)((byte *)pinmodel
			+ pheader->ofs_frames +
			i * pheader->framesize);
		poutframe =
			(daliasframe_t *)((byte *)pheader + pheader->ofs_frames +
			i * pheader->framesize);

		Q_memcpy (poutframe->name, pinframe->name, sizeof(poutframe->name));
		for (j = 0; j < 3; j++) {
			poutframe->scale[j] = LittleFloat (pinframe->scale[j]) * mod->modelScale;
			poutframe->translate[j] = LittleFloat (pinframe->translate[j]) * mod->modelScale;
		}
		// verts are all 8 bit, so no swapping needed
		Q_memcpy (poutframe->verts, pinframe->verts,
			pheader->num_xyz * sizeof(dtrivertx_t));

	}

	mod->type = mod_alias;

	// register all skins
	Q_memcpy ((char *)pheader + pheader->ofs_skins,
		(char *)pinmodel + pheader->ofs_skins,
		pheader->num_skins * MAX_SKINNAME);

	for (i = 0; i < pheader->num_skins; i++) {
		char *pname;
		char gl[128];

		pname = (char *)pheader + pheader->ofs_skins + i * MAX_SKINNAME;
		mod->skins[i] = GL_FindImage (pname, it_skin);

		// GlowMaps loading
		strcpy (gl, pname);
		gl[strlen (gl) - 4] = 0;
		strcat (gl, "_light.tga");
		mod->glowtexture[i] = GL_FindImage (gl, it_skin);

		if (!mod->glowtexture[i]) {
			strcpy (gl, pname);
			gl[strlen (gl) - 4] = 0;
			strcat (gl, "_light.dds");
			mod->glowtexture[i] = GL_FindImage (gl, it_skin);
		}

		if (!mod->glowtexture[i])
			mod->glowtexture[i] = r_notexture;

		// Loading Normal maps
		strcpy (gl, pname);
		gl[strlen (gl) - 4] = 0;
		strcat (gl, "_bump.tga");
		mod->skins_normal[i] = GL_FindImage (gl, it_bump);

		if (!mod->skins_normal[i]) {
			strcpy (gl, pname);
			gl[strlen (gl) - 4] = 0;
			strcat (gl, "_bump.dds");
			mod->skins_normal[i] = GL_FindImage (gl, it_bump);
		}

		if (!mod->skins_normal[i])
			mod->skins_normal[i] = r_defBump;
	}

	// Calculate texcoords for triangles (for compute tangents and binormals)
	mod->memorySize += pheader->num_st * sizeof(fstvert_t);
	pinst = (dstvert_t *)((byte *)pinmodel + pheader->ofs_st);
	poutst = (fstvert_t*)Hunk_Alloc (pheader->num_st * sizeof(fstvert_t));
	iw = 1.0 / pheader->skinwidth;
	ih = 1.0 / pheader->skinheight;
	for (i = 0; i < pheader->num_st; i++) {
		s = LittleShort (pinst[i].s);
		t = LittleShort (pinst[i].t);
		poutst[i].s = (s - 0.5) * iw;
		poutst[i].t = (t - 0.5) * ih;
	}

	// try loading from cache
	Com_sprintf (cachename, sizeof(cachename), "cachexp/%s", mod->name); /// Berserker: ����� Q2XP �������� ������ � /cachexp/, ����� �� �������� ��� Bers@Q2
	if (cache_Open (cachename)) {
		unsigned	ang;

		if (!cache_Fetch (&smooth, sizeof(smooth)))
			goto bad;

		if (!cache_Fetch (&ang, sizeof(ang)))
			goto bad;

		if (ang != (unsigned)(cos (DEG2RAD (45)) * 0x7fffffff))
			goto bad;

		cx = pheader->num_xyz * pheader->num_frames * sizeof(byte);
		mod->binormals = binormals = (byte*)Hunk_Alloc (cx);
		mod->tangents = tangents = (byte*)Hunk_Alloc (cx);
		mod->memorySize += cx;
		mod->memorySize += cx;

		if (!cache_Fetch (&cs_binormals, sizeof(cs_binormals)))
			goto bad;

		if (!cache_Fetch (mod->binormals, cx))
			goto bad;

		if (!cache_Fetch (&cs_tangents, sizeof(cs_tangents)))
			goto bad;

		if (!cache_Fetch (mod->tangents, cx))
			goto bad;

		if (LittleLong (Com_BlockChecksum (mod->binormals, cx)) != cs_binormals ||
			LittleLong (Com_BlockChecksum (mod->tangents, cx)) != cs_tangents) {
			Com_Printf ("^1%s: wrong checksum!\n", mod->name);
			goto bad;
		}

		// load data generated by HACK_RecalcVertsLightNormalIdx
		for (i = 0; i < pheader->num_frames; i++) {
			daliasframe_t	*frame;
			dtrivertx_t		*verts;

			frame = (daliasframe_t *)((byte *)pheader + pheader->ofs_frames + i * pheader->framesize);
			verts = frame->verts;

			for (j = 0; j < pheader->num_xyz; j++) {
				if (!cache_Fetch (&verts[j].lightnormalindex, sizeof(byte)))
					goto bad;
			}
		}

		// everything went fine
		Com_DPrintf ("%s: loaded from cache\n", mod->name);
		cache_Close ();
		goto exit;

	bad:
		// all errors end up here
		Com_Printf ("^1%s: invalid cache\n", mod->name);
		cache_Close ();
	}

	/* ========================
	cache not found - recalc it
	======================== */

	Com_Printf ("^3%s: calculating extra data\n", mod->name);

	HACK_RecalcVertsLightNormalIdx (pheader);

	cx = pheader->num_xyz * pheader->num_frames * sizeof(byte);

	// Calculate tangents for vertices (bump mapping)
	if (!binormals || !tangents) {
		mod->binormals = binormals = (byte*)Hunk_Alloc (cx);
		mod->tangents = tangents = (byte*)Hunk_Alloc (cx);
	}
	mod->memorySize += cx;
	mod->memorySize += cx;

	//for all frames
	for (i = 0; i < pheader->num_frames; i++) {

		//set temp to zero
		memset (tangents_, 0, pheader->num_xyz*sizeof(vec3_t));
		memset (binormals_, 0, pheader->num_xyz*sizeof(vec3_t));

		tris = (dtriangle_t *)((byte *)pheader + pheader->ofs_tris);
		st = (fstvert_t*)((byte *)pheader + pheader->ofs_st);

		frame = (daliasframe_t *)((byte *)pheader + pheader->ofs_frames + i * pheader->framesize);
		verts = frame->verts;

		//for all tris
		for (j = 0; j < pheader->num_tris; j++) {
			vec3_t	vv0, vv1, vv2;
			vec3_t tangent, binormal;

			vv0[0] = (float)verts[tris[j].index_xyz[0]].v[0];
			vv0[1] = (float)verts[tris[j].index_xyz[0]].v[1];
			vv0[2] = (float)verts[tris[j].index_xyz[0]].v[2];
			vv1[0] = (float)verts[tris[j].index_xyz[1]].v[0];
			vv1[1] = (float)verts[tris[j].index_xyz[1]].v[1];
			vv1[2] = (float)verts[tris[j].index_xyz[1]].v[2];
			vv2[0] = (float)verts[tris[j].index_xyz[2]].v[0];
			vv2[1] = (float)verts[tris[j].index_xyz[2]].v[1];
			vv2[2] = (float)verts[tris[j].index_xyz[2]].v[2];

			VecsForTris (vv0, vv1, vv2,
				&st[tris[j].index_st[0]].s,
				&st[tris[j].index_st[1]].s,
				&st[tris[j].index_st[2]].s,
				tangent, binormal);			//for all vertices in the tri

			for (k = 0; k < 3; k++) {
				l = tris[j].index_xyz[k];
				VectorAdd (tangents_[l], tangent, tangents_[l]);
				VectorAdd (binormals_[l], binormal, binormals_[l]);
			}
		}

		/// Berserker:
		// � ��������� ������� ������� ���������� ��������� �����, ������� ���������� ����������.
		// ��� ����� ������� ���������� �� ������� (���� ���� ����� ��������� ������� (�� 15 ��������)).
		for (j = 0; j < pheader->num_xyz; j++)
		for (k = j + 1; k < pheader->num_xyz; k++)
		if (verts[j].v[0] == verts[k].v[0] && verts[j].v[1] == verts[k].v[1] && verts[j].v[2] == verts[k].v[2]) {
			float *jnormal = r_avertexnormals[verts[j].lightnormalindex];
			float *knormal = r_avertexnormals[verts[k].lightnormalindex];
			if (DotProduct (jnormal, knormal) >= cos (DEG2RAD (45)))		/// smooth_cosine = cos(N), ��� �������� ���� ����� ��������� ����� ��� N ��������, �������, ��� ��� ���� �����
			{
				VectorAdd (tangents_[j], tangents_[k], tangents_[j]);
				VectorCopy (tangents_[j], tangents_[k]);
				VectorAdd (binormals_[j], binormals_[k], binormals_[j]);
				VectorCopy (binormals_[j], binormals_[k]);
			}
		}

		//normalize averages
		for (j = 0; j < pheader->num_xyz; j++) {
			VectorNormalize (tangents_[j]);
			VectorNormalize (binormals_[j]);

			tangents[i * pheader->num_xyz + j] = Normal2Index (tangents_[j]);
			binormals[i * pheader->num_xyz + j] = Normal2Index (binormals_[j]);
		}
	}

	// write cache to disk
	Com_sprintf (cachename, sizeof(cachename), "%s/cachexp/%s", FS_Gamedir (), mod->name); /// Berserker: ����� Q2XP �������� ������ � /cachexp/, ����� �� �������� ��� Bers@Q2
	FS_CreatePath (cachename);
	f = fopen (cachename, "wb");
	if (f) {
		unsigned	sc;
		fwrite (&smooth, 1, sizeof(smooth), f);
		sc = (unsigned)(cos (DEG2RAD (45)) * 0x7fffffff);
		fwrite (&sc, 1, sizeof(unsigned), f);

		checksum = LittleLong (Com_BlockChecksum (binormals, cx));
		fwrite (&checksum, 1, sizeof(int), f);
		fwrite (binormals, 1, cx, f);

		checksum = LittleLong (Com_BlockChecksum (tangents, cx));
		fwrite (&checksum, 1, sizeof(int), f);
		fwrite (tangents, 1, cx, f);

		// save data generated by HACK_RecalcVertsLightNormalIdx
		for (i = 0; i < pheader->num_frames; i++) {
			daliasframe_t	*frame;
			dtrivertx_t		*verts;

			frame = (daliasframe_t *)((byte *)pheader + pheader->ofs_frames + i * pheader->framesize);
			verts = frame->verts;

			for (j = 0; j < pheader->num_xyz; j++) {
				fwrite (&verts[j].lightnormalindex, 1, sizeof(byte), f);
			}
		}

		fclose (f);
	}
exit:

	ClearBounds (mod->mins, mod->maxs);
	VectorClear (mod->center);
	frame = (daliasframe_t *)((byte *)pheader + pheader->ofs_frames);	//����� ������ ������� ����!
	verts = frame->verts;

	for (k = 0; k < pheader->num_xyz; k++)
	for (l = 0; l<3; l++) {
		if (mod->mins[l] > verts[k].v[l])	mod->mins[l] = verts[k].v[l];
		if (mod->maxs[l] < verts[k].v[l])	mod->maxs[l] = verts[k].v[l];
	}

	for (l = 0; l < 3; l++) {
		mod->mins[l] = mod->mins[l] * frame->scale[l] + frame->translate[l];
		mod->maxs[l] = mod->maxs[l] * frame->scale[l] + frame->translate[l];
		mod->center[l] = (mod->mins[l] + mod->maxs[l]) * 0.5;
	}


	tempr[0] = mod->maxs[0] - mod->mins[0];
	tempr[1] = mod->maxs[1] - mod->mins[1];
	tempr[2] = 0;
	tempv[0] = 0;
	tempv[1] = 0;
	tempv[2] = mod->maxs[2] - mod->mins[2];
	mod->radius = max (VectorLength (tempr), VectorLength (tempv));

	for (i = 0; i < 3; i++)
		mod->center[i] = (mod->maxs[i] + mod->mins[i]) * 0.5;

	// generate st cache for fast md2 rendering
	tris = (dtriangle_t *)((byte *)pheader + pheader->ofs_tris);
	mod->st = (float*)malloc (pheader->num_tris * 3 * sizeof(float)* 2);
	for (l = 0, i = 0; i < pheader->num_tris; i++) {
		for (j = 0; j < 3; j++) {
			indexST = tris[i].index_st[j];
			mod->st[l++] = poutst[indexST].s;
			mod->st[l++] = poutst[indexST].t;
		}
	}
	qglGenBuffers (1, &mod->vboId);
	qglBindBuffer (GL_ARRAY_BUFFER_ARB, mod->vboId);
	qglBufferData (GL_ARRAY_BUFFER_ARB, l * sizeof(float), mod->st, GL_STATIC_DRAW_ARB);
	qglBindBuffer (GL_ARRAY_BUFFER_ARB, 0);
}





/*
==============================================================================

SPRITE MODELS

==============================================================================
*/
/*
=================
Mod_LoadSpriteModel

=================
*/
void Mod_LoadSpriteModel (model_t * mod, void *buffer) {
	dsprite_t *sprin, *sprout;
	int i;

	mod->memorySize = 0;

	sprin = (dsprite_t *)buffer;
	sprout = (dsprite_t*)Hunk_Alloc (modfilelen);

	mod->memorySize += modfilelen;

	spriteSize += mod->memorySize;

	sprout->ident = LittleLong (sprin->ident);
	sprout->version = LittleLong (sprin->version);
	sprout->numFrames = LittleLong (sprin->numFrames);

	if (sprout->version != SPRITE_VERSION)
		VID_Error (ERR_DROP, "%s has wrong version number (%i should be %i)", mod->name, sprout->version, SPRITE_VERSION);

	if (sprout->numFrames > MAX_MD2SKINS)
		VID_Error (ERR_DROP, "%s has too many frames (%i > %i)", mod->name, sprout->numFrames, MAX_MD2SKINS);

	// byte swap everything
	for (i = 0; i < sprout->numFrames; i++) {
		sprout->frames[i].width = LittleLong (sprin->frames[i].width);
		sprout->frames[i].height = LittleLong (sprin->frames[i].height);
		sprout->frames[i].origin_x = LittleLong (sprin->frames[i].origin_x);
		sprout->frames[i].origin_y = LittleLong (sprin->frames[i].origin_y);
		Q_memcpy (sprout->frames[i].name, sprin->frames[i].name,
			MAX_SKINNAME);
		mod->skins[i] = GL_FindImage (sprout->frames[i].name, it_sprite);
	}

	mod->type = mod_sprite;
}

//=============================================================================

/*
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
R_BeginRegistration
Specifies the model
that will be used as the world.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

void R_BeginRegistration (char *model) {
	char fullname[MAX_QPATH];
	cvar_t *flushmap;

	registration_sequence++;
	r_oldviewcluster = -1;		// force markleafs

	Com_sprintf (fullname, sizeof(fullname), "maps/%s.bsp", model);

	// explicitly free the old map if different
	// this guarantees that mod_known[0] is the world map
	flushmap = Cvar_Get ("flushmap", "0", 0);
	if (strcmp (mod_known[0].name, fullname) || flushmap->value)
		Mod_Free (&mod_known[0]);
	r_worldmodel = Mod_ForName (fullname, true);

	r_viewcluster = -1;
	numPreCachedLights = 0;
	flareEdit = (qboolean)false;
}



/*
@@@@@@@@@@@@@@@@@@@@@
R_RegisterModel

@@@@@@@@@@@@@@@@@@@@@
*/
struct model_s *R_RegisterModel (char *name) {
	model_t *mod;
	int i;
	dsprite_t *sprout;
	dmdl_t *pheader;
	int len = strlen (name);

	mod = Mod_ForName (name, false);
	if (mod) {
		mod->registration_sequence = registration_sequence;

		// register any images used by the models
		if (mod->type == mod_sprite) {
			sprout = (dsprite_t *)mod->extraData;
			for (i = 0; i < sprout->numFrames; i++)
				mod->skins[i] =
				GL_FindImage (sprout->frames[i].name, it_sprite);
		}
		else if (mod->type == mod_alias) {
			pheader = (dmdl_t *)mod->extraData;

			for (i = 0; i < pheader->num_skins; i++) {
				char *pname;
				char gl[128];

				pname = (char *)pheader + pheader->ofs_skins + i * MAX_SKINNAME;
				mod->skins[i] = GL_FindImage (pname, it_skin);

				// GlowMaps loading
				strcpy (gl, pname);
				gl[strlen (gl) - 4] = 0;
				strcat (gl, "_light.tga");
				mod->glowtexture[i] = GL_FindImage (gl, it_skin);

				if (!mod->glowtexture[i]) {
					strcpy (gl, pname);
					gl[strlen (gl) - 4] = 0;
					strcat (gl, "_light.dds");
					mod->glowtexture[i] = GL_FindImage (gl, it_skin);
				}

				if (!mod->glowtexture[i])
					mod->glowtexture[i] = r_notexture;

				// Loading Normal maps
				strcpy (gl, pname);
				gl[strlen (gl) - 4] = 0;
				strcat (gl, "_bump.tga");
				mod->skins_normal[i] = GL_FindImage (gl, it_bump);

				if (!mod->skins_normal[i]) {
					strcpy (gl, pname);
					gl[strlen (gl) - 4] = 0;
					strcat (gl, "_bump.dds");
					mod->skins_normal[i] = GL_FindImage (gl, it_bump);
				}


				if (!mod->skins_normal[i])
					mod->skins_normal[i] = r_defBump;

			}
			//PGM
			mod->numFrames = pheader->num_frames;
			//PGM
			//         
		}
		else if (mod->type == mod_brush) {
			for (i = 0; i < mod->numTexInfo; i++) {
				mod->texInfo[i].image->registration_sequence = registration_sequence;

				if (mod->texInfo[i].normalmap != NULL)
					mod->texInfo[i].normalmap->registration_sequence = registration_sequence;

				if (mod->texInfo[i].addTexture != NULL)
					mod->texInfo[i].addTexture->registration_sequence = registration_sequence;

				if (mod->texInfo[i].envTexture != NULL)
					mod->texInfo[i].envTexture->registration_sequence = registration_sequence;

				if (mod->texInfo[i].csmMap != NULL)
					mod->texInfo[i].csmMap->registration_sequence = registration_sequence;

			}
		}
	}
	return mod;


}

/*
@@@@@@@@@@@@@@@@@@@@@
R_EndRegistration

@@@@@@@@@@@@@@@@@@@@@
*/
void R_EndRegistration (void) {
	int i, total;
	model_t *mod;

	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++) {
		if (!mod->name[0])
			continue;
		if (mod->registration_sequence != registration_sequence) {	// don't 
			// need 
			// this 
			// model
			Mod_Free (mod);
		}
	}
	GL_FreeUnusedImages ();

	total = bspSize + aliasSize + spriteSize;
	Com_DPrintf ("=============="S_COLOR_YELLOW" model memory allocated"S_COLOR_WHITE" ==============\n");
	Com_DPrintf ("Size of Bsp model memory    "S_COLOR_GREEN"%i"S_COLOR_WHITE" Bytes ("S_COLOR_GREEN"%i"S_COLOR_WHITE" Mb)\n", bspSize, bspSize >> 20);
	Com_DPrintf ("Size of Alias model memory  "S_COLOR_GREEN"%i"S_COLOR_WHITE" Bytes ("S_COLOR_GREEN"%i"S_COLOR_WHITE" Mb)\n", aliasSize, aliasSize >> 20);
	Com_DPrintf ("Size of Sprite model memory "S_COLOR_GREEN"%i"S_COLOR_WHITE" Bytes ("S_COLOR_GREEN"%i"S_COLOR_WHITE" Mb)\n", spriteSize, spriteSize >> 20);
	Com_DPrintf ("Size of Total model memory  "S_COLOR_GREEN"%i"S_COLOR_WHITE" Bytes ("S_COLOR_GREEN"%i"S_COLOR_WHITE" Mb)\n", total, total >> 20);
	Com_DPrintf ("====================================================\n");

	bspSize = 0;
	aliasSize = 0;
	spriteSize = 0;
	qglClear (GL_COLOR_BUFFER_BIT);
	qglClearColor (0.0, 0.0, 0.0, 1);

	GL_SetDefaultState ();

	relightMap = false;
}


//=============================================================================


/*
================
Mod_Free
================
*/

void Mod_Free (model_t * mod) {
	Hunk_Free (mod->extraData);

	if (mod->neighbours)
		free (mod->neighbours);

	if (mod->type == mod_alias)
		qglDeleteBuffers (1, &mod->vboId);

	memset (mod, 0, sizeof(*mod));
}

/*
================
Mod_FreeAll
================
*/
void Mod_FreeAll (void) {
	int i;

	for (i = 0; i < mod_numknown; i++) {
		if (mod_known[i].extraDataSize)
			Mod_Free (&mod_known[i]);
	}

}

#ifdef _WIN32
/// from Tenebrae, asm by Berserker
qboolean HasSharedLeafs (byte *v1, byte *v2) {

	int numleafs__ = r_worldmodel->numLeafs;
	_asm
	{
		mov edx, numleafs__
			mov esi, v1
			mov edi, v2
			shr edx, 5
			jz short l4
		l0 : mov eax, [esi]
			 add esi, 4
			 test eax, [edi]
			 jnz short l3
			 add edi, 4
			 dec edx
			 jnz short l0
		 l4 : mov edx, numleafs__
			  mov esi, v1
			  and edx, ~0x1f
			  mov edi, v2
			  cmp edx, numleafs__
			  jz short l5
		  l1 : mov eax, edx
			   mov ecx, edx
			   mov ebx, 1
			   and ecx, 7
			   shr eax, 3
			   shl ebx, cl
			   test byte ptr[esi + eax], bl
			   jz short l2
			   test byte ptr[edi + eax], bl
			   jnz short l3
		   l2 : inc edx
				cmp edx, numleafs__
				jc short l1
	}
l5:	return false;
	_asm
	{
	l3:
	}
	return true;

}

#else

// optimized version based on previous assembly one
qboolean HasSharedLeafs (byte *v1, byte *v2) {
	int numLeafs = r_worldmodel->numLeafs;
	int i;

	while (numLeafs > 32) {
		uint32_t *v1_x4 = (uint32_t*)v1;
		uint32_t *v2_x4 = (uint32_t*)v2;
		if (*v1_x4 & *v2_x4)
			return true;

		numLeafs -= 32;
		v1 += 4;
		v2 += 4;
	}

	for (i = 0; i < numLeafs; i++) {
		if (v1[i >> 3] & (1 << (i & 7)))
		if (v2[i >> 3] & (1 << (i & 7)))
			return true;
	}

	return false;
}
#endif

