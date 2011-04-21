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

model_t *loadmodel;


vec4_t shadelight_surface;
void R_LightColor(vec3_t org, vec3_t color);

static vec3_t modelorg;			// relative to viewpoint

msurface_t *r_alpha_surfaces;

float color_black[4] = {0.0, 0.0, 0.0, 0.0};


// set GL_BGRA for fastest lightmaps upload
#define GL_LIGHTMAP_FORMAT GL_BGRA


static void LM_InitBlock(void);
static void LM_UploadBlock(qboolean dynamic);
static qboolean LM_AllocBlock(int w, int h, int *x, int *y);

extern void R_SetCacheState(msurface_t * surf);
extern void R_BuildLightMap(msurface_t * surf, byte * dest, int stride, qboolean loadModel);

	
vec3_t  wVertexArray[MAX_BATCH_SURFS];
float   wTexArray[MAX_BATCH_SURFS][2];
float   wLMArray[MAX_BATCH_SURFS][2];

float   wTmu0Array[MAX_BATCH_SURFS][2];
float   wTmu1Array[MAX_BATCH_SURFS][2];
float   wTmu2Array[MAX_BATCH_SURFS][2];
float   wTmu3Array[MAX_BATCH_SURFS][2];
float   wTmu4Array[MAX_BATCH_SURFS][2];
float   wTmu5Array[MAX_BATCH_SURFS][2];
float   wTmu6Array[MAX_BATCH_SURFS][2];

WORD	indexArray[MAX_BATCH_SURFS*3];
unsigned	numVertices, numIndeces;

vec3_t			nTexArray[MAX_BATCH_SURFS];
vec3_t			tTexArray[MAX_BATCH_SURFS];
vec3_t			bTexArray[MAX_BATCH_SURFS];



/*
=================
R_DrawArrays
=================
*/
void R_DrawArrays (void)
{
	if (numVertices == 0 || numIndeces == 0) // nothing to render
		return;

	if (gl_state.DrawRangeElements)
		qglDrawRangeElementsEXT(GL_TRIANGLES, 0, numVertices, numIndeces, GL_UNSIGNED_SHORT, indexArray);
	else
		qglDrawElements(GL_TRIANGLES, numIndeces, GL_UNSIGNED_SHORT, indexArray);
	
}

void R_DrawArraysType (unsigned int type)
{
	if (numVertices == 0 || numIndeces == 0) // nothing to render
		return;

	if (gl_state.DrawRangeElements)
		qglDrawRangeElementsEXT(type, 0, numVertices, numIndeces, GL_UNSIGNED_SHORT, indexArray);
	else
		qglDrawElements(type, numIndeces, GL_UNSIGNED_SHORT, indexArray);
	
}

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
image_t *R_TextureAnimation(mtexinfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->image;

	c = currententity->frame % tex->numframes;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->image;
}

image_t *R_TextureAnimationHm(mtexinfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->hiMap;

	c = currententity->frame % tex->numframes;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->hiMap;
}

image_t *R_TextureAnimationFx(mtexinfo_t * tex)
{
	int c;

	if (!tex->next)
		return tex->addTexture;

	c = currententity->frame % tex->numframes;
	while (c) {
		tex = tex->next;
		c--;
	}

	return tex->addTexture;
}

float SurfColorArray[MAX_BATCH_SURFS][4];
/*
================
DrawGLPoly
================
*/

void DrawGLPoly1PASS(msurface_t * fa)
{
	int i;
	float *v, alpha;
	glpoly_t *p = fa->polys;

	GL_Overbrights(false);

	if (fa->texinfo->flags & SURF_TRANS33)
		alpha = 0.33;
	else 
		alpha = 0.66;

	GL_SelectTexture			(GL_TEXTURE0_ARB);
	qglEnable					(GL_TEXTURE_2D);
	GL_Bind						(fa->texinfo->image->texnum);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglEnableClientState		(GL_COLOR_ARRAY);
	qglColorPointer				(4, GL_FLOAT, 0, SurfColorArray);

	qglEnableClientState		(GL_VERTEX_ARRAY);
	qglVertexPointer			(3, GL_FLOAT, 0, wVertexArray);

	v = p->verts[0];
	
	for (i=0; i < p->numverts-2; i++) {
			indexArray[numIndeces++] = numVertices;
			indexArray[numIndeces++] = numVertices+i+1;
			indexArray[numIndeces++] = numVertices+i+2;
			}
	c_brush_polys += numIndeces / 3;

	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE) {
		
		VectorCopy(v, wVertexArray[i]);
			
		wTexArray[i][0] = v[3];
		wTexArray[i][1] = v[4];

		R_LightColor	(v, shadelight_surface);
		VA_SetElem4		(SurfColorArray[i],	shadelight_surface[0], 
											shadelight_surface[1], 
											shadelight_surface[2], 
											alpha);	
	numVertices++;
	}
	R_DrawArrays();
		numIndeces = numVertices = 0;
	
	qglDisableClientState	(GL_VERTEX_ARRAY);
	
	GL_SelectTexture		(GL_TEXTURE0_ARB);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);
	qglDisableClientState	(GL_COLOR_ARRAY);


}

void DrawGLPolyARB(msurface_t * fa)
{
	int i;
	float *v;
	float alpha;
	glpoly_t *p;
	int			id;
	unsigned	defBits = 0;
	
	GL_Overbrights(false);

	if (!fa->texinfo->normalmap) {
		DrawGLPoly1PASS(fa);
		return;
	}


	if (fa->texinfo->flags & !(SURF_TRANS33 || SURF_TRANS66)) {
		DrawGLPoly1PASS(fa);
		return;
	}

	if (fa->texinfo->flags & SURF_TRANS33 || SURF_TRANS66)
		qglDisable(GL_BLEND);
	
	if (fa->texinfo->flags & SURF_TRANS33)
		alpha = 0.33;
	else 
		alpha = 0.66;

	// setup program
	GL_BindProgram(refractProgram, defBits);
	id = refractProgram->id[defBits];

	GL_SelectTexture			(GL_TEXTURE0_ARB);
	GL_Bind						(fa->texinfo->normalmap->texnum);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglUniform1i				(qglGetUniformLocation(id, "u_deformMap"), 0);

	GL_SelectTexture			(GL_TEXTURE1_ARB);
	GL_Bind						(fa->texinfo->image->texnum);
	qglEnable					(GL_TEXTURE_2D);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglEnableClientState		(GL_COLOR_ARRAY);
	qglColorPointer				(4, GL_FLOAT, 0, SurfColorArray);
	qglUniform1i				(qglGetUniformLocation(id, "u_colorMap"), 1);

	GL_SelectTexture			(GL_TEXTURE2_ARB);
	GL_BindRect					(ScreenMap->texnum);
	qglEnable					(GL_TEXTURE_RECTANGLE_ARB);
	qglUniform1i				(qglGetUniformLocation(id, "g_colorBufferMap"), 2);
	
	GL_SelectTexture			(GL_TEXTURE3_ARB);
	GL_BindRect					(depthMap->texnum);
	qglEnable					(GL_TEXTURE_RECTANGLE_ARB);
	qglUniform1i				(qglGetUniformLocation(id, "g_depthBufferMap"), 3);

	qglEnableClientState		(GL_VERTEX_ARRAY);
	qglVertexPointer			(3, GL_FLOAT, 0, wVertexArray);

	qglUniform1f				(qglGetUniformLocation(id, "u_deformMul"),	1.0);
	qglUniform1f				(qglGetUniformLocation(id, "u_alpha"),	alpha);
	qglUniform1f				(qglGetUniformLocation(id, "u_thickness"),	300.000);
	qglUniform2f				(qglGetUniformLocation(id, "u_viewport"),	vid.width, vid.height);
	qglUniform2f				(qglGetUniformLocation(id, "u_depthParms"), r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
	
	p = fa->polys;
	v = p->verts[0];
	for (i=0; i < p->numverts-2; i++) {
			indexArray[numIndeces++] = numVertices;
			indexArray[numIndeces++] = numVertices+i+1;
			indexArray[numIndeces++] = numVertices+i+2;
			}
	c_brush_polys += numIndeces / 3;

	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE) {
		
		VectorCopy(v, wVertexArray[i]);
			
		wTexArray[i][0] = v[3];
		wTexArray[i][1] = v[4];

		R_LightColor	(v, shadelight_surface);
		VA_SetElem4		(SurfColorArray[i],	shadelight_surface[0], 
											shadelight_surface[1], 
											shadelight_surface[2], 
											alpha);	
	numVertices++;
	}
	R_DrawArrays();
		numIndeces = numVertices = 0;
	
	qglDisableClientState	(GL_VERTEX_ARRAY);

	GL_SelectTexture		(GL_TEXTURE3_ARB);
	qglDisable				(GL_TEXTURE_RECTANGLE_ARB);
		
	GL_SelectTexture		(GL_TEXTURE2_ARB);
	qglDisable				(GL_TEXTURE_RECTANGLE_ARB);
	
	GL_SelectTexture		(GL_TEXTURE1_ARB);
	qglDisable				(GL_TEXTURE_2D);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);
	qglDisableClientState	(GL_COLOR_ARRAY);

	GL_SelectTexture		(GL_TEXTURE0_ARB);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);

	GL_BindNullProgram		();
	
	if (fa->texinfo->flags & SURF_TRANS33 || SURF_TRANS66)
		qglEnable(GL_BLEND);

}




//============
//PGM
/*
================
DrawGLFlowingPoly -- version of DrawGLPoly that handles scrolling texture
================
*/

void DrawGLFlowingPoly1PASS(msurface_t * fa)
{
	int i;
	float *v, alpha, scroll;
	glpoly_t *p = fa->polys;

	GL_Overbrights(false);

	if (fa->texinfo->flags & SURF_TRANS33)
		alpha = 0.33;
	else 
		alpha = 0.66;

	GL_SelectTexture			(GL_TEXTURE0_ARB);
	qglEnable					(GL_TEXTURE_2D);
	GL_Bind						(fa->texinfo->image->texnum);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglEnableClientState		(GL_COLOR_ARRAY);
	qglColorPointer				(4, GL_FLOAT, 0, SurfColorArray);

	qglEnableClientState		(GL_VERTEX_ARRAY);
	qglVertexPointer			(3, GL_FLOAT, 0, wVertexArray);

	v = p->verts[0];
	
	for (i=0; i < p->numverts-2; i++) {
			indexArray[numIndeces++] = numVertices;
			indexArray[numIndeces++] = numVertices+i+1;
			indexArray[numIndeces++] = numVertices+i+2;
			}
	c_brush_polys += numIndeces / 3;
	
	scroll =
		-64 * ((r_newrefdef.time / 40.0) -
			   (int) (r_newrefdef.time / 40.0));

	if (scroll == 0.0)
		scroll = -64.0;

	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE) {
		
		VectorCopy(v, wVertexArray[i]);
			
		wTexArray[i][0] = v[3]+scroll;
		wTexArray[i][1] = v[4];

		R_LightColor	(v, shadelight_surface);
		VA_SetElem4		(SurfColorArray[i],	shadelight_surface[0], 
											shadelight_surface[1], 
											shadelight_surface[2], 
											alpha);	
	numVertices++;
	}
	R_DrawArrays();
		numIndeces = numVertices = 0;
	
	qglDisableClientState	(GL_VERTEX_ARRAY);
	
	GL_SelectTexture		(GL_TEXTURE0_ARB);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);
	qglDisableClientState	(GL_COLOR_ARRAY);


}

void DrawGLFlowingPolyARB(msurface_t * fa)
{
	int i;
	float *v;
	float alpha, scroll;
	glpoly_t *p;
	int			id;
	unsigned	defBits = 0;


	GL_Overbrights(false);

	if (!fa->texinfo->normalmap) {
		DrawGLFlowingPoly1PASS(fa);
		return;
	}

	if (fa->texinfo->flags & !(SURF_TRANS33 || SURF_TRANS66)) {
		DrawGLFlowingPoly1PASS(fa);
		return;
	}

	if (fa->texinfo->flags & SURF_TRANS33 || SURF_TRANS66)
		qglDisable(GL_BLEND);
	
	if (fa->texinfo->flags & SURF_TRANS33)
		alpha = 0.33;
	else 
		alpha = 0.66;

	// setup program
	GL_BindProgram(refractProgram, defBits);
	id = refractProgram->id[defBits];

	GL_SelectTexture			(GL_TEXTURE0_ARB);
	GL_Bind						(fa->texinfo->normalmap->texnum);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglUniform1i				(qglGetUniformLocation(id, "u_deformMap"), 0);

	GL_SelectTexture			(GL_TEXTURE1_ARB);
	GL_Bind						(fa->texinfo->image->texnum);
	qglEnable					(GL_TEXTURE_2D);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglEnableClientState		(GL_COLOR_ARRAY);
	qglColorPointer				(4, GL_FLOAT, 0, SurfColorArray);
	qglUniform1i				(qglGetUniformLocation(id, "u_colorMap"), 1);
	
	GL_SelectTexture			(GL_TEXTURE2_ARB);
	GL_BindRect					(ScreenMap->texnum);
	qglEnable					(GL_TEXTURE_RECTANGLE_ARB);
	qglEnableClientState		(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer			(2, GL_FLOAT, 0, wTexArray);
	qglUniform1i				(qglGetUniformLocation(id, "g_colorBufferMap"), 2);
		
	GL_SelectTexture			(GL_TEXTURE3_ARB);
	GL_BindRect					(depthMap->texnum);
	qglEnable					(GL_TEXTURE_RECTANGLE_ARB);
	qglUniform1i				(qglGetUniformLocation(id, "g_depthBufferMap"), 3);

	qglEnableClientState		(GL_VERTEX_ARRAY);
	qglVertexPointer			(3, GL_FLOAT, 0, wVertexArray);

	qglUniform1f				(qglGetUniformLocation(id, "u_deformMul"),	1.0);
	qglUniform1f				(qglGetUniformLocation(id, "u_alpha"),	alpha);
	qglUniform1f				(qglGetUniformLocation(id, "u_thickness"),	300.000);
	qglUniform2f				(qglGetUniformLocation(id, "u_viewport"),	vid.width, vid.height);
	qglUniform2f				(qglGetUniformLocation(id, "u_depthParms"), r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
		
	qglShadeModel(GL_SMOOTH);


	p = fa->polys;
	v = p->verts[0];
	for (i=0; i < p->numverts-2; i++) {
			indexArray[numIndeces++] = numVertices;
			indexArray[numIndeces++] = numVertices+i+1;
			indexArray[numIndeces++] = numVertices+i+2;
			}
	c_brush_polys += numIndeces / 3;
	
	scroll =
		-64 * ((r_newrefdef.time / 40.0) -
			   (int) (r_newrefdef.time / 40.0));

	if (scroll == 0.0)
		scroll = -64.0;

	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE) {
		
		VectorCopy(v, wVertexArray[i]);
			
		wTmu0Array[i][0] = v[3]+scroll;
		wTmu0Array[i][1] = v[4];

		wTmu1Array[i][0] = v[3];
		wTmu1Array[i][1] = v[4];

		wTmu2Array[i][0] = v[3]+scroll;
		wTmu2Array[i][1] = v[4];

		R_LightColor	(v, shadelight_surface);
		VA_SetElem4		(SurfColorArray[i],	shadelight_surface[0], 
											shadelight_surface[1], 
											shadelight_surface[2], 
											alpha);	
	numVertices++;
	}
	R_DrawArrays();
		numIndeces = numVertices = 0;
	
	qglDisableClientState	(GL_VERTEX_ARRAY);

	GL_SelectTexture		(GL_TEXTURE3_ARB);
	qglDisable				(GL_TEXTURE_RECTANGLE_ARB);

	GL_SelectTexture		(GL_TEXTURE2_ARB);
	qglDisable				(GL_TEXTURE_RECTANGLE_ARB);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);

	GL_SelectTexture		(GL_TEXTURE1_ARB);
	qglDisable				(GL_TEXTURE_2D);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);
	qglDisableClientState	(GL_COLOR_ARRAY);
	
	GL_SelectTexture		(GL_TEXTURE0_ARB);
	qglDisableClientState	(GL_TEXTURE_COORD_ARRAY);

	GL_BindNullProgram		();
	
	if (fa->texinfo->flags & SURF_TRANS33 || SURF_TRANS66)
		qglEnable(GL_BLEND);

}




/*
================
R_RenderBrushPoly
================
*/

void R_RenderBrushPoly (msurface_t *fa)
{
	image_t *image;
	char *purename;
	char noext[MAX_QPATH];
	
	image = R_TextureAnimation(fa->texinfo);
	purename = COM_SkipPath(image->name);
	COM_StripExtension(purename, noext);

	c_brush_polys++;

	if (fa->flags & SURF_DRAWTURB)
	{	

		if (!strcmp(noext, "brlava"))
			RenderLavaSurfaces(fa);
		else
			EmitWaterPolys(fa);
		return;
	}

	if(fa->texinfo->flags & SURF_FLOWING)
		DrawGLFlowingPolyARB (fa);
	else
		DrawGLPolyARB (fa);

}





extern image_t *envmap_tex;

void R_DrawAlphaPoly(void)
{
	msurface_t *s;
	float intens;

	//
	// go back to the world matrix
	//
	
	qglLoadMatrixf(r_world_matrix);
	GLSTATE_ENABLE_BLEND 
	GL_Overbrights(true);
	qglShadeModel(GL_SMOOTH);
	qglDepthMask(0);
	intens = gl_state.inverse_intensity;
	
	for (s = r_alpha_surfaces; s; s = s->texturechain) {
		GL_Bind(s->texinfo->image->texnum);
		c_brush_polys++;

		// moving trans brushes - spaz
		if (s->ent) {
			s->ent->angles[0] = -s->ent->angles[0];	// stupid quake bug
			s->ent->angles[2] = -s->ent->angles[2];	// stupid quake bug
			R_RotateForEntity(s->ent);
			s->ent->angles[0] = -s->ent->angles[0];	// stupid quake bug
			s->ent->angles[2] = -s->ent->angles[2];	// stupid quake bug
		}

		if (s->texinfo->flags & SURF_TRANS33) 
			shadelight_surface[3] = 0.33;
		else	
		if (s->texinfo->flags & SURF_TRANS66) 
			shadelight_surface[3] = 0.66;
		else
			shadelight_surface[3] = 1.0;
		

		if (s->flags & SURF_DRAWTURB)
			EmitWaterPolys(s);
		else if (s->texinfo->flags & SURF_FLOWING)	// PGM 9/16/98
			DrawGLFlowingPolyARB(s);	// PGM
		else
			DrawGLPolyARB(s);

	}

	GL_TexEnv(GL_REPLACE);
	qglColor4f(1, 1, 1, 1);
	qglDisable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDepthMask(1);
	r_alpha_surfaces = NULL;
	GL_Overbrights(false);
}





/*
================
DrawTextureChains
================
*/
void DrawTextureChains(void)
{
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

			for (; s; s = s->texturechain) {
					R_RenderBrushPoly(s);
			}
		
			image->texturechain = NULL;
		}

	GL_TexEnv(GL_REPLACE);

}


/*
===============
SORT AND BATCH 
BSP SURFACES
===============
*/

void GL_CreateParallaxLmPoly(msurface_t * surf)
	{
	int i, nv = surf->polys->numverts;
	float *v;
	glpoly_t *p = surf->polys;
	float scroll;
	
	scroll = -64 * ((r_newrefdef.time / 40.0) - (int) (r_newrefdef.time / 40.0));
		
	if (scroll == 0.0)
		scroll = -64.0;

			v = p->verts[0];
			c_brush_polys += (nv-2);

			
			for (i=0; i < nv-2; i++) {
			indexArray[numIndeces++] = numVertices;
			indexArray[numIndeces++] = numVertices+i+1;
			indexArray[numIndeces++] = numVertices+i+2;
			}

			for (i = 0; i < nv; i++, v += VERTEXSIZE) 
				{
				VectorCopy(v, wVertexArray[i]);
				//baseTex
				if (surf->texinfo->flags & SURF_FLOWING){
				wTexArray[i][0] = v[3]+scroll;
				}else{
				wTexArray[i][0] = v[3];
				}
				wTexArray[i][1] = v[4];
				
				//lightMap
				wLMArray[i][0]  = v[5];
				wLMArray[i][1]  = v[6];
				//normals
				nTexArray[i][0] = v[7];
				nTexArray[i][1] = v[8];
				nTexArray[i][2] = v[9];
				//tangents
				tTexArray[i][0] = v[10];
				tTexArray[i][1] = v[11];
				tTexArray[i][2] = v[12];
				//binormals
				bTexArray[i][0] = v[13];
				bTexArray[i][1] = v[14];
				bTexArray[i][2] = v[15];

				numVertices++;
			}
	
}

vec3_t BmodelViewOrg;
extern cvar_t	*r_parallax;
extern cvar_t	*r_parallaxScale;

 int SurfSort( const msurface_t **a, const msurface_t **b )
{
return	(((*a)->lightmaptexturenum<<26)+((*a)->texinfo->image->texnum<<13) + (*b)->texinfo->addTexture->texnum) - 
		(((*b)->lightmaptexturenum<<26)+((*b)->texinfo->image->texnum<<13) + (*b)->texinfo->addTexture->texnum);
}


int	num_scene_surfaces;
msurface_t	*scene_surfaces[MAX_MAP_FACES];

static void GL_BatchLightmappedPoly(qboolean bmodel, qboolean caustics)
{
	msurface_t	*s;
	image_t		*image;
	image_t		*fx;
	unsigned	lmtex;
	unsigned	defBits = 0;
	int			id, i, map;
	float		scale[2];
	qboolean	is_dynamic = false;

	c_visible_textures = 0;

	qsort(scene_surfaces, num_scene_surfaces, sizeof(msurface_t*), (int (*)(const void *, const void *))SurfSort);
		
	for (i=0; i<num_scene_surfaces; i++)
	{
	s=scene_surfaces[i];
	image = R_TextureAnimation(s->texinfo);
	fx    = R_TextureAnimationFx(s->texinfo);
	lmtex = s->lightmaptexturenum;
	
	if(caustics || (s->flags & SURF_WATER)){

	if (image->has_alpha && r_parallax->value)
		defBits = worldDefs.ParallaxBit | worldDefs.CausticsBit | worldDefs.LightmapBits;
	else
		defBits = worldDefs.CausticsBit | worldDefs.LightmapBits;
	}
	else
	{
	if (image->has_alpha && r_parallax->value)
		defBits = worldDefs.ParallaxBit | worldDefs.LightmapBits;
	else
		defBits = worldDefs.LightmapBits;
	}

	// setup program
	GL_BindProgram(diffuseProgram, defBits);
	id = diffuseProgram->id[defBits];

	scale[0] = r_parallaxScale->value / image->width;
	scale[1] = r_parallaxScale->value / image->height;

	qglUniform1f(qglGetUniformLocation(id, "u_ColorModulate"), r_overBrightBits->value);

	if(caustics || (s->flags & SURF_WATER))
		qglUniform1f(qglGetUniformLocation(id, "u_CausticsModulate"), r_causticIntens->value);

	if(bmodel)
		qglUniform3fv(qglGetUniformLocation(id, "u_viewOriginES"), 1 , BmodelViewOrg);
	else
	qglUniform3fv(qglGetUniformLocation(id, "u_viewOriginES"), 1 , r_origin);

	qglUniform2f(qglGetUniformLocation(id, "u_bumpScale"), scale[0], scale[1]);
	qglUniform1i(qglGetUniformLocation(id, "u_numSteps"), r_parallaxSteps->value);
	qglUniform1i(qglGetUniformLocation(id, "u_parallaxType"), r_parallax->value);

		
	GL_CreateParallaxLmPoly(s);
	
	c_brush_polys++;

		for (map = 0; map < MAXLIGHTMAPS && s->styles[map] != 255; map++) {
		if (r_newrefdef.lightstyles[s->styles[map]].white != s->cached_light[map])
			goto dynamic;
	}
	
	// dynamic this frame or dynamic previously
	if ((s->dlightframe == r_framecount)) {
	
	dynamic:
		if (r_dynamic->value) {
			
			if (!
				(s->texinfo->
				 flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 |
						  SURF_WARP))) {
				is_dynamic = true;
			}
		}
	}

	if (is_dynamic) {
		unsigned temp[128 * 128];
		int smax, tmax;
				
		if ((s->styles[map] >= 32 || s->styles[map] == 0)
			&& (s->dlightframe != r_framecount)) {
			smax = (s->extents[0] / r_worldmodel->lightmap_scale) + 1;
			tmax = (s->extents[1] / r_worldmodel->lightmap_scale) + 1;

			R_BuildLightMap(s, (byte *) temp, smax * 4, false);
			R_SetCacheState(s);

			GL_MBind(GL_TEXTURE1_ARB, gl_state.lightmap_textures + s->lightmaptexturenum);

			lmtex = s->lightmaptexturenum;

			qglTexSubImage2D(GL_TEXTURE_2D, 0,
							 s->light_s, s->light_t,
							 smax, tmax,
							 GL_LIGHTMAP_FORMAT, GL_UNSIGNED_INT_8_8_8_8_REV, temp);

		} else {
			smax = (s->extents[0] / r_worldmodel->lightmap_scale) + 1;
			tmax = (s->extents[1] / r_worldmodel->lightmap_scale) + 1;

			R_BuildLightMap(s, (byte *) temp, smax * 4, false);

			GL_MBind(GL_TEXTURE1_ARB, gl_state.lightmap_textures + 0);

			lmtex = 0;

			qglTexSubImage2D(GL_TEXTURE_2D, 0, s->light_s, s->light_t, smax, tmax, GL_LIGHTMAP_FORMAT, GL_UNSIGNED_INT_8_8_8_8_REV, temp);
		
		}

		c_brush_polys++;
				
		if(numVertices) {
		GL_MBind(GL_TEXTURE0_ARB, image->texnum);
		qglUniform1i(qglGetUniformLocation(id, "u_Diffuse"), 0);
		GL_MBind(GL_TEXTURE1_ARB, gl_state.lightmap_textures + lmtex);
		qglUniform1i(qglGetUniformLocation(id, "u_LightMap"), 1);
		GL_MBind(GL_TEXTURE2_ARB, fx->texnum);
		qglUniform1i(qglGetUniformLocation(id, "u_Add"), 2);
		if(caustics || (s->flags & SURF_WATER)){
		GL_MBind(GL_TEXTURE3_ARB, r_caustic[((int) (r_newrefdef.time * 15)) & (MAX_CAUSTICS - 1)]->texnum);
		qglUniform1i(qglGetUniformLocation(id, "u_Caustics"), 3);
		}
		
		R_DrawArrays();
		}
		numIndeces = numVertices = 0;
 		
		} else {
	
		c_brush_polys++;
		
		qglUniform3fv(qglGetUniformLocation(id, "u_LightOrg"), 1 , vec3_origin);
		qglUniform3fv(qglGetUniformLocation(id, "u_LightColor"), 1, vec3_origin);
		qglUniform1f(qglGetUniformLocation(id, "u_LightRadius"), 0.0);

		if(numVertices) {
		GL_MBind(GL_TEXTURE0_ARB, image->texnum);
		qglUniform1i(qglGetUniformLocation(id, "u_Diffuse"), 0);
		GL_MBind(GL_TEXTURE1_ARB, gl_state.lightmap_textures + lmtex);
		qglUniform1i(qglGetUniformLocation(id, "u_LightMap"), 1);
		GL_MBind(GL_TEXTURE2_ARB, fx->texnum);
		qglUniform1i(qglGetUniformLocation(id, "u_Add"), 2);
		if(caustics || (s->flags & SURF_WATER)){
		GL_MBind(GL_TEXTURE3_ARB, r_caustic[((int) (r_newrefdef.time * 15)) & (MAX_CAUSTICS - 1)]->texnum);
		qglUniform1i(qglGetUniformLocation(id, "u_Caustics"), 3);
		}

		R_DrawArrays();
		}
		numIndeces = numVertices = 0;
		}	
	}
	GL_BindNullProgram();
}



/*
=================
R_DrawInlineBModel
=================
*/
extern qboolean bmodelcaust = false;

void R_DrawBrushModel(entity_t * e);

static void R_DrawInlineBModel(void)
{
	int i, k;
	cplane_t *pplane;
	float dot;
	msurface_t *psurf;
	dlight_t *lt;
	
	// calculate dynamic lighting for bmodel
	if (!r_flashBlend->value) {
		lt = r_newrefdef.dlights;
		for (k = 0; k < r_newrefdef.num_dlights; k++, lt++) {
			R_MarkLights(lt, 1 << k,
						 currentmodel->nodes + currentmodel->firstnode);
		}
	}

	psurf = &currentmodel->surfaces[currentmodel->firstmodelsurface];

	//
	// draw texture
	//
	for (i = 0; i < currentmodel->nummodelsurfaces; i++, psurf++) {
		// find which side of the node we are on
		pplane = psurf->plane;



		dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

		// draw the polygon
		if (((psurf->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON))
			|| (!(psurf->flags & SURF_PLANEBACK)
				&& (dot > BACKFACE_EPSILON))) {
			/*===============================
			berserker - flares for brushmodels
			=================================*/
			psurf->visframe = r_framecount;
			psurf->ent = currententity;
			// ================================


			if (psurf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66)) {	
				psurf->texturechain = r_alpha_surfaces;
				r_alpha_surfaces = psurf;
			} else if (!(psurf->flags & SURF_DRAWTURB)) {
				
				scene_surfaces[num_scene_surfaces++] = psurf;
								
			} 
			else 
			{
					R_RenderBrushPoly(psurf);
					qglDisable(GL_BLEND);
			}
		}
	}

	if (!(currententity->flags & RF_TRANSLUCENT)) {
		
		qglDisable(GL_BLEND); 
		qglColor4f(1, 1, 1, 1);
		GL_TexEnv(GL_REPLACE);
	}

	qglDisable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/*
=================
R_DrawBrushModel
=================
*/


void R_DrawBrushModel(entity_t * e)
{
	vec3_t		mins, maxs;
	int			i;
	qboolean	rotated;
	int			cont[5];
	vec3_t		org;
	

	if (currentmodel->nummodelsurfaces == 0)
		return;

	currententity = e;
	gl_state.currenttextures[0] = gl_state.currenttextures[1] = -1;

	if (e->angles[0] || e->angles[1] || e->angles[2]) {
		rotated = true;
		for (i = 0; i < 3; i++) {
			mins[i] = e->origin[i] - currentmodel->radius;
			maxs[i] = e->origin[i] + currentmodel->radius;
		}
	} else {
		rotated = false;
		VectorAdd(e->origin, currentmodel->mins, mins);
		VectorAdd(e->origin, currentmodel->maxs, maxs);
	}

	if (R_CullBox(mins, maxs))
		return;

	qglColor3f(1, 1, 1);
	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));


	VectorSubtract(r_newrefdef.vieworg, e->origin, modelorg);

	if (rotated) {
		vec3_t temp;
		vec3_t forward, right, up;

		VectorCopy(modelorg, temp);
		AngleVectors(e->angles, forward, right, up);
		modelorg[0] = DotProduct(temp, forward);
		modelorg[1] = -DotProduct(temp, right);
		modelorg[2] = DotProduct(temp, up);
	}

	qglPushMatrix();
	e->angles[0] = -e->angles[0];	// stupid quake bug
	e->angles[2] = -e->angles[2];	// stupid quake bug
	R_RotateForEntity(e);
	e->angles[0] = -e->angles[0];	// stupid quake bug
	e->angles[2] = -e->angles[2];	// stupid quake bug

	// ==================================
	// detect underwater position
	// for bmodels caustics by Berserker
	// ==================================
	currententity->minmax[0] = mins[0];
	currententity->minmax[1] = mins[1];
	currententity->minmax[2] = mins[2];
	currententity->minmax[3] = maxs[0];
	currententity->minmax[4] = maxs[1];
	currententity->minmax[5] = maxs[2];

	VectorSet(org, currententity->minmax[0], currententity->minmax[1],
			  currententity->minmax[5]);
	cont[0] = CL_PMpointcontents2(org, currentmodel);
	if (!cont[0])
		goto nono;

	VectorSet(org, currententity->minmax[3], currententity->minmax[1],
			  currententity->minmax[5]);
	cont[1] = CL_PMpointcontents2(org, currentmodel);
	if (!cont[1])
		goto nono;
	VectorSet(org, currententity->minmax[0], currententity->minmax[4],
			  currententity->minmax[5]);
	cont[2] = CL_PMpointcontents2(org, currentmodel);
	if (!cont[2])
		goto nono;
	VectorSet(org, currententity->minmax[3], currententity->minmax[4],
			  currententity->minmax[5]);
	cont[3] = CL_PMpointcontents2(org, currentmodel);
	if (!cont[3])
		goto nono;
	org[0] = (currententity->minmax[0] + currententity->minmax[3]) * 0.5;
	org[1] = (currententity->minmax[1] + currententity->minmax[4]) * 0.5;
	org[2] = (currententity->minmax[2] + currententity->minmax[5]) * 0.5;

	cont[4] = CL_PMpointcontents2(org, currentmodel);
	if (!cont[4])
		goto nono;

	if ((cont[0] & MASK_WATERONLY) || (cont[1] & MASK_WATERONLY)
		|| (cont[2] & MASK_WATERONLY) || (cont[3] & MASK_WATERONLY)
		|| (cont[4] & MASK_WATERONLY))
		bmodelcaust = true;
	else
		bmodelcaust = false;


  nono:

	//Put camera into model space view angle for bmodels parallax
	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2])
		{
		vec3_t	forward, right, up, temp;
		VectorSubtract(r_origin, currententity->origin, temp);
		AngleVectors (currententity->angles, forward, right, up);
		BmodelViewOrg[0] = DotProduct (temp, forward);
		BmodelViewOrg[1] = -DotProduct (temp, right);
		BmodelViewOrg[2] = DotProduct (temp, up);
		}
	else
		VectorSubtract(r_origin, currententity->origin, BmodelViewOrg);


	//diffuse
	GL_SelectTexture(GL_TEXTURE0_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, 0, wTexArray);
	
	//lighmap
	GL_SelectTexture(GL_TEXTURE1_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, 0, wLMArray);
	qglEnable(GL_TEXTURE_2D);
	
	//addative map
	GL_SelectTexture(GL_TEXTURE2_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(3, GL_FLOAT, 0, wTexArray);
	qglEnable(GL_TEXTURE_2D);
	
	//caustics map
	GL_SelectTexture(GL_TEXTURE3_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, 0, wTexArray);
	qglEnable(GL_TEXTURE_2D);
	

	//normal
	qglEnableClientState(GL_NORMAL_ARRAY);
	qglNormalPointer(GL_FLOAT, 0, nTexArray);

	qglEnableClientState(GL_VERTEX_ARRAY);
	qglVertexPointer(3, GL_FLOAT, 0, wVertexArray);

	qglEnableVertexAttribArray(10);
	qglEnableVertexAttribArray(11);
	
	// tangent & binormal
	qglVertexAttribPointer(10, 3, GL_FLOAT, false, 0, tTexArray);
	qglVertexAttribPointer(11, 3, GL_FLOAT, false, 0, bTexArray);
	
	num_scene_surfaces = 0;
	R_DrawInlineBModel();
	GL_BatchLightmappedPoly(true, bmodelcaust);
	bmodelcaust = false;
	
	
	GL_SelectTexture(GL_TEXTURE3_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisable(GL_TEXTURE_2D);
	
	GL_SelectTexture(GL_TEXTURE2_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisable(GL_TEXTURE_2D);
	
	GL_SelectTexture(GL_TEXTURE1_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisable(GL_TEXTURE_2D);
	
	GL_SelectTexture(GL_TEXTURE0_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	qglDisableClientState(GL_VERTEX_ARRAY);
	qglDisableClientState(GL_NORMAL_ARRAY);

	qglDisableVertexAttribArray(10);
	qglDisableVertexAttribArray(11);

	qglPopMatrix();
	qglDisable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*
=============================================================

	WORLD MODEL

=============================================================
*/
qboolean SurfInFrustum(msurface_t *s)
{
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
		pleaf = (mleaf_t *) node;

		// check for door connected areas
		if (r_newrefdef.areabits) {
			if (!(r_newrefdef.areabits[pleaf->area >> 3] & (1 << (pleaf->area & 7))))
				return;			// not visible
		}

		mark = pleaf->firstmarksurface;
		c = pleaf->nummarksurfaces;

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
	} else {
		side = 1;
		sidebit = SURF_PLANEBACK;
	}

	// recurse down the children, front side first
	R_RecursiveWorldNode(node->children[side]);

	// draw stuff
	for (c = node->numsurfaces, surf = r_worldmodel->surfaces + node->firstsurface; c; c--, surf++) {
		
		if (surf->visframe != r_framecount)
			continue;

		if ((surf->flags & SURF_PLANEBACK) != sidebit)
			continue;			// wrong side

		if (surf->texinfo->flags & SURF_SKY) {	// just adds to visible sky bounds
			R_AddSkySurface(surf);
		} else if (surf->texinfo->flags & SURF_NODRAW)
			continue;
		else if (surf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66)) {	// add to the translucent chain
			surf->texturechain = r_alpha_surfaces;
			r_alpha_surfaces = surf;
		} else {
			if (!(surf->flags & SURF_DRAWTURB)) {
		
				scene_surfaces[num_scene_surfaces++] = surf;

			} else {
				// the polygon is visible, so add it to the texture
				// sorted chain
				// FIXME: this is a hack for animation
				image = R_TextureAnimation(surf->texinfo);
				fx = R_TextureAnimationFx(surf->texinfo); // fix glow hack
				surf->texturechain = image->texturechain;
				image->texturechain = surf;

			}
		
		}
	}

	// recurse down the back side
	R_RecursiveWorldNode(node->children[!side]);
}

/*
=============
r_drawWorld
=============
*/
void R_DrawBSP(void)
{
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
	currententity = &ent;

	gl_state.currenttextures[0] = gl_state.currenttextures[1] = -1;

	qglColor3f(1, 1, 1);
	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));

	R_InitSun();

	R_ClearSkyBox();
	
	//diffuse
	GL_SelectTexture(GL_TEXTURE0_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, sizeof(wTexArray[0]), wTexArray);
	
	//lighmap
	GL_SelectTexture(GL_TEXTURE1_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, sizeof(wLMArray[0]), wLMArray);
	qglEnable(GL_TEXTURE_2D);
	
	//addative map
	GL_SelectTexture(GL_TEXTURE2_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(3, GL_FLOAT, sizeof(wTexArray[0]), wTexArray);
	qglEnable(GL_TEXTURE_2D);
	
	//caustics map
	GL_SelectTexture(GL_TEXTURE3_ARB);
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, sizeof(wTexArray[0]), wTexArray);
	qglEnable(GL_TEXTURE_2D);
	
	//normal
	qglEnableClientState(GL_NORMAL_ARRAY);
	qglNormalPointer(GL_FLOAT, sizeof(nTexArray[0]), nTexArray);

	qglEnableClientState(GL_VERTEX_ARRAY);
	qglVertexPointer(3, GL_FLOAT, sizeof(wVertexArray[0]), wVertexArray);

	qglEnableVertexAttribArray(10);
	qglEnableVertexAttribArray(11);
	
	// tangent & binormal
	qglVertexAttribPointer(10, 3, GL_FLOAT, false, sizeof(tTexArray[0]), tTexArray);
	qglVertexAttribPointer(11, 3, GL_FLOAT, false, sizeof(bTexArray[0]), bTexArray);
	
	num_scene_surfaces = 0;
	R_RecursiveWorldNode(r_worldmodel->nodes);
	GL_BatchLightmappedPoly(false, false);

//	GL_MsgGLError("after draw: ");

	GL_SelectTexture(GL_TEXTURE3_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisable(GL_TEXTURE_2D);
	
	GL_SelectTexture(GL_TEXTURE2_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisable(GL_TEXTURE_2D);
	
	GL_SelectTexture(GL_TEXTURE1_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisable(GL_TEXTURE_2D);
	
	GL_SelectTexture(GL_TEXTURE0_ARB);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	qglDisableClientState(GL_VERTEX_ARRAY);
	qglEnableClientState(GL_NORMAL_ARRAY);	

	qglDisableVertexAttribArray(10);
	qglDisableVertexAttribArray(11);

	DrawTextureChains();
	
	qglDepthMask(0);
	R_DrawSkyBox();
	qglDepthMask(1);
			
}



/*
===============
R_MarkLeaves

Mark the leaves and nodes that are in the PVS for the current
cluster
===============
*/
void R_MarkLeaves(void)
{
	byte *vis;
	byte fatvis[MAX_MAP_LEAFS / 8];
	mnode_t *node;
	int i, c;
	mleaf_t *leaf;
	int cluster;

	if (r_oldviewcluster == r_viewcluster
		&& r_oldviewcluster2 == r_viewcluster2 && !r_noVis->value
		&& r_viewcluster != -1)
		return;

	// development aid to let you run around and see exactly where
	// the pvs ends
	if (r_lockPvs->value)
		return;

	r_visframecount++;
	r_oldviewcluster = r_viewcluster;
	r_oldviewcluster2 = r_viewcluster2;

	if (r_noVis->value || r_viewcluster == -1 || !r_worldmodel->vis) {
		// mark everything
		for (i = 0; i < r_worldmodel->numleafs; i++)
			r_worldmodel->leafs[i].visframe = r_visframecount;
		for (i = 0; i < r_worldmodel->numnodes; i++)
			r_worldmodel->nodes[i].visframe = r_visframecount;
		memset(&viewvis, 0xff, (r_worldmodel->numleafs+7)>>3);
		return;
	}

	vis = Mod_ClusterPVS(r_viewcluster, r_worldmodel);
	// may have to combine two clusters because of solid water boundaries
	if (r_viewcluster2 != r_viewcluster) {
		Q_memcpy(fatvis, vis,
				 (int) ((r_worldmodel->numleafs + 7) * 0.125));
		vis = Mod_ClusterPVS(r_viewcluster2, r_worldmodel);
		c = (r_worldmodel->numleafs + 31) * 0.03125;
		for (i = 0; i < c; i++)
			((int *) fatvis)[i] |= ((int *) vis)[i];
		vis = fatvis;
	}

	memset(&viewvis, 0xff, (r_worldmodel->numleafs+7)>>3);

	for (i = 0, leaf = r_worldmodel->leafs; i < r_worldmodel->numleafs;
		 i++, leaf++) {
		cluster = leaf->cluster;
		if (cluster == -1)
			continue;
		if (vis[cluster >> 3] & (1 << (cluster & 7))) {
			node = (mnode_t *) leaf;
			do {
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}


/*
=============================================================================
LIGHTMAP ALLOCATION
=============================================================================
*/

_inline static void LM_InitBlock(void)
{
	memset(gl_lms.allocated, 0, sizeof(gl_lms.allocated));
}

static void LM_UploadBlock(qboolean dynamic)
{
	int texture;
	int height = 0;

	if (dynamic) {
		texture = 0;
	} else {
		texture = gl_lms.current_lightmap_texture;
	}

	GL_Bind(gl_state.lightmap_textures + texture);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (dynamic) {
		int i;

		for (i = 0; i < LIGHTMAP_SIZE; i++) {
			if (gl_lms.allocated[i] > height)
				height = gl_lms.allocated[i];
		}

		qglTexSubImage2D(GL_TEXTURE_2D,
						 0,
						 0, 0,
						 LIGHTMAP_SIZE, height,
						 GL_LIGHTMAP_FORMAT,
						 GL_UNSIGNED_INT_8_8_8_8_REV, gl_lms.lightmap_buffer);
	} else {
		qglTexImage2D(GL_TEXTURE_2D,
					  0,
					  gl_lms.internal_format,
					  LIGHTMAP_SIZE, LIGHTMAP_SIZE,
					  0,
					  GL_LIGHTMAP_FORMAT,
					  GL_UNSIGNED_INT_8_8_8_8_REV, gl_lms.lightmap_buffer);
		if (++gl_lms.current_lightmap_texture == MAX_LIGHTMAPS)
			VID_Error(ERR_DROP,
					  "LM_UploadBlock() - MAX_LIGHTMAPS exceeded\n");
	}
}

// returns a texture number and the position inside it
static qboolean LM_AllocBlock(int w, int h, int *x, int *y)
{
	int i, j;
	int best, best2;

	best = LIGHTMAP_SIZE;

	for (i = 0; i < LIGHTMAP_SIZE - w; i++) {
		best2 = 0;

		for (j = 0; j < w; j++) {
			if (gl_lms.allocated[i + j] >= best)
				break;
			if (gl_lms.allocated[i + j] > best2)
				best2 = gl_lms.allocated[i + j];
		}
		if (j == w) {			// this is a valid spot
			*x = i;
			*y = best = best2;
		}
	}

	if (best + h > LIGHTMAP_SIZE)
		return false;

	for (i = 0; i < w; i++)
		gl_lms.allocated[*x + i] = best + h;

	return true;
}

/*
================
GL_BuildPolygonFromSurface
================
*/
void GL_BuildPolygonFromSurface(msurface_t * fa)
{
	int i, lindex, lnumverts;
	medge_t *pedges, *r_pedge;
	int vertpage;
	float *vec;
	float s, t;
	glpoly_t *poly;
	vec3_t total;
	
	fa->numVertices = fa->numedges;
    fa->numIndices = (fa->numVertices - 2) * 3;

	// reconstruct the polygon
	pedges = currentmodel->edges;
	lnumverts = fa->numedges;
	vertpage = 0;

	VectorClear(total);
	//
	// draw texture
	//
	poly = (glpoly_t*)Hunk_Alloc(sizeof(glpoly_t) + (lnumverts - 4) * VERTEXSIZE * sizeof(float));
	poly->next = fa->polys;
	poly->flags = fa->flags;
	fa->polys = poly;
	poly->numverts = lnumverts;

	currentmodel->memorySize += sizeof(glpoly_t) + (lnumverts - 4) * VERTEXSIZE * sizeof(float);
	
	for (i = 0; i < lnumverts; i++) {
		lindex = currentmodel->surfedges[fa->firstedge + i];

		if (lindex > 0) {
			r_pedge = &pedges[lindex];
			vec = currentmodel->vertexes[r_pedge->v[0]].position;
		} else {
			r_pedge = &pedges[-lindex];
			vec = currentmodel->vertexes[r_pedge->v[1]].position;
		}
		s = DotProduct(vec,
					   fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s /= fa->texinfo->image->width;

		t = DotProduct(vec,
					   fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t /= fa->texinfo->image->height;

		VectorAdd(total, vec, total);
		VectorCopy(vec, poly->verts[i]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;

		//
		// lightmap texture coordinates
		//
		  s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
          s -= fa->texturemins[0];
          s += fa->light_s * loadmodel->lightmap_scale;
          s += loadmodel->lightmap_scale / 2;
          s /= LIGHTMAP_SIZE * loadmodel->lightmap_scale;

          t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
          t -= fa->texturemins[1];
          t += fa->light_t * loadmodel->lightmap_scale;
          t += loadmodel->lightmap_scale / 2;
          t /= LIGHTMAP_SIZE * loadmodel->lightmap_scale;

		poly->verts[i][5] = s;
		poly->verts[i][6] = t;

	}

	poly->numverts = lnumverts;

	VectorScale(total, 1.0f / (float) lnumverts, total);

	fa->c_s =
		(DotProduct(total, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3])
		/ fa->texinfo->image->width;
	fa->c_t =
		(DotProduct(total, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3])
		/ fa->texinfo->image->height;
}

/*
========================
GL_CreateSurfaceLightmap
========================
*/
void GL_CreateSurfaceLightmap(msurface_t * surf)
{
	int smax, tmax;
	byte *base;

	if (surf->flags & (SURF_DRAWSKY | SURF_DRAWTURB))
		return;

	smax = (surf->extents[0] / loadmodel->lightmap_scale) + 1; 
	tmax = (surf->extents[1] / loadmodel->lightmap_scale) + 1;
	
	if (!LM_AllocBlock(smax, tmax, &surf->light_s, &surf->light_t)) {
		LM_UploadBlock(false);
		LM_InitBlock();
		if (!LM_AllocBlock(smax, tmax, &surf->light_s, &surf->light_t)) {
			VID_Error(ERR_FATAL,
					  "Consecutive calls to LM_AllocBlock(%d,%d) failed\n",
					  smax, tmax);
		}
	}

	surf->lightmaptexturenum = gl_lms.current_lightmap_texture;

	base = gl_lms.lightmap_buffer;
	base += (surf->light_t * LIGHTMAP_SIZE + surf->light_s) * LIGHTMAP_BYTES;

	R_SetCacheState(surf);
	R_BuildLightMap(surf, base, LIGHTMAP_SIZE * LIGHTMAP_BYTES, true);
}


/*
==================
GL_BeginBuildingLightmaps
==================
*/
void GL_BeginBuildingLightmaps(model_t * m)
{
	static lightstyle_t lightstyles[MAX_LIGHTSTYLES];
	int i;
	byte *dummy;

	memset(gl_lms.allocated, 0, sizeof(gl_lms.allocated));
	
	dummy = (byte*)Z_Malloc(LIGHTMAP_BYTES * LIGHTMAP_SIZE * LIGHTMAP_SIZE);

	r_framecount = 1;			// no dlightcache

	GL_SelectTexture( GL_TEXTURE1 );
	qglEnable( GL_TEXTURE_2D );
	GL_TexEnv( GL_REPLACE );

	/*
	 ** setup the base lightstyles so the lightmaps won't have to be regenerated
	 ** the first time they're seen
	 */
	for (i = 0; i < MAX_LIGHTSTYLES; i++) {
		lightstyles[i].rgb[0] = 1;
		lightstyles[i].rgb[1] = 1;
		lightstyles[i].rgb[2] = 1;
		lightstyles[i].white = 3;
	}
	r_newrefdef.lightstyles = lightstyles;

	if (!gl_state.lightmap_textures)
		gl_state.lightmap_textures = TEXNUM_LIGHTMAPS;


	gl_lms.current_lightmap_texture = 1;
	gl_lms.internal_format = gl_tex_solid_format;

	/*
	 ** initialize the dynamic lightmap texture
	 */
	GL_Bind(gl_state.lightmap_textures + 0);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	qglTexImage2D(GL_TEXTURE_2D,
				  0,
				  gl_lms.internal_format,
				  LIGHTMAP_SIZE, LIGHTMAP_SIZE,
				  0, GL_LIGHTMAP_FORMAT, GL_UNSIGNED_INT_8_8_8_8_REV, dummy);
	
	Z_Free(dummy);
}

/*
=======================
GL_EndBuildingLightmaps
=======================
*/
void GL_EndBuildingLightmaps(void)
{
	LM_UploadBlock(false);

	GL_SelectTexture( GL_TEXTURE1 );
	qglDisable( GL_TEXTURE_2D );
	GL_TexEnv( GL_REPLACE );
	GL_SelectTexture( GL_TEXTURE0 );
	GL_TexEnv( GL_REPLACE );
}


/*=====================
GLOOM MINI MAP !!!
======================*/

//sul's minimap thing
void R_RecursiveRadarNode(mnode_t * node)
{
	int c, side, sidebit;
	cplane_t *plane;
	msurface_t *surf, **mark;
	mleaf_t *pleaf;
	float dot, distance;
	glpoly_t *p;
	float *v;
	int i;

	if (node->contents == CONTENTS_SOLID)
		return;					// solid

	if (r_radarZoom->value >= 0.1) {
		distance = 1024.0 / r_radarZoom->value;
	} else {
		distance = 1024.0;
	}

	if (r_origin[0] + distance < node->minmaxs[0] ||
		r_origin[0] - distance > node->minmaxs[3] ||
		r_origin[1] + distance < node->minmaxs[1] ||
		r_origin[1] - distance > node->minmaxs[4] ||
		r_origin[2] + 256 < node->minmaxs[2] ||
		r_origin[2] - 256 > node->minmaxs[5])
		return;

	// if a leaf node, draw stuff
	if (node->contents != -1) {
		pleaf = (mleaf_t *) node;
		// check for door connected areas
		if (r_newrefdef.areabits) {
			// not visible
			if (!
				(r_newrefdef.
				 areabits[pleaf->area >> 3] & (1 << (pleaf->area & 7))))
				return;
		}
		mark = pleaf->firstmarksurface;
		c = pleaf->nummarksurfaces;

		if (c) {
			do {
				(*mark)->visframe = r_framecount;
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
	} else {
		side = 1;
		sidebit = SURF_PLANEBACK;
	}

	// recurse down the children, front side first
	R_RecursiveRadarNode(node->children[side]);

	if (plane->normal[2]) {
		// draw stuff
		if (plane->normal[2] > 0)
			for (c = node->numsurfaces, surf =
				 r_worldmodel->surfaces + node->firstsurface; c;
				 c--, surf++) {
				if (surf->texinfo->flags & SURF_SKY) {
					continue;
				}
                        
				if (surf->texinfo->flags & (SURF_TRANS33|SURF_TRANS66)&& surf->texinfo->flags & !(SURF_WARP|SURF_FLOWING)) 
				{
					qglColor4f(0,1,0,0.7);
				} else 
				if (surf->texinfo->flags & (SURF_WARP|SURF_FLOWING)) 
				{
				qglColor4f(0,0,1,0.7);
				} 
				else 
				{
				qglColor4f(0.7,0.7,0.7,0.7);
				}

			for ( p = surf->polys; p; p = p->chain ) {
				v = p->verts[0];
				qglBegin (GL_TRIANGLE_FAN);
				for (i=0 ; i< p->numverts; i++, v+= VERTEXSIZE) {
					qglVertex3fv (v);
				}
				qglEnd ();
			}
			}
	}
	// recurse down the back side
	R_RecursiveRadarNode(node->children[!side]);
}



int numRadarEnts = 0;
RadarEnt_t RadarEnts[MAX_RADAR_ENTS];

void GL_DrawRadar(void)
{
	int i;
	float fS[4] = { 0, 0, -1.0 / 512.0, 0 };

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;
	if (!r_radar->value)
		return;

	qglViewport(vid.width - r_radarSize->value, 0, r_radarSize->value, r_radarSize->value);

	qglDisable(GL_DEPTH_TEST);
	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();

	qglOrtho(-1024, 1024, -512, 1536, -256, 256);
	

	qglMatrixMode(GL_MODELVIEW);
	qglPushMatrix();
	qglLoadIdentity();

	{
		qglStencilMask(255);
		qglClear(GL_STENCIL_BUFFER_BIT);
		qglEnable(GL_STENCIL_TEST);
		qglStencilFunc(GL_ALWAYS, 4, 4);
		qglStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


		qglEnable(GL_ALPHA_TEST);
		qglAlphaFunc(GL_LESS, 0.1);
		qglColorMask(0, 0, 0, 0);
                
		
		GL_Bind(r_around->texnum);
		
        qglBegin(GL_TRIANGLE_FAN);
	        
        qglTexCoord2f(0, 1);
		qglVertex3f(1024, -512, 1);
		qglTexCoord2f(1, 1);
		qglVertex3f(-1024, -512, 1);
		qglTexCoord2f(1, 0);
		qglVertex3f(-1024, 1536, 1);
		qglTexCoord2f(0, 0);
		qglVertex3f(1024, 1536, 1);
		
		qglEnd();

		qglColorMask(1, 1, 1, 1);
		qglDisable(GL_ALPHA_TEST);
		qglAlphaFunc(GL_GREATER, 0.5);
		qglStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		qglStencilFunc(GL_NOTEQUAL, 4, 4);

	}

	if (r_radarZoom->value >= 0.1) {
		qglScalef(r_radarZoom->value, r_radarZoom->value,
				  r_radarZoom->value);
	}
	//draw player origin
        qglDisable(GL_TEXTURE_2D);
		qglBegin(GL_TRIANGLES);
		qglColor4f(1, 1, 0, 1);
		qglVertex3f(0, 32, 0);
		qglColor4f(1, 1, 0, 1);
		qglVertex3f(24, -32, 0);
		qglColor4f(1, 1, 0, 1);
        qglVertex3f(-24,-32, 0);
		qglEnd();

	qglRotatef(90 - r_newrefdef.viewangles[1], 0, 0, 1);
	
	qglTranslatef(-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1],
				  -r_newrefdef.vieworg[2]);

	if (!deathmatch->value) {
		qglBegin(GL_QUADS);
		for (i = 0; i < numRadarEnts; i++) {
			float x = RadarEnts[i].org[0];
			float y = RadarEnts[i].org[1];
			float z = RadarEnts[i].org[2];
			qglColor3fv(RadarEnts[i].color);

			qglVertex3f(x + 9, y + 9, z);
			qglVertex3f(x + 9, y - 9, z);
			qglVertex3f(x - 9, y - 9, z);
			qglVertex3f(x - 9, y + 9, z);
		}
		qglEnd();
		qglColor4f(1, 1, 1, 1);
	}

	qglEnable(GL_TEXTURE_2D);

	GL_Bind(r_radarmap->texnum);
	qglBlendFunc(GL_SRC_ALPHA,GL_ONE);
	qglEnable(GL_BLEND);
	qglColor4f(1, 1, 0, 1);

	fS[3] = 0.5 + r_newrefdef.vieworg[2] / 512.0;
	qglTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	GLSTATE_ENABLE_TEXGEN;
	qglTexGenfv(GL_S, GL_OBJECT_PLANE, fS);

	// draw the stuff
	R_RecursiveRadarNode(r_worldmodel->nodes);

	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLSTATE_DISABLE_TEXGEN;
                     
        qglPopMatrix();
        
	qglViewport(0, 0, vid.width, vid.height);

	qglMatrixMode(GL_PROJECTION);
	qglPopMatrix();
	qglMatrixMode(GL_MODELVIEW);
	qglDisable(GL_STENCIL_TEST);
	qglStencilMask(0);
	GL_TexEnv(GL_REPLACE);
	qglDisable(GL_BLEND);
	qglEnable(GL_DEPTH_TEST);
	qglColor4f(1, 1, 1, 1);
}
