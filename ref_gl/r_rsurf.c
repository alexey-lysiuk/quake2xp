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
// GL_RSURF.C: surface-related refresh code
//#include <assert.h>

#include "r_local.h"

vec4_t shadelight_surface;
void R_LightColor(vec3_t org, vec3_t color);

static vec3_t modelorg;			// relative to viewpoint

msurface_t *r_alpha_surfaces;		// all BSP surfaces with TRANS33/66
msurface_t *r_reflective_surfaces;	// all BSP surfaces with WARP (for now)

float color_black[4] = {0.0, 0.0, 0.0, 0.0};

vec3_t  wVertexArray[MAX_BATCH_SURFS];
float   wTexArray[MAX_BATCH_SURFS][2];
float   wLMArray[MAX_BATCH_SURFS][2];
vec4_t   wColorArray[MAX_BATCH_SURFS];

float   wTmu0Array[MAX_BATCH_SURFS][2];
float   wTmu1Array[MAX_BATCH_SURFS][2];
float   wTmu2Array[MAX_BATCH_SURFS][2];
float   wTmu3Array[MAX_BATCH_SURFS][2];
float   wTmu4Array[MAX_BATCH_SURFS][2];
float   wTmu5Array[MAX_BATCH_SURFS][2];
float   wTmu6Array[MAX_BATCH_SURFS][2];
float	SurfColorArray[MAX_BATCH_SURFS][4];

index_t	indexArray[MAX_BATCH_SURFS * 3];
vec3_t	nTexArray[MAX_BATCH_SURFS];
vec3_t	tTexArray[MAX_BATCH_SURFS];
vec3_t	bTexArray[MAX_BATCH_SURFS];

/*
=============================================================

	BRUSH MODELS

=============================================================
*/

/*
===============
R_TextureAnimation

Returns the proper texture for a given time and base texture
===============
*/
image_t *R_TextureAnimation(mtexInfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->image;

	c = currententity->frame % tex->numFrames;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->image;
}

image_t *R_TextureAnimationCSM(mtexInfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->csmMap;

	c = currententity->frame % tex->numFrames;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->csmMap;
}

image_t *R_TextureAnimationFx(mtexInfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->addTexture;

	c = currententity->frame % tex->numFrames;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->addTexture;
}

image_t *R_TextureAnimationNormal(mtexInfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->normalmap;

	c = currententity->frame % tex->numFrames;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->normalmap;
}

image_t *R_TextureAnimationEnv(mtexInfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->envTexture;

	c = currententity->frame % tex->numFrames;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->normalmap;
}

/*
================
DrawGLPoly
================
*/

void DrawGLPolyGLSL(msurface_t * fa, qboolean scrolling)
{
	int i;
	float *v;
	float alpha, scroll;
	glpoly_t *p;
	int	nv = fa->polys->numVerts;

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);

	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, wVertexArray);
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, wTexArray);
	
	if (fa->texInfo->flags & SURF_TRANS33 || SURF_TRANS66) {

		if (fa->texInfo->flags & SURF_TRANS33)
			alpha = 0.33;
		else
			alpha = 0.66;

		// setup program
		GL_BindProgram(refractProgram, 0);

		GL_MBind(GL_TEXTURE0_ARB, fa->texInfo->normalmap->texnum);
		qglUniform1i(refract_normalMap, 0);
		GL_MBind(GL_TEXTURE1_ARB, fa->texInfo->image->texnum);
		qglUniform1i(refract_baseMap, 1);
		GL_MBindRect(GL_TEXTURE2_ARB, ScreenMap->texnum);
		qglUniform1i(refract_screenMap, 2);
		GL_MBindRect(GL_TEXTURE3_ARB, depthMap->texnum);
		qglUniform1i(refract_depthMap, 3);

		qglUniform1f(refract_deformMul, 1.0);
		qglUniform1f(refract_alpha, alpha);
		qglUniform1f(refract_thickness, 150.0);
		qglUniform2f(refract_screenSize, vid.width, vid.height);
		qglUniform2f(refract_depthParams, r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
		qglUniform1f(refract_ambient, r_ambientLevel->value);
		qglUniform1i(refract_alphaMask, 0);
		}  
	else 
	{
		GL_BindProgram(genericProgram, 0);
		qglUniform1i(gen_attribColors, 1);
		qglUniform1i(gen_attribConsole, 0);

		GL_MBind(GL_TEXTURE0_ARB, fa->texInfo->image->texnum);
		qglUniform1i(gen_tex, 0);
		qglUniform1f(gen_colorModulate, r_ambientLevel->value);
	}


	if (scrolling){
		scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));

		if (scroll == 0.0)
			scroll = -64.0;
	} else
		scroll = 0;

	p = fa->polys;
	v = p->verts[0];

	c_brush_polys += (nv-2);

	for (i = 0; i < p->numVerts; i++, v += VERTEXSIZE) {
		
		VectorCopy(v, wVertexArray[i]);
			
		wTexArray[i][0] = v[3] + scroll;
		wTexArray[i][1] = v[4];
	}
	qglDrawElements(GL_TRIANGLES, fa->numIndices, GL_UNSIGNED_SHORT, fa->indices);	
			
	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
	GL_BindNullProgram();
	
}

/*
================
R_RenderBrushPoly

================
*/
void R_RenderBrushPoly (msurface_t *fa) {
	image_t *image;
	char *purename;
	char noext[MAX_QPATH];
	
	image = R_TextureAnimation(fa->texInfo);
	purename = COM_SkipPath(image->name);
	COM_StripExtension(purename, noext);

	if (fa->flags & MSURF_DRAWTURB)
	{	
		if (!strcmp(noext, "brlava") || !strcmp(noext, "lava") || !strcmp(noext, "tlava1_3"))
			RenderLavaSurfaces(fa);
		else
			R_DrawWaterPolygons(fa);
		return;
	}

	if(fa->texInfo->flags & SURF_FLOWING)
		DrawGLPolyGLSL(fa, true);
	else
		DrawGLPolyGLSL(fa, false);

}

void R_DrawAlphaPoly (void) {
	msurface_t *s;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	GL_LoadMatrix(GL_MODELVIEW, r_newrefdef.modelViewMatrix);
	
	for (s = r_alpha_surfaces; s; s = s->texturechain) {
		if (s->texInfo->flags & SURF_TRANS33) 
			shadelight_surface[3] = 0.33f;
		else if (s->texInfo->flags & SURF_TRANS66) 
			shadelight_surface[3] = 0.66f;
		else
			shadelight_surface[3] = 1.f;

		if (s->texInfo->flags & SURF_WARP)
			R_DrawWaterPolygons(s);
		else if (s->texInfo->flags & SURF_FLOWING)
			DrawGLPolyGLSL(s, true);
		else
			DrawGLPolyGLSL(s, false);

	}

	r_alpha_surfaces = NULL;
}

void R_DrawReflectivePoly (void) {
	msurface_t *s;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	GL_LoadMatrix(GL_MODELVIEW, r_newrefdef.modelViewMatrix);
	
	for (s = r_reflective_surfaces; s; s = s->texturechain) {
/*
// KRIGS: at the moment only opaque WARP surfaces go to this list

		if (s->texInfo->flags & SURF_TRANS33) 
			shadelight_surface[3] = 0.33f;
		else if (s->texInfo->flags & SURF_TRANS66) 
			shadelight_surface[3] = 0.66f;
		else
*/
			shadelight_surface[3] = 1.f;

// KRIGS: rename from Water to Reflective or smth
//		if (s->texInfo->flags & SURF_WARP)
			R_DrawWaterPolygons(s);
	}

	r_reflective_surfaces = NULL;
}

/*
================
DrawTextureChains

================
*/
void DrawTextureChains (void) {
	int i;
	msurface_t *s;
	image_t *image;

	c_visible_textures = 0;

	for (i = 0, image = gltextures; i < numgltextures; i++, image++) {
		if (!image->registration_sequence)
			continue;

		s = image->texturechain;
		if (!s)
			continue;

		for (; s; s = s->texturechain)
			R_RenderBrushPoly(s);
		
		image->texturechain = NULL;
	}
}

/*
===============
SORT AND BATCH 
BSP SURFACES

===============
*/
qboolean R_FillAmbientBatch (msurface_t *surf, qboolean newBatch, unsigned *vertices, unsigned *indeces) {
	unsigned	numVertices, numIndices;
	int			i, nv = surf->numEdges;
	float		scroll, scale[2];

	numVertices = *vertices;
	numIndices	= *indeces;

	if (numVertices + nv > MAX_BATCH_SURFS)
		return false;	// force the start new batch

	c_brush_polys++;

	if (newBatch) {
		image_t	*image, *fx, *env, *normal;

		image	= R_TextureAnimation(surf->texInfo);
		fx		= R_TextureAnimationFx(surf->texInfo);
		env		= R_TextureAnimationEnv(surf->texInfo);
		normal		= R_TextureAnimationNormal(surf->texInfo);

		qglUniform1f(ambientWorld_specularScale,
			image->specularScale ? r_specularScale->value * image->specularScale : r_specularScale->value);
		qglUniform1f(ambientWorld_specularExp, image->SpecularExp ? image->SpecularExp : 16.f);

		if (r_parallax->value){

			if (!image->parallaxScale){
				scale[0] = r_parallaxScale->value / image->width;
				scale[1] = r_parallaxScale->value / image->height;
			}
			else
			{
				scale[0] = image->parallaxScale / image->width;
				scale[1] = image->parallaxScale / image->height;
			}
			qglUniform4f(ambientWorld_parallaxParams, scale[0], scale[1], image->upload_width, image->upload_height);
		}

		GL_MBind(GL_TEXTURE0_ARB, image->texnum);
		GL_MBind(GL_TEXTURE2_ARB, fx->texnum);
		GL_MBind(GL_TEXTURE3_ARB, normal->texnum);

		if (r_ssao->value)
			GL_MBindRect(GL_TEXTURE6_ARB, fboColor[fboColorIndex]);

		if (surf->texInfo->flags & SURF_FLOWING)
		{
			scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
			if (scroll == 0.0)
				scroll = -64.0;
		}
		else
			scroll = 0.0;

		qglUniform1f(ambientWorld_scroll, scroll);
	}


		// create indexes
		if (numIndices == 0xffffffff)
			numIndices = 0;

		for (i = 0; i < nv - 2; i++)
		{
			indexArray[numIndices++] = surf->baseIndex;
			indexArray[numIndices++] = surf->baseIndex + i + 1;
			indexArray[numIndices++] = surf->baseIndex + i + 2;
		}

		*vertices	= numVertices;
		*indeces	= numIndices;

	return true;	
}

int SurfSort( const msurface_t **a, const msurface_t **b )
{
	return	(((*a)->lightmaptexturenum<<26)+((*a)->texInfo->image->texnum<<13)) - 
			(((*b)->lightmaptexturenum<<26)+((*b)->texInfo->image->texnum<<13));
}

vec3_t		BmodelViewOrg;
int			num_scene_surfaces;
msurface_t	*scene_surfaces[MAX_MAP_FACES];

static void GL_DrawLightmappedPoly(qboolean bmodel)
{
	msurface_t	*s;
	int			i;
	qboolean	newBatch;
	unsigned	oldTex		= 0xffffffff;
	unsigned	oldFlag		= 0xffffffff;
	unsigned	numIndices	= 0xffffffff,
				numVertices = 0;

	// setup program
	GL_BindProgram(ambientWorldProgram, 0);

	qglUniform1f(ambientWorld_colorScale, r_worldColorScale->value);

	if (r_parallax->value > 0 || r_worldmodel->useXPLM)
		qglUniform3fv(ambientWorld_viewOrigin, 1, bmodel ? BmodelViewOrg : r_origin);
	
	qglUniform1i(ambientWorld_parallaxType, (int)clamp(r_parallax->value, 0, 1));
	qglUniform1f(ambientWorld_ambientLevel, r_ambientLevel->value);

	qglUniform1i(ambientWorld_diffuse,		0);
	qglUniform1i(ambientWorld_lightmap[0],	1);
	qglUniform1i(ambientWorld_add,			2);
	qglUniform1i(ambientWorld_normalmap,	3);
	qglUniform1i(ambientWorld_lightmap[1],	4);
	qglUniform1i(ambientWorld_lightmap[2],	5);
	qglUniform1i(ambientWorld_lightmapType, r_worldmodel->useXPLM ? 1 : 0);
	qglUniform1i(ambientWorld_ssaoMap, 6);
	qglUniform1i(ambientWorld_ssao, r_ssao->value ? 1 : 0);

	qsort(scene_surfaces, num_scene_surfaces, sizeof(msurface_t*), (int(*)(const void *, const void *))SurfSort);

	for (i = 0; i < num_scene_surfaces; i++){
		s = scene_surfaces[i];

		// update lightmaps
		if (gl_state.currenttextures[1] != gl_state.lightmap_textures + s->lightmaptexturenum)
		{
			if (numIndices != 0xFFFFFFFF){
				qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);
				numVertices = 0;
				numIndices = 0xFFFFFFFF;
			}

			GL_MBind(GL_TEXTURE1_ARB, gl_state.lightmap_textures + s->lightmaptexturenum);

			if (r_worldmodel->useXPLM) {
				GL_MBind(GL_TEXTURE4_ARB, gl_state.lightmap_textures + s->lightmaptexturenum + MAX_LIGHTMAPS);
				GL_MBind(GL_TEXTURE5_ARB, gl_state.lightmap_textures + s->lightmaptexturenum + MAX_LIGHTMAPS * 2);
			}
		}
		
		// flush batch (new texture)
		if (s->texInfo->image->texnum != oldTex)
		{
			if (numIndices != 0xFFFFFFFF){
				qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);
				numVertices = 0;
				numIndices = 0xFFFFFFFF;
			}

			oldTex = s->texInfo->image->texnum;
			newBatch = true;
		}
	else
		newBatch = false;
	
	// fill new batch
//	repeat:	
		if (!R_FillAmbientBatch(s, newBatch, &numVertices, &numIndices))
		{
			if (numIndices != 0xFFFFFFFF){
				qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);
				numVertices = 0;
				numIndices = 0xFFFFFFFF;
				}
//			goto repeat;
		}
	}
	
	// draw the rest
	if (numIndices != 0xFFFFFFFF)
		qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);
	
	GL_BindNullProgram();
}


int	r_lightTimestamp;
int	num_light_surfaces;
msurface_t	*light_surfaces[MAX_MAP_FACES];

qboolean R_FillLightBatch(msurface_t *surf, qboolean newBatch, unsigned *vertices, unsigned *indeces, qboolean bmodel, qboolean caustics)
{
	unsigned	numVertices, numIndices;
	int			i, nv = surf->numEdges;
	float		scroll, scale[2];

	numVertices = *vertices;
	numIndices = *indeces;

	if (numVertices + nv > MAX_BATCH_SURFS)
		return false;	// force the start new batch

	c_brush_polys++;

	if (newBatch)
	{
		image_t		*image, *normalMap;
		char		*purename;
		char		noext[MAX_QPATH];
		image		= R_TextureAnimation		(surf->texInfo);
		normalMap	= R_TextureAnimationNormal	(surf->texInfo);

		purename = COM_SkipPath(image->name);
		COM_StripExtension(purename, noext);

		qglUniform1f(lightWorld_specularScale,
			image->specularScale ? r_specularScale->value * image->specularScale : r_specularScale->value);

		qglUniform1f(lightWorld_specularExp, image->SpecularExp ? image->SpecularExp : 16.f);
		
		if (bmodel){
			if (caustics && currentShadowLight->castCaustics)
				qglUniform1i(lightWorld_caustics, 1);
			else
				qglUniform1i(lightWorld_caustics, 0);
		}
		else{
			if ((surf->flags & MSURF_WATER) && currentShadowLight->castCaustics)
				qglUniform1i(lightWorld_caustics, 1);
			else
				qglUniform1i(lightWorld_caustics, 0);
		}

		if (r_parallax->value){

			if (!image->parallaxScale){

				scale[0] = r_parallaxScale->value / image->width;
				scale[1] = r_parallaxScale->value / image->height;
			}
			else
			{
				scale[0] = image->parallaxScale / image->width;
				scale[1] = image->parallaxScale / image->height;
			}

			qglUniform4f(lightWorld_parallaxParams, scale[0], scale[1], image->upload_width, image->upload_height);

		}

		GL_MBind		(GL_TEXTURE0_ARB, image->texnum);
		GL_MBind		(GL_TEXTURE1_ARB, normalMap->texnum);
		GL_MBindCube	(GL_TEXTURE2_ARB, r_lightCubeMap[currentShadowLight->filter]->texnum);
		GL_MBind3d		(GL_TEXTURE3_ARB, r_lightAttenMap->texnum);
		GL_MBind		(GL_TEXTURE4_ARB, r_caustic[((int)(r_newrefdef.time * 15)) & (MAX_CAUSTICS - 1)]->texnum);

		if (surf->texInfo->flags & SURF_FLOWING)
		{
			scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
			if (scroll == 0.0)
				scroll = -64.0;
		}
		else
			scroll = 0.0;

		qglUniform1f(lightWorld_scroll, scroll);
	}

	// create indexes
	if (numIndices == 0xffffffff)
		numIndices = 0;

	for (i = 0; i < nv - 2; i++)
	{
		indexArray[numIndices++] = surf->baseIndex;
		indexArray[numIndices++] = surf->baseIndex + i + 1;
		indexArray[numIndices++] = surf->baseIndex + i + 2;
	}


	*vertices = numVertices;
	*indeces = numIndices;

	return true;
}

 int lightSurfSort( const msurface_t **a, const msurface_t **b )
{
	return	(*a)->texInfo->image->texnum - (*b)->texInfo->image->texnum;
}
 
static void GL_DrawLightPass(qboolean bmodel, qboolean caustics)
{
	msurface_t	*s;
	unsigned	defBits = 0;
	int			i;
	glpoly_t	*poly;
	qboolean	newBatch, oldCaust;
	unsigned	oldTex		= 0xffffffff;
	unsigned	oldFlag		= 0xffffffff;
	unsigned	numIndices	= 0xffffffff,
				numVertices = 0;
	mat4_t		entAttenMatrix;


	// setup program
	GL_BindProgram(lightWorldProgram, defBits);

	qglUniform1f(lightWorld_colorScale, r_worldColorScale->value);
	qglUniform1i(lightWorld_ambient, (int)currentShadowLight->isAmbient);
	qglUniform3fv(lightWorld_viewOrigin, 1, bmodel ? BmodelViewOrg : r_origin);
	qglUniform3fv(lightWorld_lightOrigin, 1, currentShadowLight->origin);
	qglUniform4f(lightWorld_lightColor, currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2], 1.0);

	R_CalcCubeMapMatrix(bmodel);
	qglUniformMatrix4fv(lightWorld_cubeMatrix, 1, false, (const float *)currentShadowLight->cubeMapMatrix);

	if (!bmodel){
		qglUniformMatrix4fv(lightWorld_attenMatrix, 1, false, (const float *)currentShadowLight->attenMapMatrix);
	}
	else
	{
		Mat4_TransposeMultiply(currententity->matrix, currentShadowLight->attenMapMatrix, entAttenMatrix);
		qglUniformMatrix4fv(lightWorld_attenMatrix, 1, false, (const float *)entAttenMatrix);
	}

	qglUniform1i(lightWorld_parallaxType, (int)clamp(r_parallax->value, 0, 1));

	if(currentShadowLight->isFog){
		qglUniform1i(lightWorld_fog, (int)currentShadowLight->isFog);
		qglUniform1f(lightWorld_fogDensity, currentShadowLight->fogDensity);
	}else
		qglUniform1i(lightWorld_fog, 0);
	
	qglUniform1f(lightWorld_causticsIntens, r_causticIntens->value);

	qglUniform1i(lightWorld_diffuse, 0);
	qglUniform1i(lightWorld_normal, 1);
	qglUniform1i(lightWorld_cube, 2);
	qglUniform1i(lightWorld_atten, 3);
	qglUniform1i(lightWorld_caustic, 4);

	qsort(light_surfaces, num_light_surfaces, sizeof(msurface_t*), (int (*)(const void *, const void *))lightSurfSort);

	for (i = 0; i < num_light_surfaces; i++){
		s = light_surfaces[i];
		poly = s->polys;

		if (poly->lightTimestamp != r_lightTimestamp)
			continue;
		
		if (s->visframe != r_framecount)
			continue;

		// flush batch (new texture or flag)
		if (s->texInfo->image->texnum != oldTex || s->flags != oldFlag || caustics != oldCaust)
		{
			if (numIndices != 0xffffffff){
				qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);
				numVertices = 0;
				numIndices = 0xffffffff;
			}
			oldTex = s->texInfo->image->texnum;
			oldFlag = s->flags;
			oldCaust = caustics;
			newBatch = true;
		}
		else
			newBatch = false;

	// fill new batch
//	repeat:
		if (!R_FillLightBatch(s, newBatch, &numVertices, &numIndices, bmodel, caustics))
		{
			if (numIndices != 0xffffffff){
				qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);
				numVertices = 0;
				numIndices = 0xffffffff;
			}
//			goto repeat;
		}
	}
	// draw the rest
	if (numIndices != 0xffffffff)
		qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indexArray);

	GL_BindNullProgram();
}

qboolean SurfInFrustum (msurface_t *s) {
	if (s->polys)
		return !R_CullBox(s->mins, s->maxs);

	return true;
}

/*
================
R_RecursiveWorldNode

================
*/
static void R_RecursiveWorldNode(mnode_t * node)
{
	int c, side, sidebit;
	cplane_t *plane;
	msurface_t *surf, **mark;
	mleaf_t *pleaf;
	float dot;
	image_t *fx, *image;

	if (node->contents == CONTENTS_SOLID)
		return;					// solid

	if (node->visframe != r_visframecount)
		return;

	if (R_CullBox(node->minmaxs, node->minmaxs + 3))
		return;

	// if a leaf node, draw stuff
	if (node->contents != -1) {
		pleaf = (mleaf_t *)node;

		// check for door connected areas
		if (r_newrefdef.areabits) {
			if (!(r_newrefdef.areabits[pleaf->area >> 3] & (1 << (pleaf->area & 7))))
				return;			// not visible
		}

		mark = pleaf->firstmarksurface;
		c = pleaf->numMarkSurfaces;

		if (c) {
			do {
				if (SurfInFrustum(*mark))
					(*mark)->visframe = r_framecount;
				(*mark)->ent = NULL;
				mark++;
			} while (--c);
		}

		return;
	}
	// node is just a decision point, so go down the apropriate sides
	// find which side of the node we are on
	plane = node->plane;

	switch (plane->type) {
	case PLANE_X:
		dot = modelorg[0] - plane->dist;
		break;
	case PLANE_Y:
		dot = modelorg[1] - plane->dist;
		break;
	case PLANE_Z:
		dot = modelorg[2] - plane->dist;
		break;
	default:
		dot = DotProduct(modelorg, plane->normal) - plane->dist;
		break;
	}

	if (dot >= 0) {
		side = 0;
		sidebit = 0;
	}
	else {
		side = 1;
		sidebit = MSURF_PLANEBACK;
	}

	// recurse down the children, front side first
	R_RecursiveWorldNode(node->children[side]);

	// draw stuff
	for (c = node->numsurfaces, surf = r_worldmodel->surfaces + node->firstsurface; c; c--, surf++) {

		if (surf->visframe != r_framecount)
			continue;

		if ((surf->flags & MSURF_PLANEBACK) != sidebit)
			continue;			// wrong side

		if (surf->texInfo->flags & SURF_SKY) {	// just adds to visible sky bounds
			R_AddSkySurface(surf);
		}
		else if (surf->texInfo->flags & SURF_NODRAW)
			continue;

		else if (surf->texInfo->flags & (SURF_TRANS33 | SURF_TRANS66)) {
			// add to the translucent chain
			surf->texturechain = r_alpha_surfaces;
			r_alpha_surfaces = surf;
		}
		else {
			if (surf->flags & MSURF_DRAWTURB) {
				// the polygon is visible, so add it to the texture
				// sorted chain
				// FIXME: this is a hack for animation
				image = R_TextureAnimation(surf->texInfo);
				fx = R_TextureAnimationFx(surf->texInfo); // fix glow hack
				
				surf->texturechain = r_reflective_surfaces;
				r_reflective_surfaces = surf;
				surf->texturechain = image->texturechain;
				image->texturechain = surf;

			}
			else
				scene_surfaces[num_scene_surfaces++] = surf;
		}
	}

	// recurse down the back side
	R_RecursiveWorldNode(node->children[!side]);
}


qboolean R_MarkLightSurf (msurface_t *surf, qboolean world) {
	cplane_t	*plane;
	float		dist, dot;
	glpoly_t	*poly;
	image_t		*image;
	char		*purename;
	char		noext[MAX_QPATH];

	image = R_TextureAnimation(surf->texInfo);
	purename = COM_SkipPath(image->name);
	COM_StripExtension(purename, noext);

	if (surf->flags & MSURF_DRAWTURB)
	if (!strcmp(noext, "brlava") || !strcmp(noext, "lava") || !strcmp(noext, "tlava1_3"))
		goto hack;

	if (world){
		if (!SurfInFrustum(surf))
			return false;
	}

	if ((surf->texInfo->flags & (SURF_TRANS33|SURF_TRANS66|SURF_SKY|SURF_WARP|SURF_NODRAW)) || (surf->flags & MSURF_DRAWTURB))
		return false;
hack:

	plane = surf->plane;
	poly = surf->polys;
	
	if (poly->lightTimestamp == r_lightTimestamp)
		return false;

	switch (plane->type)
	{
	case PLANE_X:
		dist = currentShadowLight->origin[0] - plane->dist;
		break;
	case PLANE_Y:
		dist = currentShadowLight->origin[1] - plane->dist;
		break;
	case PLANE_Z:
		dist = currentShadowLight->origin[2] - plane->dist;
		break;
	default:
		dist = DotProduct (currentShadowLight->origin, plane->normal) - plane->dist;
		break;
	}
	
	if (currentShadowLight->isFog && !currentShadowLight->isShadow)
		goto next;

		//the normals are flipped when surf_planeback is 1
		if (((surf->flags & MSURF_PLANEBACK) && (dist > 0)) ||
			(!(surf->flags & MSURF_PLANEBACK) && (dist < 0)))
			return false;
next:

	if (world){
		switch (plane->type) //now check surf_planeback for camera pos
		{
		case PLANE_X:
			dot = r_origin[0] - plane->dist;
			break;
		case PLANE_Y:
			dot = r_origin[1] - plane->dist;
			break;
		case PLANE_Z:
			dot = r_origin[2] - plane->dist;
			break;
		default:
			dot = DotProduct(r_origin, plane->normal) - plane->dist;
			break;
		}
	
	if (currentShadowLight->isFog && !currentShadowLight->isShadow)
			goto next2;

		if (((surf->flags & MSURF_PLANEBACK) && (dot > 0)) ||
			(!(surf->flags & MSURF_PLANEBACK) && (dot < 0)))
			return false;
		}
next2:

	if (abs(dist) > currentShadowLight->len)
		return false;

	if(world)
	{
		float	lbbox[6], pbbox[6];

		lbbox[0] = currentShadowLight->origin[0] - currentShadowLight->radius[0];
		lbbox[1] = currentShadowLight->origin[1] - currentShadowLight->radius[1];
		lbbox[2] = currentShadowLight->origin[2] - currentShadowLight->radius[2];
		lbbox[3] = currentShadowLight->origin[0] + currentShadowLight->radius[0];
		lbbox[4] = currentShadowLight->origin[1] + currentShadowLight->radius[1];
		lbbox[5] = currentShadowLight->origin[2] + currentShadowLight->radius[2];

		// surface bounding box
		pbbox[0] = surf->mins[0];
		pbbox[1] = surf->mins[1];
		pbbox[2] = surf->mins[2];
		pbbox[3] = surf->maxs[0];
		pbbox[4] = surf->maxs[1];
		pbbox[5] = surf->maxs[2];

		if(!BoundsIntersect(&lbbox[0], &lbbox[3], &pbbox[0], &pbbox[3]))
			return false;
		if(currentShadowLight->_cone && R_CullBox_(&pbbox[0], &pbbox[3], currentShadowLight->frust))
			return false;
	}

	poly->lightTimestamp = r_lightTimestamp;

	return true;
}

void R_MarkLightCasting (mnode_t *node)
{
	cplane_t	*plane;
	float		dist;
	msurface_t	**surf;
	mleaf_t		*leaf;
	int			c, cluster;

	if (R_CullBox(node->minmaxs, node->minmaxs + 3))
		return;

	if (node->contents != -1)
	{
		//we are in a leaf
		leaf = (mleaf_t *)node;
		cluster = leaf->cluster;

		if (!(currentShadowLight->vis[cluster>>3] & (1<<(cluster&7))))
			return;

		surf = leaf->firstmarksurface;

		for (c = 0; c < leaf->numMarkSurfaces; c++, surf++)
		{
			if (R_MarkLightSurf ((*surf), true))
			{
				light_surfaces[num_light_surfaces++] = (*surf);
			}
		}
		return;
	}

	plane = node->plane;
	dist = DotProduct (currentShadowLight->origin, plane->normal) - plane->dist;

	if (dist > currentShadowLight->len)
	{
		R_MarkLightCasting (node->children[0]);
		return;
	}
	if (dist < -currentShadowLight->len)
	{
		R_MarkLightCasting (node->children[1]);
		return;
	}

	R_MarkLightCasting (node->children[0]);
	R_MarkLightCasting (node->children[1]);
}

qboolean R_FillLightChain (void) {
	R_MarkLightCasting (r_worldmodel->nodes);
	return num_light_surfaces;
}


void R_DrawLightWorld(void)
{

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	GL_StencilFunc(GL_EQUAL, 128, 255);
	GL_StencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	GL_StencilMask(0);
	GL_DepthFunc(GL_LEQUAL);

	qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_BSP);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_TEX1);
	qglEnableVertexAttribArray(ATRB_NORMAL);
	qglEnableVertexAttribArray(ATRB_TANGENT);
	qglEnableVertexAttribArray(ATRB_BINORMAL);

	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.xyz_offset));
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.st_offset));
	qglVertexAttribPointer(ATRB_TEX1, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.lm_offset));
	qglVertexAttribPointer(ATRB_NORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.nm_offset));
	qglVertexAttribPointer(ATRB_TANGENT, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.tg_offset));
	qglVertexAttribPointer(ATRB_BINORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.bn_offset));
	
	r_lightTimestamp++;
	num_light_surfaces = 0;
				
	if(R_FillLightChain())
		GL_DrawLightPass(false, false);

	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_NORMAL);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_TANGENT);
	qglDisableVertexAttribArray(ATRB_BINORMAL);
	qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}


/*
=============
R_DrawBSP

=============
*/
void R_DrawBSP (void) {
	entity_t ent;

	if (!r_drawWorld->value)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;
	
	currentmodel = r_worldmodel;

	VectorCopy(r_newrefdef.vieworg, modelorg);

	// auto cycle the world frame for texture animation
	memset(&ent, 0, sizeof(ent));
	ent.frame = (int) (r_newrefdef.time * 2);
	Mat3_Identity(ent.axis);
	currententity = &ent;

	gl_state.currenttextures[0] = gl_state.currenttextures[1] = -1;

	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));
		
	R_ClearSkyBox();	

	qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_BSP);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_TEX1);
	qglEnableVertexAttribArray(ATRB_NORMAL);
	qglEnableVertexAttribArray(ATRB_TANGENT);
	qglEnableVertexAttribArray(ATRB_BINORMAL);
	
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.xyz_offset));
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.st_offset));
	qglVertexAttribPointer(ATRB_TEX1, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.lm_offset));
	qglVertexAttribPointer(ATRB_NORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.nm_offset));
	qglVertexAttribPointer(ATRB_TANGENT, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.tg_offset));
	qglVertexAttribPointer(ATRB_BINORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.bn_offset));

	num_scene_surfaces = 0;
	R_RecursiveWorldNode(r_worldmodel->nodes);
	GL_DrawLightmappedPoly(false);
	
	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_TEX1);
	qglDisableVertexAttribArray(ATRB_NORMAL);
	qglDisableVertexAttribArray(ATRB_TANGENT);
	qglDisableVertexAttribArray(ATRB_BINORMAL);
	
	qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

	DrawTextureChains();
	R_DrawSkyBox(true);
}

/*
=================
R_DrawInlineBModel

=================
*/
extern qboolean bmodelcaust = false;

void R_DrawBrushModel();

static void R_DrawInlineBModel(void) {
	int i;
	cplane_t *pplane;
	float dot;
	msurface_t *psurf;

	psurf = &currentmodel->surfaces[currentmodel->firstModelSurface];

	//
	// draw texture
	//

	for (i = 0; i < currentmodel->numModelSurfaces; i++, psurf++) {
		// find which side of the node we are on
		pplane = psurf->plane;
		dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

		// draw the polygon
		if (((psurf->flags & MSURF_PLANEBACK) && (dot < -BACKFACE_EPSILON))
			|| (!(psurf->flags & MSURF_PLANEBACK) && (dot > BACKFACE_EPSILON))) {

			if (psurf->visframe == r_framecount)	// reckless fix
				continue;

			/*===============================
			berserker - flares for brushmodels
			=================================*/
			psurf->visframe = r_framecount;
			psurf->ent = currententity;
			// ================================

			if (psurf->texInfo->flags & (SURF_TRANS33 | SURF_TRANS66)) {
				psurf->texturechain = r_alpha_surfaces;
				r_alpha_surfaces = psurf;
				continue;
			}

			if (!(psurf->texInfo->flags & SURF_WARP))
				scene_surfaces[num_scene_surfaces++] = psurf;
			else {
				// KRIGS: reflection pass
				psurf->texturechain = r_reflective_surfaces;
				r_reflective_surfaces = psurf;
			}
		}
	}
}


static void R_DrawInlineBModel2(void)
{
	int i;
	cplane_t *pplane;
	float dot;
	msurface_t *psurf;
		
	psurf = &currentmodel->surfaces[currentmodel->firstModelSurface];
	//
	// draw texture
	//
	for (i = 0; i < currentmodel->numModelSurfaces; i++, psurf++) {
		// find which side of the node we are on
		pplane = psurf->plane;
		dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

		// draw the polygon
		if (((psurf->flags & MSURF_PLANEBACK) && (dot < -BACKFACE_EPSILON))
			|| (!(psurf->flags & MSURF_PLANEBACK) && (dot > BACKFACE_EPSILON))) {

			if (psurf->visframe == r_framecount) //reckless fix
				continue;
		
			if (psurf->texInfo->flags & (SURF_TRANS33 | SURF_TRANS66)) {	
				continue;
			} 
			else 
				if (psurf->flags & MSURF_DRAWTURB)
						R_RenderBrushPoly(psurf);
						
		}
	}
}


/*
=================
R_DrawBrushModel

=================
*/
int CL_PMpointcontents(vec3_t point);

void R_DrawBrushModel (void) {
	vec3_t		mins, maxs, tmp;
	int			i;
    qboolean	rotated;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (currentmodel->numModelSurfaces == 0)
		return;
	
	gl_state.currenttextures[0] = gl_state.currenttextures[1] = -1;

	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2]) {
		rotated = true;
		for (i = 0; i < 3; i++) {
			mins[i] = currententity->origin[i] - currentmodel->radius;
			maxs[i] = currententity->origin[i] + currentmodel->radius;
		}
	} else {
		rotated = false;
		VectorAdd(currententity->origin, currentmodel->mins, mins);
		VectorAdd(currententity->origin, currentmodel->maxs, maxs);
	}

	if (R_CullBox(mins, maxs))
		return;

	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));


	VectorSubtract(r_newrefdef.vieworg, currententity->origin, modelorg);

	if (rotated) {
		vec3_t temp;
		vec3_t forward, right, up;

		VectorCopy(modelorg, temp);
		AngleVectors(currententity->angles, forward, right, up);
		modelorg[0] = DotProduct(temp, forward);
		modelorg[1] = -DotProduct(temp, right);
		modelorg[2] = DotProduct(temp, up);
	}

	qglPushMatrix();
	R_RotateForEntity(currententity);

	//Put camera into model space view angle for bmodels parallax
	VectorSubtract(r_origin, currententity->origin, tmp);
	Mat3_TransposeMultiplyVector(currententity->axis, tmp, BmodelViewOrg);

	R_DrawInlineBModel2();

	qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_BSP);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_TEX1);
	qglEnableVertexAttribArray(ATRB_NORMAL);
	qglEnableVertexAttribArray(ATRB_TANGENT);
	qglEnableVertexAttribArray(ATRB_BINORMAL);

	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.xyz_offset));
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.st_offset));
	qglVertexAttribPointer(ATRB_TEX1, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.lm_offset));
	qglVertexAttribPointer(ATRB_NORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.nm_offset));
	qglVertexAttribPointer(ATRB_TANGENT, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.tg_offset));
	qglVertexAttribPointer(ATRB_BINORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.bn_offset));
	
	num_scene_surfaces = 0;
	R_DrawInlineBModel();
	GL_DrawLightmappedPoly(true);
	
	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_NORMAL);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_TEX1);
	qglDisableVertexAttribArray(ATRB_TANGENT);
	qglDisableVertexAttribArray(ATRB_BINORMAL);
	qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

	qglPopMatrix();
}



qboolean R_MarkBrushModelSurfaces()
{
	int			i;
	msurface_t	*psurf;
	model_t		*clmodel;

	clmodel = currententity->model;
	psurf = &clmodel->surfaces[clmodel->firstModelSurface];


	for (i=0 ; i<clmodel->numModelSurfaces ; i++, psurf++)
	{

		if (R_MarkLightSurf (psurf, false))
			{
				light_surfaces[num_light_surfaces++] = psurf;
			}
	}

	return num_light_surfaces;
}

void R_DrawLightBrushModel()
{

	vec3_t		mins, maxs, org;
	int			i;
    qboolean	rotated;
	vec3_t		tmp, oldLight;
	qboolean	caustics;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (currentmodel->numModelSurfaces == 0)
		return;

	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2]) {
		rotated = true;
		for (i = 0; i < 3; i++) {
			mins[i] = currententity->origin[i] - currentmodel->radius;
			maxs[i] = currententity->origin[i] + currentmodel->radius;
		}
	} else {
		rotated = false;
		VectorAdd(currententity->origin, currentmodel->mins, mins);
		VectorAdd(currententity->origin, currentmodel->maxs, maxs);
	}

	if(currentShadowLight->spherical){
		if(!BoundsAndSphereIntersect(mins, maxs, currentShadowLight->origin, currentShadowLight->radius[0]))
			return;
	}else{
		if(!BoundsIntersect(mins, maxs, currentShadowLight->mins, currentShadowLight->maxs))
			return;
	}
	
	qglPushMatrix();
	R_RotateForEntity(currententity);
	
	//Put camera into model space view angle for bmodels parallax
	VectorSubtract(r_origin, currententity->origin, tmp);
	Mat3_TransposeMultiplyVector(currententity->axis, tmp, BmodelViewOrg);

	VectorCopy(currentShadowLight->origin, oldLight);
	VectorSubtract(currentShadowLight->origin, currententity->origin, tmp);
	Mat3_TransposeMultiplyVector(currententity->axis, tmp, currentShadowLight->origin);

	caustics = false;
	currententity->minmax[0] = mins[0];
	currententity->minmax[1] = mins[1];
	currententity->minmax[2] = mins[2];
	currententity->minmax[3] = maxs[0];
	currententity->minmax[4] = maxs[1];
	currententity->minmax[5] = maxs[2];

	VectorSet(org, currententity->minmax[0], currententity->minmax[1], currententity->minmax[5]);
	if (CL_PMpointcontents2(org, currentmodel) & MASK_WATER)
		caustics = true;
	else
	{
		VectorSet(org, currententity->minmax[3], currententity->minmax[1], currententity->minmax[5]);
		if (CL_PMpointcontents2(org, currentmodel) & MASK_WATER)
			caustics = true;
		else
		{
			VectorSet(org, currententity->minmax[0], currententity->minmax[4], currententity->minmax[5]);
			if (CL_PMpointcontents2(org, currentmodel) & MASK_WATER)
				caustics = true;
			else
			{
				VectorSet(org, currententity->minmax[3], currententity->minmax[4], currententity->minmax[5]);
				if (CL_PMpointcontents2(org, currentmodel) & MASK_WATER)
					caustics = true;
			}
		}
	}
	
	GL_StencilFunc(GL_EQUAL, 128, 255);
	GL_StencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	GL_StencilMask(0);
	GL_DepthFunc(GL_LEQUAL);

	qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_BSP);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_NORMAL);
	qglEnableVertexAttribArray(ATRB_TANGENT);
	qglEnableVertexAttribArray(ATRB_BINORMAL);

	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.xyz_offset));
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.st_offset));
	qglVertexAttribPointer(ATRB_NORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.nm_offset));
	qglVertexAttribPointer(ATRB_TANGENT, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.tg_offset));
	qglVertexAttribPointer(ATRB_BINORMAL, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(gl_state.bn_offset));

	r_lightTimestamp++;
	num_light_surfaces = 0;
	
	if(R_MarkBrushModelSurfaces())
		GL_DrawLightPass(true, caustics);

	VectorCopy(oldLight, currentShadowLight->origin);
	
	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_NORMAL);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_TANGENT);
	qglDisableVertexAttribArray(ATRB_BINORMAL);
	qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	qglPopMatrix();
}


/*
===============
R_MarkLeaves

Mark the leaves and nodes that are in the PVS for the current
cluster
===============
*/
void R_MarkLeaves (void)
{
	byte	*vis;
	byte	fatvis[MAX_MAP_LEAFS/8];
	mnode_t	*node;
	int		i, c;
	mleaf_t	*leaf;
	int		cluster;

	if (r_oldviewcluster == r_viewcluster && r_oldviewcluster2 == r_viewcluster2 /*&& !r_novis->value*/ && r_viewcluster != -1)
		return;


	r_visframecount++;
	r_oldviewcluster = r_viewcluster;
	r_oldviewcluster2 = r_viewcluster2;

	if (/*r_novis->value ||*/ r_viewcluster == -1 || !r_worldmodel->vis)
	{
		// mark everything
		for (i=0 ; i<r_worldmodel->numLeafs ; i++)
			r_worldmodel->leafs[i].visframe = r_visframecount;
		for (i=0 ; i<r_worldmodel->numNodes ; i++)
			r_worldmodel->nodes[i].visframe = r_visframecount;
		memset(&viewvis, 0xff, (r_worldmodel->numLeafs+7)>>3);	// all visible
		return;
	}

	vis = Mod_ClusterPVS (r_viewcluster, r_worldmodel);
	// may have to combine two clusters because of solid water boundaries
	if (r_viewcluster2 != r_viewcluster)
	{
		memcpy (fatvis, vis, (r_worldmodel->numLeafs+7)>>3);
		vis = Mod_ClusterPVS (r_viewcluster2, r_worldmodel);
		c = (r_worldmodel->numLeafs+31)/32;
		for (i=0 ; i<c ; i++)
			((int *)fatvis)[i] |= ((int *)vis)[i];
		vis = fatvis;
	}

	memcpy(&viewvis, vis, (r_worldmodel->numLeafs+7)>>3);

	for (i=0,leaf=r_worldmodel->leafs ; i<r_worldmodel->numLeafs ; i++, leaf++)
	{
		cluster = leaf->cluster;
		if (cluster == -1)
			continue;
		if (vis[cluster>>3] & (1<<(cluster&7)))
		{
			node = (mnode_t *)leaf;
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}