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

cplane_t frustum[5];

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

int GL_MsgGLError(char* Info)
{
	char	S[1024];
	int		n = qglGetError();
	
	if(r_ignoreGlErrors->value)
		return false;

	if(n == GL_NO_ERROR) return false;

	switch(n) {        
		case GL_INVALID_ENUM: 
			sprintf(S, "%s GL_INVALID_ENUM An unacceptable value is specified for an enumerated argument. The offending command is ignored, having no side effect other than to set the error flag.\n",Info);
			break;
		case GL_INVALID_VALUE: 
			sprintf(S, "%s GL_INVALID_VALUE A numeric argument is out of range. The offending command is ignored, having no side effect other than to set the error flag.\n",Info);
			break;
		case GL_INVALID_OPERATION: 
			sprintf(S, "%s GL_INVALID_OPERATION The specified operation is not allowed in the current state. The offending command is ignored, having no side effect other than to set the error flag.\n",Info);
			break;
		case GL_STACK_OVERFLOW: 
			sprintf(S, "%s GL_STACK_OVERFLOW This command would cause a stack overflow. The offending command is ignored, having no side effect other than to set the error flag.\n",Info);
			break;
		case GL_STACK_UNDERFLOW: 
			sprintf(S, "%s GL_STACK_UNDERFLOW This command would cause a stack underflow. The offending command is ignored, having no side effect other than to set the error flag.\n",Info);
			break;
		case GL_OUT_OF_MEMORY: 
			sprintf(S, "%s GL_OUT_OF_MEMORY There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n",Info);
			break;
		default: 
			sprintf(S, "UNKNOWN GL ERROR\n");
			break;
	}

	Con_Printf(PRINT_ALL, S);

	return n;
}


void R_RotateForEntity(entity_t * e) {
	mat4_t entViewMatrix;

	AnglesToMat3(e->angles, e->axis);
	Mat4_SetOrientation(e->matrix, e->axis, e->origin);
	Mat4_TransposeMultiply(e->matrix, r_newrefdef.modelViewMatrix, entViewMatrix);
	GL_LoadMatrix(GL_MODELVIEW, entViewMatrix);
}


/*
=============================================================

  SPRITE MODELS

=============================================================
*/


void R_DrawSpriteModel(entity_t * e)
{

	vec3_t point;
	dsprframe_t *frame;
	float *up, *right;
	dsprite_t *psprite;

	if (e->flags & RF_DISTORT)
		return;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache

	psprite = (dsprite_t *) currentmodel->extraData;
	e->frame %= psprite->numFrames;
	frame = &psprite->frames[e->frame];

	// normal sprite
	up = vup;
	right = vright;

	GL_DepthMask(false);
	
	GL_Enable(GL_BLEND);
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GL_Bind(currentmodel->skins[e->frame]->texnum);

	qglBegin(GL_QUADS);

	qglTexCoord2f(0, 1);
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, -frame->origin_x, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(0, 0);
	VectorMA(e->origin, frame->height - frame->origin_y, up, point);
	VectorMA(point, -frame->origin_x, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 0);
	VectorMA(e->origin, frame->height - frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 1);
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	qglVertex3fv(point);

	qglEnd();

	GL_Disable(GL_BLEND);
	GL_DepthMask(true);
}


static void R_DrawDistortSpriteModel(entity_t * e)
{
	dsprframe_t *frame;
	float		*up, *right;
	dsprite_t	*psprite;
	int			id;
	unsigned	defBits = 0;
	unsigned	Index[MAX_INDICES];
	int			vert=0, index=0;
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

	GL_Enable(GL_BLEND);
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	defBits = worldDefs.AlphaMaskBits;

	// setup program
	GL_BindProgram(refractProgram, defBits);
	id = refractProgram->id[defBits];
		
	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);

	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, wVertexArray);	
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, wTexArray);
	
	GL_MBind(GL_TEXTURE0_ARB, r_distort->texnum);
	qglUniform1i(qglGetUniformLocation(id, "u_deformMap"), 0);
	GL_MBind(GL_TEXTURE1_ARB, currentmodel->skins[e->frame]->texnum);
	qglUniform1i(qglGetUniformLocation(id, "u_colorMap"), 1);
	GL_MBindRect(GL_TEXTURE2_ARB, ScreenMap->texnum);
	qglUniform1i(qglGetUniformLocation(id, "g_colorBufferMap"), 2);
	GL_MBindRect(GL_TEXTURE3_ARB, depthMap->texnum);
	qglUniform1i(qglGetUniformLocation(id, "g_depthBufferMap"), 3);
			
	qglUniform1f(qglGetUniformLocation(id, "u_deformMul"),	2.5);
	qglUniform1f(qglGetUniformLocation(id, "u_alpha"),	e->alpha);
	qglUniform1f(qglGetUniformLocation(id, "u_thickness"),	len*0.5);
	qglUniform1f(qglGetUniformLocation(id, "u_thickness2"),	frame->height * 0.5);
	qglUniform2f(qglGetUniformLocation(id, "u_viewport"),	vid.width, vid.height);
	qglUniform2f(qglGetUniformLocation(id, "u_depthParms"), r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
	qglUniform2f(qglGetUniformLocation(id, "u_mask"),	0.0, 1.0);

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

	Index[index++] = vert+0;
	Index[index++] = vert+1;
	Index[index++] = vert+3;
	Index[index++] = vert+3;
	Index[index++] = vert+1;
	Index[index++] = vert+2;
			
	vert+=4;

	
	if(vert)
	{
		if(gl_state.DrawRangeElements && r_DrawRangeElements->value)
			qglDrawRangeElementsEXT(GL_TRIANGLES, 0, vert, index, GL_UNSIGNED_INT, Index);
		else
			qglDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, Index);
	}

	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
	GL_BindNullProgram();
	GL_Disable(GL_BLEND);
}

//==================================================================================

/*
=============
R_DrawNullModel
=============
*/
void R_DrawNullModel(void)
{
	int i;

	if (currententity->flags & (RF_VIEWERMODEL))
		return;

	qglPushMatrix();
	R_RotateForEntity(currententity);

	qglDisable(GL_TEXTURE_2D);
	GL_Color3f(1, 0, 1);

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3f(0, 0, -8);
	for (i = 0; i <= 4; i++)
		qglVertex3f(8 * cos(i * M_PI * 0.5f), 8 * sin(i * M_PI * 0.5f),
					0);
	qglEnd();

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3f(0, 0, 8);
	for (i = 4; i >= 0; i--)
		qglVertex3f(8 * cos(i * M_PI * 0.5f), 8 * sin(i * M_PI * 0.5f),
					0);
	qglEnd();

	GL_Color3f(1, 1, 1);
	qglPopMatrix();
	qglEnable(GL_TEXTURE_2D);
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
		
		if (r_viewcluster == -1)
			qglClear(GL_COLOR_BUFFER_BIT);
		
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
		outMap = true;
	else
		outMap = false;

	for (i = 0; i < 4; i++)
		v_blend[i] = r_newrefdef.blend[i];

	// clear out the portion of the screen that the NOWORLDMODEL defines
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL) {
		GL_Enable(GL_SCISSOR_TEST);
		GL_Scissor(r_newrefdef.viewport[0], r_newrefdef.viewport[1], r_newrefdef.viewport[2], r_newrefdef.viewport[3]);

		if (!(r_newrefdef.rdflags & RDF_NOCLEAR)) {
			qglClearColor(0.35, 0.35, 0.35, 1);
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			qglClearColor(0.35, 0.35, 0.35, 1);
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
static void R_SetupViewMatrices (void) {
	float	xMin, xMax, xDiv;
	float	yMin, yMax, yDiv;
	float	zNear, zFar, zDiv;

	// setup perspective projection matrix
	zNear = max(r_zNear->value, 3.0);
	zFar = 0.0;			// infinite

	r_newrefdef.depthParms[0] = zNear;
	r_newrefdef.depthParms[1] = 0.9995f;
	
	xMax = zNear * tan(DEG2RAD(r_newrefdef.fov_x) * 0.5);
	xMin = -xMax;

	yMax = zNear * tan(DEG2RAD(r_newrefdef.fov_y) * 0.5);
	yMin = -yMax;

	xDiv = 1.0f / (xMax - xMin);
	yDiv = 1.0f / (yMax - yMin);
	zDiv = 1.0f / (zFar - zNear);

	r_newrefdef.projectionMatrix[0][0] = 2.0f * zNear * xDiv;
	r_newrefdef.projectionMatrix[0][1] = 0.0f;
	r_newrefdef.projectionMatrix[0][2] = 0.0f;
	r_newrefdef.projectionMatrix[0][3] = 0.0f;
	r_newrefdef.projectionMatrix[1][0] = 0.0f;
	r_newrefdef.projectionMatrix[1][1] = 2.0f * zNear * yDiv;
	r_newrefdef.projectionMatrix[1][2] = 0.0f;
	r_newrefdef.projectionMatrix[1][3] = 0.0f;
	r_newrefdef.projectionMatrix[2][0] = (xMax + xMin) * xDiv;
	r_newrefdef.projectionMatrix[2][1] = (yMax + yMin) * yDiv;
	r_newrefdef.projectionMatrix[2][2] = -0.999f;
	r_newrefdef.projectionMatrix[2][3] = -1.0f;
	r_newrefdef.projectionMatrix[3][0] = 0.0f;
	r_newrefdef.projectionMatrix[3][1] = 0.0f;
	r_newrefdef.projectionMatrix[3][2] = -2.0f * zNear;
	r_newrefdef.projectionMatrix[3][3] = 0.0f;

	if (zFar > zNear) {
		r_newrefdef.projectionMatrix[2][2] = -(zNear + zFar) * zDiv;
		r_newrefdef.projectionMatrix[3][2] = -2.0f * zNear * zFar * zDiv;
	}

	// setup view matrix
	AnglesToMat3(r_newrefdef.viewangles, r_newrefdef.axis);

	r_newrefdef.modelViewMatrix[0][0] = -r_newrefdef.axis[1][0];
	r_newrefdef.modelViewMatrix[0][1] =  r_newrefdef.axis[2][0];
	r_newrefdef.modelViewMatrix[0][2] = -r_newrefdef.axis[0][0];
	r_newrefdef.modelViewMatrix[0][3] = 0.0;

	r_newrefdef.modelViewMatrix[1][0] = -r_newrefdef.axis[1][1];
	r_newrefdef.modelViewMatrix[1][1] =	 r_newrefdef.axis[2][1];
	r_newrefdef.modelViewMatrix[1][2] = -r_newrefdef.axis[0][1];
	r_newrefdef.modelViewMatrix[1][3] = 0.0;

	r_newrefdef.modelViewMatrix[2][0] = -r_newrefdef.axis[1][2];
	r_newrefdef.modelViewMatrix[2][1] =  r_newrefdef.axis[2][2];
	r_newrefdef.modelViewMatrix[2][2] = -r_newrefdef.axis[0][2];
	r_newrefdef.modelViewMatrix[2][3] = 0.0;

	r_newrefdef.modelViewMatrix[3][0] = DotProduct(r_newrefdef.vieworg, r_newrefdef.axis[1]);
	r_newrefdef.modelViewMatrix[3][1] = -DotProduct(r_newrefdef.vieworg, r_newrefdef.axis[2]);
	r_newrefdef.modelViewMatrix[3][2] = DotProduct(r_newrefdef.vieworg, r_newrefdef.axis[0]);
	r_newrefdef.modelViewMatrix[3][3] = 1.0;
	
	// load matrices
	GL_LoadMatrix(GL_PROJECTION, r_newrefdef.projectionMatrix); // q2 r_project_matrix
	GL_LoadMatrix(GL_MODELVIEW, r_newrefdef.modelViewMatrix); // q2 r_world_matrix
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




void R_DrawEntitiesOnList(void)
{
	int i;

	if (!r_drawEntities->value)
		return;

	// draw non-transparent first
	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];

		if (currententity->flags & RF_TRANSLUCENT)
			continue;			// solid

		if ( r_newrefdef.rdflags & RDF_IRGOGGLES) 
				goto jump;

			if (currententity->flags & RF_DISTORT)
				continue;
jump:

		if (currententity->flags & RF_BEAM) {
			R_DrawBeam();
		} else {
			currentmodel = currententity->model;
			if (!currentmodel) {
				R_DrawNullModel();
				continue;
			}
			switch (currentmodel->type) {
			case mod_alias:
				R_DrawAliasModel(currententity, false);
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

	// draw transparent entities
	// we could sort these if it ever becomes a problem...
	GL_DepthMask(0);			// no z writes

	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];

		if (!(currententity->flags & RF_TRANSLUCENT))
			continue;			// solid

		if ( r_newrefdef.rdflags & RDF_IRGOGGLES) 
				goto next;

			if (currententity->flags & RF_DISTORT)
				continue;
next:

		if (currententity->flags & RF_BEAM) {
			R_DrawBeam();
		} else {
			currentmodel = currententity->model;

			if (!currentmodel) {
				R_DrawNullModel();
				continue;
			}
			switch (currentmodel->type) {
			case mod_alias:
				R_DrawAliasModel(currententity, false);
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
	GL_DepthMask(1);			// back to writing
	
}

void R_DrawPlayerWeaponFBO(void)
{
	int i;

	if (!r_drawEntities->value)
		return;

	qglBindFramebuffer(GL_FRAMEBUFFER, gl_state.fbo_weaponMask);
	qglClear(GL_COLOR_BUFFER_BIT);
	qglClearColor(0, 0, 0, 0);

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

		R_DrawAliasModel(currententity, true);
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
			R_DrawAliasModelLightPass(true);
		}

}

void R_DrawLightInteractions(void)
{
	int i;
	
	if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	num_visLights = 0;

	GL_DepthMask(0);
	GL_Enable(GL_BLEND);
	GL_BlendFunc(GL_ONE, GL_ONE);

	if(r_useLightScissors->value)
		GL_Enable(GL_SCISSOR_TEST);
	
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Enable(GL_DEPTH_BOUNDS_TEST_EXT);

	if(r_shadows->value)
		GL_Enable(GL_STENCIL_TEST);
	
	R_PrepareShadowLightFrame();
	
	if(shadowLight_frame) {

	for(currentShadowLight = shadowLight_frame; currentShadowLight; currentShadowLight = currentShadowLight->next) {

	if (r_skipStaticLights->value && currentShadowLight->isStatic)
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

//	if (!R_DrawLightOccluders())
//		continue;

	R_DebugScissors();

	R_CastBspShadowVolumes();		// bsp and bmodels shadows
	R_DrawPlayerWeaponLightPass();	// shade player weapon only from bsp!
	R_CastAliasShadowVolumes();		// alias models shadows
	R_DrawLightWorld();				// light world
	R_DrawLightFlare();				// light flare

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
			R_DrawAliasModelLightPass(false);
		}
	num_visLights++;
	}
	}
	
	GL_DepthMask(1);
	GL_Disable(GL_STENCIL_TEST);
	GL_Disable(GL_SCISSOR_TEST);
	if(gl_state.depthBoundsTest && r_useDepthBounds->value)
		GL_Disable(GL_DEPTH_BOUNDS_TEST_EXT);
	GL_Disable(GL_BLEND);
}

void R_RenderDistortModels(void)
{
	int i;

	GL_DepthMask(0);
	
	for (i = 0; i < r_newrefdef.num_entities; i++) {
		currententity = &r_newrefdef.entities[i];
		currentmodel = currententity->model;

		if (!currentmodel)
			continue;

		if ( r_newrefdef.rdflags & RDF_IRGOGGLES) 
				continue;

		if (!(currententity->flags & RF_DISTORT))
			continue;

		if (currentmodel->type == mod_alias)
			R_DrawAliasDistortModel(currententity);
		else if (currentmodel->type == mod_sprite)
			R_DrawDistortSpriteModel(currententity);


	}
	GL_DepthMask(1);

}


/*
================
R_RenderView

r_newrefdef must be set before the first call
================
*/

void R_RenderView(refdef_t * fd)
{
	if (r_noRefresh->value)
		return;
	
	r_newrefdef = *fd;
	r_newrefdef.viewport[0] = fd->x;
	r_newrefdef.viewport[1] = vid.height - fd->height - fd->y;
	r_newrefdef.viewport[2] = fd->width;
	r_newrefdef.viewport[3] = fd->height;
	qglViewport(r_newrefdef.viewport[0], r_newrefdef.viewport[1], r_newrefdef.viewport[2], r_newrefdef.viewport[3]);

	if (!r_worldmodel && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		VID_Error(ERR_DROP, "R_RenderView: NULL worldmodel");

	if (r_finish->value)
		qglFinish();

	R_SetupFrame();
	R_SetFrustum();
	R_SetupViewMatrices();
	R_SetupGL();
	R_MarkLeaves();				// done here so we know if we're in water

	R_DrawBSP();
	R_DrawEntitiesOnList();
	R_CaptureDepthBuffer();
	R_DrawLightInteractions();
	R_RenderDecals();

	R_RenderFlares();
	R_LightScale();
	R_CaptureColorBuffer();
	R_DrawAlphaPoly();
	R_DrawParticles();
	R_CaptureColorBuffer();
	R_RenderDistortModels();
	R_CaptureColorBuffer();
	R_DrawPlayerWeaponFBO();
}


void R_SetGL2D(void)
{
	// set 2D virtual screen size

	qglViewport(0, 0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);

	qglLoadIdentity();
	qglOrtho(0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();
	GL_Disable(GL_DEPTH_TEST);
	GL_Disable(GL_CULL_FACE);
}


/*
====================
R_SetLightLevel

====================
*/
void R_SetLightLevel(void)
{
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
void R_DrawFullScreenQuad();


void R_RenderFrame(refdef_t * fd, qboolean client)
{
	
	R_SetLightLevel();
	R_RenderView(fd);
	R_SetGL2D();

	// post processing - cut off if player camera out map bounds
	if(!outMap){
	R_FXAA();
	R_RadialBlur();
	R_ThermalVision();
	R_DofBlur();
	R_Bloom();
	R_MotionBlur();
	R_FilmGrain();
	}
	
	// set alpha blend for 2d mode
	GL_Enable(GL_BLEND); 
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (v_blend[3] && r_polyBlend->value) {
		
		qglDisable(GL_TEXTURE_2D);
		GL_Color4f(v_blend[0],v_blend[1],v_blend[2], 0.15);

		qglBegin(GL_QUADS);
		qglVertex2f(0, 0);
		qglVertex2f(vid.width, 0);
		qglVertex2f(vid.width, vid.height);
		qglVertex2f(0, vid.height);
		qglEnd();

		GL_Color4f(1, 1, 1, 1);
		qglEnable(GL_TEXTURE_2D);

		}

	if(selectedShadowLight && r_lightEditor->value){
	GL_Color3f(0,1,1);
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

	GL_Color3f(1,1,1);
	}

	GL_MsgGLError("R_RenderFrame: ");
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
	
	if (strstr(gl_config.extensions_string, "GL_NVX_gpu_memory_info")) {
						
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

	} else 
		if (strstr(gl_config.extensions_string, "GL_ATI_meminfo")) {
		
        Com_Printf("\nATI/AMD specific memory info:\n");
		Com_Printf("\n");
        qglGetIntegerv (VBO_FREE_MEMORY_ATI, mem);
        Com_Printf("VBO: total memory free in the pool %i MB\n", mem[0] >> 10);
        Com_Printf("VBO: largest available free block in the pool %i MB\n", mem[1] >> 10);
        Com_Printf("VBO: total auxiliary memory free %i MB\n", mem[2] >> 10);
        Com_Printf("VBO: largest auxiliary free block %i MB\n", mem[3] >> 10);

        qglGetIntegerv (TEXTURE_FREE_MEMORY_ATI, mem);
        Com_Printf("Texture: total memory free in the pool %i MB\n", mem[0] >> 10);
        Com_Printf("Texture: largest available free block in the pool %i MB\n", mem[1] >> 10);
        Com_Printf("Texture: total auxiliary memory free %i MB\n", mem[2] >> 10);
        Com_Printf("Texture: largest auxiliary free block %i MB\n", mem[3] >> 10);

        qglGetIntegerv (RENDERBUFFER_FREE_MEMORY_ATI, mem);
        Com_Printf("RenderBuffer: total memory free in the pool %i MB\n", mem[0] >> 10);
        Com_Printf("RenderBuffer: largest available free block in the pool %i MB\n", mem[1] >> 10);
        Com_Printf("RenderBuffer: total auxiliary memory free %i MB\n", mem[2] >> 10);
        Com_Printf("RenderBuffer: largest auxiliary free block %i MB\n", mem[3] >> 10);
    }
	else
#endif
		Com_Printf("MemInfo not availabled for your video card or driver!\n");
}

void R_LowSpecMachine_f(void)
{
Cvar_Set("r_textureCompression", "1");
Cvar_Set("r_maxTextureSize", "256");
Cvar_Set("r_anisotropic", "1");
Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_NEAREST");

Cvar_Set("r_shadows", "0");
Cvar_Set("r_drawFlares", "1");
Cvar_Set("r_parallax", "0");
Cvar_Set("r_skipStaticLights", "1");
Cvar_Set("r_pplWorldAmbient", "1.0");
Cvar_Set("r_bloom", "0");
Cvar_Set("r_dof", "0");
Cvar_Set("r_radialBlur", "0");
Cvar_Set("r_softParticles", "0");
Cvar_Set("r_motionBlur", "0");

vid_ref->modified = true;
}

void R_MediumSpecMachine_f(void)
{
Cvar_Set("r_textureCompression", "1");
Cvar_Set("r_maxTextureSize", "0");
Cvar_Set("r_anisotropic", "8");
Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR");

Cvar_Set("r_shadows", "1");
Cvar_Set("r_drawFlares", "1");
Cvar_Set("r_parallax", "1");
Cvar_Set("r_skipStaticLights", "0");
Cvar_Set("r_pplWorldAmbient", "0.5");
Cvar_Set("r_bloom", "1");
Cvar_Set("r_dof", "0");
Cvar_Set("r_radialBlur", "1");
Cvar_Set("r_softParticles", "1");
Cvar_Set("r_motionBlur", "0");

vid_ref->modified = true;
}

void R_HiSpecMachine_f(void)
{
Cvar_Set("r_textureCompression", "0");
Cvar_Set("r_maxTextureSize", "0");
Cvar_Set("r_anisotropic", "16");
Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR");

Cvar_Set("r_shadows", "1");
Cvar_Set("r_drawFlares", "1");
Cvar_Set("r_parallax", "2");
Cvar_Set("r_skipStaticLights", "0");
Cvar_Set("r_pplWorldAmbient", "0.5");
Cvar_Set("r_bloom", "1");
Cvar_Set("r_dof", "1");
Cvar_Set("r_radialBlur", "1");
Cvar_Set("r_softParticles", "1");
Cvar_Set("r_fxaa", "1");
Cvar_Set("r_motionBlur", "1");

vid_ref->modified = true;
}

void R_ChangeLightColor_f(void) ;

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
	r_dynamic =							Cvar_Get("r_dynamic", "1", 0);
	r_noBind =							Cvar_Get("r_noBind", "0", 0);
	r_polyBlend =						Cvar_Get("r_polyBlend", "1", CVAR_ARCHIVE);
	r_lockPvs =							Cvar_Get("r_lockPvs", "0", 0);

	r_vsync =							Cvar_Get("r_vsync", "0", CVAR_ARCHIVE);
	r_finish =							Cvar_Get("r_finish", "0", 0);
	
	r_fullScreen =						Cvar_Get("r_fullScreen", "1", CVAR_ARCHIVE);
	
	r_gamma =							Cvar_Get("r_gamma", "1.5", CVAR_ARCHIVE);
	r_brightness	=					Cvar_Get("r_brightness", "1", CVAR_ARCHIVE);
	r_contrast	=						Cvar_Get("r_contrast", "1", CVAR_ARCHIVE);
	r_saturation =						Cvar_Get("r_saturation", "1", CVAR_ARCHIVE);

	vid_ref =							Cvar_Get("vid_ref", "xpgl", CVAR_ARCHIVE);
	r_displayRefresh =					Cvar_Get("r_displayRefresh", "0", CVAR_ARCHIVE);

	r_shadows =							Cvar_Get("r_shadows", "1", CVAR_ARCHIVE);
	r_shadowWorldLightScale =			Cvar_Get("r_shadowWorldLightScale", "12", CVAR_ARCHIVE);
	r_playerShadow =					Cvar_Get("r_playerShadow", "1", CVAR_ARCHIVE);

	r_anisotropic =						Cvar_Get("r_anisotropic", "16", CVAR_ARCHIVE);
	r_maxAnisotropy =					Cvar_Get("r_maxAnisotropy", "0", 0);
	r_maxTextureSize=					Cvar_Get("r_maxTextureSize", "0", CVAR_ARCHIVE);
	r_worldColorScale =					Cvar_Get("r_worldColorScale", "2", CVAR_ARCHIVE);
	r_picsColorScale =					Cvar_Get("r_picsColorScale", "2", CVAR_ARCHIVE);
	r_textureCompression =				Cvar_Get("r_textureCompression", "0", CVAR_ARCHIVE);			
	r_causticIntens =					Cvar_Get("r_causticIntens", "2.0", CVAR_ARCHIVE);
	r_textureMode =						Cvar_Get("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE);

	r_screenShot =						Cvar_Get("r_screenShot", "jpg", CVAR_ARCHIVE);
	r_screenShotJpegQuality =			Cvar_Get("r_screenShotJpegQuality", "99", CVAR_ARCHIVE);
	r_screenShotGamma =					Cvar_Get("r_screenShotGamma", "1.0", CVAR_ARCHIVE);
	r_screenShotContrast =				Cvar_Get("r_screenShotContrast", "1.0", CVAR_ARCHIVE);

	r_radarSize =						Cvar_Get("r_radarSize", "256", CVAR_ARCHIVE);
	r_radarZoom =						Cvar_Get("r_radarZoom", "1", CVAR_ARCHIVE);
	r_radar =							Cvar_Get("r_radar", "0", CVAR_ARCHIVE);
	
	r_arbSamples =						Cvar_Get("r_arbSamples", "1", CVAR_ARCHIVE);
	r_fxaa =							Cvar_Get("r_fxaa", "0", CVAR_ARCHIVE);

	deathmatch =						Cvar_Get("deathmatch", "0", CVAR_SERVERINFO);
	
	r_drawFlares =						Cvar_Get("r_drawFlares", "1", CVAR_ARCHIVE);
	r_flaresIntens =					Cvar_Get("r_flaresIntens", "3", CVAR_ARCHIVE);
	r_flareWeldThreshold =				Cvar_Get("r_flareWeldThreshold", "32", CVAR_ARCHIVE);

	r_customWidth =						Cvar_Get("r_customWidth", "1024", CVAR_ARCHIVE);
	r_customHeight =					Cvar_Get("r_customHeight", "500", CVAR_ARCHIVE);

	sys_priority =						Cvar_Get("sys_priority", "0", CVAR_ARCHIVE);
		
	r_DrawRangeElements	=				Cvar_Get("r_DrawRangeElements","1",CVAR_ARCHIVE);
			
	hunk_bsp=							Cvar_Get("hunk_bsp", "20", CVAR_ARCHIVE);
	hunk_model=							Cvar_Get("hunk_model", "2.4", CVAR_ARCHIVE);
	hunk_sprite=						Cvar_Get("hunk_sprite", "0.08", CVAR_ARCHIVE);
	
//	r_vbo=								Cvar_Get("r_vbo", "1", CVAR_ARCHIVE);

	r_parallax=							Cvar_Get("r_parallax", "2", CVAR_ARCHIVE);
	r_parallaxScale=					Cvar_Get("r_parallaxScale", "2.0", CVAR_ARCHIVE);

	r_skipStaticLights =				Cvar_Get("r_skipStaticLights", "0", CVAR_ARCHIVE);
	r_pplWorldAmbient =					Cvar_Get("r_pplWorldAmbient", "0.5", CVAR_ARCHIVE);
	r_useLightScissors = 				Cvar_Get("r_useLightScissors", "1", 0);
	r_useDepthBounds =					Cvar_Get("r_useDepthBounds", "1", 0);
	r_debugLightScissors =				Cvar_Get("r_debugLightScissors", "0", 0);
	r_tbnSmoothAngle =					Cvar_Get("r_tbnSmoothAngle", "65", CVAR_ARCHIVE);
	r_lightsWeldThreshold =				Cvar_Get("r_lightsWeldThreshold", "40", CVAR_ARCHIVE);
//	r_debugLights =						Cvar_Get("r_debugLights", "0", 0);
//	r_occLightBoundsSize =				Cvar_Get("r_occLightBoundsSize", "0.75", CVAR_ARCHIVE);
//	r_debugOccLightBoundsSize =			Cvar_Get("r_debugOccLightBoundsSize", "0.75", 0);
	r_lightScale =						Cvar_Get("r_lightScale", "1", CVAR_ARCHIVE);
	r_specularScale =					Cvar_Get("r_specularScale", "1", CVAR_ARCHIVE);
	r_toksvigFactor =					Cvar_Get("r_toksvigFactor", "0.5", CVAR_ARCHIVE);

	r_zNear =							Cvar_Get("r_zNear", "3", CVAR_ARCHIVE);

	r_bloom =							Cvar_Get("r_bloom", "1", CVAR_ARCHIVE);
	r_bloomThreshold =					Cvar_Get("r_bloomThreshold", "0.75", CVAR_ARCHIVE);
	r_bloomIntens =						Cvar_Get("r_bloomIntens", "2.0", CVAR_ARCHIVE);
	r_bloomBright =						Cvar_Get("r_bloomBright", "0.9", CVAR_ARCHIVE);
	r_bloomExposure =					Cvar_Get("r_bloomExposure", "0.86", CVAR_ARCHIVE);
	r_bloomStarIntens =					Cvar_Get("r_bloomStarIntens", "3.0", CVAR_ARCHIVE);

	r_dof =								Cvar_Get("r_dof", "1", CVAR_ARCHIVE);
	r_dofBias =							Cvar_Get("r_dofBias", "0.002", CVAR_ARCHIVE);
	r_dofFocus =						Cvar_Get("r_dofFocus", "0.0", CVAR_ARCHIVE);

	r_motionBlur =						Cvar_Get("r_motionBlur", "1", CVAR_ARCHIVE);
	r_motionBlurSamples	=				Cvar_Get("r_motionBlurSamples", "16", CVAR_ARCHIVE);
	r_motionBlurFrameLerp =				Cvar_Get("r_motionBlurFrameLerp", "10", CVAR_ARCHIVE);

	r_radialBlur =						Cvar_Get("r_radialBlur", "1", CVAR_ARCHIVE);
	r_radialBlurFov =                   Cvar_Get("r_radialBlurFov", "30", CVAR_ARCHIVE);
	r_softParticles =					Cvar_Get("r_softParticles", "1", CVAR_ARCHIVE);
	r_filmGrain = 						Cvar_Get("r_filmGrain", "0", CVAR_ARCHIVE);
	r_ignoreGlErrors =					Cvar_Get("r_ignoreGlErrors", "1", 0);
	
	r_lightEditor =						Cvar_Get("r_lightEditor", "0", 0);
	r_CameraSpaceLightMove =			Cvar_Get("r_CameraSpaceLightMove", "0", CVAR_ARCHIVE);

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
	Cmd_AddCommand("copyLight",					R_Light_Copy_f);
	Cmd_AddCommand("changeLightCone",			R_ChangeLightCone_f);
	Cmd_AddCommand("clearWorldLights",          R_ClearWorldLights);
	Cmd_AddCommand("unselectLight",				R_Light_UnSelect_f);
	Cmd_AddCommand("editFlare",					R_FlareEdit_f);
	Cmd_AddCommand("resetFlarePos",				R_ResetFlarePos_f);
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

	r_fullScreen->modified = false;
	r_mode->modified = false;

    err = GLimp_SetMode(&vid.width, &vid.height, r_mode->value, fullscreen);

    // success, update variables
	if (err == rserr_ok) {
        Cvar_SetValue("r_fullScreen", gl_state.fullscreen);
        r_fullScreen->modified = false;
		gl_state.prev_mode = r_mode->value;
        return true;

    // try without fullscreen
	} else if (err == rserr_invalid_fullscreen) {
        Com_Printf(S_COLOR_RED "ref_xpgl::R_SetMode() - fullscreen unavailable in this mode\n");
        if ((err = GLimp_SetMode(&vid.width, &vid.height, r_mode->value, false)) == rserr_ok) {
            Cvar_SetValue("r_fullScreen", 0);
            r_fullScreen->modified = false;
            gl_state.prev_mode = r_mode->value;
            return true;
        }

    } else if (err == rserr_invalid_mode) {
        Com_Printf(S_COLOR_RED"ref_xpgl::R_SetMode() - invalid mode\n");
    }

    // revert to previous mode
    if (GLimp_SetMode(&vid.width, &vid.height, gl_state.prev_mode, false) == rserr_ok) {
        Cvar_SetValue("r_mode", gl_state.prev_mode);
        r_mode->modified = false;
        Cvar_SetValue("r_fullScreen", 0);
        r_fullScreen->modified = false;
        return true;
    } else {
        Com_Printf(S_COLOR_RED"ref_xpgl::R_SetMode() - could not revert to safe mode\n");
        return false;
    }
}

static void DevIL_Init() {
    static qboolean init = false;

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

    init = true;
}


/*
===============
R_Init
===============
*/
int GL_QueryBits;
//int flareQueries[MAX_WORLD_SHADOW_LIHGTS];
int lightsQueries[MAX_WORLD_SHADOW_LIHGTS];

int R_Init(void *hinstance, void *hWnd)
{
	char	vendor_buffer[1000];
	int		aniso_level, max_aniso;

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
	Com_Printf(S_COLOR_WHITE "GL_VENDOR:" S_COLOR_GREEN "   %s\n", gl_config.vendor_string);
	gl_config.renderer_string = (const char*)qglGetString(GL_RENDERER);
	Com_Printf(S_COLOR_WHITE "GL_RENDERER:" S_COLOR_GREEN " %s\n", gl_config.renderer_string);
	gl_config.version_string = (const char*)qglGetString(GL_VERSION);
	Com_Printf(S_COLOR_WHITE "GL_VERSION:" S_COLOR_GREEN "  %s\n", gl_config.version_string);
	gl_config.extensions_string = (const char*)qglGetString(GL_EXTENSIONS);
	
	
	strcpy(vendor_buffer, gl_config.vendor_string);
	strlwr(vendor_buffer);
	
	{
	// check GL version /:-#)
	float version = atof(gl_config.version_string);

	if (version < 2.0){
		Com_Printf(S_COLOR_RED"Quake2xp requires OpenGL version 2.0 or higher.\nProbably your graphics card is unsupported or the drivers are not up-to-date.\nCurrent GL version is %3.1f\n", version);
		VID_Error(ERR_FATAL,  "Quake2xp requires OpenGL version 2.0 or higher.\nProbably your graphics card is unsupported or the drivers are not up-to-date.\nCurrent GL version is %3.1f\n", version);
		}
	}
/*
	if (strstr(vendor_buffer, "intel")) // fuck the intel lol
		{
		if(r_allowIntel->value){
		Com_Printf(S_COLOR_RED"Intel graphics card detected. Renderer may be unstable and slow.\n");
		}else{
		Com_Printf(S_COLOR_RED"Intel graphics card is unsupported.\n");
		VID_Error(ERR_FATAL,  "Intel graphics card is unsupported.\n");
		}
	}
*/
	Com_DPrintf(S_COLOR_WHITE "GL_EXTENSIONS:\n"); 
	Com_DPrintf(S_COLOR_YELLOW"%s\n", gl_config.extensions_string);
	
	Cvar_Set("scr_drawall", "0");

	Com_Printf("\n");
	Com_Printf("=====================================\n");
	Com_Printf(S_COLOR_GREEN"Checking Basic Quake II XP Extensions\n");
	Com_Printf("=====================================\n");
	Com_Printf("\n");

	if (strstr(gl_config.extensions_string, "GL_ARB_multitexture")) {
		Com_Printf("...using GL_ARB_multitexture\n");

		qglActiveTextureARB =		(PFNGLACTIVETEXTUREARBPROC)			qwglGetProcAddress("glActiveTextureARB");
		qglClientActiveTextureARB =	(PFNGLCLIENTACTIVETEXTUREARBPROC)	qwglGetProcAddress("glClientActiveTextureARB");
	
	} else {
		Com_Printf(S_COLOR_RED"...GL_ARB_multitexture not found\n");
		VID_Error(ERR_FATAL, "GL_ARB_multitexture not found!");
	}

	qglGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	Cvar_SetValue("r_maxAnisotropy", max_aniso);
	if (r_anisotropic->value >= r_maxAnisotropy->value)
		Cvar_SetValue("r_anisotropic", r_maxAnisotropy->value);

	aniso_level = r_anisotropic->value;
	if (strstr
		(gl_config.extensions_string,
		 "GL_EXT_texture_filter_anisotropic")) {
		if (r_anisotropic->value == 1) {
			Com_Printf(S_COLOR_YELLOW"...ignoring GL_EXT_texture_filter_anisotropic\n");

		} else {

			Com_Printf("...using GL_EXT_texture_filter_anisotropic\n   ["S_COLOR_GREEN"%i"S_COLOR_WHITE" max] ["S_COLOR_GREEN"%i" S_COLOR_WHITE" selected]\n",
					   max_aniso, aniso_level);

		}
	} else {
		Com_Printf(S_COLOR_RED "...GL_EXT_texture_filter_anisotropic not found\n");
		r_anisotropic = Cvar_Set("r_anisotropic", "1");
		r_maxAnisotropy = Cvar_Set("r_maxAnisotropy", "0");
	}


	gl_state.texture_compression_arb = false;
	if (strstr(gl_config.extensions_string, "GL_ARB_texture_compression"))
		if (!r_textureCompression->value) {
			Com_Printf(S_COLOR_YELLOW"...ignoring GL_ARB_texture_compression\n");
			gl_state.texture_compression_arb = false;
		} else {
			Com_Printf("...using GL_ARB_texture_compression\n");
			gl_state.texture_compression_arb = true;

	} else {
		Com_Printf(S_COLOR_RED"...GL_ARB_texture_compression not found\n");
		gl_state.texture_compression_arb = false;
		r_textureCompression = Cvar_Set("r_textureCompression", "0");
	}

	if (strstr(gl_config.extensions_string, "GL_ARB_texture_cube_map"))
		Com_Printf("...using GL_ARB_texture_cube_map\n");
	else 
		Com_Printf(S_COLOR_RED"...GL_ARB_texture_cube_map not found\n");

	if (strstr(gl_config.extensions_string, "GL_ARB_seamless_cube_map")){
		Com_Printf("...using GL_ARB_seamless_cube_map\n");
		qglEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	else
		Com_Printf(S_COLOR_RED"...GL_ARB_seamless_cube_map not found\n");

	// ===========================================================================================================================

		gl_state.DrawRangeElements = false;
	if (strstr(gl_config.extensions_string, "GL_EXT_draw_range_elements")) {
		gl_state.DrawRangeElements = true;
		qglDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC) qwglGetProcAddress("glDrawRangeElementsEXT");
		
		if(!r_DrawRangeElements->value)
		Com_Printf(S_COLOR_YELLOW"...ignoring GL_EXT_draw_range_elements\n");
		else
		Com_Printf("...using GL_EXT_draw_range_elements\n");
				
		} else {
		Com_Printf(S_COLOR_RED"...GL_EXT_draw_range_elements not found\n");
		gl_state.DrawRangeElements = false;
		r_DrawRangeElements = Cvar_Set("r_DrawRangeElements", "0");
		}

	// openGL 2.0 Unified Separate Stencil
	qglStencilFuncSeparate		= (PFNGLSTENCILFUNCSEPARATEPROC)	qwglGetProcAddress("glStencilFuncSeparate");
	qglStencilOpSeparate		= (PFNGLSTENCILOPSEPARATEPROC)		qwglGetProcAddress("glStencilOpSeparate");
	qglStencilMaskSeparate		= (PFNGLSTENCILMASKSEPARATEPROC)	qwglGetProcAddress("glStencilMaskSeparate");
	
	gl_state.separateStencil = false;
	if(qglStencilFuncSeparate && qglStencilOpSeparate && qglStencilMaskSeparate){
			Com_Printf("...using GL_EXT_stencil_two_side\n");
			gl_state.separateStencil = true;
	
	}else
		Com_Printf(S_COLOR_RED"...GL_EXT_stencil_two_side not found\n");

	gl_state.depthBoundsTest = false;
	if (strstr(gl_config.extensions_string, "GL_EXT_depth_bounds_test")) {
	Com_Printf("...using GL_EXT_depth_bounds_test\n");

	glDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC) qwglGetProcAddress("glDepthBoundsEXT");
	gl_state.depthBoundsTest = true;
	} else {
		Com_Printf(S_COLOR_RED"...GL_EXT_depth_bounds_test not found\n");
	gl_state.depthBoundsTest = false;
	}
	/*
	gl_state.arb_occlusion = false;
	gl_state.arb_occlusion2 = false;
	if (strstr(gl_config.extensions_string, "GL_ARB_occlusion_query")) {
		Com_Printf("...using GL_ARB_occlusion_query\n");
		gl_state.arb_occlusion = true;
		
		qglGenQueriesARB		=	(PFNGLGENQUERIESARBPROC) qwglGetProcAddress("glGenQueriesARB");
		qglDeleteQueriesARB		=	(PFNGLDELETEQUERIESARBPROC) qwglGetProcAddress("glDeleteQueriesARB");
		qglIsQueryARB			=	(PFNGLISQUERYARBPROC) qwglGetProcAddress("glIsQueryARB");
		qglBeginQueryARB		=	(PFNGLBEGINQUERYARBPROC) qwglGetProcAddress("glBeginQueryARB");
		qglEndQueryARB			=	(PFNGLENDQUERYARBPROC) qwglGetProcAddress("glEndQueryARB");
		qglGetQueryivARB		=	(PFNGLGETQUERYIVARBPROC) qwglGetProcAddress("glGetQueryivARB");
		qglGetQueryObjectivARB	=	(PFNGLGETQUERYOBJECTIVARBPROC) qwglGetProcAddress("glGetQueryObjectivARB");
		qglGetQueryObjectuivARB =	(PFNGLGETQUERYOBJECTUIVARBPROC) qwglGetProcAddress("glGetQueryObjectuivARB");

		qglGetQueryivARB(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &GL_QueryBits);
		
		if (GL_QueryBits) {
	//		qglGenQueriesARB(MAX_FLARES, (GLuint*)flareQueries);
			qglGenQueriesARB(MAX_WORLD_SHADOW_LIHGTS, (GLuint*)lightsQueries);

			Com_Printf("   Found "S_COLOR_GREEN "%i" S_COLOR_WHITE " occlusion query bits\n", GL_QueryBits);

			if (strstr(gl_config.extensions_string, "GL_ARB_occlusion_query2")){
				Com_Printf("...using GL_ARB_occlusion_query2\n");
				gl_state.arb_occlusion2 = true;
			}
			else{
				Com_Printf(S_COLOR_RED"...GL_ARB_occlusion_query2 not found\n");
				gl_state.arb_occlusion2 = false;
			}
		}
		if(gl_state.arb_occlusion2)
			gl_state.query_passed = GL_ANY_SAMPLES_PASSED;
		else
			gl_state.query_passed = GL_SAMPLES_PASSED;

		} else {
		Com_Printf(S_COLOR_RED"...GL_ARB_occlusion_query not found\n");
		gl_state.arb_occlusion = false;
	}

*/
	gl_state.nPot = false;
	if (strstr
		(gl_config.extensions_string, "GL_ARB_texture_non_power_of_two")) {
		Com_Printf("...using GL_ARB_texture_non_power_of_two\n");
		gl_state.nPot = true;
	} else {
		Com_Printf(S_COLOR_RED "...GL_ARB_texture_non_power_of_two not found\n");
		gl_state.nPot = false;
	}
	
	
	if (strstr (gl_config.extensions_string, "GL_ARB_texture_rectangle")) {
		Com_Printf("...using GL_ARB_texture_rectangle\n");
		
		} else {
		Com_Printf(S_COLOR_RED "...GL_ARB_texture_rectangle not found\n");
		
	}
	

	if (strstr(gl_config.extensions_string, "GL_ARB_vertex_buffer_object")) {
			
		qglBindBuffer =		(PFNGLBINDBUFFERPROC)		qwglGetProcAddress("glBindBuffer");
		qglDeleteBuffers =	(PFNGLDELETEBUFFERSPROC)	qwglGetProcAddress("glDeleteBuffers");
		qglGenBuffers =		(PFNGLGENBUFFERSPROC)		qwglGetProcAddress("glGenBuffers");
		qglBufferData =		(PFNGLBUFFERDATAPROC)		qwglGetProcAddress("glBufferData");
		qglBufferSubData =	(PFNGLBUFFERSUBDATAPROC)	qwglGetProcAddress("glBufferSubData");
		qglMapBuffer =		(PFNGLMAPBUFFERPROC)		qwglGetProcAddress("glMapBuffer");
		qglUnmapBuffer =	(PFNGLUNMAPBUFFERPROC)		qwglGetProcAddress("glUnmapBuffer");

		if (qglGenBuffers && qglBindBuffer && qglBufferData && qglDeleteBuffers && qglBufferSubData){
			vec2_t		tmpVerts[4];	
			Com_Printf("...using GL_ARB_vertex_buffer_object\n");
			// precalc screen quads for postprocessing
			// full quad
			VA_SetElem2(tmpVerts[0],0 ,			vid.height);
			VA_SetElem2(tmpVerts[1],vid.width,	vid.height);
			VA_SetElem2(tmpVerts[2],vid.width,	0);
			VA_SetElem2(tmpVerts[3],0,			0);
			qglGenBuffers(1, &gl_state.vbo_fullScreenQuad);
			qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_fullScreenQuad);
			qglBufferData(GL_ARRAY_BUFFER_ARB, sizeof(vec2_t)*4, tmpVerts, GL_STATIC_DRAW_ARB);
			qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

			// half quad
			VA_SetElem2(tmpVerts[0],0,					vid.height);
			VA_SetElem2(tmpVerts[1],vid.width * 0.5 ,	vid.height);
			VA_SetElem2(tmpVerts[2],vid.width * 0.5 ,	vid.height * 0.5);
			VA_SetElem2(tmpVerts[3],0,					vid.height * 0.5);
			qglGenBuffers(1, &gl_state.vbo_halfScreenQuad);
			qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_halfScreenQuad);
			qglBufferData(GL_ARRAY_BUFFER_ARB, sizeof(vec2_t)*4, tmpVerts, GL_STATIC_DRAW_ARB);
			qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

			// quater quad
			VA_SetElem2(tmpVerts[0],0,						vid.height);
			VA_SetElem2(tmpVerts[1],vid.width * 0.25 ,		vid.height);
			VA_SetElem2(tmpVerts[2],vid.width * 0.25 ,		vid.height * 0.25);
			VA_SetElem2(tmpVerts[3],0,						vid.height * 0.25);
			qglGenBuffers(1, &gl_state.vbo_quarterScreenQuad);
			qglBindBuffer(GL_ARRAY_BUFFER_ARB, gl_state.vbo_quarterScreenQuad);
			qglBufferData(GL_ARRAY_BUFFER_ARB, sizeof(vec2_t)*4, tmpVerts, GL_STATIC_DRAW_ARB);
			qglBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

			qglGenBuffers(1, &gl_state.vbo_Dynamic);
			qglGenBuffers(1, &gl_state.ibo_Dynamic);
	
		}
	} else {
		Com_Printf(S_COLOR_RED "...GL_ARB_vertex_buffer_object not found\n");
	}

/*	gl_state.conditional_render = false;
			
	glBeginConditionalRenderNV	= (PFNGLBEGINCONDITIONALRENDERNVPROC)	qwglGetProcAddress("glBeginConditionalRenderNV");
	glEndConditionalRenderNV	= (PFNGLENDCONDITIONALRENDERNVPROC)		qwglGetProcAddress("glEndConditionalRenderNV");

	glBeginConditionalRender	= (PFNGLBEGINCONDITIONALRENDERPROC)		qwglGetProcAddress("glBeginConditionalRender");
	glEndConditionalRender		= (PFNGLENDCONDITIONALRENDERPROC)		qwglGetProcAddress("glEndConditionalRender");

	if(glBeginConditionalRender && glEndConditionalRender){
			Com_Printf("...using GL_conditional_render\n");
				gl_state.conditional_render = true;

	} else {
		Com_Printf(S_COLOR_RED"...GL_conditional_render not found\n");
		gl_state.conditional_render = false;		
	}
	*/
	if (strstr(gl_config.extensions_string, "GL_ARB_draw_buffers")) {
		qglDrawBuffers =	(PFNGLDRAWBUFFERSARBPROC) qwglGetProcAddress("glDrawBuffersARB");
		
	if (qglDrawBuffers)
		Com_Printf("...using GL_ARB_draw_buffers\n");
	else
		Com_Printf(S_COLOR_RED"...using GL_ARB_draw_buffers not found\n");
	}

	if (strstr(gl_config.extensions_string, "GL_ARB_texture_float")) 
		Com_Printf("...using GL_ARB_texture_float\n");
	else
		Com_Printf(S_COLOR_RED"...using GL_ARB_texture_float not found\n");


	if (strstr(gl_config.extensions_string, "GL_ARB_framebuffer_object")) {
		Com_Printf("...using GL_ARB_framebuffer_object\n");  

		qglGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,	&gl_state.maxRenderBufferSize);
		qglGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,	&gl_state.maxColorAttachments);
		qglGetIntegerv(GL_MAX_SAMPLES,				&gl_state.maxSamples);

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
	}
	else {
		Com_Printf(S_COLOR_RED"...GL_ARB_framebuffer_object not found\n");
	}

	gl_state.glsl = false;	
	if ( strstr( gl_config.extensions_string, "GL_ARB_shading_language_100" ) )
	{
	if ( strstr( gl_config.extensions_string, "GL_ARB_fragment_shader" ) )
		{
		Com_Printf("...using GL_ARB_fragment_shader\n");

		if ( strstr( gl_config.extensions_string, "GL_ARB_vertex_shader" ) ){
			Com_Printf("...using GL_ARB_vertex_shader\n");
	gl_state.glsl = true;	
	
	gl_state.shader5= false;

	if ( strstr( gl_config.extensions_string, "GL_ARB_gpu_shader5" ) ){
			Com_Printf("...using GL_ARB_gpu_shader5\n");
	gl_state.shader5= true;
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
		
	} else {
		Com_Printf(S_COLOR_RED"...GLSL render not supported\n");
		gl_state.glsl = false;
		VID_Error(ERR_FATAL, "...GLSL render not supported!");
	}
			
	Com_Printf("\n");
	Com_Printf("=====================================\n");

	GL_SetDefaultState();
	GL_InitImages();
	Mod_Init();
	R_InitEngineTextures();
	R_LoadFont();

	GL_MsgGLError("Init GL Errors: ");

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
	Cmd_RemoveCommand("moveSelectedLight_right");
	Cmd_RemoveCommand("moveSelectedLight_forward");
	Cmd_RemoveCommand("moveSelectedLight_z");
	Cmd_RemoveCommand("spawnLightToCamera");
	Cmd_RemoveCommand("changeLightRadius");
	Cmd_RemoveCommand("copyLight");
	Cmd_RemoveCommand("changeLightCone");
	Cmd_RemoveCommand("clearWorldLights");
	Cmd_RemoveCommand("unselectLight");
	Cmd_RemoveCommand("editFlare");
	Cmd_RemoveCommand("resetFlarePos");

	Mod_FreeAll();
	GL_ShutdownImages();

	R_ClearWorldLights();
	GLimp_Shutdown();
	QGL_Shutdown();
	ilShutDown();
	R_ShutdownPrograms();
		    
//	if(qglDeleteQueriesARB){
//	qglDeleteQueriesARB(MAX_FLARES, (GLuint*)flareQueries);
//	qglDeleteQueriesARB(MAX_WORLD_SHADOW_LIHGTS, (GLuint*)lightsQueries);
//	}
	
	DeleteShadowVertexBuffers();

	qglDeleteBuffers(1, &gl_state.vbo_fullScreenQuad);
	qglDeleteBuffers(1, &gl_state.vbo_halfScreenQuad);
	qglDeleteBuffers(1, &gl_state.vbo_quarterScreenQuad);
	qglDeleteBuffers(1, &gl_state.vbo_Dynamic);
	qglDeleteBuffers(1, &gl_state.ibo_Dynamic);
	qglDeleteBuffers(1, &gl_state.vbo_BSP);
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
		r_fullScreen->modified = false;
    }
#endif

	/* 
	 ** change modes if necessary
	 */
	if (r_mode->modified || r_fullScreen->modified)
        vid_ref->modified = true;

	// realtime update
	if(r_softParticles->modified)
		r_softParticles->modified = false;
	
	if(r_dof->modified)
		r_dof->modified = false;

	if(r_pplWorldAmbient->modified)
		r_pplWorldAmbient->modified = false;

	if(r_pplWorldAmbient->value >1)
		Cvar_SetValue("r_pplWorldAmbient", 1);

	/* 
	** go into 2D mode
	 */
	qglViewport(0, 0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);
	qglLoadIdentity();
	qglOrtho(0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();
	GL_Disable(GL_DEPTH_TEST);
	GL_Disable(GL_CULL_FACE);
	GL_Enable(GL_BLEND); // alpha blend for chars
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDrawBuffer( GL_BACK );

	/* 
	 ** texturemode stuff
	 */
	// Realtime set level of anisotropy filtering
	if (r_textureMode->modified || r_anisotropic->modified) {
		GL_TextureMode(r_textureMode->string);

		if (r_textureMode->modified)
			r_textureMode->modified = false;

		if (r_anisotropic->modified)
			r_anisotropic->modified = false;
	}
	
	/* 
	 ** swapinterval stuff
	 */
	GL_UpdateSwapInterval();

	// 
	// clear screen if desired
	// 
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
	qglClearColor(0.35, 0.35, 0.35, 1);
	qglClear(GL_COLOR_BUFFER_BIT);
	qglClearColor(0.35, 0.35, 0.35, 1);
	
}

/*
** R_DrawBeam
*/
void R_DrawBeam()
{
	return;
}
