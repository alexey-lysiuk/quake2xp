/*
Copyright (C) 2004-2013 Quake2xp Team, Berserker.

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
// r_calcAlias.c: calc triangles for alias models


#include "r_local.h"

vec3_t	tempVertexArray	[MAX_VERTICES  * 4];

void R_Init_AliasArrays() {

	vertexArray		= malloc(MAX_TRIANGLES * 3 * sizeof(vec3_t));
	normalArray		= malloc(MAX_TRIANGLES * 3 * sizeof(vec3_t));
	tangentArray	= malloc(MAX_TRIANGLES * 3 * sizeof(vec3_t));
	binormalArray	= malloc(MAX_TRIANGLES * 3 * sizeof(vec3_t));
	colorArray		= malloc(MAX_TRIANGLES * 3 * sizeof(vec4_t));
}

void R_Shutdown_AliasArrays() {

	free(vertexArray);
	free(normalArray);
	free(tangentArray);
	free(binormalArray);
	free(colorArray);
}

extern float	*shadedots;

void R_CalcAliasFrameLerp (dmdl_t *paliashdr, float shellScale) {
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t	*v, *ov, *verts;
	float	frontlerp;
	vec3_t	move, vectors[3];
	vec3_t	frontv, backv;
	int		i;
	float	*lerp;
	float	backlerp;

	if (currentmodel->numFrames < 1)
		return;

	backlerp = currententity->backlerp;

	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = v = frame->verts;
	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	ov = oldframe->verts;

	frontlerp = 1.0 - backlerp;

	// move should be the delta back to the previous frame * backlerp
	VectorSubtract (currententity->oldorigin, currententity->origin, move);

	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2]) {
		vec3_t	temp;
		VectorCopy (move, temp);
		AngleVectors (currententity->angles, vectors[0], vectors[1], vectors[2]);
		move[0] = DotProduct (temp, vectors[0]);
		move[1] = -DotProduct (temp, vectors[1]);
		move[2] = DotProduct (temp, vectors[2]);
	}

	VectorAdd (move, oldframe->translate, move);

	for (i = 0; i < 3; i++) {
		move[i] = backlerp*move[i] + frontlerp*frame->translate[i];
		frontv[i] = frontlerp*frame->scale[i];
		backv[i] = backlerp*oldframe->scale[i];
	}

	lerp = tempVertexArray[0];
	
	if (currententity->flags & (RF_SHELL_RED | RF_SHELL_GREEN | RF_SHELL_BLUE | RF_SHELL_DOUBLE | RF_SHELL_HALF_DAM | RF_SHELL_GOD)) {
		for (i = 0; i < paliashdr->num_xyz; i++, v++, ov++, lerp += 3) {
			float *normal = q_byteDirs[verts[i].lightnormalindex];
			lerp[0] = move[0] + ov->v[0] * backv[0] + v->v[0] * frontv[0] + normal[0] * shellScale;
			lerp[1] = move[1] + ov->v[1] * backv[1] + v->v[1] * frontv[1] + normal[1] * shellScale;
			lerp[2] = move[2] + ov->v[2] * backv[2] + v->v[2] * frontv[2] + normal[2] * shellScale;
		}
	}
	else {
		for (i = 0; i < paliashdr->num_xyz; i++, v++, ov++, lerp += 3) {

			lerp[0] = move[0] + ov->v[0] * backv[0] + v->v[0] * frontv[0];
			lerp[1] = move[1] + ov->v[1] * backv[1] + v->v[1] * frontv[1];
			lerp[2] = move[2] + ov->v[2] * backv[2] + v->v[2] * frontv[2];
		}
	}
}

int CL_PMpointcontents (vec3_t point);

void GL_DrawAliasFrameLerp (dmdl_t *paliashdr, vec3_t lightColor) {
	int				index_xyz;
	int				i, j, jj = 0;
	dtriangle_t		*tris;
	image_t			*skin, *skinNormalmap, *glowskin;
	float			alphaShift, alpha, l;
	float			backlerp, frontlerp;
	int				index2, oldindex2;
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t		*verts, *oldverts;

	alphaShift = sin (ref_realtime * currentmodel->glowCfg[2]);
	alphaShift = (alphaShift + 1) * 0.5f;
	alphaShift = clamp (alphaShift, currentmodel->glowCfg[0], currentmodel->glowCfg[1]);

	if (currententity->flags & RF_TRANSLUCENT) {
		alpha = currententity->alpha;
	}
	else
		alpha = 1.0;

	if (currententity->flags & RF_NOCULL)
		GL_Disable(GL_CULL_FACE);

	if (currententity->flags & (RF_VIEWERMODEL))
		return;

	if (r_skipStaticLights->integer) {

		if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
			VectorSet(lightColor, 0.5, 0.5, 0.5);
	}
	else {
		if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
			VectorSet(lightColor, 0.18, 0.18, 0.18);
	}

	if (r_newrefdef.rdflags & RDF_IRGOGGLES)
		VectorSet(lightColor, 1, 1, 1);

	// select skin
	if (currententity->skin)
		skin = currententity->skin;	// custom player skin
	else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			skin = currentmodel->skins[0];
			currententity->skinnum = 0;
		}
		else {
			skin = currentmodel->skins[currententity->skinnum];
			if (!skin) {
				skin = currentmodel->skins[0];
				currententity->skinnum = 0;
			}
		}
	}

	if (!skin)
		skin = r_notexture;

	// select skin
	if (currententity->bump)
		skinNormalmap = currententity->bump;	// custom player skin
	else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			skinNormalmap = currentmodel->skins_normal[0];
			currententity->skinnum = 0;
		}
		else {
			skinNormalmap = currentmodel->skins_normal[currententity->skinnum];
			if (!skin) {
				skinNormalmap = currentmodel->skins_normal[0];
				currententity->skinnum = 0;
			}
		}
	}
	if (!skinNormalmap)
		skinNormalmap = r_defBump;

	glowskin = currentmodel->glowtexture[currententity->skinnum];

	if (!glowskin)
		glowskin = r_notexture;

	if (!skin)
		skin = r_notexture;

	R_CalcAliasFrameLerp (paliashdr, 0);

	qglEnableVertexAttribArray (ATT_POSITION);
	qglVertexAttribPointer (ATT_POSITION, 3, GL_FLOAT, qfalse, 0, vertexArray);

	qglEnableVertexAttribArray (ATT_NORMAL);
	qglVertexAttribPointer (ATT_NORMAL, 3, GL_FLOAT, qfalse, 0, normalArray);

	qglEnableVertexAttribArray (ATT_COLOR);
	qglVertexAttribPointer (ATT_COLOR, 4, GL_FLOAT, qfalse, 0, colorArray);

//	qglBindBufferARB (GL_ARRAY_BUFFER_ARB, currentmodel->vboId);
	qglEnableVertexAttribArray (ATT_TEX0);
	qglVertexAttribPointer (ATT_TEX0, 2, GL_FLOAT, qfalse, 0, currentmodel->st);


	c_alias_polys += paliashdr->num_tris;
	tris = (dtriangle_t *)((byte *)paliashdr + paliashdr->ofs_tris);
	jj = 0;

	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	oldverts = oldframe->verts;
	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = frame->verts;
	backlerp = currententity->backlerp;
	frontlerp = 1 - backlerp;

	for (i = 0; i < paliashdr->num_tris; i++) {
		for (j = 0; j < 3; j++, jj++) {
			index_xyz = tris[i].index_xyz[j];
			VectorCopy (tempVertexArray[index_xyz], vertexArray[jj]);

			VA_SetElem4 (colorArray[jj], lightColor[0], lightColor[1], lightColor[2], alpha);

			if (currentmodel->envMap) {
				index2 = verts[index_xyz].lightnormalindex;
				oldindex2 = oldverts[index_xyz].lightnormalindex;
				normalArray[jj][0] = q_byteDirs[oldindex2][0] * backlerp + q_byteDirs[index2][0] * frontlerp;
				normalArray[jj][1] = q_byteDirs[oldindex2][1] * backlerp + q_byteDirs[index2][1] * frontlerp;
				normalArray[jj][2] = q_byteDirs[oldindex2][2] * backlerp + q_byteDirs[index2][2] * frontlerp;
			}
		}
	}

	// setup program
	GL_BindProgram (aliasAmbientProgram, 0);

	if (currentmodel->envMap)
		qglUniform1i (ambientAlias_isEnvMaping, 1);
	else
		qglUniform1i (ambientAlias_isEnvMaping, 0);

	qglUniform1i (ambientAlias_isShell, 0);

	qglUniform1f (ambientAlias_colorModulate, r_textureColorScale->value);
	qglUniform1f (ambientAlias_addShift, alphaShift);

	GL_MBind (GL_TEXTURE0, skin->texnum);
	GL_MBind (GL_TEXTURE1, glowskin->texnum);
	GL_MBind (GL_TEXTURE2, r_envTex->texnum);
	GL_MBind (GL_TEXTURE3, skinNormalmap->texnum);

	qglUniform1f(ambientAlias_envScale, currentmodel->envScale);

	if (r_ssao->integer && !(currententity->flags & RF_WEAPONMODEL) && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL) && !(r_newrefdef.rdflags & RDF_IRGOGGLES)) {
		GL_MBindRect (GL_TEXTURE4, fboColor[fboColorIndex]->texnum);
		qglUniform1i(ambientAlias_ssao, 1);
	}
	else
		qglUniform1i(ambientAlias_ssao, 0);

	qglUniform3fv(ambientAlias_viewOrg, 1, r_origin);
	qglUniformMatrix4fv(ambientAlias_mvp, 1, qfalse, (const float *)currententity->orMatrix);

	qglDrawArrays (GL_TRIANGLES, 0, jj);

	qglDisableVertexAttribArray (ATT_POSITION);
	qglDisableVertexAttribArray (ATT_NORMAL);
	qglDisableVertexAttribArray (ATT_COLOR);
	qglDisableVertexAttribArray (ATT_TEX0);
//	qglBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	GL_BindNullProgram ();

	if (currententity->flags & RF_NOCULL)
		GL_Enable(GL_CULL_FACE);
}

void GL_DrawAliasFrameLerpShell (dmdl_t *paliashdr) {
	int			index_xyz, i, j, jj = 0;
	dtriangle_t	*tris;
	unsigned	defBits = 0;
	float		scroll = 0.0;
	float		backlerp, frontlerp;
	int			index2, oldindex2;
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t		*verts, *oldverts;

	if (currententity->flags & (RF_VIEWERMODEL))
		return;

	scroll = r_newrefdef.time *0.45;

	if (currententity->flags & RF_WEAPONMODEL)
		R_CalcAliasFrameLerp (paliashdr, 0.1);
	else if (currententity->flags & RF_CAMERAMODEL2)
		R_CalcAliasFrameLerp (paliashdr, 0.0);
	else
		R_CalcAliasFrameLerp (paliashdr, 0.5);

	c_alias_polys += paliashdr->num_tris;

	jj = 0;
	tris = (dtriangle_t *)((byte *)paliashdr + paliashdr->ofs_tris);
	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	oldverts = oldframe->verts;
	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = frame->verts;
	backlerp = currententity->backlerp;
	frontlerp = 1 - backlerp;

	for (i = 0; i < paliashdr->num_tris; i++) {
		for (j = 0; j < 3; j++, jj++) {
			index_xyz = tris[i].index_xyz[j];
			VectorCopy (tempVertexArray[index_xyz], vertexArray[jj]);

			index2 = verts[index_xyz].lightnormalindex;
			oldindex2 = oldverts[index_xyz].lightnormalindex;

			normalArray[jj][0] = q_byteDirs[oldindex2][0] * backlerp + q_byteDirs[index2][0] * frontlerp;
			normalArray[jj][1] = q_byteDirs[oldindex2][1] * backlerp + q_byteDirs[index2][1] * frontlerp;
			normalArray[jj][2] = q_byteDirs[oldindex2][2] * backlerp + q_byteDirs[index2][2] * frontlerp;

		}
	}

	// setup program
	GL_BindProgram (aliasAmbientProgram, defBits);

	qglUniform1i (ambientAlias_isShell, 1);
	qglUniform1i (ambientAlias_isEnvMaping, 0);
	qglUniform1f (ambientAlias_colorModulate, r_textureColorScale->value);
	qglUniform1f (ambientAlias_scroll, scroll);
	qglUniform3fv(ambientAlias_viewOrg, 1, r_origin);

	qglUniformMatrix4fv(ambientAlias_mvp, 1, qfalse, (const float *)currententity->orMatrix);

	if (currententity->flags & RF_SHELL_BLUE)
		GL_MBind (GL_TEXTURE0, r_texshell[0]->texnum);
	if (currententity->flags & RF_SHELL_RED)
		GL_MBind (GL_TEXTURE0, r_texshell[1]->texnum);
	if (currententity->flags & RF_SHELL_GREEN)
		GL_MBind (GL_TEXTURE0, r_texshell[2]->texnum);
	if (currententity->flags & RF_SHELL_GOD)
		GL_MBind (GL_TEXTURE0, r_texshell[3]->texnum);
	if (currententity->flags & RF_SHELL_HALF_DAM)
		GL_MBind (GL_TEXTURE0, r_texshell[4]->texnum);
	if (currententity->flags & RF_SHELL_DOUBLE)
		GL_MBind (GL_TEXTURE0, r_texshell[5]->texnum);

	qglEnableVertexAttribArray (ATT_POSITION);
	qglVertexAttribPointer (ATT_POSITION, 3, GL_FLOAT, qfalse, 0, vertexArray);

	qglEnableVertexAttribArray (ATT_NORMAL);
	qglVertexAttribPointer (ATT_NORMAL, 3, GL_FLOAT, qfalse, 0, normalArray);

//	qglBindBufferARB(GL_ARRAY_BUFFER_ARB, currentmodel->vboId);
	qglEnableVertexAttribArray (ATT_TEX0);
	qglVertexAttribPointer (ATT_TEX0, 2, GL_FLOAT, qfalse, 0, currentmodel->st);

	qglDrawArrays (GL_TRIANGLES, 0, jj);

	GL_Disable (GL_BLEND);

	qglDisableVertexAttribArray (ATT_POSITION);
	qglDisableVertexAttribArray (ATT_NORMAL);
	qglDisableVertexAttribArray (ATT_TEX0);
//	qglBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
	GL_BindNullProgram ();
}

void R_UpdateLightAliasUniforms()
{
	mat4_t	entAttenMatrix, entSpotMatrix;

	qglUniform1f(lightAlias_colorScale, r_textureColorScale->value);
	qglUniform1i(lightAlias_ambient, (int)currentShadowLight->isAmbient);
	qglUniform1f(lightAlias_specularScale, r_specularScale->value);
	qglUniform4f(lightAlias_lightColor, currentShadowLight->color[0], currentShadowLight->color[1], currentShadowLight->color[2], 1.0);
	qglUniform1i(lightAlias_fog, (int)currentShadowLight->isFog);
	if (currententity->flags & RF_WEAPONMODEL)
		qglUniform1f(lightAlias_fogDensity, currentShadowLight->fogDensity * 8.0);
	else
		qglUniform1f(lightAlias_fogDensity, currentShadowLight->fogDensity);
	qglUniform1f(lightAlias_causticsIntens, r_causticIntens->value);
	qglUniform3fv(lightAlias_viewOrigin, 1, r_origin);
	qglUniform3fv(lightAlias_lightOrigin, 1, currentShadowLight->origin);

	Mat4_TransposeMultiply(currententity->matrix, currentShadowLight->attenMatrix, entAttenMatrix);
	qglUniformMatrix4fv(lightAlias_attenMatrix, 1, qfalse, (const float *)entAttenMatrix);


	Mat4_TransposeMultiply(currententity->matrix, currentShadowLight->spotMatrix, entSpotMatrix);
	qglUniformMatrix4fv(lightAlias_spotMatrix, 1, qfalse, (const float *)entSpotMatrix);
	qglUniform3f(lightAlias_spotParams, currentShadowLight->hotSpot, 1.f / (1.f - currentShadowLight->hotSpot), currentShadowLight->coneExp);

	if (currentShadowLight->isCone)
		qglUniform1i(lightAlias_spotLight, 1);
	else
		qglUniform1i(lightAlias_spotLight, 0);
	
	R_CalcCubeMapMatrix(qtrue);
	qglUniformMatrix4fv(lightAlias_cubeMatrix, 1, qfalse, (const float *)currentShadowLight->cubeMapMatrix);

	qglUniformMatrix4fv(lightAlias_mvp, 1, qfalse, (const float *)currententity->orMatrix);
	qglUniformMatrix4fv(lightAlias_mv, 1, qfalse, (const float *)r_newrefdef.modelViewMatrix);

}

void GL_DrawAliasFrameLerpLight (dmdl_t *paliashdr) {
	int				i, j, jj = 0;
	int				index_xyz;
	byte			*binormals, *oldbinormals;
	byte			*tangents, *oldtangents;
	dtriangle_t		*tris;
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t		*verts, *oldverts;
	float			backlerp, frontlerp;
	unsigned		offs, offs2;
	vec3_t			maxs;
	image_t			*skin, *skinNormalmap, *rgh;
	int				index2, oldindex2;
	qboolean		inWater;

	if (currententity->flags & (RF_VIEWERMODEL))
		return;

	if (currentmodel->noSelfShadow && r_shadows->integer)
		GL_Disable(GL_STENCIL_TEST);

	backlerp = currententity->backlerp;
	frontlerp = 1 - backlerp;

	offs = paliashdr->num_xyz;

	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	oldverts = oldframe->verts;
	offs2 = offs*currententity->oldframe;
	oldbinormals = currentmodel->binormals + offs2;
	oldtangents = currentmodel->tangents + offs2;

	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = frame->verts;
	offs2 = offs*currententity->frame;
	binormals = currentmodel->binormals + offs2;
	tangents = currentmodel->tangents + offs2;
	tris = (dtriangle_t *)((byte *)paliashdr + paliashdr->ofs_tris);

	// select skin
	if (currententity->skin)
		skin = currententity->skin;	// custom player skin
	else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			skin = currentmodel->skins[0];
			currententity->skinnum = 0;
		}
		else {
			skin = currentmodel->skins[currententity->skinnum];
			if (!skin) {
				skin = currentmodel->skins[0];
				currententity->skinnum = 0;
			}
		}
	}
	if (!skin)
		skin = r_notexture;

//	// select skin
	if (currententity->bump)
		skinNormalmap = currententity->bump;	// custom player skin
	else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			skinNormalmap = currentmodel->skins_normal[0];
			currententity->skinnum = 0;
		}
		else {
			skinNormalmap = currentmodel->skins_normal[currententity->skinnum];
			if (!skin) {
				skinNormalmap = currentmodel->skins_normal[0];
				currententity->skinnum = 0;
			}
		}
	}
	if (!skinNormalmap)
		skinNormalmap = r_defBump;
	
	rgh = currentmodel->skins_roughness[currententity->skinnum];
	if (!rgh)
		rgh = r_notexture;

	R_CalcAliasFrameLerp(paliashdr, 0);			/// ������ ���� ����������� ���������� Lerp...
	
	c_alias_polys += paliashdr->num_tris;

	for (i = 0; i < paliashdr->num_tris; i++) {
		for (j = 0; j < 3; j++, jj++) {
			index_xyz = tris[i].index_xyz[j];
			index2 = verts[index_xyz].lightnormalindex;
			oldindex2 = oldverts[index_xyz].lightnormalindex;

			normalArray[jj][0] = q_byteDirs[oldindex2][0] * backlerp + q_byteDirs[index2][0] * frontlerp;
			normalArray[jj][1] = q_byteDirs[oldindex2][1] * backlerp + q_byteDirs[index2][1] * frontlerp;
			normalArray[jj][2] = q_byteDirs[oldindex2][2] * backlerp + q_byteDirs[index2][2] * frontlerp;

			tangentArray[jj][0] = q_byteDirs[oldtangents[index_xyz]][0] * backlerp + q_byteDirs[tangents[index_xyz]][0] * frontlerp;
			tangentArray[jj][1] = q_byteDirs[oldtangents[index_xyz]][1] * backlerp + q_byteDirs[tangents[index_xyz]][1] * frontlerp;
			tangentArray[jj][2] = q_byteDirs[oldtangents[index_xyz]][2] * backlerp + q_byteDirs[tangents[index_xyz]][2] * frontlerp;

			binormalArray[jj][0] = q_byteDirs[oldbinormals[index_xyz]][0] * backlerp + q_byteDirs[binormals[index_xyz]][0] * frontlerp;
			binormalArray[jj][1] = q_byteDirs[oldbinormals[index_xyz]][1] * backlerp + q_byteDirs[binormals[index_xyz]][1] * frontlerp;
			binormalArray[jj][2] = q_byteDirs[oldbinormals[index_xyz]][2] * backlerp + q_byteDirs[binormals[index_xyz]][2] * frontlerp;

			VectorCopy(tempVertexArray[index_xyz], vertexArray[jj]);

		}
	}

	// setup program
	GL_BindProgram (aliasBumpProgram, 0);

	VectorAdd (currententity->origin, currententity->model->maxs, maxs);
	if (CL_PMpointcontents (maxs) & MASK_WATER)
		inWater = qtrue;
	else
		inWater = qfalse;

	R_UpdateLightAliasUniforms();
	
	if (r_imageAutoBump->integer && skinNormalmap == r_defBump) {
		qglUniform1i(lightAlias_autoBump, 1);
		qglUniform2f(lightAlias_autoBumpParams, r_imageAutoBumpScale->value, r_imageAutoSpecularScale->value);
	}
	else
		qglUniform1i(lightAlias_autoBump, 0);

	if (inWater && currentShadowLight->castCaustics && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		qglUniform1i(lightAlias_isCaustics, 1);
	else
		qglUniform1i(lightAlias_isCaustics, 0);

	GL_MBind (GL_TEXTURE0, skinNormalmap->texnum);
	GL_MBind (GL_TEXTURE1, skin->texnum);
	GL_MBind (GL_TEXTURE2, r_caustic[((int)(r_newrefdef.time * 15)) & (MAX_CAUSTICS - 1)]->texnum);
	GL_MBindCube (GL_TEXTURE3, r_lightCubeMap[currentShadowLight->filter]->texnum);

	if (rgh == r_notexture)
		qglUniform1i(lightAlias_isRgh, 0);
	else {
		qglUniform1i(lightAlias_isRgh, 1);
		GL_MBind(GL_TEXTURE4, rgh->texnum);
	}
	
	GL_MBind(GL_TEXTURE5, skinBump->texnum);

	qglEnableVertexAttribArray (ATT_POSITION);
	qglEnableVertexAttribArray(ATT_TANGENT);
	qglEnableVertexAttribArray(ATT_BINORMAL);
	qglEnableVertexAttribArray(ATT_NORMAL);
	qglEnableVertexAttribArray(ATT_TEX0);

	qglVertexAttribPointer (ATT_POSITION, 3, GL_FLOAT, qfalse, 0, vertexArray);
	qglVertexAttribPointer (ATT_TANGENT, 3, GL_FLOAT, qfalse, 0, tangentArray);
	qglVertexAttribPointer (ATT_BINORMAL, 3, GL_FLOAT, qfalse, 0, binormalArray);
	qglVertexAttribPointer (ATT_NORMAL, 3, GL_FLOAT, qfalse, 0, normalArray);
	qglVertexAttribPointer (ATT_TEX0, 2, GL_FLOAT, qfalse, 0, currentmodel->st);

	qglDrawArrays (GL_TRIANGLES, 0, jj);

	qglDisableVertexAttribArray (ATT_POSITION);
	qglDisableVertexAttribArray (ATT_TANGENT);
	qglDisableVertexAttribArray (ATT_BINORMAL);
	qglDisableVertexAttribArray (ATT_NORMAL);
	qglDisableVertexAttribArray (ATT_TEX0);
	GL_BindNullProgram ();
}