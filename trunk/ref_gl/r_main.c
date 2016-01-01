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
// r_main.c
#include "r_local.h"

#ifndef _WIN32
#include <dlfcn.h>
#define qwglGetProcAddress( a ) dlsym( glw_state.hinstOpenGL, a )
#endif

viddef_t vid;

model_t *r_worldmodel;

float gldepthmin, gldepthmax;

glconfig_t gl_config;
glstate_t gl_state;
entity_t *currententity;
model_t *currentmodel;

cplane_t frustum[6];

int r_visframecount;			// bumped when going to a new PVS
int r_framecount;				// used for dlight push checking

float v_blend[4];				// final blending color

void GL_Strings_f(void);

//
// view origin
//
vec3_t vup;
vec3_t vpn;
vec3_t vright;
vec3_t r_origin;

//
// screen size info
//
refdef_t r_newrefdef;

glwstate_t glw_state;

int r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;
int	occ_framecount;

/*=================
GL_ARB_Debug_output
From https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
=================*/

void DebugOutput(unsigned source, unsigned type, unsigned id, unsigned severity, const char* message)
{
	char debSource[64], debType[64], debSev[64];

	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	if (source == GL_DEBUG_SOURCE_API_ARB)
		strcpy(debSource, "OpenGL");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		strcpy(debSource, "Windows");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		strcpy(debSource, "Shader Compiler");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		strcpy(debSource, "Third Party");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		strcpy(debSource, "Application");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
		strcpy(debSource, "Other");
	else
		strcpy(debSource, va("Source 0x%X", source));

	if (type == GL_DEBUG_TYPE_ERROR_ARB)
		strcpy(debType, "Error");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
		strcpy(debType, "Deprecated behavior");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
		strcpy(debType, "Undefined behavior");
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
		strcpy(debType, "Portability");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
		strcpy(debType, "Performance");
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)
		strcpy(debType, "Other");
	else
		strcpy(debType, va("Type 0x%X", type));

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		strcpy(debSev, "High");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		strcpy(debSev, "Medium");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
		strcpy(debSev, "Low");
	else
		strcpy(debSev, va("0x%X", severity));

	Com_Printf("GL_DEBUG: %s %s\nSeverity '%s': '%s'\nID: '%d'\n", debSource, debType, debSev, message, id);
}

void GL_CheckError(const char *fileName, int line, const char *subr)
{
	int         err;
	char        s[128];

	if (r_ignoreGlErrors->value)
		return;

	err = qglGetError();
	if (err == GL_NO_ERROR)
		return;

	if (gl_state.debugOutput){

		#define			MAX_GL_DEBUG_MESSAGES   16
		unsigned        sources[MAX_GL_DEBUG_MESSAGES];
		unsigned        types[MAX_GL_DEBUG_MESSAGES];
		unsigned        ids[MAX_GL_DEBUG_MESSAGES];
		unsigned        severities[MAX_GL_DEBUG_MESSAGES];
		int             lengths[MAX_GL_DEBUG_MESSAGES];
		char            messageLog[2048];
		unsigned        count = MAX_GL_DEBUG_MESSAGES;
		int             bufsize = 2048;

		unsigned retVal = glGetDebugMessageLogARB(count, bufsize, sources, types, ids, severities, lengths, messageLog);
		if (retVal > 0)
		{
			unsigned pos = 0;
			for (unsigned i = 0; i < retVal; i++)
			{
				DebugOutput(sources[i], types[i], ids[i], severities[i], &messageLog[pos]);
				pos += lengths[i];
			}
		}
	}

	switch (err)
	{
	case GL_INVALID_ENUM:
		strcpy(s, "GL_INVALID_ENUM");
		break;
	case GL_INVALID_VALUE:
		strcpy(s, "GL_INVALID_VALUE");
		break;
	case GL_INVALID_OPERATION:
		strcpy(s, "GL_INVALID_OPERATION");
		break;
	case GL_STACK_OVERFLOW:
		strcpy(s, "GL_STACK_OVERFLOW");
		break;
	case GL_STACK_UNDERFLOW:
		strcpy(s, "GL_STACK_UNDERFLOW");
		break;
	case GL_OUT_OF_MEMORY:
		strcpy(s, "GL_OUT_OF_MEMORY");
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
		strcpy(s, "GL_INVALID_FRAMEBUFFER_OPERATION_EXT");
		break;
	default:
		Com_sprintf(s, sizeof(s), "0x%X", err);
		break;
	}

	Com_Printf("GL_CheckErrors: %s in file '%s' subroutine '%s' line %i\n", s, fileName, subr, line);
}



/*
=============================================================

  SPRITE MODELS

=============================================================
*/


void R_DrawSpriteModel(entity_t * e)
{
	return;
}


static void R_DrawDistortSpriteModel(entity_t * e)
{
	dsprframe_t *frame;
	float		*up, *right;
	dsprite_t	*psprite;
	int			vert=0;
	vec3_t		dist;
	float		len;
	
	psprite = (dsprite_t *) currentmodel->extraData;
	e->frame %= psprite->numFrames;
	frame = &psprite->frames[e->frame];

	VectorSubtract(e->origin, r_origin, dist);
	len = VectorLength(dist);

	// normal sprite
	up = vup;
	right = vright;

	// setup program
	GL_BindProgram(refractProgram, 0);

	GL_MBind(GL_TEXTURE0_ARB, r_distort->texnum);
	qglUniform1i(refract_normalMap, 0);
	GL_MBind(GL_TEXTURE1_ARB, currentmodel->skins[e->frame]->texnum);
	qglUniform1i(refract_baseMap, 1);
	GL_MBindRect(GL_TEXTURE2_ARB, ScreenMap->texnum);
	qglUniform1i(refract_screenMap, 2);
	GL_MBindRect(GL_TEXTURE3_ARB, depthMap->texnum);
	qglUniform1i(refract_depthMap, 3);

	qglUniform1f(refract_deformMul, 9.5);
	qglUniform1f(refract_alpha, e->alpha);
	qglUniform1f(refract_thickness, len * 0.5);
	if (currententity->flags & RF_BFG_SPRITE)
		qglUniform1f(refract_thickness2, 3.0);
	else
		qglUniform1f(refract_thickness2, len * 0.5);

	qglUniform2f(refract_screenSize, vid.width, vid.height);
	qglUniform2f(refract_depthParams, r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
	qglUniform1i(refract_alphaMask, 1);
	qglUniform2f(refract_mask, 0.0, 1.0);
	
	qglUniformMatrix4fv(refract_mvp, 1, qfalse, (const float *)r_newrefdef.modelViewProjectionMatrix);
	qglUniformMatrix4fv(refract_mv, 1, qfalse, (const float *)r_newrefdef.modelViewMatrix);
	qglUniformMatrix4fv(refract_pm, 1, qfalse, (const float *)r_newrefdef.projectionMatrix);

	VectorMA (e->origin, -frame->origin_y, up, wVertexArray[vert+0]);
	VectorMA (wVertexArray[vert+0], -frame->origin_x, right, wVertexArray[vert+0]);
	VA_SetElem2(wTexArray[vert+0], 0, 1);
	
	VectorMA (e->origin, frame->height - frame->origin_y, up, wVertexArray[vert+1]);
	VectorMA (wVertexArray[vert+1], -frame->origin_x, right, wVertexArray[vert+1]);
    VA_SetElem2(wTexArray[vert+1], 0, 0);

	VectorMA (e->origin, frame->height - frame->origin_y, up, wVertexArray[vert+2]);
	VectorMA (wVertexArray[vert+2], frame->width - frame->origin_x, right, wVertexArray[vert+2]);
    VA_SetElem2(wTexArray[vert+2], 1, 0);

	VectorMA (e->origin, -frame->origin_y, up, wVertexArray[vert+3]);
	VectorMA (wVertexArray[vert+3],frame->width - frame->origin_x, right, wVertexArray[vert+3]);
    VA_SetElem2(wTexArray[vert+3], 1, 1);

	vert+=4;
	
	if(vert)
		qglDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

	GL_BindNullProgram();
}

//==================================================================================

/*
=============
R_DrawNullModel
=============
*/
void R_DrawNullModel(void)
{
	return;
}

//=======================================================================

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame(void)
{
	int i;
	mleaf_t *leaf;

	r_framecount++;
	occ_framecount++;

	// build the transformation matrix for the given view angles
	VectorCopy(r_newrefdef.vieworg, r_origin);

	AngleVectors(r_newrefdef.viewangles, vpn, vright, vup);
	
	// current viewcluster
	if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL)) {
		r_oldviewcluster = r_viewcluster;
		r_oldviewcluster2 = r_viewcluster2;
		leaf = Mod_PointInLeaf(r_origin, r_worldmodel);
		r_viewcluster = r_viewcluster2 = leaf->cluster;
		
		// check above and below so crossing solid water doesn't draw
		// wrong
		if (!leaf->contents) {	// look down a bit
			vec3_t temp;

			VectorCopy(r_origin, temp);
			temp[2] -= 16;
			leaf = Mod_PointInLeaf(temp, r_worldmodel);
			if (!(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2))
				r_viewcluster2 = leaf->cluster;
		} else {				// look up a bit
			vec3_t temp;

			VectorCopy(r_origin, temp);
			temp[2] += 16;
			leaf = Mod_PointInLeaf(temp, r_worldmodel);
			if (!(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2))
				r_viewcluster2 = leaf->cluster;
		}
	}

	if(CL_PMpointcontents(r_origin) & CONTENTS_SOLID)
		outMap = qtrue;
	else
		outMap = qfalse;

	for (i = 0; i < 4; i++)
		v_blend[i] = r_newrefdef.blend[i];

	// clear out the portion of the screen that the NOWORLDMODEL defines
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL) {
		GL_Enable(GL_SCISSOR_TEST);
		GL_Scissor(r_newrefdef.viewport[0], r_newrefdef.viewport[1], r_newrefdef.viewport[2], r_newrefdef.viewport[3]);

		if (!(r_newrefdef.rdflags & RDF_NOCLEAR)) {
			qglClearColor(0.0, 0.0, 0.0, 0.0);
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		} else
			qglClear(GL_DEPTH_BUFFER_BIT);
		GL_Disable(GL_SCISSOR_TEST);
	}

	
}

/*
=============
R_SetupViewMatrices

=============
*/

// convert from Q2 coordinate system (screen depth is X)
// to OpenGL's coordinate system (screen depth is -Z)
static mat4_t r_flipMatrix = {
	{  0.f, 0.f, -1.f, 0.f },	// world +X -> screen -Z
	{ -1.f, 0.f,  0.f, 0.f },	// world +Y -> screen -X
	{  0.f, 1.f,  0.f, 0.f },	// world +Z -> screen +Y (Y=0 being the bottom of the screen)
	{  0.f, 0.f,  0.f, 1.f }
};

static void R_SetupViewMatrices (void) {
	mat4_t tmpMatrix;

	// setup perspective projection matrix
	r_newrefdef.projectionMatrix[0][0] = 1.f / tan(DEG2RAD(r_newrefdef.fov_x) * 0.5f);
	r_newrefdef.projectionMatrix[0][1] = 0.f;
	r_newrefdef.projectionMatrix[0][2] = 0.f;
	r_newrefdef.projectionMatrix[0][3] = 0.f;

	r_newrefdef.projectionMatrix[1][0] = 0.f;
	r_newrefdef.projectionMatrix[1][1] = 1.f / tan(DEG2RAD(r_newrefdef.fov_y) * 0.5f);
	r_newrefdef.projectionMatrix[1][2] = 0.f;
	r_newrefdef.projectionMatrix[1][3] = 0.f;

	r_newrefdef.projectionMatrix[2][0] = 0.f;
	r_newrefdef.projectionMatrix[2][1] = 0.f;
	r_newrefdef.projectionMatrix[2][2] = -0.999f; // infinite
	r_newrefdef.projectionMatrix[2][3] = -1.f;

	r_newrefdef.projectionMatrix[3][0] = 0.f;
	r_newrefdef.projectionMatrix[3][1] = 0.f;
	r_newrefdef.projectionMatrix[3][2] = -2.f * r_zNear->value; // infinite
	r_newrefdef.projectionMatrix[3][3] = 0.f;

	r_newrefdef.depthParms[0] = r_zNear->value;
	r_newrefdef.depthParms[1] = 0.9995f;

	// setup view matrix
	AnglesToMat3(r_newrefdef.viewangles, r_newrefdef.axis);
	Mat4_SetupTransform(r_newrefdef.modelViewMatrix, r_newrefdef.axis, r_newrefdef.vieworg);
	Mat4_AffineInvert(r_newrefdef.modelViewMatrix, tmpMatrix);
	Mat4_Multiply(tmpMatrix, r_flipMatrix, r_newrefdef.modelViewMatrix);

	// scissors transform
	Mat4_Multiply	(r_newrefdef.modelViewMatrix, r_newrefdef.projectionMatrix, r_newrefdef.modelViewProjectionMatrix);
	Mat4_Transpose	(r_newrefdef.modelViewProjectionMatrix, r_newrefdef.modelViewProjectionMatrixTranspose);

	// set sky matrix
	Mat4_Copy(r_newrefdef.modelViewMatrix, tmpMatrix);
	Mat4_Translate(tmpMatrix, r_newrefdef.vieworg[0], r_newrefdef.vieworg[1], r_newrefdef.vieworg[2]);
	if (skyrotate)
		Mat4_Rotate(tmpMatrix, r_newrefdef.time * skyrotate, skyaxis[0], skyaxis[1], skyaxis[2]);

	Mat4_Multiply(tmpMatrix, r_newrefdef.projectionMatrix, r_newrefdef.skyMatrix);

}

void R_SetupEntityMatrix(entity_t * e) {

	AnglesToMat3(e->angles, e->axis);
	Mat4_SetOrientation(e->matrix, e->axis, e->origin);
	Mat4_TransposeMultiply(e->matrix, r_newrefdef.modelViewProjectionMatrix, e->orMatrix);

}

void R_SetupOrthoMatrix(void) {
	// set 2D virtual screen size
	qglViewport(0, 0, vid.width, vid.height);

	// setup orthographic projection
	r_newrefdef.orthoMatrix[0][0] = 2.f / (float)vid.width;
	r_newrefdef.orthoMatrix[0][1] = 0.f;
	r_newrefdef.orthoMatrix[0][2] = 0.f;
	r_newrefdef.orthoMatrix[0][3] = 0.f;
	r_newrefdef.orthoMatrix[1][0] = 0.f;
	r_newrefdef.orthoMatrix[1][1] = -2.f / (float)vid.height;
	r_newrefdef.orthoMatrix[1][2] = 0.f;
	r_newrefdef.orthoMatrix[1][3] = 0.f;
	r_newrefdef.orthoMatrix[2][0] = 0.f;
	r_newrefdef.orthoMatrix[2][1] = 0.f;
	r_newrefdef.orthoMatrix[2][2] = -1.f;
	r_newrefdef.orthoMatrix[2][3] = 0.f;
	r_newrefdef.orthoMatrix[3][0] = -1.f;
	r_newrefdef.orthoMatrix[3][1] = 1.f;
	r_newrefdef.orthoMatrix[3][2] = 0.f;
	r_newrefdef.orthoMatrix[3][3] = 1.f;

	GL_Disable(GL_DEPTH_TEST);
	GL_Disable(GL_CULL_FACE);
}

/*
=============
R_SetupGL
=============
*/

void R_SetupGL(void)
{
	// set drawing parms
	GL_CullFace(GL_FRONT);
	GL_Enable(GL_CULL_FACE);
	GL_Disable(GL_BLEND);
	GL_Enable(GL_DEPTH_TEST);
	GL_DepthMask(1);
}

/*
=============
R_Clear
=============
*/

void R_Clear(void)
{
	qglClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	gldepthmin = 0.0;
	gldepthmax = 1.0;
	GL_DepthFunc(GL_LEQUAL);
	GL_DepthRange(gldepthmin, gldepthmax);
}

void R_DrawPlayerWeaponFBO(void)
{
	int i;

	if (!r_drawEntities->value)
		return;

	qglBindFramebuffer(GL_FRAMEBUFFER, fbo_weaponMask);
	qglClear(GL_COLOR_BUFFER_BIT);
	qglClearColor(0.0, 0.0, 0.0, 1.0);
	qglDrawBuffer(GL_COLOR_ATTACHMENT0);

	for (i = 0; i < r_newrefdef.num_entities; i++)	// weapon model
	{
		currententity = &r_newrefdef.entities[i];
		currentmodel = currententity->model;

		if (!currentmodel)
			continue;

		if (currentmodel->type != mod_alias)
			continue;

		if (!(currententity->flags & RF_WEAPONMODEL))
			continue;

		R_DrawAliasModel(currententity, qtrue);
	}

	qglBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void R_DrawPlayerWeaponLightPass(void)
{
	int i;

	if (!r_drawEntities->value)
		return;

	GL_DepthFunc(GL_LEQUAL);
	GL_StencilFunc(GL_EQUAL, 128, 255);
	GL_StencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	GL_StencilMask(0);

		for (i = 0; i < r_newrefdef.num_entities; i++)	// weapon model
		{
			currententity = &r_newrefdef.entities[i];
			currentmodel = currententity->model;
			if (currententity->flags & RF_TRANSLUCENT)
				continue;

			if (!currentmodel)
				continue;
			if (currentmodel->type != mod_alias)
				continue;
			if (!(currententity->flags & RF_WEAPONMODEL))
				continue;

			R_DrawAliasModelLightPass(qtrue);
		}

}

void R_DrawPlayerWeaponAmbient(void)
{
	int i;

	if (!r_drawEntities->value)
		return;

	// draw non-transparent first
	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];

		if (currententity->flags & RF_TRANSLUCENT)
			continue;

		currentmodel = currententity->model;

		if (!(currententity->flags & RF_WEAPONMODEL))
			continue;

		if (currentmodel->type == mod_alias)
			R_DrawAliasModel(currententity, qfalse);

	}
	// draw transluscent shells
	GL_DepthMask(0);
	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];

		if (!(currententity->flags & RF_TRANSLUCENT))
			continue;

		currentmodel = currententity->model;

		if (!(currententity->flags & RF_WEAPONMODEL))
			continue;

		if (currentmodel->type == mod_alias)
			R_DrawAliasModel(currententity, qfalse);
	}
	GL_DepthMask(1);
}

void R_DrawLightScene (void)
{
	int i;
	
	num_visLights = 0;

	GL_DepthMask(0);
	GL_Enable(GL_BLEND);
	GL_BlendFunc(GL_ONE, GL_ONE /*GL_DST_COLOR, GL_ZERO*/);

	if(r_useLightScissors->value)
		GL_Enable(GL_SCISSOR_TEST);
	
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable(GL_DEPTH_BOUNDS_TEST_EXT);

	if(r_shadows->value)
		GL_Enable(GL_STENCIL_TEST);
	
	R_PrepareShadowLightFrame(qfalse);
	
	if(shadowLight_frame) {

	for(currentShadowLight = shadowLight_frame; currentShadowLight; currentShadowLight = currentShadowLight->next) {

	if (r_skipStaticLights->value && currentShadowLight->isStatic && currentShadowLight->style == 0)
		continue;
	
	UpdateLightEditor();
	
	R_SetViewLightScreenBounds();

	if(r_useLightScissors->value)
		GL_Scissor(currentShadowLight->scissor[0], currentShadowLight->scissor[1], currentShadowLight->scissor[2], currentShadowLight->scissor[3]);
	
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_DepthBoundsTest(currentShadowLight->depthBounds[0], currentShadowLight->depthBounds[1]);

	qglClearStencil(128);
	GL_StencilMask(255);
	qglClear(GL_STENCIL_BUFFER_BIT);
	
	R_CastBspShadowVolumes();		// bsp and bmodels shadows
	R_CastAliasShadowVolumes();		// alias models shadows
	R_DrawLightWorld();				// light world
	R_DrawLightFlare();				// light flare
	R_DrawLightBounds();			// debug stuff
	
	num_visLights++;

	//entities lightpass w/o player weapon
	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];

		if (currententity->flags & RF_WEAPONMODEL)
			continue;

		if (currententity->flags & RF_TRANSLUCENT)
			continue;			

		if (currententity->flags & RF_DISTORT)
				continue;

		currentmodel = currententity->model;

		if (!currentmodel) {
			R_DrawNullModel();
			continue;
		}
		if (currentmodel->type == mod_brush) 
			R_DrawLightBrushModel();
		if(currentmodel->type == mod_alias)
			R_DrawAliasModelLightPass(qfalse);
		}
	}
	}
	
	GL_DepthMask(1);
	GL_Disable(GL_STENCIL_TEST);
	GL_Disable(GL_SCISSOR_TEST);
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable(GL_DEPTH_BOUNDS_TEST_EXT);
	GL_Disable(GL_BLEND);
}

void R_DrawPlayerWeapon(void)
{
	if (!r_drawEntities->value)
		return;

	R_DrawPlayerWeaponAmbient();

	GL_DepthMask(0);
	GL_Enable(GL_BLEND);
	GL_BlendFunc(GL_ONE, GL_ONE);

	if (r_useLightScissors->value)
		GL_Enable(GL_SCISSOR_TEST);

	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable(GL_DEPTH_BOUNDS_TEST_EXT);

	if (r_shadows->value)
		GL_Enable(GL_STENCIL_TEST);

	R_PrepareShadowLightFrame(qtrue);

	if (shadowLight_frame) {

		for (currentShadowLight = shadowLight_frame; currentShadowLight; currentShadowLight = currentShadowLight->next) {

			if (r_skipStaticLights->value && currentShadowLight->isStatic && currentShadowLight->style == 0)
				continue;

			R_SetViewLightScreenBounds();

			if (r_useLightScissors->value)
				GL_Scissor(currentShadowLight->scissor[0], currentShadowLight->scissor[1], currentShadowLight->scissor[2], currentShadowLight->scissor[3]);

			if (gl_state.depthBoundsTest && r_useDepthBounds->value)
				GL_DepthBoundsTest(currentShadowLight->depthBounds[0], currentShadowLight->depthBounds[1]);

			qglClearStencil(128);
			GL_StencilMask(255);
			qglClear(GL_STENCIL_BUFFER_BIT);
			
			R_CastBspShadowVolumes();
			R_DrawPlayerWeaponLightPass();
		}
	}

	GL_DepthMask(1);
	GL_Disable(GL_STENCIL_TEST);
	GL_Disable(GL_SCISSOR_TEST);
	if (gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable(GL_DEPTH_BOUNDS_TEST_EXT);
	GL_Disable(GL_BLEND);
}

void R_RenderSprites(void)
{
	int i;

	GL_DepthMask(0);
	GL_Enable(GL_BLEND);
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	qglBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo.ibo_quadTris);
	qglEnableVertexAttribArray(ATT_POSITION);
	qglEnableVertexAttribArray(ATT_TEX0);

	qglVertexAttribPointer(ATT_POSITION, 3, GL_FLOAT, qfalse, 0, wVertexArray);
	qglVertexAttribPointer(ATT_TEX0, 2, GL_FLOAT, qfalse, 0, wTexArray);

	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];
		currentmodel = currententity->model;

		if (!currentmodel)
			continue;

		if ( r_newrefdef.rdflags & RDF_IRGOGGLES) 
				continue;
		
		if (currentmodel->type == mod_sprite)
			R_DrawDistortSpriteModel(currententity);
	}


	qglDisableVertexAttribArray(ATT_POSITION);
	qglDisableVertexAttribArray(ATT_TEX0);
	qglBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
	GL_Disable(GL_BLEND);
	GL_DepthMask(1);
}

// draws ambient opaque entities
static void R_DrawEntitiesOnList (void) {
	int i;

	if (!r_drawEntities->value)
		return;

	// draw non-transparent first
	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];

		if (currententity->flags & (RF_TRANSLUCENT | RF_WEAPONMODEL) || ((currententity->flags & RF_DISTORT) && !(r_newrefdef.rdflags & RDF_IRGOGGLES)))
			continue;

		if (currententity->flags & RF_BEAM) {
			R_DrawBeam();
			continue;
		}

		currentmodel = currententity->model;

		if (!currentmodel) {
			R_DrawNullModel();
			continue;
		}

		switch (currentmodel->type) {
			case mod_alias:
				R_DrawAliasModel(currententity, qfalse);
				break;
			case mod_brush:
				R_DrawBrushModel();
				break;
			case mod_sprite:
				R_DrawSpriteModel(currententity);
				break;
			default:
				VID_Error(ERR_DROP, "Bad modeltype");
				break;
		}
	}
}

// draw all opaque, non-reflective stuff
// fills reflective & alpha chains from world model
static void R_DrawAmbientScene (void) {
	R_DrawBSP();
	R_DrawEntitiesOnList();
}

// draws reflective & alpha chains from world model
// draws reflective surfaces from brush models
// draws translucent entities
static void R_DrawRAScene (void) {
	int i;

	R_DrawChainsRA(qfalse);

	if (!r_drawEntities->value)
		return;

	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];
		currentmodel = currententity->model;

		if (currentmodel && currentmodel->type == mod_brush) {
			R_DrawBrushModelRA();
			continue;
		}

		if (!(currententity->flags & RF_TRANSLUCENT))
			continue;
		if ((currententity->flags & RF_WEAPONMODEL) || ((currententity->flags & RF_DISTORT) && !(r_newrefdef.rdflags & RDF_IRGOGGLES)))
			continue;

		if (currententity->flags & RF_BEAM) {
			R_DrawBeam();
			continue;
		}

		if (!currentmodel) {
			R_DrawNullModel();
			continue;
		}

		switch (currentmodel->type) {
			case mod_alias:
				R_DrawAliasModel(currententity, qfalse);
				break;
			case mod_brush:
				break;
			case mod_sprite:
				R_DrawSpriteModel(currententity);
				break;
			default:
				VID_Error(ERR_DROP, "Bad modeltype");
				break;
		}
	}
}

/*
================
R_RenderView

r_newrefdef must be set before the first call.
================
*/

void R_RenderView (refdef_t *fd) {
	if (r_noRefresh->value)
		return;

	r_newrefdef = *fd;
	r_newrefdef.viewport[0] = fd->x;
	r_newrefdef.viewport[1] = vid.height - fd->height - fd->y;
	r_newrefdef.viewport[2] = fd->width;
	r_newrefdef.viewport[3] = fd->height;
	qglViewport(r_newrefdef.viewport[0], r_newrefdef.viewport[1], r_newrefdef.viewport[2], r_newrefdef.viewport[3]);

	if (!r_worldmodel && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		VID_Error(ERR_DROP, "R_RenderView: NULL worldmodel.");

	if (r_finish->value)
		qglFinish();

	R_SetupFrame();
	R_SetFrustum();
	R_SetupViewMatrices();
	R_SetupGL();
	R_MarkLeaves();				// done here so we know if we're in water
	
	R_DrawDepthScene();
	R_CaptureDepthBuffer();

	if (r_ssao->value && !(r_newrefdef.rdflags & (RDF_NOWORLDMODEL | RDF_IRGOGGLES))) {

		R_SetupOrthoMatrix();
		GL_DepthMask(0);
		R_DownsampleDepth();
		R_SSAO();

		GL_Enable(GL_CULL_FACE);
		GL_Enable(GL_DEPTH_TEST);
		GL_DepthMask(1);
		qglViewport(r_newrefdef.viewport[0], r_newrefdef.viewport[1], 
					r_newrefdef.viewport[2], r_newrefdef.viewport[3]);
	}

	R_DrawAmbientScene();
	R_DrawLightScene();

	R_RenderDecals();
	R_RenderFlares();
	
	R_DrawParticles();
	R_CaptureColorBuffer();

	R_DrawRAScene();

	R_DrawPlayerWeapon();
	R_DrawParticles();

	R_CaptureColorBuffer();
	R_RenderSprites();

	R_CaptureColorBuffer();
	R_DrawPlayerWeaponFBO();
	GL_CheckError(__FILE__, __LINE__, "end frame");
}


/*
====================
R_SetLightLevel

====================
*/
void R_SetLightLevel (void) {
	vec3_t shadelight;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	// save off light value for server to look at (BIG HACK!)
	R_LightPoint(r_newrefdef.vieworg, shadelight);

	// pick the greatest component, which should be the same
	// as the mono value returned by software
	if (shadelight[0] > shadelight[1]) {
		if (shadelight[0] > shadelight[2])
			r_lightLevel->value = 150 * shadelight[0];
		else
			r_lightLevel->value = 150 * shadelight[2];
	} else {
		if (shadelight[1] > shadelight[2])
			r_lightLevel->value = 150 * shadelight[1];
		else
			r_lightLevel->value = 150 * shadelight[2];
	}

}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame

@@@@@@@@@@@@@@@@@@@@@
*/
extern char buff0[128];
extern char buff1[128];
extern char buff2[128];
extern char buff3[128];
extern char buff4[128];
extern char buff5[128];
extern char buff6[128];
extern char buff7[128];
extern char buff8[128];
extern char buff9[128];
extern char buff10[128];
extern char buff11[128];
extern char buff12[128];
extern char buff13[128];
extern char buff14[128];
extern char buff15[128];

extern worldShadowLight_t *selectedShadowLight;

void R_MotionBlur (void);
void R_DrawFullScreenQuad (void);

void R_RenderFrame(refdef_t * fd, qboolean client) {
	R_SetLightLevel();
	R_RenderView(fd);
	R_SetupOrthoMatrix();

	// post processing - cut off if player camera is out of map bounds
	if (!outMap) {
		R_FXAA();
		R_RadialBlur();
		R_ThermalVision();
		R_DofBlur();
		R_Bloom();
		R_MotionBlur();
		R_FilmGrain();
	}
	
	// set alpha blend for 2D mode
	GL_Enable(GL_BLEND); 
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (selectedShadowLight && r_lightEditor->value){
		Set_FontShader(qtrue);
		RE_SetColor(colorCyan);
		Draw_StringScaled(0, vid.height*0.5,     2, 2, buff0);
		Draw_StringScaled(0, vid.height*0.5+25,  2, 2, buff1);
		Draw_StringScaled(0, vid.height*0.5+45,  2, 2, buff2);
		Draw_StringScaled(0, vid.height*0.5+65,  2, 2, buff3);
		Draw_StringScaled(0, vid.height*0.5+85,  2, 2, buff4);
		Draw_StringScaled(0, vid.height*0.5+105, 2, 2, buff5);
		Draw_StringScaled(0, vid.height*0.5+125, 2, 2, buff6);
		Draw_StringScaled(0, vid.height*0.5+145, 2, 2, buff7);
		Draw_StringScaled(0, vid.height*0.5+165, 2, 2, buff8);
		Draw_StringScaled(0, vid.height*0.5+185, 2, 2, buff9);
		Draw_StringScaled(0, vid.height*0.5+205, 2, 2, buff12);
		Draw_StringScaled(0, vid.height*0.5+225, 2, 2, buff13);
		Draw_StringScaled(0, vid.height*0.5+245, 2, 2, buff10);
		Draw_StringScaled(0, vid.height*0.5+265, 2, 2, buff11);
		Draw_StringScaled(0, vid.height*0.5+285, 2, 2, buff14);
		Draw_StringScaled(0, vid.height*0.5+305, 2, 2, buff15);
		RE_SetColor(colorWhite);
		Set_FontShader(qfalse);
	}
}

void FlareStatsList_f(void){

	Com_Printf("%i raw flares\n", r_numflares);
	Com_Printf("%i clean flares\n", r_numIgnoreflares);
	Com_Printf("%i total flares\n", r_numflares - r_numIgnoreflares);
}

void Dump_EntityString(void){

	char *buf;
	FILE *f;
	char name[MAX_OSPATH];
	
	if(!r_worldmodel->name){
		Com_Printf(S_COLOR_RED"You must be in a game to dump entity string\n");
	return;
	}

	Com_sprintf(name, sizeof(name), "%s/%s", FS_Gamedir(), r_worldmodel->name);
	
	name[strlen(name) - 4] = 0;
	strcat(name, ".ent");
	
	Com_Printf("Dump entity string to "S_COLOR_GREEN"%s\n", name);
	FS_CreatePath(name);

	buf = CM_EntityString();
	
	f = fopen(name, "w");
	if (!f) {
		Com_Printf(S_COLOR_RED"ERROR: couldn't open.\n");
		return;
	}

	fputs(buf, f);
	fclose(f);


}

#define		GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define		GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define		GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define		GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define		GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

#define     VBO_FREE_MEMORY_ATI                     0x87FB
#define     TEXTURE_FREE_MEMORY_ATI                 0x87FC
#define     RENDERBUFFER_FREE_MEMORY_ATI            0x87FD

void R_VideoInfo_f(void){

#ifdef _WIN32
	int mem[4];
	
	if (IsExtensionSupported("GL_NVX_gpu_memory_info")) {
						
		Com_Printf("\nNvidia specific memory info:\n");
		Com_Printf("\n");
		qglGetIntegerv ( GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX , mem);
		Com_Printf("dedicated video memory %i MB\n", mem[0] >>10);

		qglGetIntegerv ( GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX , mem);
		Com_Printf("total available memory %i MB\n", mem[0] >>10);

		qglGetIntegerv ( GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX , mem);
		Com_Printf("currently unused GPU memory %i MB\n", mem[0] >>10);

		qglGetIntegerv ( GPU_MEMORY_INFO_EVICTION_COUNT_NVX , mem);
		Com_Printf("count of total evictions seen by system %i MB\n", mem[0] >>10);

		qglGetIntegerv ( GPU_MEMORY_INFO_EVICTED_MEMORY_NVX , mem);
		Com_Printf("total video memory evicted %i MB\n", mem[0] >>10);

	}
	else
#endif
		Com_Printf("MemInfo not availabled for your video card or driver!\n");
}
// 512 mb vram
void R_LowSpecMachine_f(void)
{
Cvar_Set("r_textureCompression", "1");
Cvar_Set("r_maxTextureSize", "512");
Cvar_Set("r_anisotropic", "1");
Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR");

Cvar_Set("r_shadows", "0");
Cvar_Set("r_drawFlares", "1");
Cvar_Set("r_reliefMapping", "0");
Cvar_Set("r_skipStaticLights", "1");
Cvar_Set("r_lightmapScale", "1.0");
Cvar_Set("r_bloom", "0");
Cvar_Set("r_dof", "0");
Cvar_Set("r_radialBlur", "1");
Cvar_Set("r_softParticles", "1");
Cvar_Set("r_motionBlur", "0");
Cvar_Set("r_ssao", "0");

vid_ref->modified = qtrue;
}
// 1 gb vram
void R_MediumSpecMachine_f(void)
{
Cvar_Set("r_textureCompression", "0");
Cvar_Set("r_maxTextureSize", "512");
Cvar_Set("r_anisotropic", "8");
Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR");

Cvar_Set("r_shadows", "1");
Cvar_Set("r_drawFlares", "1");
Cvar_Set("r_reliefMapping", "0");
Cvar_Set("r_skipStaticLights", "0");
Cvar_Set("r_lightmapScale", "0.5");
Cvar_Set("r_bloom", "1");
Cvar_Set("r_dof", "0");
Cvar_Set("r_radialBlur", "1");
Cvar_Set("r_softParticles", "1");
Cvar_Set("r_motionBlur", "0");
Cvar_Set("r_ssao", "0");

vid_ref->modified = qtrue;
}

void R_HiSpecMachine_f(void)
{
Cvar_Set("r_textureCompression", "0");
Cvar_Set("r_maxTextureSize", "0");
Cvar_Set("r_anisotropic", "16");
Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR");

Cvar_Set("r_shadows", "1");
Cvar_Set("r_drawFlares", "1");
Cvar_Set("r_reliefMapping", "1");
Cvar_Set("r_skipStaticLights", "0");
Cvar_Set("r_lightmapScale", "0.5");
Cvar_Set("r_bloom", "1");
Cvar_Set("r_dof", "1");
Cvar_Set("r_radialBlur", "1");
Cvar_Set("r_softParticles", "1");
Cvar_Set("r_fxaa", "1");
Cvar_Set("r_motionBlur", "1");
Cvar_Set("r_ssao", "1");

vid_ref->modified = qtrue;
}

void R_ChangeLightColor_f(void) ;
void R_GLSLinfo_f(void);

void R_RegisterCvars(void)
{
	r_leftHand =						Cvar_Get("hand", "0", CVAR_USERINFO | CVAR_ARCHIVE);
	r_noRefresh =						Cvar_Get("r_noRefresh", "0", 0);
	r_drawEntities =					Cvar_Get("r_drawEntities", "1", 0);
	r_drawWorld =						Cvar_Get("r_drawWorld", "1", 0);
	r_noVis =							Cvar_Get("r_noVis", "0", 0);
	r_noCull =							Cvar_Get("r_noCull", "0", 0);
	r_speeds =							Cvar_Get("r_speeds", "0", 0);

	r_lightLevel =						Cvar_Get("r_lightLevel", "0", 0);

	r_mode =							Cvar_Get("r_mode", "0", CVAR_ARCHIVE);
	r_noBind =							Cvar_Get("r_noBind", "0", 0);
	r_lockPvs =							Cvar_Get("r_lockPvs", "0", 0);

	r_vsync =							Cvar_Get("r_vsync", "0", CVAR_ARCHIVE);
	r_finish =							Cvar_Get("r_finish", "0", 0);
	
	r_fullScreen =						Cvar_Get("r_fullScreen", "1", CVAR_ARCHIVE);
	
	r_brightness =							Cvar_Get("r_brightness", "1.5", CVAR_ARCHIVE);
	r_brightness	=					Cvar_Get("r_brightness", "1", CVAR_ARCHIVE);
	r_contrast	=						Cvar_Get("r_contrast", "1", CVAR_ARCHIVE);
	r_saturation =						Cvar_Get("r_saturation", "1", CVAR_ARCHIVE);

	vid_ref =							Cvar_Get("vid_ref", "xpgl", CVAR_ARCHIVE);
	r_displayRefresh =					Cvar_Get("r_displayRefresh", "0", CVAR_ARCHIVE);

	r_anisotropic =						Cvar_Get("r_anisotropic", "16", CVAR_ARCHIVE);
	r_maxAnisotropy =					Cvar_Get("r_maxAnisotropy", "0", 0);
	r_maxTextureSize=					Cvar_Get("r_maxTextureSize", "0", CVAR_ARCHIVE);
	r_textureColorScale =				Cvar_Get("r_textureColorScale", "2", CVAR_ARCHIVE);
	r_textureCompression =				Cvar_Get("r_textureCompression", "0", CVAR_ARCHIVE);			
	r_causticIntens =					Cvar_Get("r_causticIntens", "5.0", CVAR_ARCHIVE);
	r_textureMode =						Cvar_Get("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE);

	r_screenShot =						Cvar_Get("r_screenShot", "jpg", CVAR_ARCHIVE);
	r_screenShotJpegQuality =			Cvar_Get("r_screenShotJpegQuality", "99", CVAR_ARCHIVE);
	r_screenShotGamma =					Cvar_Get("r_screenShotGamma", "1.0", CVAR_ARCHIVE);
	r_screenShotContrast =				Cvar_Get("r_screenShotContrast", "1.0", CVAR_ARCHIVE);

	r_arbSamples =						Cvar_Get("r_arbSamples", "1", CVAR_ARCHIVE);
	r_fxaa =							Cvar_Get("r_fxaa", "0", CVAR_ARCHIVE);

	deathmatch =						Cvar_Get("deathmatch", "0", CVAR_SERVERINFO);
	
	r_drawFlares =						Cvar_Get("r_drawFlares", "1", CVAR_ARCHIVE);
	r_flaresIntens =					Cvar_Get("r_flaresIntens", "3", CVAR_ARCHIVE);
	r_flareWeldThreshold =				Cvar_Get("r_flareWeldThreshold", "32", CVAR_ARCHIVE);

	r_customWidth =						Cvar_Get("r_customWidth", "1024", CVAR_ARCHIVE);
	r_customHeight =					Cvar_Get("r_customHeight", "768", CVAR_ARCHIVE);

	sys_priority =						Cvar_Get("sys_priority", "0", CVAR_ARCHIVE);
		
	hunk_bsp=							Cvar_Get("hunk_bsp", "70", CVAR_ARCHIVE);
	hunk_model=							Cvar_Get("hunk_model", "2.4", CVAR_ARCHIVE);
	hunk_sprite=						Cvar_Get("hunk_sprite", "0.08", CVAR_ARCHIVE);

	r_reliefMapping =					Cvar_Get("r_reliefMapping", "1", CVAR_ARCHIVE);
	r_reliefScale=						Cvar_Get("r_reliefScale", "2.0", CVAR_ARCHIVE);

	r_shadows =							Cvar_Get("r_shadows", "1", CVAR_ARCHIVE);
	r_playerShadow =					Cvar_Get("r_playerShadow", "1", CVAR_ARCHIVE);

	r_skipStaticLights =				Cvar_Get("r_skipStaticLights", "0", CVAR_ARCHIVE);
	r_lightmapScale =					Cvar_Get("r_lightmapScale", "0.5", CVAR_ARCHIVE);
	r_useLightScissors = 				Cvar_Get("r_useLightScissors", "1", 0);
	r_useDepthBounds =					Cvar_Get("r_useDepthBounds", "1", 0);
	r_tbnSmoothAngle =					Cvar_Get("r_tbnSmoothAngle", "65", CVAR_ARCHIVE);
	r_lightsWeldThreshold =				Cvar_Get("r_lightsWeldThreshold", "40", CVAR_ARCHIVE);
	r_debugLights =						Cvar_Get("r_debugLights", "0", 0);
//	r_useLightOccluders =				Cvar_Get("r_useLightOccluders", "0", 0);
//	r_occLightBoundsSize =				Cvar_Get("r_occLightBoundsSize", "0.75", 0);
//	r_debugOccLightBoundsSize =			Cvar_Get("r_debugOccLightBoundsSize", "0.75", 0);
	r_specularScale =					Cvar_Get("r_specularScale", "1", CVAR_ARCHIVE);
	r_ambientSpecularScale =			Cvar_Get("r_ambientSpecularScale", "0.3", CVAR_ARCHIVE);
	r_useRadiosityBump =				Cvar_Get("r_useRadiosityBump", "1", CVAR_ARCHIVE);
	r_zNear =							Cvar_Get("r_zNear", "3", CVAR_ARCHIVE);
	r_zFar =							Cvar_Get("r_zFar", "4096", CVAR_ARCHIVE);

	r_bloom =							Cvar_Get("r_bloom", "1", CVAR_ARCHIVE);
	r_bloomThreshold =					Cvar_Get("r_bloomThreshold", "0.75", CVAR_ARCHIVE);
	r_bloomIntens =						Cvar_Get("r_bloomIntens", "2.0", CVAR_ARCHIVE);
	r_bloomBright =						Cvar_Get("r_bloomBright", "0.9", CVAR_ARCHIVE);
	r_bloomExposure =					Cvar_Get("r_bloomExposure", "0.86", CVAR_ARCHIVE);
	r_bloomStarIntens =					Cvar_Get("r_bloomStarIntens", "3.0", CVAR_ARCHIVE);

	r_ssao =							Cvar_Get ("r_ssao", "1", CVAR_ARCHIVE);
	r_ssaoIntensity =					Cvar_Get ("r_ssaoIntensity", "2.0", CVAR_ARCHIVE);
	r_ssaoScale =						Cvar_Get ("r_ssaoScale", "80.0", CVAR_ARCHIVE);
	r_ssaoBlur	=						Cvar_Get ("r_ssaoBlur", "2", CVAR_ARCHIVE);

	r_dof =								Cvar_Get("r_dof", "1", CVAR_ARCHIVE);
	r_dofBias =							Cvar_Get("r_dofBias", "0.002", CVAR_ARCHIVE);
	r_dofFocus =						Cvar_Get("r_dofFocus", "0.0", CVAR_ARCHIVE);

	r_motionBlur =						Cvar_Get("r_motionBlur", "1", CVAR_ARCHIVE);
	r_motionBlurSamples	=				Cvar_Get("r_motionBlurSamples", "16", CVAR_ARCHIVE);
	r_motionBlurFrameLerp =				Cvar_Get("r_motionBlurFrameLerp", "10", CVAR_ARCHIVE);

	r_radialBlur =						Cvar_Get("r_radialBlur", "1", CVAR_ARCHIVE);
	r_radialBlurFov =                   Cvar_Get("r_radialBlurFov", "30", CVAR_ARCHIVE);
	r_filmGrain = 						Cvar_Get("r_filmGrain", "0", CVAR_ARCHIVE);
	r_ignoreGlErrors =					Cvar_Get("r_ignoreGlErrors", "1", 0);
	
	r_lightEditor =						Cvar_Get("r_lightEditor", "0", 0);
	r_cameraSpaceLightMove =			Cvar_Get("r_cameraSpaceLightMove", "0", CVAR_ARCHIVE);

	r_allowIntel =						Cvar_Get("r_allowIntel", "0", 0);

	Cmd_AddCommand("imagelist",			GL_ImageList_f);
	Cmd_AddCommand("screenshot",		GL_ScreenShot_f);
	Cmd_AddCommand("modellist",			Mod_Modellist_f);
	Cmd_AddCommand("openglInfo",		GL_Strings_f);
	Cmd_AddCommand("flaresStats",		FlareStatsList_f);
	Cmd_AddCommand("dumpEntityString",	Dump_EntityString);
	Cmd_AddCommand("glslInfo",			R_ListPrograms_f);
	Cmd_AddCommand("r_meminfo",			R_VideoInfo_f);
	Cmd_AddCommand("low_spec",			R_LowSpecMachine_f);
	Cmd_AddCommand("medium_spec",		R_MediumSpecMachine_f);
	Cmd_AddCommand("hi_spec",			R_HiSpecMachine_f);
	Cmd_AddCommand("glsl",				R_GLSLinfo_f);
/*
bind INS		"spawnLight"
bind HOME		"spawnLightToCamera"
bind END		"saveLights"
bind DEL		"removeLight"
bind LEFTARROW	"moveLight_right   -1"
bind RIGHTARROW "moveLight_right    1"
bind UPARROW	"moveLight_forward  1"
bind DOWNARROW	"moveLight_forward -1"
bind PGUP		"moveLight_z        1"
bind PGDN		"moveLight_z       -1"
bind KP_MINUS	"changeLightRadius -5"
bind KP_PLUS	"changeLightRadius  5"
bind KP_INS		"copyLight"
bind KP_DEL		"unselectLight"
bind c			"copy"
bind v			"paste"
*/

	Cmd_AddCommand("saveLights",				R_SaveLights_f);
	Cmd_AddCommand("spawnLight",				R_Light_Spawn_f);
	Cmd_AddCommand("spawnLightToCamera",		R_Light_SpawnToCamera_f);
	Cmd_AddCommand("removeLight",				R_Light_Delete_f);
	Cmd_AddCommand("editLight",					R_EditSelectedLight_f);
	Cmd_AddCommand("moveLight_right",			R_MoveLightToRight_f);
	Cmd_AddCommand("moveLight_forward",			R_MoveLightForward_f);
	Cmd_AddCommand("moveLight_z",				R_MoveLightUpDown_f);
	Cmd_AddCommand("changeLightRadius",			R_ChangeLightRadius_f);
	Cmd_AddCommand("cloneLight",				R_Light_Clone_f);
	Cmd_AddCommand("changeLightCone",			R_ChangeLightCone_f);
	Cmd_AddCommand("clearWorldLights",          R_ClearWorldLights);
	Cmd_AddCommand("unselectLight",				R_Light_UnSelect_f);
	Cmd_AddCommand("editFlare",					R_FlareEdit_f);
	Cmd_AddCommand("resetFlarePos",				R_ResetFlarePos_f);
	Cmd_AddCommand("copy",						R_Copy_Light_Properties_f);
	Cmd_AddCommand("paste",						R_Paste_Light_Properties_f);
}

/*
==================
R_SetMode
==================
*/
qboolean R_SetMode(void)
{
	rserr_t err;
	const qboolean fullscreen = (qboolean)r_fullScreen->value;

	r_fullScreen->modified = qfalse;
	r_mode->modified = qfalse;

    err = GLimp_SetMode(&vid.width, &vid.height, r_mode->value, fullscreen);

    // success, update variables
	if (err == rserr_ok) {
        Cvar_SetValue("r_fullScreen", gl_state.fullscreen);
        r_fullScreen->modified = qfalse;
		gl_state.prev_mode = r_mode->value;
        return qtrue;

    // try without fullscreen
	} else if (err == rserr_invalid_fullscreen) {
        Com_Printf(S_COLOR_RED "ref_xpgl::R_SetMode() - fullscreen unavailable in this mode\n");
        if ((err = GLimp_SetMode(&vid.width, &vid.height, r_mode->value, qfalse)) == rserr_ok) {
            Cvar_SetValue("r_fullScreen", 0);
            r_fullScreen->modified = qfalse;
            gl_state.prev_mode = r_mode->value;
            return qtrue;
        }

    } else if (err == rserr_invalid_mode) {
        Com_Printf(S_COLOR_RED"ref_xpgl::R_SetMode() - invalid mode\n");
    }

    // revert to previous mode
    if (GLimp_SetMode(&vid.width, &vid.height, gl_state.prev_mode, qfalse) == rserr_ok) {
        Cvar_SetValue("r_mode", gl_state.prev_mode);
        r_mode->modified = qfalse;
        Cvar_SetValue("r_fullScreen", 0);
        r_fullScreen->modified = qfalse;
        return qtrue;
    } else {
        Com_Printf(S_COLOR_RED"ref_xpgl::R_SetMode() - could not revert to safe mode\n");
        return qfalse;
    }
}

static void DevIL_Init() {
    static qboolean init = qfalse;

    if (init)
        return;

	Com_Printf ("==="S_COLOR_YELLOW"OpenIL library initiation..."S_COLOR_WHITE"===\n");
	
	ilInit();
	iluInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET);
	ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_UPPER_LEFT);

	Con_Printf (PRINT_ALL, "OpenIL VENDOR: "S_COLOR_GREEN" %s\n", ilGetString(IL_VENDOR));
	Con_Printf (PRINT_ALL, "OpenIL Version: "S_COLOR_GREEN"%i\n", ilGetInteger(IL_VERSION_NUM));
	Com_Printf ("==================================\n\n");

    init = qtrue;
}


/*
===============
R_Init
===============
*/
qboolean IsExtensionSupported(const char *name)
{
	int			i;
	uint		n = 0;
	const char	*extension;

	qglGetIntegerv(GL_NUM_EXTENSIONS, &n);
	
	for (i = 0; i<n; i++){
		extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (!strcmp(name, extension))
			return qtrue;		
	}
	return qfalse;
}

int R_Init(void *hinstance, void *hWnd)
{
	int			aniso_level, max_aniso;
	int			profile, minor, major;
	const char *profileName[] = { "core", "compatibility" };

	Draw_GetPalette();
	R_RegisterCvars();

	// initialize our QGL dynamic bindings
	if (!QGL_Init()) {
		QGL_Shutdown();
		Com_Printf(S_COLOR_RED "quake2xp::R_Init() - could not load opengl32.dll");
		return -1;
	}
	// initialize OS-specific parts of OpenGL
	if (!GLimp_Init(hinstance, hWnd)) {
		QGL_Shutdown();
		return -1;
	}
	// set our "safe" modes
	gl_state.prev_mode = 0;

    // initialize IL library
    DevIL_Init();

	// create the window and set up the context
	if (!R_SetMode()) {
		QGL_Shutdown();
		Com_Printf(S_COLOR_RED "ref_xpgl::R_Init() - could not R_SetMode()\n");
		return -1;
	}

	VID_MenuInit();

	/* 
	 ** get our various GL strings
	 */
	Com_Printf( "\n");
	gl_config.vendor_string = (const char*)qglGetString(GL_VENDOR);

	Com_Printf(S_COLOR_WHITE "GL_VENDOR:" S_COLOR_GREEN "    %s\n", gl_config.vendor_string);
	gl_config.renderer_string = (const char*)qglGetString(GL_RENDERER);
	Com_Printf(S_COLOR_WHITE "GL_RENDERER:" S_COLOR_GREEN "  %s\n", gl_config.renderer_string);
	gl_config.version_string = (const char*)qglGetString(GL_VERSION);
	Com_Printf(S_COLOR_WHITE "GL_VERSION:" S_COLOR_GREEN "   %s\n", gl_config.version_string);
	
	qglGetIntegerv(GL_MAJOR_VERSION, &major);
	qglGetIntegerv(GL_MINOR_VERSION, &minor);
	qglGetIntegerv(WGL_CONTEXT_PROFILE_MASK_ARB, &profile);
	Com_Printf("Using OpenGL: "S_COLOR_GREEN"%i.%i"S_COLOR_WHITE" %s profile context\n", major, minor, profileName[profile == WGL_CONTEXT_CORE_PROFILE_BIT_ARB ? 0 : 1]);

	Cvar_Set("scr_drawall", "0");

	Com_Printf("\n");
	Com_Printf("=====================================\n");
	Com_Printf(S_COLOR_GREEN"Checking Basic Quake II XP Extensions\n");
	Com_Printf("=====================================\n");
	Com_Printf("\n");

	if (IsExtensionSupported("GL_ARB_multitexture")) {
		Com_Printf("...using GL_ARB_multitexture\n");

		qglActiveTextureARB =		(PFNGLACTIVETEXTUREARBPROC)			qwglGetProcAddress("glActiveTextureARB");
		qglClientActiveTextureARB =	(PFNGLCLIENTACTIVETEXTUREARBPROC)	qwglGetProcAddress("glClientActiveTextureARB");
	
	} else {
		Com_Printf(S_COLOR_RED"...GL_ARB_multitexture not found\n");
		VID_Error(ERR_FATAL, "GL_ARB_multitexture not found!");  //wtf????!!!! kill!
	}

	qglGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	Cvar_SetValue("r_maxAnisotropy", max_aniso);
	if (r_anisotropic->value >= r_maxAnisotropy->value)
		Cvar_SetValue("r_anisotropic", r_maxAnisotropy->value);

	aniso_level = r_anisotropic->value;
	if (IsExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
		if (r_anisotropic->value <= 1) {
			r_anisotropic = Cvar_Set("r_anisotropic", "1");
			Com_Printf(S_COLOR_YELLOW"...ignoring GL_EXT_texture_filter_anisotropic\n");
		} else {
			Com_Printf("...using GL_EXT_texture_filter_anisotropic\n   ["S_COLOR_GREEN"%i"S_COLOR_WHITE" max] ["S_COLOR_GREEN"%i" S_COLOR_WHITE" selected]\n",
					   max_aniso, aniso_level);
		}
	}

	gl_state.texture_compression_bptc = qfalse;
	if (IsExtensionSupported("GL_ARB_texture_compression_bptc"))
		if (!r_textureCompression->value) {
			Com_Printf(S_COLOR_YELLOW"...ignoring GL_ARB_texture_compression_bptc\n");
			gl_state.texture_compression_bptc = qfalse;
		}
		else {
			Com_Printf("...using GL_ARB_texture_compression_bptc\n");
			gl_state.texture_compression_bptc = qtrue;

		}
		else {
			Com_Printf(S_COLOR_RED"...GL_ARB_texture_compression_bptc not found\n");
			gl_state.texture_compression_bptc = qfalse;
		}
	
	if (!gl_state.texture_compression_bptc)
		if (IsExtensionSupported("GL_EXT_texture_compression_s3tc"))
			if (!r_textureCompression->value) {
				Com_Printf(S_COLOR_YELLOW"...ignoring GL_EXT_texture_compression_s3tc\n");
				gl_state.texture_compression_dxt = qfalse;
		}
		else {
			Com_Printf("...using GL_EXT_texture_compression_s3tc\n");
			gl_state.texture_compression_dxt = qtrue;
		}

	if (IsExtensionSupported("GL_ARB_texture_cube_map"))
		Com_Printf("...using GL_ARB_texture_cube_map\n");

	if (IsExtensionSupported("GL_ARB_seamless_cube_map")){
		Com_Printf("...using GL_ARB_seamless_cube_map\n");
		qglEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	else
		Com_Printf(S_COLOR_RED"...GL_ARB_seamless_cube_map not found\n");

	// ===========================================================================================================================

	// openGL 2.0 Unified Separate Stencil
	qglStencilFuncSeparate		= (PFNGLSTENCILFUNCSEPARATEPROC)	qwglGetProcAddress("glStencilFuncSeparate");
	qglStencilOpSeparate		= (PFNGLSTENCILOPSEPARATEPROC)		qwglGetProcAddress("glStencilOpSeparate");
	qglStencilMaskSeparate		= (PFNGLSTENCILMASKSEPARATEPROC)	qwglGetProcAddress("glStencilMaskSeparate");

	if(qglStencilFuncSeparate && qglStencilOpSeparate && qglStencilMaskSeparate){
			Com_Printf("...using GL_EXT_stencil_two_side\n");
	}else
		Com_Printf(S_COLOR_RED"...GL_EXT_stencil_two_side not found\n");

	gl_state.depthBoundsTest = qfalse;
	if (IsExtensionSupported("GL_EXT_depth_bounds_test")) {
	Com_Printf("...using GL_EXT_depth_bounds_test\n");

	glDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC) qwglGetProcAddress("glDepthBoundsEXT");
	gl_state.depthBoundsTest = qtrue;
	} else {
		Com_Printf(S_COLOR_RED"...GL_EXT_depth_bounds_test not found\n");
	gl_state.depthBoundsTest = qfalse;
	}

	if (IsExtensionSupported("GL_ARB_texture_non_power_of_two")) 
		Com_Printf("...using GL_ARB_texture_non_power_of_two\n");
	
	glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC)	qwglGetProcAddress("glDebugMessageControlARB");
	glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC)		qwglGetProcAddress("glDebugMessageInsertARB");
	glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)	qwglGetProcAddress("glDebugMessageCallbackARB");
	glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC)		qwglGetProcAddress("glGetDebugMessageLogARB");
	
	gl_state.debugOutput = qfalse;

	if (glDebugMessageControlARB && glDebugMessageInsertARB && glDebugMessageCallbackARB && glGetDebugMessageLogARB){
		Com_Printf("...using GL_ARB_debug_output\n");
		gl_state.debugOutput = qtrue;
	}
	else
		Com_Printf(S_COLOR_RED"...GL_ARB_debug_output not found\n");

	if (IsExtensionSupported("GL_ARB_texture_rectangle")) 
		Com_Printf("...using GL_ARB_texture_rectangle\n");		

	// unused vao stuff
	glGenVertexArrays		= (PFNGLGENVERTEXARRAYSPROC)	qwglGetProcAddress	("glGenVertexArrays");
	glDeleteVertexArrays	= (PFNGLDELETEVERTEXARRAYSPROC)	qwglGetProcAddress	("glDeleteVertexArrays");
	glBindVertexArray		= (PFNGLBINDVERTEXARRAYPROC)	qwglGetProcAddress	("glBindVertexArray");

	if (IsExtensionSupported("GL_ARB_vertex_buffer_object")) {
			
		qglBindBufferARB =		(PFNGLBINDBUFFERARBPROC)		qwglGetProcAddress("glBindBufferARB");
		qglDeleteBuffersARB =	(PFNGLDELETEBUFFERSARBPROC)	qwglGetProcAddress("glDeleteBuffersARB");
		qglGenBuffersARB =		(PFNGLGENBUFFERSARBPROC)		qwglGetProcAddress("glGenBuffersARB");
		qglBufferDataARB =		(PFNGLBUFFERDATAARBPROC)		qwglGetProcAddress("glBufferDataARB");
		qglBufferSubDataARB =	(PFNGLBUFFERSUBDATAARBPROC)	qwglGetProcAddress("glBufferSubDataARB");
		qglMapBufferARB =		(PFNGLMAPBUFFERARBPROC)		qwglGetProcAddress("glMapBufferARB");
		qglUnmapBufferARB =		(PFNGLUNMAPBUFFERARBPROC)		qwglGetProcAddress("glUnmapBufferARB");

		if (qglGenBuffersARB && qglBindBufferARB && qglBufferDataARB && qglDeleteBuffersARB && qglBufferSubDataARB){
			vec2_t		tmpVerts[4];
			index_t		iCache[6 * MAX_DRAW_STRING_LENGTH];
			int			idx = 0, numVerts = 0, i;

			Com_Printf("...using GL_ARB_vertex_buffer_object\n");
			// precalc screen quads for postprocessing
			// full quad
			VA_SetElem2(tmpVerts[0],0 ,			vid.height);
			VA_SetElem2(tmpVerts[1],vid.width,	vid.height);
			VA_SetElem2(tmpVerts[2],vid.width,	0);
			VA_SetElem2(tmpVerts[3],0,			0);
			qglGenBuffersARB(1, &vbo.vbo_fullScreenQuad);
			qglBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo.vbo_fullScreenQuad);
			qglBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vec2_t)*4, tmpVerts, GL_STATIC_DRAW_ARB);
			qglBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

			// half quad
			VA_SetElem2(tmpVerts[0],0,					vid.height);
			VA_SetElem2(tmpVerts[1],vid.width * 0.5 ,	vid.height);
			VA_SetElem2(tmpVerts[2],vid.width * 0.5 ,	vid.height * 0.5);
			VA_SetElem2(tmpVerts[3],0,					vid.height * 0.5);
			qglGenBuffersARB(1, &vbo.vbo_halfScreenQuad);
			qglBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo.vbo_halfScreenQuad);
			qglBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vec2_t)*4, tmpVerts, GL_STATIC_DRAW_ARB);
			qglBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

			// quater quad
			VA_SetElem2(tmpVerts[0],0,						vid.height);
			VA_SetElem2(tmpVerts[1],vid.width * 0.25 ,		vid.height);
			VA_SetElem2(tmpVerts[2],vid.width * 0.25 ,		vid.height * 0.25);
			VA_SetElem2(tmpVerts[3],0,						vid.height * 0.25);
			qglGenBuffersARB(1, &vbo.vbo_quarterScreenQuad);
			qglBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo.vbo_quarterScreenQuad);
			qglBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vec2_t)*4, tmpVerts, GL_STATIC_DRAW_ARB);
			qglBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

			for (i = 0, idx = 0; i < MAX_DRAW_STRING_LENGTH * 4; i +=4)
			{
				iCache[idx++] = i + 0;
				iCache[idx++] = i + 1;
				iCache[idx++] = i + 2;
				iCache[idx++] = i + 0;
				iCache[idx++] = i + 2;
				iCache[idx++] = i + 3;
			}

			qglGenBuffersARB(1, &vbo.ibo_quadTris);
			qglBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo.ibo_quadTris);
			qglBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, idx * sizeof(GL_UNSIGNED_SHORT), iCache, GL_STATIC_DRAW_ARB);
			qglBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);

			qglGenBuffersARB(1, &vbo.vbo_Dynamic);
			qglGenBuffersARB(1, &vbo.ibo_Dynamic);
	
		}
	} else {
		Com_Printf(S_COLOR_RED "...GL_ARB_vertex_buffer_object not found\n");
	}
	
	if (IsExtensionSupported("GL_ARB_draw_buffers")) {
		qglDrawBuffers =	(PFNGLDRAWBUFFERSARBPROC) qwglGetProcAddress("glDrawBuffersARB");
		
	if (qglDrawBuffers)
		Com_Printf("...using GL_ARB_draw_buffers\n");
	else
		Com_Printf(S_COLOR_RED"...using GL_ARB_draw_buffers not found\n");
	}
	/*
	if (IsExtensionSupported("GL_ARB_texture_float")) 
		Com_Printf("...using GL_ARB_texture_float\n");
	else
		Com_Printf(S_COLOR_RED"...using GL_ARB_texture_float not found\n");
*/

	if (IsExtensionSupported("GL_ARB_framebuffer_object")) {
		Com_Printf("...using GL_ARB_framebuffer_object\n");  

		qglGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,	&gl_state.maxRenderBufferSize);
		qglGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,	&gl_state.maxColorAttachments);
		qglGetIntegerv(GL_MAX_SAMPLES,				&gl_state.maxSamples);
		
		Com_Printf("\n");

		Com_Printf(S_COLOR_YELLOW"   Max Render Buffer Size:  "S_COLOR_GREEN"%i\n", gl_state.maxRenderBufferSize);
		Com_Printf(S_COLOR_YELLOW"   Max Color Attachments:   "S_COLOR_GREEN"%i\n", gl_state.maxColorAttachments);
		Com_Printf(S_COLOR_YELLOW"   Max Buffer Samples:      "S_COLOR_GREEN"%i\n", gl_state.maxSamples);

		qglIsRenderbuffer =							(PFNGLISRENDERBUFFERPROC) qwglGetProcAddress("glIsRenderbuffer");
		qglBindRenderbuffer =						(PFNGLBINDRENDERBUFFERPROC) qwglGetProcAddress("glBindRenderbuffer");
		qglDeleteRenderbuffers =					(PFNGLDELETERENDERBUFFERSPROC) qwglGetProcAddress("glDeleteRenderbuffers");
		qglGenRenderbuffers =						(PFNGLGENRENDERBUFFERSPROC) qwglGetProcAddress("glGenRenderbuffers");
		qglRenderbufferStorage =					(PFNGLRENDERBUFFERSTORAGEPROC) qwglGetProcAddress("glRenderbufferStorage");
		qglGetRenderbufferParameteriv =				(PFNGLGETRENDERBUFFERPARAMETERIVPROC) qwglGetProcAddress("glGetRenderbufferParameteriv");
		qglRenderbufferStorageMultisample =			(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) qwglGetProcAddress("glRenderbufferStorageMultisample");

		qglIsFramebuffer =							(PFNGLISFRAMEBUFFERPROC) qwglGetProcAddress("glIsFramebuffer");
		qglBindFramebuffer =						(PFNGLBINDFRAMEBUFFERPROC) qwglGetProcAddress("glBindFramebuffer");
		qglDeleteFramebuffers =						(PFNGLDELETEFRAMEBUFFERSPROC) qwglGetProcAddress("glDeleteFramebuffers");
		qglGenFramebuffers =						(PFNGLGENFRAMEBUFFERSPROC) qwglGetProcAddress("glGenFramebuffers");
		qglCheckFramebufferStatus =					(PFNGLCHECKFRAMEBUFFERSTATUSPROC) qwglGetProcAddress("glCheckFramebufferStatus");
		qglFramebufferTexture1D =					(PFNGLFRAMEBUFFERTEXTURE1DPROC) qwglGetProcAddress("glFramebufferTexture1D");
		qglFramebufferTexture2D =					(PFNGLFRAMEBUFFERTEXTURE2DPROC) qwglGetProcAddress("glFramebufferTexture2D");
		qglFramebufferTexture3D =					(PFNGLFRAMEBUFFERTEXTURE3DPROC) qwglGetProcAddress("glFramebufferTexture3D");
		qglFramebufferRenderbuffer =				(PFNGLFRAMEBUFFERRENDERBUFFERPROC) qwglGetProcAddress("glFramebufferRenderbuffer");
		qglGetFramebufferAttachmentParameteriv =	(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) qwglGetProcAddress("glGetFramebufferAttachmentParameteriv");
		qglGenerateMipmap =							(PFNGLGENERATEMIPMAPPROC) qwglGetProcAddress("glGenerateMipmap");
		qglBlitFramebuffer =						(PFNGLBLITFRAMEBUFFERPROC) qwglGetProcAddress("glBlitFramebuffer");
		
		Com_Printf("\n");
		CreateWeaponFboMask ();
		CreateSSAOBuffer ();
		Com_Printf("\n");
	}
	else {
		Com_Printf(S_COLOR_RED"...GL_ARB_framebuffer_object not found\n");
	}

	if (IsExtensionSupported("GL_ARB_shading_language_100"))
	{
		if (IsExtensionSupported("GL_ARB_fragment_shader"))
		{
		Com_Printf("...using GL_ARB_fragment_shader\n");

		if (IsExtensionSupported("GL_ARB_vertex_shader")){
			Com_Printf("...using GL_ARB_vertex_shader\n");
	
	gl_state.shader5= qfalse;
	gl_state.shader4 = qfalse;
	if (IsExtensionSupported("GL_ARB_gpu_shader5")){
		Com_Printf("...using GL_ARB_gpu_shader5\n");
		gl_state.shader5= qtrue;
	}else
		Com_Printf(S_COLOR_RED"...GL_ARB_gpu_shader5 not found\n");

	if (IsExtensionSupported("GL_EXT_gpu_shader4")){
		Com_Printf("...using GL_EXT_gpu_shader4\n");
		gl_state.shader4 = qtrue;
	}

	gl_state.eal = qfalse;
	if (IsExtensionSupported("GL_ARB_explicit_attrib_location")){
		Com_Printf("...using GL_ARB_explicit_attrib_location\n");
		gl_state.eal = qtrue;
	}

	if (IsExtensionSupported("GL_ARB_separate_shader_objects")){
		Com_Printf("...using GL_ARB_separate_shader_objects\n");
	}

	
	gl_config.shadingLanguageVersionString = (const char*)qglGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
	qglGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &gl_config.maxFragmentUniformComponents);
	qglGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_config.maxVertexUniformComponents);
	qglGetIntegerv(GL_MAX_VARYING_FLOATS, &gl_config.maxVaryingFloats);
	qglGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &gl_config.maxVertexTextureImageUnits);
	qglGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &gl_config.maxCombinedTextureImageUnits);
	qglGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &gl_config.maxFragmentUniformComponents);
	qglGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_config.maxVertexAttribs);
	qglGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_config.maxTextureImageUnits);

	Com_Printf("\n");
	Com_Printf(S_COLOR_YELLOW"   GLSL Version:               "S_COLOR_GREEN"   %s\n", gl_config.shadingLanguageVersionString);
	Com_Printf(S_COLOR_YELLOW"   maxFragmentUniformComponents:"S_COLOR_GREEN"  %i\n", gl_config.maxFragmentUniformComponents);
	Com_Printf(S_COLOR_YELLOW"   maxVertexUniformComponents: "S_COLOR_GREEN"   %i\n", gl_config.maxVertexUniformComponents);
	Com_Printf(S_COLOR_YELLOW"   maxVertexAttribs:           "S_COLOR_GREEN"   %i\n", gl_config.maxVertexAttribs);
	Com_Printf(S_COLOR_YELLOW"   maxVaryingFloats:           "S_COLOR_GREEN"   %i\n", gl_config.maxVaryingFloats);
	Com_Printf(S_COLOR_YELLOW"   maxVertexTextureImageUnits: "S_COLOR_GREEN"   %i\n", gl_config.maxVertexTextureImageUnits);
	Com_Printf(S_COLOR_YELLOW"   maxTextureImageUnits:       "S_COLOR_GREEN"   %i\n", gl_config.maxTextureImageUnits);
	Com_Printf(S_COLOR_YELLOW"   maxCombinedTextureImageUnits: "S_COLOR_GREEN" %i\n", gl_config.maxCombinedTextureImageUnits);
	Com_Printf(S_COLOR_YELLOW"   maxFragmentUniformComponents: "S_COLOR_GREEN" %i\n", gl_config.maxFragmentUniformComponents);

	qglCreateShader =				(PFNGLCREATESHADERPROC)				qwglGetProcAddress("glCreateShader");
	qglCreateProgram =				(PFNGLCREATEPROGRAMOBJECTARBPROC)	qwglGetProcAddress("glCreateProgram");
	qglDeleteShader =				(PFNGLDELETESHADERPROC)				qwglGetProcAddress("glDeleteShader");
	qglDeleteProgram =				(PFNGLDELETEPROGRAMPROC)			qwglGetProcAddress("glDeleteProgram");
	qglGetShaderiv =				(PFNGLGETSHADERIVPROC)				qwglGetProcAddress("glGetShaderiv");
	qglGetProgramiv =				(PFNGLGETPROGRAMIVARBPROC)			qwglGetProcAddress("glGetProgramiv");
	qglGetShaderInfoLog =			(PFNGLGETSHADERINFOLOGPROC)			qwglGetProcAddress("glGetShaderInfoLog");
	qglGetProgramInfoLog =			(PFNGLGETPROGRAMINFOLOGPROC)		qwglGetProcAddress("glGetProgramInfoLog");
	qglShaderSource =				(PFNGLSHADERSOURCEPROC)				qwglGetProcAddress("glShaderSource");
	qglCompileShader =				(PFNGLCOMPILESHADERARBPROC)			qwglGetProcAddress("glCompileShader");
	qglAttachShader =				(PFNGLATTACHSHADERPROC)				qwglGetProcAddress("glAttachShader");
	qglDetachShader =				(PFNGLDETACHSHADERPROC)				qwglGetProcAddress("glDetachShader");
	qglLinkProgram =				(PFNGLLINKPROGRAMPROC)				qwglGetProcAddress("glLinkProgram");
	qglUseProgram =					(PFNGLUSEPROGRAMPROC)				qwglGetProcAddress("glUseProgram");
	qglVertexAttribPointer =		(PFNGLVERTEXATTRIBPOINTERPROC)				qwglGetProcAddress("glVertexAttribPointer");
	qglEnableVertexAttribArray =	(PFNGLENABLEVERTEXATTRIBARRAYARBPROC)		qwglGetProcAddress("glEnableVertexAttribArray");
	qglDisableVertexAttribArray =	(PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)		qwglGetProcAddress("glDisableVertexAttribArray");
	qglBindAttribLocation =			(PFNGLBINDATTRIBLOCATIONARBPROC)			qwglGetProcAddress("glBindAttribLocation");
	qglGetAttribLocation =			(PFNGLGETATTRIBLOCATIONARBPROC)				qwglGetProcAddress("glGetAttribLocation");
	qglGetActiveUniform =			(PFNGLGETACTIVEUNIFORMPROC)					qwglGetProcAddress("glGetActiveUniform");
	qglGetUniformLocation =			(PFNGLGETUNIFORMLOCATIONPROC)				qwglGetProcAddress("glGetUniformLocation");
	qglUniform1f =					(PFNGLUNIFORM1FPROC)				qwglGetProcAddress("glUniform1f");
	qglUniform2f =					(PFNGLUNIFORM2FPROC)				qwglGetProcAddress("glUniform2f");
	qglUniform3f =					(PFNGLUNIFORM3FPROC)				qwglGetProcAddress("glUniform3f");
	qglUniform4f =					(PFNGLUNIFORM4FPROC)				qwglGetProcAddress("glUniform4f");
	qglUniform1i =					(PFNGLUNIFORM1IPROC)				qwglGetProcAddress("glUniform1i");
	qglUniform2i =					(PFNGLUNIFORM2IPROC)				qwglGetProcAddress("glUniform2i");
	qglUniform3i =					(PFNGLUNIFORM3IPROC)				qwglGetProcAddress("glUniform3i");
	qglUniform4i =					(PFNGLUNIFORM4IPROC)				qwglGetProcAddress("glUniform4i");
	qglUniform1fv =					(PFNGLUNIFORM1FVPROC)				qwglGetProcAddress("glUniform1fv");
	qglUniform2fv =					(PFNGLUNIFORM2FVPROC)				qwglGetProcAddress("glUniform2fv");
	qglUniform3fv =					(PFNGLUNIFORM3FVPROC)				qwglGetProcAddress("glUniform3fv");
	qglUniform4fv =					(PFNGLUNIFORM4FVPROC)				qwglGetProcAddress("glUniform4fv");
	qglUniform1iv =					(PFNGLUNIFORM1IVPROC)				qwglGetProcAddress("glUniform1iv");
	qglUniform2iv =					(PFNGLUNIFORM2IVPROC)				qwglGetProcAddress("glUniform2iv");
	qglUniform3iv =					(PFNGLUNIFORM3IVPROC)				qwglGetProcAddress("glUniform3iv");
	qglUniform4iv =					(PFNGLUNIFORM4IVPROC)				qwglGetProcAddress("glUniform4iv");
	qglUniformMatrix2fv =			(PFNGLUNIFORMMATRIX2FVPROC)			qwglGetProcAddress("glUniformMatrix2fv");
	qglUniformMatrix3fv =			(PFNGLUNIFORMMATRIX3FVPROC)         qwglGetProcAddress("glUniformMatrix3fv");
	qglUniformMatrix4fv =			(PFNGLUNIFORMMATRIX4FVPROC)			qwglGetProcAddress("glUniformMatrix4fv");

	R_InitPrograms();
	}

	}
		
	}
			
	Com_Printf("\n");
	Com_Printf("=====================================\n");

	GL_SetDefaultState();
	GL_InitImages();
	Mod_Init();
	R_InitEngineTextures();
	R_LoadFont();

	flareEdit = (qboolean)qfalse;

	return 0;
}



/*
===============
R_Shutdown
===============
*/

void R_Shutdown(void)
{
	Cmd_RemoveCommand("modellist");
	Cmd_RemoveCommand("screenshot");
	Cmd_RemoveCommand("imagelist");
	Cmd_RemoveCommand("flaresStats");
	Cmd_RemoveCommand("dumpEntityString");
	Cmd_RemoveCommand("r_meminfo");	
	Cmd_RemoveCommand("low_spec");
	Cmd_RemoveCommand("medium_spec");
	Cmd_RemoveCommand("hi_spec");
	
	Cmd_RemoveCommand("saveLights");
	Cmd_RemoveCommand("spawnLight");
	Cmd_RemoveCommand("removeLight");
	Cmd_RemoveCommand("editLight");
	Cmd_RemoveCommand("spawnLightToCamera");
	Cmd_RemoveCommand("changeLightRadius");
	Cmd_RemoveCommand("cloneLight");
	Cmd_RemoveCommand("changeLightCone");
	Cmd_RemoveCommand("clearWorldLights");
	Cmd_RemoveCommand("unselectLight");
	Cmd_RemoveCommand("editFlare");
	Cmd_RemoveCommand("resetFlarePos");
	Cmd_RemoveCommand("copy");
	Cmd_RemoveCommand("paste");

	Mod_FreeAll();
	GL_ShutdownImages();

	if (fboId)
		qglDeleteFramebuffers (1, &fboId);
	if (fbo_weaponMask)
		qglDeleteFramebuffers (1, &fbo_weaponMask);

	R_ClearWorldLights();
	GLimp_Shutdown();
	QGL_Shutdown();
	ilShutDown();
	R_ShutdownPrograms();
	
	DeleteShadowVertexBuffers();

	qglDeleteBuffersARB(1, &vbo.vbo_fullScreenQuad);
	qglDeleteBuffersARB(1, &vbo.vbo_halfScreenQuad);
	qglDeleteBuffersARB(1, &vbo.vbo_quarterScreenQuad);
	qglDeleteBuffersARB(1, &vbo.ibo_quadTris);
	qglDeleteBuffersARB(1, &vbo.vbo_Dynamic);
	qglDeleteBuffersARB(1, &vbo.ibo_Dynamic);
	qglDeleteBuffersARB(1, &vbo.vbo_BSP);
}



/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/

void R_BeginFrame()
{
	
#ifndef _WIN32
    // there is no need to restart video mode with SDL
    if (r_fullScreen->modified) {
        R_SetMode();
		r_fullScreen->modified = qfalse;
    }
#endif

	/* 
	 ** change modes if necessary
	 */
	if (r_mode->modified || r_fullScreen->modified)
        vid_ref->modified = qtrue;
	
	if(r_dof->modified)
		r_dof->modified = qfalse;

	if(r_lightmapScale->modified)
		r_lightmapScale->modified = qfalse;

	if(r_lightmapScale->value >1)
		Cvar_SetValue("r_lightmapScale", 1);

	if (r_lightmapScale->value < 0)
		Cvar_SetValue("r_lightmapScale", 0);

	if (r_ssao->modified)
		r_ssao->modified = qfalse;
	
	if (r_reliefMapping->modified)
		r_reliefMapping->modified = qfalse;

	if (r_reliefMapping->value >1)
		Cvar_SetValue("r_reliefMapping", 1);

	if (r_textureMode->modified || r_anisotropic->modified) {
		GL_TextureMode(r_textureMode->string);

		if (r_textureMode->modified)
			r_textureMode->modified = qfalse;

		if (r_anisotropic->modified)
			r_anisotropic->modified = qfalse;
	}

	//go into 2D mode
	R_SetupOrthoMatrix();

	GL_Enable(GL_BLEND); // alpha blend for chars
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDrawBuffer( GL_BACK );

	GL_UpdateSwapInterval();

	R_Clear();
}

/*
=============
R_SetPalette
=============
*/
unsigned r_rawpalette[256];

void R_SetPalette(const unsigned char *palette)
{
	int i;

	byte *rp = (byte *) r_rawpalette;
	
	if (palette) {
		for (i = 0; i < 256; i++) {
			rp[i * 4 + 0] = palette[i * 3 + 0];
			rp[i * 4 + 1] = palette[i * 3 + 1];
			rp[i * 4 + 2] = palette[i * 3 + 2];
			rp[i * 4 + 3] = 0xff;
		}
	} else {
		for (i = 0; i < 256; i++) {
			rp[i * 4 + 0] = d_8to24table[i] & 0xff;
			rp[i * 4 + 1] = (d_8to24table[i] >> 8) & 0xff;
			rp[i * 4 + 2] = (d_8to24table[i] >> 16) & 0xff;
			rp[i * 4 + 3] = 0xff;
		}
	}
	qglClearColor(0, 0, 0, 0);
	qglClear(GL_COLOR_BUFFER_BIT);
}

/*
** R_DrawBeam
*/
void R_DrawBeam()
{
	return;
}
