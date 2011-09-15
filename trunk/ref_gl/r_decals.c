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
//Decals based on EGL code. 

#include "r_local.h"

vec4_t shadelight_surface;

int R_GetClippedFragments(vec3_t origin, float radius, mat3_t axis,
						  int maxfverts, vec3_t * fverts, int maxfragments,
						  fragment_t * fragments);

/*
=================
PlaneTypeForNormal
=================
*/

int PlaneTypeForNormal(const vec3_t normal)
{
	vec_t ax, ay, az;

	// NOTE: should these have an epsilon around 1.0? 
	if (normal[0] >= 1.0)
		return PLANE_X;
	if (normal[1] >= 1.0)
		return PLANE_Y;
	if (normal[2] >= 1.0)
		return PLANE_Z;

	ax = fabs(normal[0]);
	ay = fabs(normal[1]);
	az = fabs(normal[2]);

	if (ax >= ay && ax >= az)
		return PLANE_ANYX;
	if (ay >= ax && ay >= az)
		return PLANE_ANYY;
	return PLANE_ANYZ;
}

/*
===============
R_RenderDecals
===============
*/

#define MAX_DECAL_ARRAY_VERTS 4096
#define MAX_DECAL_INDICES     8192

vec4_t DecalColorArray [MAX_DECAL_ARRAY_VERTS];
vec2_t DecalTexCoordArray [MAX_DECAL_ARRAY_VERTS];
vec3_t DecalVertexArray [MAX_DECAL_ARRAY_VERTS];

qboolean R_CullSphere( const vec3_t centre, const float radius, const int clipflags );

void R_RenderDecals(void)
{
     rdecals_t *dl;
     int i, x;
     vec3_t v;
     unsigned     tex, texture = 0;
     int     blendS = 0, blendD = 0;
     int     numIndices = 0, numVertices = 0;
     int     indices[MAX_DECAL_INDICES];
          
     qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
     qglTexCoordPointer (2, GL_FLOAT, sizeof(vec2_t), DecalTexCoordArray[0]);
     qglEnableClientState (GL_COLOR_ARRAY);
     qglColorPointer (4, GL_FLOAT, sizeof(vec4_t), DecalColorArray[0]);
     qglEnableClientState (GL_VERTEX_ARRAY);
     qglVertexPointer (3, GL_FLOAT, sizeof(vec3_t), DecalVertexArray[0]);
     
	 qglEnable(GL_POLYGON_OFFSET_FILL);
     qglPolygonOffset(-1, -1);

     qglDepthMask(GL_FALSE);
     qglEnable(GL_BLEND);

     for (dl = r_newrefdef.decals, i = 0; i < r_newrefdef.numDecals; i++, dl++) {
          
		 if (!dl->node || dl->node->visframe != r_visframecount)
               continue;
			
		     // look if it has a bad type
          if (dl->type < 0 || dl->type >= DECAL_MAX)
               continue;

          VectorSubtract(dl->org, r_origin, v);
          if (DotProduct(dl->direction, v) < 0.0)
               continue;
       	
		  if( R_CullSphere(dl->org, dl->size*1.3, 15 ) )
				continue;

          if (dl->flags == DF_OVERBRIGHT)
               GL_Overbrights(true);
          else
               GL_Overbrights(false);
          
          tex = r_decaltexture[dl->type]->texnum;
          
          if (texture != tex || blendD != dl->blendD || blendS != dl->blendS) {
          // flush array if new texture/blend
          if (numIndices) {
				qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);
				c_decal_tris += numIndices/3;
				numVertices = 0;
				numIndices = 0;
          }

			texture = tex;
            blendD = dl->blendD;
            blendS = dl->blendS;
			
            GL_MBind(GL_TEXTURE0_ARB, texture);
            qglBlendFunc(dl->blendD, dl->blendS);
          }

     //
     // array is full, flush to screen
     //
     if ((numIndices >= MAX_DECAL_INDICES - (dl->numverts - 2) * 3) || 
		 (numVertices >= MAX_DECAL_ARRAY_VERTS - dl->numverts)) {
          
		 qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);
         c_decal_tris = numIndices/3;
		 numVertices = 0;
         numIndices = 0;
     }

     // set vertices
          for (x = 0; x < dl->numverts; x++) {
               DecalColorArray      [x + numVertices][0] = dl->color[0];
               DecalColorArray      [x + numVertices][1] = dl->color[1];
               DecalColorArray      [x + numVertices][2] = dl->color[2];
               DecalColorArray      [x + numVertices][3] = dl->alpha;

               DecalTexCoordArray   [x + numVertices][0] = dl->stcoords[x][0];
               DecalTexCoordArray   [x + numVertices][1] = dl->stcoords[x][1];

               DecalVertexArray     [x + numVertices][0] = dl->verts[x][0];
               DecalVertexArray     [x + numVertices][1] = dl->verts[x][1];
               DecalVertexArray     [x + numVertices][2] = dl->verts[x][2];
          }

     // set indices
     for (x = 0; x < dl->numverts - 2; x++) {
          indices[numIndices+x*3+0] = numVertices;
          indices[numIndices+x*3+1] = numVertices + x + 1;
          indices[numIndices+x*3+2] = numVertices + x + 2;
     }
     numVertices += dl->numverts;
     numIndices += (dl->numverts - 2) * 3;
     c_decals++;
     }     

     // draw the rest
	 if (numIndices){
          qglDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);
		c_decal_tris += numIndices/3;
	 }

     qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     qglDisable(GL_BLEND);
     qglDisableClientState(GL_VERTEX_ARRAY);
     qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
     qglDisableClientState(GL_COLOR_ARRAY);
     qglColor4f(1, 1, 1, 1);
     qglDepthMask(GL_TRUE);
     qglDisable(GL_POLYGON_OFFSET_FILL);
}





#define	ON_EPSILON			0.1	// point on plane side epsilon
#define BACKFACE_EPSILON	0.01

static int numFragmentVerts;
static int maxFragmentVerts;
static vec3_t *fragmentVerts;

static int numClippedFragments;
static int maxClippedFragments;
static fragment_t *clippedFragments;

static int fragmentFrame;
static cplane_t fragmentPlanes[6];

/*
=================
R_ClipPoly
=================
*/

static void R_ClipPoly(int nump, vec4_t vecs, int stage, fragment_t * fr)
{
	cplane_t *plane;
	qboolean front, back;
	vec4_t newv[MAX_DECAL_VERTS];
	float *v, d, dists[MAX_DECAL_VERTS];
	int newc, i, j, sides[MAX_DECAL_VERTS];

	if (nump > MAX_DECAL_VERTS - 2)
		Com_Printf("R_ClipPoly: MAX_DECAL_VERTS");
	if (stage == 6) {			// fully clipped
		if (nump > 2) {
			fr->numverts = nump;
			fr->firstvert = numFragmentVerts;

			if (numFragmentVerts + nump >= maxFragmentVerts)
				nump = maxFragmentVerts - numFragmentVerts;

			for (i = 0, v = vecs; i < nump; i++, v += 4)
				VectorCopy(v, fragmentVerts[numFragmentVerts + i]);

			numFragmentVerts += nump;
		}

		return;
	}

	front = back = false;
	plane = &fragmentPlanes[stage];
	for (i = 0, v = vecs; i < nump; i++, v += 4) {
		d = PlaneDiff(v, plane);
		if (d > ON_EPSILON) {
			front = true;
			sides[i] = SIDE_FRONT;
		} else if (d < -ON_EPSILON) {
			back = true;
			sides[i] = SIDE_BACK;
		} else {
			sides[i] = SIDE_ON;
		}

		dists[i] = d;
	}

	if (!front)
		return;

	// clip it
	sides[i] = sides[0];
	dists[i] = dists[0];
	VectorCopy(vecs, (vecs + (i * 4)));
	newc = 0;

	for (i = 0, v = vecs; i < nump; i++, v += 4) {
		switch (sides[i]) {
		case SIDE_FRONT:
			VectorCopy(v, newv[newc]);
			newc++;
			break;
		case SIDE_BACK:
			break;
		case SIDE_ON:
			VectorCopy(v, newv[newc]);
			newc++;
			break;
		}

		if (sides[i] == SIDE_ON || sides[i + 1] == SIDE_ON
			|| sides[i + 1] == sides[i])
			continue;

		d = dists[i] / (dists[i] - dists[i + 1]);
		for (j = 0; j < 3; j++)
			newv[newc][j] = v[j] + d * (v[j + 4] - v[j]);
		newc++;
	}

	// continue
	R_ClipPoly(newc, newv[0], stage + 1, fr);
}

/*
=================
R_PlanarSurfClipFragment
=================
*/

static void R_PlanarSurfClipFragment(mnode_t * node, msurface_t * surf,
									 vec3_t normal)
{
	int i;
	float *v, *v2, *v3;
	fragment_t *fr;
	vec4_t verts[MAX_DECAL_VERTS];

	// bogus face
	if (surf->numedges < 3)
		return;

	// greater than 60 degrees
	if (surf->flags & SURF_PLANEBACK) {
		if (-DotProduct(normal, surf->plane->normal) < 0.5)
			return;
	} else {
		if (DotProduct(normal, surf->plane->normal) < 0.5)
			return;
	}

	v = surf->polys->verts[0];
	// copy vertex data and clip to each triangle
	for (i = 0; i < surf->polys->numverts - 2; i++) {
		fr = &clippedFragments[numClippedFragments];
		fr->numverts = 0;
		fr->node = node;
		fr->surf = surf;

		v2 = surf->polys->verts[0] + (i + 1) * VERTEXSIZE;
		v3 = surf->polys->verts[0] + (i + 2) * VERTEXSIZE;

		VectorCopy(v, verts[0]);
		VectorCopy(v2, verts[1]);
		VectorCopy(v3, verts[2]);
		R_ClipPoly(3, verts[0], 0, fr);

		if (fr->numverts) {
			numClippedFragments++;

			if ((numFragmentVerts >= maxFragmentVerts)
				|| (numClippedFragments >= maxClippedFragments)) {
				return;
			}
		}
	}
}

/*
=================
R_RecursiveFragmentNode
=================
*/

static void R_RecursiveFragmentNode(mnode_t * node, vec3_t origin,
									float radius, vec3_t normal)
{
	float dist;
	cplane_t *plane;

  mark0:
	if ((numFragmentVerts >= maxFragmentVerts)
		|| (numClippedFragments >= maxClippedFragments))
		return;					// already reached the limit somewhere
								// else

	if (node->contents == CONTENTS_SOLID)
		return;

	if (node->contents != CONTENTS_NODE) {
		// leaf
		int c;
		mleaf_t *leaf;
		msurface_t *surf, **mark;

		leaf = (mleaf_t *) node;
		if (!(c = leaf->nummarksurfaces))
			return;

		mark = leaf->firstmarksurface;
		do {
			if ((numFragmentVerts >= maxFragmentVerts)
				|| (numClippedFragments >= maxClippedFragments))
				return;
			
			surf = *mark++;
			if (surf->fragmentframe == fragmentFrame)
				continue;

			surf->fragmentframe = fragmentFrame;
			if ((surf->texinfo->
				 flags & (SURF_SKY | SURF_WARP | SURF_NODRAW)))
				continue;

			R_PlanarSurfClipFragment(node, surf, normal);

		} while (--c);

		return;
	}

	plane = node->plane;
	dist = PlaneDiff(origin, plane);

	if (dist > radius) {
		node = node->children[0];
		goto mark0;
	}
	if (dist < -radius) {
		node = node->children[1];
		goto mark0;
	}

	R_RecursiveFragmentNode(node->children[0], origin, radius, normal);
	R_RecursiveFragmentNode(node->children[1], origin, radius, normal);
}

/*
=================
R_GetClippedFragments
=================
*/

int R_GetClippedFragments(vec3_t origin, float radius, mat3_t axis,
						  int maxfverts, vec3_t * fverts, int maxfragments,
						  fragment_t * fragments)
{
	int i;
	float d;

	fragmentFrame++;

	// initialize fragments
	numFragmentVerts = 0;
	maxFragmentVerts = maxfverts;
	fragmentVerts = fverts;

	numClippedFragments = 0;
	maxClippedFragments = maxfragments;
	clippedFragments = fragments;

	// calculate clipping planes
	for (i = 0; i < 3; i++) {
		d = DotProduct(origin, axis[i]);

		VectorCopy(axis[i], fragmentPlanes[i * 2].normal);
		fragmentPlanes[i * 2].dist = d - radius;
		fragmentPlanes[i * 2].type =
			PlaneTypeForNormal(fragmentPlanes[i * 2].normal);

		VectorNegate(axis[i], fragmentPlanes[i * 2 + 1].normal);
		fragmentPlanes[i * 2 + 1].dist = -d - radius;
		fragmentPlanes[i * 2 + 1].type =
			PlaneTypeForNormal(fragmentPlanes[i * 2 + 1].normal);
	}

	R_RecursiveFragmentNode(r_worldmodel->nodes, origin, radius, axis[0]);

	return numClippedFragments;
}