/*
Copyright (C) 2004-2011 Quake2xp Team.

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
// shadows.c: shadow functions



#include "r_local.h"

typedef float vec4_t[4];
vec4_t s_lerped[MAX_VERTS];
float shadelight[3];

/*
===============
SHADOW VOLUMES
===============
*/
vec3_t		ShadowArray[MAX_SHADOW_VERTS];
unsigned	ShadowIndex[MAX_INDICES];

qboolean	triangleFacingLight	[MAX_INDICES / 3];


void GL_LerpVerts(int nverts, dtrivertx_t *v, dtrivertx_t *ov, dtrivertx_t *verts, float *lerp, float move[3], float frontv[3], float backv[3])
{
	int i;

		for (i = 0; i < nverts; i++, v++, ov++, lerp += 4) {
			lerp[0] = move[0] + ov->v[0]*backv[0] + v->v[0]*frontv[0];
			lerp[1] = move[1] + ov->v[1]*backv[1] + v->v[1]*frontv[1];
			lerp[2] = move[2] + ov->v[2]*backv[2] + v->v[2]*frontv[2];
		}
}

void R_MarkShadowTriangles(dmdl_t *paliashdr, dtriangle_t *tris, vec3_t lightOrg){
	
	vec3_t	r_triangleNormals[MAX_INDICES / 3];
	vec3_t	temp, dir0, dir1;
	int		i;
	float	f;
	float	*v0, *v1, *v2;

		
	for (i = 0; i < paliashdr->num_tris; i++, tris++) {
		
			v0 = (float*)s_lerped[tris->index_xyz[0]];
			v1 = (float*)s_lerped[tris->index_xyz[1]];
			v2 = (float*)s_lerped[tris->index_xyz[2]];
		
		//Calculate shadow volume triangle normals
		VectorSubtract( v0, v1, dir0 );
		VectorSubtract( v2, v1, dir1 );
		
		CrossProduct( dir0, dir1, r_triangleNormals[i] );

		// Find front facing triangles
		VectorSubtract(lightOrg, v0, temp);
		f = DotProduct(temp, r_triangleNormals[i]);

		triangleFacingLight[i] = f > 0;
			
	}

}

void BuildShadowVolumeTriangles(dmdl_t * hdr, vec3_t light, float projectdistance)
{
	dtriangle_t *ot, *tris;
	neighbors_t *neighbors;
	int i, j, shadow_vert = 0, index = 0;
	vec3_t v0, v1, v2, v3, l0, l1, l2, l3;
	daliasframe_t *frame;
	dtrivertx_t *verts;
	vec3_t	offset0, offset1, offset2;
	float cap_offset = r_shadowCapOffset->value;
	
	frame = (daliasframe_t *) ((byte *) hdr + hdr->ofs_frames
							   + currententity->frame * hdr->framesize);
	verts = frame->verts;

	ot = tris = (dtriangle_t *) ((unsigned char *) hdr + hdr->ofs_tris);
	
	R_MarkShadowTriangles(hdr, tris, light);

	for (i = 0, tris = ot, neighbors = currentmodel->neighbors; i < hdr->num_tris; i++, tris++, neighbors++) {
		
		if (!triangleFacingLight[i])
			continue;
	
		if (neighbors->n[0] < 0 || !triangleFacingLight[neighbors->n[0]]) {
			
			for (j = 0; j < 3; j++) {
				v0[j] = s_lerped[tris->index_xyz[1]][j];
				v1[j] = s_lerped[tris->index_xyz[0]][j];

				offset0[j] = v0[j] - light[j];
				offset1[j] = v1[j] - light[j];
			}

			VectorNormalize(offset0);
			VectorNormalize(offset1);
			VectorMA(v0, cap_offset, offset0, v0);	
			VectorMA(v1, cap_offset, offset1, v1);	
			VectorSubtract(v1, light, l2);
			VectorSubtract(v0, light, l3);
			VectorNormalize(l2);
			VectorNormalize(l3);
			VectorMA(v1, projectdistance, l2, v2);
			VectorMA(v0, projectdistance, l3, v3);


		VA_SetElem3(ShadowArray[shadow_vert+0], v0[0], v0[1], v0[2]);
		VA_SetElem3(ShadowArray[shadow_vert+1], v1[0], v1[1], v1[2]);
		VA_SetElem3(ShadowArray[shadow_vert+2], v2[0], v2[1], v2[2]);
		VA_SetElem3(ShadowArray[shadow_vert+3], v3[0], v3[1], v3[2]);
            

		ShadowIndex[index++] = shadow_vert+0;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+3;
		ShadowIndex[index++] = shadow_vert+3;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+2;
		shadow_vert +=4;
		}

		if (neighbors->n[1] < 0 || !triangleFacingLight[neighbors->n[1]]) {
			
			for (j = 0; j < 3; j++) {
				v0[j] = s_lerped[tris->index_xyz[2]][j];
				v1[j] = s_lerped[tris->index_xyz[1]][j];

				offset0[j] = v0[j] - light[j];
				offset1[j] = v1[j] - light[j];
			}

			VectorNormalize(offset0);
			VectorNormalize(offset1);
			VectorMA(v0, cap_offset, offset0, v0);					
			VectorMA(v1, cap_offset, offset1, v1);	
			VectorSubtract(v1, light, l2);
			VectorSubtract(v0, light, l3);
			VectorNormalize(l2);
			VectorNormalize(l3);
			VectorMA(v1, projectdistance, l2, v2);
			VectorMA(v0, projectdistance, l3, v3);


		VA_SetElem3(ShadowArray[shadow_vert+0], v0[0], v0[1], v0[2]);
		VA_SetElem3(ShadowArray[shadow_vert+1], v1[0], v1[1], v1[2]);
		VA_SetElem3(ShadowArray[shadow_vert+2], v2[0], v2[1], v2[2]);
		VA_SetElem3(ShadowArray[shadow_vert+3], v3[0], v3[1], v3[2]);
            

		ShadowIndex[index++] = shadow_vert+0;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+3;
		ShadowIndex[index++] = shadow_vert+3;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+2;
		shadow_vert +=4;
		}

		if (neighbors->n[2] < 0 || !triangleFacingLight[neighbors->n[2]]) {
			
			for (j = 0; j < 3; j++) {
				v0[j] = s_lerped[tris->index_xyz[0]][j];
				v1[j] = s_lerped[tris->index_xyz[2]][j];

				offset0[j] = v0[j] - light[j];
				offset1[j] = v1[j] - light[j];
			}

			VectorNormalize(offset0);
			VectorNormalize(offset1);
			VectorMA(v0, cap_offset, offset0, v0);					
			VectorMA(v1, cap_offset, offset1, v1);	
			VectorSubtract(v1, light, l2);
			VectorSubtract(v0, light, l3);
			VectorNormalize(l2);
			VectorNormalize(l3);
			VectorMA(v1, projectdistance, l2, v2);
			VectorMA(v0, projectdistance, l3, v3);
	

	
		VA_SetElem3(ShadowArray[shadow_vert+0], v0[0], v0[1], v0[2]);
		VA_SetElem3(ShadowArray[shadow_vert+1], v1[0], v1[1], v1[2]);
		VA_SetElem3(ShadowArray[shadow_vert+2], v2[0], v2[1], v2[2]);
		VA_SetElem3(ShadowArray[shadow_vert+3], v3[0], v3[1], v3[2]);
            

		ShadowIndex[index++] = shadow_vert+0;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+3;
		ShadowIndex[index++] = shadow_vert+3;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+2;
		shadow_vert +=4;
		}
	}

	 // build shadows caps
	for (i = 0, tris = ot; i < hdr->num_tris; i++, tris++)
	{
		if (!triangleFacingLight[i])
			continue;
		
			for (j = 0; j < 3; j++)
			{
				v0[j] = s_lerped[tris->index_xyz[0]][j];
				v1[j] = s_lerped[tris->index_xyz[1]][j];
				v2[j] = s_lerped[tris->index_xyz[2]][j];

				offset0[j] = v0[j] - light[j];
				offset1[j] = v1[j] - light[j];
				offset2[j] = v2[j] - light[j];
			}

			VectorNormalize(offset0);
			VectorNormalize(offset1);
			VectorNormalize(offset2);

			for (j = 0; j < 3; j++)
			{
				v0[j] += offset0[j] * cap_offset;
				v1[j] += offset1[j] * cap_offset;
				v2[j] += offset2[j] * cap_offset;
			}

		VA_SetElem3(ShadowArray[shadow_vert+0], v0[0], v0[1], v0[2]);
		VA_SetElem3(ShadowArray[shadow_vert+1], v1[0], v1[1], v1[2]);
		VA_SetElem3(ShadowArray[shadow_vert+2], v2[0], v2[1], v2[2]);
			
		ShadowIndex[index++] = shadow_vert+0;
		ShadowIndex[index++] = shadow_vert+1;
		ShadowIndex[index++] = shadow_vert+2;
        shadow_vert +=3;

			// rear cap (with flipped winding order)

			for (j = 0; j < 3; j++) {
				v0[j] = s_lerped[tris->index_xyz[0]][j];
				v1[j] = s_lerped[tris->index_xyz[1]][j];
				v2[j] = s_lerped[tris->index_xyz[2]][j];

				offset0[j] = v0[j] - light[j];
				offset1[j] = v1[j] - light[j];
				offset2[j] = v2[j] - light[j];
			}

			VectorNormalize(offset0);
			VectorNormalize(offset1);
			VectorNormalize(offset2);
			VectorMA(v0, cap_offset, offset0, v0);					
			VectorMA(v1, cap_offset, offset1, v1);					
			VectorMA(v2, cap_offset, offset2, v2);				
			VectorSubtract(v0, light, l0);
			VectorSubtract(v1, light, l1);
			VectorSubtract(v2, light, l2);
			VectorNormalize(l0);
			VectorNormalize(l1);
			VectorNormalize(l2);
			VectorMA(v0, projectdistance, l0, v0);
			VectorMA(v1, projectdistance, l1, v1);
			VectorMA(v2, projectdistance, l2, v2);

			
		VA_SetElem3(ShadowArray[shadow_vert+0], v0[0], v0[1], v0[2]);
		VA_SetElem3(ShadowArray[shadow_vert+1], v1[0], v1[1], v1[2]);
		VA_SetElem3(ShadowArray[shadow_vert+2], v2[0], v2[1], v2[2]);
				 
		ShadowIndex[index++] = shadow_vert+2; 
		ShadowIndex[index++] = shadow_vert+1; 
		ShadowIndex[index++] = shadow_vert+0; 
		shadow_vert +=3;
	}

	if(gl_state.DrawRangeElements && r_DrawRangeElements->value)
		qglDrawRangeElementsEXT(GL_TRIANGLES, 0, shadow_vert, index, GL_UNSIGNED_INT, ShadowIndex);
		else
		qglDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, ShadowIndex);
			
	c_shadow_tris += index/3;
}


void GL_RenderVolumes(dmdl_t * paliashdr, vec3_t lightdir, int projdist){

	qglDisable(GL_CULL_FACE);
	
	qglStencilOpSeparate(GL_BACK, GL_KEEP,  GL_INCR_WRAP_EXT, GL_KEEP);
	qglStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP);
	
	BuildShadowVolumeTriangles(paliashdr, lightdir, projdist);
	
	qglEnable(GL_CULL_FACE);
	c_shadow_volumes++;
}


void GL_DrawAliasShadowVolume(dmdl_t * paliashdr)
{
	vec3_t		light, temp;
	int			i, worldlight = 0, dlight = 0;
	dlight_t	*dl;
	flare_t		*lightSurf;
	float		dist, projdist;
	mat3_t		entityAxis;
	trace_t		r_trace;

	if (r_newrefdef.vieworg[2] < (currententity->origin[2] - 10))
		return;

//============================================================================
	dl = r_newrefdef.dlights;

	if (r_shadows->value >3) {

		for (i = 0; i < r_newrefdef.num_dlights; i++, dl++) {
			
			if(VectorCompare(dl->origin, currententity->origin))
			   continue;

			VectorSubtract(currententity->origin, dl->origin, temp);
			dist = VectorLength(temp);

			if (dist > dl->intensity)
				continue;		// big distance!

			// light surf behind the wall 
			if (r_newrefdef.areabits){
			r_trace = CM_BoxTrace(currententity->origin, dl->origin, vec3_origin, vec3_origin, r_worldmodel->firstnode, MASK_OPAQUE);
			if(r_trace.fraction != 1.0)
				continue;
			}

			AnglesToMat3(currententity->angles, entityAxis);
			VectorSubtract(dl->origin, currententity->origin, temp);
			Mat3_TransposeMultiplyVector(entityAxis, temp, light);	
			projdist = (dl->intensity + currententity->model->radius - dist) * 2.5;									
			GL_RenderVolumes(paliashdr, light, projdist);
			worldlight++;
			dlight++;
		}
	}
//============================================================================

	if (r_shadows->value >2) {

		for (i = 0; i < r_numflares; i++) {
			int sidebit;
			float viewplane;
						
			lightSurf = &r_flares[i];
						
			if(dlight)
				continue;
			
			// PVS coolling 
			if (r_newrefdef.areabits){
				if (!(r_newrefdef.areabits[lightSurf->area >> 3] & (1 << (lightSurf->area & 7)))){
					continue;
				}
			}
			if (!HasSharedLeafs (lightSurf->vis, viewvis))
				continue;
			
			if(lightSurf->ignore)
				continue;

			VectorSubtract(lightSurf->origin, currententity->origin, temp);
			dist = VectorNormalize(temp);
			if (dist > lightSurf->size * r_shadowWorldLightScale->value)
				continue;		// big distance!

			viewplane = DotProduct(currententity->origin, lightSurf->surf->plane->normal) - lightSurf->surf->plane->dist;
			
			if (viewplane >= 0)
				sidebit = 0;
			else
				sidebit = SURF_PLANEBACK;

			if ((lightSurf->surf->flags & SURF_PLANEBACK) != sidebit)
					continue;		// wrong light poly side!
			
			// light surf behind the wall 
			if (r_newrefdef.areabits){
			r_trace = CM_BoxTrace(currententity->origin, lightSurf->origin, vec3_origin, vec3_origin, r_worldmodel->firstnode, MASK_OPAQUE);
			if(r_trace.fraction != 1.0)
			continue;
			}
			AnglesToMat3(currententity->angles, entityAxis);
			VectorSubtract(lightSurf->origin, currententity->origin, temp);
			Mat3_TransposeMultiplyVector(entityAxis, temp, light);	
			light[2] += currententity->model->maxs[2] + 56;
			projdist = ((currententity->model->radius+lightSurf->size * r_shadowWorldLightScale->value) - dist) * 2.5;
			worldlight++;
			
		}
	}
//============================================================================
	if (!worldlight) {

		VectorSet(light, currententity->origin[0], currententity->origin[1], currententity->origin[2]);
		light[2] += 200;
		AnglesToMat3(currententity->angles, entityAxis);
		VectorSubtract(light, currententity->origin, temp);
		Mat3_TransposeMultiplyVector(entityAxis, temp, light);	
		projdist = currententity->model->radius + 25;	
		GL_RenderVolumes(paliashdr, light, projdist);

	}
	if (worldlight && !dlight)
		GL_RenderVolumes(paliashdr, light, projdist);

}




/*==============
Vis's CullSphere
==============*/

qboolean R_CullSphere( const vec3_t centre, const float radius, const int clipflags )
{
	int		i;
	cplane_t *p;

	if (r_noCull->value)
		return false;

	for (i=0,p=frustum ; i<4; i++,p++)
	{
		if ( !(clipflags & (1<<i)) ) {
			continue;
		}

		if ( DotProduct ( centre, p->normal ) - p->dist <= -radius )
			return true;
	}

	return false;
}

void R_DrawShadowVolume(entity_t * e)
{
	dmdl_t			*paliashdr;
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t		*v, *ov, *verts;
	int				*order, i;
	float			frontlerp, rad;
	vec3_t			move, delta, vectors[3], frontv, backv, tmp, water;

	VectorAdd(e->origin, currententity->model->maxs, water); 
	if(CL_PMpointcontents(water) & MASK_WATER)
		return;
	
	VectorSubtract(currententity->model->maxs, currententity->model->mins, tmp);
	VectorScale (tmp, 1.666, tmp); // Hail Satan!
	rad = VectorLength (tmp);
	
	if( R_CullSphere( e->origin, rad, 15 ) )
		return;

	paliashdr = (dmdl_t *) currentmodel->extradata;

	frame = (daliasframe_t *) ((byte *) paliashdr   + paliashdr->ofs_frames
						        + currententity->frame *
							  paliashdr->framesize);
	verts = v = frame->verts;

	oldframe =
		(daliasframe_t *) ((byte *) paliashdr + paliashdr->ofs_frames +
						   currententity->oldframe * paliashdr->framesize);
	ov = oldframe->verts;

	order = (int *) ((byte *) paliashdr + paliashdr->ofs_glcmds);

	frontlerp = 1.0 - currententity->backlerp;

	// move should be the delta back to the previous frame * backlerp
	VectorSubtract(currententity->oldorigin, currententity->origin, delta);
	AngleVectors(currententity->angles, vectors[0], vectors[1],
				 vectors[2]);

	move[0] = DotProduct(delta, vectors[0]);	// forward
	move[1] = -DotProduct(delta, vectors[1]);	// left
	move[2] = DotProduct(delta, vectors[2]);	// up

	VectorAdd(move, oldframe->translate, move);

	for (i = 0; i < 3; i++) {
		move[i] =
			currententity->backlerp * move[i] +
			frontlerp * frame->translate[i];
		frontv[i] = frontlerp * frame->scale[i];
		backv[i] = currententity->backlerp * oldframe->scale[i];
	}

	GL_LerpVerts(paliashdr->num_xyz, v, ov, verts, s_lerped[0], move, frontv, backv);
		
		qglPushMatrix();
		qglDisable(GL_TEXTURE_2D);
		e->angles[PITCH] = -e->angles[PITCH];	// sigh.
		R_RotateForEntity (e);
		e->angles[PITCH] = -e->angles[PITCH];	// sigh.

		GL_DrawAliasShadowVolume(paliashdr);
		
		qglEnable(GL_TEXTURE_2D);
		qglPopMatrix();
}


/*
===================
Soft shadow volumes
===================
*/
void R_ShadowBlend()
{
	float shadowalpha;
	int id;
	unsigned	defBits = 0;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (r_shadows->value < 2)
		return;

	shadowalpha = 1.0 - r_pplWorldAmbient->value;
	
	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();
	qglOrtho(0, 1, 1, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
	qglPushMatrix();
	qglLoadIdentity();

	qglDepthMask(0);
	qglDepthFunc(GL_ALWAYS);

	qglEnable(GL_STENCIL_TEST);
	qglStencilFunc(GL_EQUAL, 128, 255);
	qglStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	qglStencilMask(0);

	qglColorMask(0, 0, 0, 1);
	qglColor4f(0.0, 0.0, 0.0, 0.0);

	// bind shadow mask texture and render stencil marks in to alpha chanel
	GL_SelectTexture(GL_TEXTURE0_ARB);	
	GL_BindRect(shadowMask->texnum);
    
	qglBegin(GL_TRIANGLES);
	qglVertex2f(-5, -5);
	qglVertex2f(10, -5);
	qglVertex2f(-5, 10);
	qglEnd();
	
	qglCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, 0, 0, vid.width, vid.height);
	
	qglDisable(GL_STENCIL_TEST);
	qglColorMask(1.0, 1.0, 1.0, 1.0);
	qglColor4f(1.0, 1.0, 1.0, 1.0);
		
	//blur alpha mask and visualize it!
	GL_Blend(true, 0, 0);
	GL_BindProgram(shadowProgram, defBits);
	id = shadowProgram->id[defBits];
	
	GL_SelectTexture(GL_TEXTURE0_ARB);
	GL_BindRect(shadowMask->texnum);
	
	qglUniform1i(qglGetUniformLocation(id, "u_mask"), 0);
	qglUniform1f(qglGetUniformLocation(id, "u_alpha"), shadowalpha);
	qglUniform2f(qglGetUniformLocation(id, "u_screenSize"), vid.width, vid.height);

	qglBegin(GL_TRIANGLES);
	qglVertex2f(-5, -5);
	qglVertex2f(10, -5);
	qglVertex2f(-5, 10);
	qglEnd();
	
	GL_Blend(false, 0, 0);
	GL_BindNullProgram();
	GL_SelectTexture(GL_TEXTURE0_ARB);
	qglDepthFunc(GL_LEQUAL);
	qglDepthMask(1);
	qglMatrixMode(GL_PROJECTION);
	qglPopMatrix();
	qglMatrixMode(GL_MODELVIEW);
	qglPopMatrix();

}


/*
==================
Simple blob shadow
==================
*/
void MakeNormalVectors(vec3_t forward, vec3_t right, vec3_t up);
trace_t CL_PMTraceWorld(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int mask);
#define MAX_BLOB_SHADOW_VERT 4096*4

void R_BlobShadow(void){

	vec3_t			end, mins = {-15, -15, 0}, maxs = {15, 15, 2};
	trace_t			trace;
	float			alpha, dist;
	int				sVert=0, index=0, i, id;
	unsigned		sIndex[MAX_INDICES], defBits = 0;
	vec3_t			axis[3];
	vec3_t			bbox[8], temp;
	vec4_t			bsColor[MAX_BLOB_SHADOW_VERT];
	vec3_t			bsVert[MAX_BLOB_SHADOW_VERT];
	vec2_t			bsTextCoord[MAX_BLOB_SHADOW_VERT];
	
	if(r_shadows->value != 1)
		return;
	
	qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	qglTexCoordPointer(2, GL_FLOAT, 0, bsTextCoord);
	qglEnableClientState(GL_COLOR_ARRAY);
	qglColorPointer(4, GL_FLOAT, 0, bsColor);
	qglEnableClientState(GL_VERTEX_ARRAY);
	qglVertexPointer(3, GL_FLOAT, 0, bsVert);
	
	GL_BindProgram(genericProgram, defBits);
	id = genericProgram->id[defBits];
	qglUniform1i(qglGetUniformLocation(id, "u_map"), 0);
	qglUniform1f(qglGetUniformLocation(id, "u_colorScale"), 1.0);

	qglEnable(GL_POLYGON_OFFSET_FILL);
    qglPolygonOffset(-2, -1);
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDepthMask(0);
	GL_MBind(GL_TEXTURE0_ARB, r_particletexture[PT_DEFAULT]->texnum);
	

	for (i = 0; i < r_newrefdef.num_entities; i++)	
	{
		
		currententity = &r_newrefdef.entities[i];
		currentmodel = currententity->model;
		
		if (!currentmodel)
			continue;
		
		if (currentmodel->type != mod_alias)
			continue;

		if (currententity->
		flags & (RF_SHELL_HALF_DAM | RF_SHELL_GREEN | RF_SHELL_RED |
				 RF_SHELL_BLUE | RF_SHELL_DOUBLE | RF_SHELL_GOD |
				 RF_TRANSLUCENT | RF_BEAM | RF_WEAPONMODEL | RF_NOSHADOW | RF_DISTORT))
				 continue;
		
		if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
			continue;
	
		if (R_CullSphere(currententity->origin, 35, 15))
			continue;
		
		VectorCopy(currententity->origin, end);
		end[2] -= 128;

		trace = CL_PMTraceWorld (currententity->origin, vec3_origin, vec3_origin, end, MASK_SOLID);
				
		if (trace.fraction > 0 && trace.fraction < 1){
				
		if(sVert){
		if(gl_state.DrawRangeElements && r_DrawRangeElements->value)
			qglDrawRangeElementsEXT(GL_TRIANGLES, 0, sVert, index, GL_UNSIGNED_INT, sIndex);
		else
			qglDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, sIndex);

		index = 0;
		sVert = 0;
		}
			alpha = 0.7 - trace.fraction; 
						
			// Find axes
			VectorCopy(trace.plane.normal, axis[0]);
			MakeNormalVectors(axis[0], axis[1], axis[2]);
			
			//find model radiuse
			R_CullAliasModel(bbox, currententity);
			VectorSubtract(bbox[0], bbox[2], temp);
			dist = VectorNormalize(temp);

			// Scale the axes by radius
			VectorScale(axis[1], dist, axis[1]);
			VectorScale(axis[2], dist, axis[2]);

			// Build by surface aligned quad
			VA_SetElem2(bsTextCoord[sVert+0],0, 1);
			VA_SetElem3(bsVert[sVert+0],	trace.endpos[0] + axis[1][0] + axis[2][0],
											trace.endpos[1] + axis[1][1] + axis[2][1],
											trace.endpos[2] + axis[1][2] + axis[2][2]);
			VA_SetElem4(bsColor[sVert+0],	0.0, 0.0, 0.0, alpha);

			VA_SetElem2(bsTextCoord[sVert+1],0, 0);
			VA_SetElem3(bsVert[sVert+1],	trace.endpos[0] - axis[1][0] + axis[2][0],
											trace.endpos[1] - axis[1][1] + axis[2][1],
											trace.endpos[2] - axis[1][2] + axis[2][2]);
			VA_SetElem4(bsColor[sVert+1],	0.0, 0.0, 0.0, alpha);

			VA_SetElem2(bsTextCoord[sVert+2],1, 0);
			VA_SetElem3(bsVert[sVert+2],	trace.endpos[0] - axis[1][0] - axis[2][0],
											trace.endpos[1] - axis[1][1] - axis[2][1],
											trace.endpos[2] - axis[1][2] - axis[2][2]);
			VA_SetElem4(bsColor[sVert+2],	0.0, 0.0, 0.0, alpha);

			VA_SetElem2(bsTextCoord[sVert+3],1, 1);
			VA_SetElem3(bsVert[sVert+3],	trace.endpos[0] + axis[1][0] - axis[2][0],
											trace.endpos[1] + axis[1][1] - axis[2][1],
											trace.endpos[2] + axis[1][2] - axis[2][2]);
			VA_SetElem4(bsColor[sVert+3],	0.0, 0.0, 0.0, alpha);
			
			sIndex[index++] = sVert+0;
			sIndex[index++] = sVert+1;
			sIndex[index++] = sVert+3;
			sIndex[index++] = sVert+3;
			sIndex[index++] = sVert+1;
			sIndex[index++] = sVert+2;

			sVert+=4;
	}

	if(sVert)
	{
		if(gl_state.DrawRangeElements && r_DrawRangeElements->value)
			qglDrawRangeElementsEXT(GL_TRIANGLES, 0, sVert, index, GL_UNSIGNED_INT, sIndex);
		else
			qglDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, sIndex);
	}
	
	}

	qglDisable(GL_POLYGON_OFFSET_FILL);
	qglDisable(GL_BLEND);
	qglDepthMask(1);
	qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	qglDisableClientState(GL_COLOR_ARRAY);
	qglDisableClientState(GL_VERTEX_ARRAY);
	GL_BindNullProgram();
	
}