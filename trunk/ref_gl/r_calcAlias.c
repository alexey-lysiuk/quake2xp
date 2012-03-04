/*
Copyright (C) 2004-20011 Quake2xp Team, Berserker.

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

static float	r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.h"
};
vec3_t	tempVertexArray[MAX_VERTICES*4];


void R_CalcAliasFrameLerp (dmdl_t *paliashdr, float shellScale)
{
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t	*v, *ov, *verts;
	float	frontlerp;
	vec3_t	move, vectors[3];
	vec3_t	frontv, backv;
	int		i;
	float	*lerp;
	float	backlerp;

	backlerp = currententity->backlerp;
	
	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = v = frame->verts;
	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	ov = oldframe->verts;

	frontlerp = 1.0 - backlerp;

	// move should be the delta back to the previous frame * backlerp
	VectorSubtract (currententity->oldorigin, currententity->origin, move);

	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2])
	{
		vec3_t	temp;
		VectorCopy(move, temp);
		AngleVectors (currententity->angles, vectors[0], vectors[1], vectors[2]);
		move[0] = DotProduct (temp, vectors[0]);
		move[1] = -DotProduct (temp, vectors[1]);
		move[2] = DotProduct (temp, vectors[2]);
	}

	VectorAdd (move, oldframe->translate, move);

	for (i=0 ; i<3 ; i++)
	{
		move[i] = backlerp*move[i] + frontlerp*frame->translate[i];
		frontv[i] = frontlerp*frame->scale[i];
		backv[i] = backlerp*oldframe->scale[i];
	}

	lerp = tempVertexArray[0];
	
	if (currententity->flags & (RF_SHELL_RED | RF_SHELL_GREEN | RF_SHELL_BLUE | RF_SHELL_DOUBLE | RF_SHELL_HALF_DAM | RF_SHELL_GOD)) {
		for (i=0 ; i < paliashdr->num_xyz; i++, v++, ov++, lerp+=3 ) {
			float *normal = r_avertexnormals[verts[i].lightnormalindex];
			lerp[0] = move[0] + ov->v[0]*backv[0] + v->v[0]*frontv[0] + normal[0] * shellScale;
			lerp[1] = move[1] + ov->v[1]*backv[1] + v->v[1]*frontv[1] + normal[1] * shellScale;
			lerp[2] = move[2] + ov->v[2]*backv[2] + v->v[2]*frontv[2] + normal[2] * shellScale;
		}
	} 
	else 
	{
	for (i=0 ; i < paliashdr->num_xyz; i++, v++, ov++, lerp+=3)	{

			lerp[0] = move[0] + ov->v[0]*backv[0] + v->v[0]*frontv[0];
			lerp[1] = move[1] + ov->v[1]*backv[1] + v->v[1]*frontv[1];
			lerp[2] = move[2] + ov->v[2]*backv[2] + v->v[2]*frontv[2];
		}
	}
}

int CL_PMpointcontents(vec3_t point);

void GL_DrawAliasFrameLerpAmbient(dmdl_t *paliashdr, vec3_t lightColor)
{
	vec3_t		vertexArray[3*MAX_TRIANGLES];
	int			index_xyz;
	int			i, j, jj =0;
	dtriangle_t	*tris;
	image_t		*skin, *glowskin;
	float		alphaShift, alpha;
	vec3_t		water;
	qboolean	caustics = false;
	unsigned	defBits = 0;
	int			id;

	alphaShift =			sin (ref_realtime * currentmodel->glowCfg[2]); 
	alphaShift =			(alphaShift + 1) * 0.5f;
	alphaShift =			clamp(alphaShift, currentmodel->glowCfg[0], currentmodel->glowCfg[1]);
	
	if (currententity->flags & RF_TRANSLUCENT)
		alpha = currententity->alpha;
	else
		alpha = 1.0;

	VectorAdd(currententity->origin, currententity->model->maxs, water); 
	if(CL_PMpointcontents(water) & MASK_WATER)
		caustics = true;
	
	qglDisable(GL_BLEND);

	if (currententity->flags & (RF_VIEWERMODEL))
			return;

	if(r_bumpAlias->value){
	if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		VectorSet(lightColor, 0.2, 0.2, 0.2);
	}

	if(r_newrefdef.rdflags & RDF_IRGOGGLES) 
		VectorSet(lightColor, 1,1,1);

	
	qglColor4f(lightColor[0], lightColor[1], lightColor[2], alpha);	

	// select skin
	if (currententity->skin)
		skin = currententity->skin;	// custom player skin
	else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			skin = currentmodel->skins[0];
			currententity->skinnum = 0;
		} else {
			skin = currentmodel->skins[currententity->skinnum];
			if (!skin) {
				skin = currentmodel->skins[0];
				currententity->skinnum = 0;
			}
		}
	}
	glowskin	= currentmodel->glowtexture[currententity->skinnum];

	if (!glowskin)
		glowskin = r_notexture;
	
	if (!skin)
		skin = r_notexture;

	R_CalcAliasFrameLerp(paliashdr, 0);			/// ������ ���� ����������� ���������� Lerp...
	
	c_alias_polys += paliashdr->num_tris;
	tris = (dtriangle_t *) ((byte *)paliashdr + paliashdr->ofs_tris);
	jj = 0;

	for (i=0; i<paliashdr->num_tris; i++)
		{
			for (j=0; j<3; j++, jj++)
			{
			index_xyz = tris[i].index_xyz[j];
			VectorCopy(tempVertexArray[index_xyz], vertexArray[jj]);
			}
		}



	if(caustics)
	defBits = worldDefs.CausticsBit;

	// setup program
	GL_BindProgram(aliasAmbientProgram, defBits);
	id = aliasAmbientProgram->id[defBits];
	
	qglUniform1f(qglGetUniformLocation(id, "u_ColorModulate"), r_worldColorScale->value);
	
	if(caustics)
	qglUniform1f(qglGetUniformLocation(id, "u_CausticsModulate"), r_causticIntens->value);
	
	qglUniform1f(qglGetUniformLocation(id, "u_AddShift"), alphaShift);

	GL_MBind				(GL_TEXTURE0_ARB, skin->texnum);
	qglUniform1i			(qglGetUniformLocation(id, "u_Diffuse"), 0);
	GL_MBind				(GL_TEXTURE1_ARB, glowskin->texnum);
	qglUniform1i			(qglGetUniformLocation(id, "u_Add"), 1);	
	if(caustics){
	GL_MBind				(GL_TEXTURE2_ARB, r_caustic[((int) (r_newrefdef.time * 15)) & (MAX_CAUSTICS - 1)]->texnum);
	qglUniform1i			(qglGetUniformLocation(id, "u_Caustics"), 2);
	}

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false,	0, vertexArray);
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false,		0, currentmodel->st);

	qglDrawArrays(GL_TRIANGLES, 0, jj);
	
	qglColor4f(1, 1, 1, 1);	
	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
	GL_SelectTexture(GL_TEXTURE0_ARB);
	GL_BindNullProgram();
}


void GL_DrawAliasFrameLerpAmbientDistort(dmdl_t *paliashdr, vec4_t color)
{
	vec3_t		vertexArray[3*MAX_TRIANGLES];
	vec4_t		colorArray[4*MAX_TRIANGLES];
	int			index_xyz;
	int			i, j, jj =0;
	dtriangle_t	*tris;
	image_t		*bump;
	unsigned	defBits = 0;
	int			id;

	if (currententity->flags & (RF_VIEWERMODEL))
		return;
	
	// select skin
		if (currententity->bump)
		bump = currententity->bump;	// custom player skin
		else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			bump = currentmodel->skins_normal[0];
			currententity->skinnum = 0;
		} else {
			bump	= currentmodel->skins_normal[currententity->skinnum];
			if (!bump) {
				bump = currentmodel->skins_normal[0];
				currententity->skinnum = 0;
			}
		}
	}
	if (!bump)
		bump = r_predator;

	R_CalcAliasFrameLerp(paliashdr, 0);			/// ������ ���� ����������� ���������� Lerp...
	
	c_alias_polys += paliashdr->num_tris;
	tris = (dtriangle_t *) ((byte *)paliashdr + paliashdr->ofs_tris);
	jj = 0;

	for (i=0; i<paliashdr->num_tris; i++)
		{
			for (j=0; j<3; j++, jj++)
			{
			index_xyz = tris[i].index_xyz[j];
			VectorCopy(tempVertexArray[index_xyz], vertexArray[jj]);
			VA_SetElem4(colorArray[jj], color[0], color[1], color[2], 1.0);
			}
		}

	// setup program
	GL_BindProgram(refractProgram, defBits);
	id = refractProgram->id[defBits];

	GL_MBind(GL_TEXTURE0_ARB, bump->texnum);
	qglUniform1i(qglGetUniformLocation(id, "u_deformMap"), 0);
	GL_MBindRect(GL_TEXTURE1_ARB, ScreenMap->texnum);
	qglUniform1i(qglGetUniformLocation(id, "g_colorBufferMap"), 1);
	GL_MBindRect(GL_TEXTURE2_ARB, depthMap->texnum);
	qglUniform1i(qglGetUniformLocation(id, "g_depthBufferMap"), 2);
		
	qglUniform1f(qglGetUniformLocation(id, "u_deformMul"),	2.0);
	qglUniform1f(qglGetUniformLocation(id, "u_alpha"),	0.0);
	qglUniform1f(qglGetUniformLocation(id, "u_thickness"),	0.0);
	qglUniform2f(qglGetUniformLocation(id, "u_viewport"),	vid.width, vid.height);
	qglUniform2f(qglGetUniformLocation(id, "u_depthParms"), r_newrefdef.depthParms[0], r_newrefdef.depthParms[1]);
	
	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_COLOR);
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false,	0, vertexArray);
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false,		0, currentmodel->st);
	qglVertexAttribPointer(ATRB_COLOR, 4, GL_FLOAT, false,		0, colorArray);

	qglDrawArrays(GL_TRIANGLES, 0, jj);

	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_COLOR);
	GL_SelectTexture(GL_TEXTURE0_ARB);
	GL_BindNullProgram();
}

void GL_DrawAliasFrameLerpAmbientShell(dmdl_t *paliashdr)
{
	vec3_t		vertexArray[3 * MAX_TRIANGLES];
	int			index_xyz, id, i, j, jj = 0;
	dtriangle_t	*tris;
	unsigned	defBits = 0;
	float		scroll = 0.0;
	vec3_t		normalArray[3*MAX_TRIANGLES];
	float		backlerp, frontlerp;
	int			index2, oldindex2;
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t		*verts, *oldverts;

	if (currententity->flags & (RF_VIEWERMODEL))
		return;
	
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_ONE, GL_ONE);

	scroll = r_newrefdef.time *0.45;

	if (currententity->flags & RF_WEAPONMODEL)
		R_CalcAliasFrameLerp(paliashdr, 0.1);		
	else if (currententity->flags & RF_CAMERAMODEL2)
		R_CalcAliasFrameLerp(paliashdr, 0.0);
	else
		R_CalcAliasFrameLerp(paliashdr, 0.5);

	c_alias_polys += paliashdr->num_tris;
	
	jj = 0;
	tris = (dtriangle_t *) ((byte *)paliashdr + paliashdr->ofs_tris);
	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	oldverts = oldframe->verts;
	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = frame->verts;
	backlerp = currententity->backlerp;
	frontlerp = 1 - backlerp;

	for (i=0; i<paliashdr->num_tris; i++)
	{
		for (j=0; j<3; j++, jj++)
		{
		index_xyz = tris[i].index_xyz[j];
		VectorCopy(tempVertexArray[index_xyz], vertexArray[jj]);

		index2 = verts[index_xyz].lightnormalindex;
		oldindex2 = oldverts[index_xyz].lightnormalindex;
		
		normalArray[jj][0] = r_avertexnormals[oldindex2][0]*backlerp + r_avertexnormals[index2][0]*frontlerp;
		normalArray[jj][1] = r_avertexnormals[oldindex2][1]*backlerp + r_avertexnormals[index2][1]*frontlerp;
		normalArray[jj][2] = r_avertexnormals[oldindex2][2]*backlerp + r_avertexnormals[index2][2]*frontlerp;

		}
	}
		
	// setup program
	defBits |= worldDefs.ShellBits;
	GL_BindProgram(aliasAmbientProgram, defBits);
	id = aliasAmbientProgram->id[defBits];
	
	qglUniform1f(qglGetUniformLocation(id, "u_ColorModulate"), r_worldColorScale->value);
	qglUniform1f(qglGetUniformLocation(id, "u_scroll"), scroll);

	if (currententity->flags & RF_SHELL_BLUE)
		GL_MBind(GL_TEXTURE0_ARB, r_texshell[0]->texnum);
	if (currententity->flags & RF_SHELL_RED)
		GL_MBind(GL_TEXTURE0_ARB, r_texshell[1]->texnum);
	if (currententity->flags & RF_SHELL_GREEN)
		GL_MBind(GL_TEXTURE0_ARB, r_texshell[2]->texnum);
	if (currententity->flags & RF_SHELL_GOD)
		GL_MBind(GL_TEXTURE0_ARB, r_texshell[3]->texnum);
	if (currententity->flags & RF_SHELL_HALF_DAM)
		GL_MBind(GL_TEXTURE0_ARB, r_texshell[4]->texnum);
	if (currententity->flags & RF_SHELL_DOUBLE)
		GL_MBind(GL_TEXTURE0_ARB, r_texshell[5]->texnum);
	qglUniform1i(qglGetUniformLocation(id, "u_Diffuse"), 0);

	qglEnableVertexAttribArray(ATRB_POSITION);
	qglEnableVertexAttribArray(ATRB_NORMAL);
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false,	0, vertexArray);
	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false,		0, currentmodel->st);
	qglVertexAttribPointer(ATRB_NORMAL, 3, GL_FLOAT, false,		0, normalArray);

	qglDrawArrays(GL_TRIANGLES, 0, jj);

	qglDisable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglDisableVertexAttribArray(ATRB_POSITION);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_NORMAL);
	GL_BindNullProgram();
}


void GL_DrawAliasFrameLerpArb(dmdl_t *paliashdr, vec3_t light, float rad, vec3_t lightColor)
{
	int				i, j, jj = 0;
	int				index_xyz;
	byte			*norms, *oldnorms;
	byte			*binormals, *oldbinormals;
	byte			*tangents, *oldtangents;
	dtriangle_t		*tris;
	daliasframe_t	*frame, *oldframe;
	dtrivertx_t		*verts, *oldverts;
	float			backlerp, frontlerp;
	unsigned		offs, offs2;
	vec3_t			normalArray[3*MAX_TRIANGLES], 
					tangentArray[3*MAX_TRIANGLES], 
					binormalArray[3*MAX_TRIANGLES], 
					vertexArray[3*MAX_TRIANGLES]; 
	image_t			*skin, *skinNormalmap;
	int				index2, oldindex2;
	unsigned		defBits = 0;
	int				id;

	if (currententity->flags & (RF_VIEWERMODEL))
			return;
	
	if(currentmodel->noselfshadow)
		qglDisable(GL_STENCIL_TEST);
	
	backlerp = currententity->backlerp;
	frontlerp = 1 - backlerp;
	
	offs = paliashdr->num_xyz;

	oldframe = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->oldframe * paliashdr->framesize);
	oldverts = oldframe->verts;
	offs2 = offs*currententity->oldframe;
	oldnorms = currentmodel->normals + offs2;
	oldbinormals = currentmodel->binormals + offs2;
	oldtangents = currentmodel->tangents + offs2;

	frame = (daliasframe_t *)((byte *)paliashdr + paliashdr->ofs_frames + currententity->frame * paliashdr->framesize);
	verts = frame->verts;
	offs2 = offs*currententity->frame;
	norms = currentmodel->normals + offs2;
	binormals = currentmodel->binormals + offs2;
	tangents = currentmodel->tangents + offs2;
	tris = (dtriangle_t *) ((byte *)paliashdr + paliashdr->ofs_tris);
	
	// select skin
	if (currententity->skin)
		skin = currententity->skin;	// custom player skin
	else {
		if (currententity->skinnum >= MAX_MD2SKINS) {
			skin = currentmodel->skins[0];
			currententity->skinnum = 0;
		} else {
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
		} else {
			skinNormalmap	= currentmodel->skins_normal[currententity->skinnum];
			if (!skin) {
				skinNormalmap = currentmodel->skins_normal[0];
				currententity->skinnum = 0;
			}
		}
	}
	if (!skinNormalmap)
		skinNormalmap = r_defBump;

	R_CalcAliasFrameLerp(paliashdr,0);			/// ������ ���� ����������� ���������� Lerp...

	for (i=0; i<paliashdr->num_tris; i++)
	{
		for (j=0; j<3; j++, jj++)
		{
			index_xyz = tris[i].index_xyz[j];
			index2 = verts[index_xyz].lightnormalindex;
			oldindex2 = oldverts[index_xyz].lightnormalindex;
		
			normalArray[jj][0] = r_avertexnormals[oldindex2][0]*backlerp + r_avertexnormals[index2][0]*frontlerp;
			normalArray[jj][1] = r_avertexnormals[oldindex2][1]*backlerp + r_avertexnormals[index2][1]*frontlerp;
			normalArray[jj][2] = r_avertexnormals[oldindex2][2]*backlerp + r_avertexnormals[index2][2]*frontlerp;

			tangentArray[jj][0] = r_avertexnormals[oldtangents[index_xyz]][0]*backlerp + r_avertexnormals[tangents[index_xyz]][0]*frontlerp;
			tangentArray[jj][1] = r_avertexnormals[oldtangents[index_xyz]][1]*backlerp + r_avertexnormals[tangents[index_xyz]][1]*frontlerp;
			tangentArray[jj][2] = r_avertexnormals[oldtangents[index_xyz]][2]*backlerp + r_avertexnormals[tangents[index_xyz]][2]*frontlerp;

			binormalArray[jj][0] = r_avertexnormals[oldbinormals[index_xyz]][0]*backlerp + r_avertexnormals[binormals[index_xyz]][0]*frontlerp;
			binormalArray[jj][1] = r_avertexnormals[oldbinormals[index_xyz]][1]*backlerp + r_avertexnormals[binormals[index_xyz]][1]*frontlerp;
			binormalArray[jj][2] = r_avertexnormals[oldbinormals[index_xyz]][2]*backlerp + r_avertexnormals[binormals[index_xyz]][2]*frontlerp;

			index_xyz = tris[i].index_xyz[j];
			VectorCopy(tempVertexArray[index_xyz], vertexArray[jj]);
	
		}
	}


	// setup program
	GL_BindProgram(aliasBumpProgram, defBits);
	id = aliasBumpProgram->id[defBits];
		
	qglUniform1f(qglGetUniformLocation(id, "u_LightRadius"), rad);
	qglUniform3fv(qglGetUniformLocation(id, "u_LightColor"), 1 , lightColor);
	qglUniform3fv(qglGetUniformLocation(id, "u_LightOrg"), 1 , light);
	qglUniform3fv(qglGetUniformLocation(id, "u_ViewOrigin"), 1 , r_newrefdef.vieworg);

	GL_MBind(GL_TEXTURE0_ARB, skinNormalmap->texnum);
	qglUniform1i(qglGetUniformLocation(id, "u_bumpMap"), 0);
	GL_MBind(GL_TEXTURE1_ARB, skin->texnum);
	qglUniform1i(qglGetUniformLocation(id, "u_diffuseMap"), 1);
	
	qglEnableVertexAttribArray(ATRB_TEX0);
	qglEnableVertexAttribArray(ATRB_TANGENT);
	qglEnableVertexAttribArray(ATRB_BINORMAL);
	qglEnableVertexAttribArray(ATRB_NORMAL);
	qglEnableVertexAttribArray(ATRB_POSITION);

	qglVertexAttribPointer(ATRB_TEX0, 2, GL_FLOAT, false, 0, currentmodel->st);
	qglVertexAttribPointer(ATRB_TANGENT, 3, GL_FLOAT, false, 0, tangentArray);
	qglVertexAttribPointer(ATRB_NORMAL, 3, GL_FLOAT, false, 0, normalArray);
	qglVertexAttribPointer(ATRB_BINORMAL, 3, GL_FLOAT, false, 0, binormalArray);
	qglVertexAttribPointer(ATRB_POSITION, 3, GL_FLOAT, false, 0, vertexArray);
	
	qglDrawArrays	(GL_TRIANGLES, 0, jj);

	GL_SelectTexture(GL_TEXTURE0_ARB);
	qglDisableVertexAttribArray(ATRB_TEX0);
	qglDisableVertexAttribArray(ATRB_TANGENT);
	qglDisableVertexAttribArray(ATRB_BINORMAL);
	qglDisableVertexAttribArray(ATRB_NORMAL);
	qglDisableVertexAttribArray(ATRB_POSITION);
	GL_BindNullProgram();
	
}

void R_DebugLights (vec3_t lightOrg)
{
	int		i, j;
	float	a;
	vec3_t	v;
	float	rad;

	rad = 5.0;

	qglDisable(GL_CULL_FACE);
	VectorSubtract (lightOrg, r_origin, v);
	qglColor3f (1, 0, 1);

	qglBegin (GL_TRIANGLE_FAN);
	for (i=0 ; i<3 ; i++)
		v[i] = lightOrg[i] - vpn[i]*rad;
	qglVertex3fv (v);
	
	for (i=16 ; i>=0 ; i--)
	{
		a = i/16.0 * M_PI*2;
		for (j=0 ; j<3 ; j++)
			v[j] = lightOrg[j] + vright[j]*cos(a)*rad
				+ vup[j]*sin(a)*rad;
		qglVertex3fv (v);
	}
	qglEnd ();
	qglColor3f (1,1,1);
	qglEnable(GL_CULL_FACE);
}

qboolean R_CullSphere( const vec3_t centre, const float radius, const int clipflags );

void GL_DrawAliasFrameLerpArbBump (dmdl_t *paliashdr)
{
	
	vec3_t			temp, tmp, light;
	float			dist;
	int				i, dlActive = 0, slActive = 0;
	dlight_t		*dlight;     //dynamic lights
	flare_t			*lightSurf;  //lights surfaces lights
	float			diffuseColor[3];
	float			lightRad = 0;
	mat3_t			entityAxis;
	trace_t			r_trace;


	if(r_newrefdef.rdflags & RDF_NOWORLDMODEL){

			diffuseColor[0] = 1.0;
			diffuseColor[1] = 1.0;
			diffuseColor[2] = 1.0;
			VectorCopy(currententity->origin, light);
			light[0] -= 100;
			light[2] += 100;

			VectorSubtract(light, currententity->origin, tmp);
			AnglesToMat3(currententity->angles, entityAxis);
			Mat3_TransposeMultiplyVector(entityAxis, tmp, light);

			GL_DrawAliasFrameLerpArb(paliashdr, light, 999, diffuseColor);

	}
	
	for (i = 0; i < r_numflares; i++) {
		int sidebit;
		float viewplane;

		lightSurf = &r_flares[i];
		
		if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
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

		VectorSubtract(currententity->origin, lightSurf->origin, temp);
		dist = VectorLength(temp);
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
			r_trace = CM_BoxTrace(	currententity->origin, lightSurf->origin, vec3_origin, 
									vec3_origin, r_worldmodel->firstnode, MASK_OPAQUE);
				
			if(r_trace.fraction != 1.0)
				continue;
			}
			
			VectorSubtract(lightSurf->origin, currententity->origin, tmp);
			AnglesToMat3(currententity->angles, entityAxis);
			Mat3_TransposeMultiplyVector(entityAxis, tmp, light);	
			
			lightRad = lightSurf->size * r_shadowWorldLightScale->value;

			R_LightPoint(lightSurf->origin, diffuseColor, true);
			VectorScale(diffuseColor, 2.0, diffuseColor);
			GL_DrawAliasFrameLerpArb(paliashdr, light, lightRad, diffuseColor);
			slActive++;
		}

		dlight = r_newrefdef.dlights;
		for (i = 0; i < r_newrefdef.num_dlights; i++, dlight++) {

			if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
				continue;

			VectorSubtract(currententity->origin, dlight->origin, temp);
			dist = VectorLength(temp);

			if (dist > dlight->intensity)
				continue;		// big distance!

			// light surf behind the wall 
			if (r_newrefdef.areabits){
			r_trace = CM_BoxTrace(	currententity->origin, dlight->origin, vec3_origin, 
									vec3_origin, r_worldmodel->firstnode, MASK_OPAQUE);
			if(r_trace.fraction != 1.0)
			continue;
			}
									
			lightRad = dlight->intensity;
			
			diffuseColor[0] = dlight->color[0];
			diffuseColor[1] = dlight->color[1];
			diffuseColor[2] = dlight->color[2];

			VectorSubtract(dlight->origin, currententity->origin, tmp);
			AnglesToMat3(currententity->angles, entityAxis);
			Mat3_TransposeMultiplyVector(entityAxis, tmp, light);	
			
			GL_DrawAliasFrameLerpArb(paliashdr, light, lightRad, diffuseColor);
			
			dlActive++;
		}	

		if(!dlActive && !slActive){
			
			if(r_newrefdef.rdflags & RDF_NOWORLDMODEL)
				return;

			if (r_newrefdef.areabits){
			r_trace = CM_BoxTrace(	currententity->origin, r_origin, vec3_origin, 
									vec3_origin, r_worldmodel->firstnode, MASK_OPAQUE);
			if(r_trace.fraction != 1.0)
			return;
			}

			VectorCopy(currententity->origin, light);
			light[2]+=100;
			
			R_LightPoint(currententity->origin, diffuseColor, true);
			VectorSubtract(light, currententity->origin, tmp);
			AnglesToMat3(currententity->angles, entityAxis);
			Mat3_TransposeMultiplyVector(entityAxis, tmp, light);	
			GL_DrawAliasFrameLerpArb(paliashdr, light, 250, diffuseColor);
		}

}