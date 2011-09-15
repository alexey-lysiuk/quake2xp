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
// cl_scrn.c -- master for refresh, status bar, console, chat, notify, etc

/*

  full screen console
  put up loading plaque
  blanked background with loading plaque
  blanked background with menu
  cinematics
  full screen image for quit and victory

  end of unit intermissions

  */

#include "client.h"

float scr_con_current;			// aproaches scr_conlines at scr_conspeed
float scr_conlines;				// 0.0 to 1.0 lines of console to display

qboolean scr_initialized;		// ready to draw

int scr_draw_loading;

vrect_t scr_vrect;				// position of render window on screen


cvar_t *scr_viewsize;
cvar_t *scr_conspeed;
cvar_t *scr_centertime;
cvar_t *scr_showturtle;
cvar_t *scr_showpause;
cvar_t *scr_printspeed;

cvar_t *scr_netgraph;
cvar_t *scr_timegraph;
cvar_t *scr_debuggraph;
cvar_t *scr_graphheight;
cvar_t *scr_graphscale;
cvar_t *scr_graphshift;
cvar_t *scr_drawall;

extern cvar_t *cl_drawclock;	// JKnife -- HUD Clock

typedef struct {
	int x1, y1, x2, y2;
} dirty_t;

dirty_t scr_dirty, scr_old_dirty[2];

char crosshair_pic[MAX_QPATH];
int crosshair_width, crosshair_height;

void SCR_TimeRefresh_f(void);
void SCR_Loading_f(void);


/*
===============================================================================

BAR GRAPHS

===============================================================================
*/

/*
==============
CL_AddNetgraph

A new packet was just parsed
==============
*/
void CL_AddNetgraph(void)
{
	int i;
	int in;
	int ping;

	// if using the debuggraph for something else, don't
	// add the net lines
	if (scr_debuggraph->value || scr_timegraph->value)
		return;

	for (i = 0; i < cls.netchan.dropped; i++)
		SCR_DebugGraph(30, 0x40);

	for (i = 0; i < cl.surpressCount; i++)
		SCR_DebugGraph(30, 0xdf);

	// see what the latency was on this packet
	in = cls.netchan.incoming_acknowledged & (CMD_BACKUP - 1);
	ping = cls.realtime - cl.cmd_time[in];
	ping /= 30;
	if (ping > 30)
		ping = 30;
	SCR_DebugGraph(ping, 0xd0);
}


typedef struct {
	float value;
	int color;
} graphsamp_t;

static int current;
static graphsamp_t values[1024];

/*
==============
SCR_DebugGraph
==============
*/
void SCR_DebugGraph(float value, int color)
{
	values[current & 1023].value = value;
	values[current & 1023].color = color;
	current++;
}

/*
==============
SCR_DrawDebugGraph
==============
*/
void SCR_DrawDebugGraph(void)
{
	int a, x, y, w, i, h;
	float v;
	int color;

	//
	// draw the graph
	//
	w = scr_vrect.width;

	x = scr_vrect.x;
	y = scr_vrect.y + scr_vrect.height;
	Draw_Fill(x, y - scr_graphheight->value, w, scr_graphheight->value, 8);

	for (a = 0; a < w; a++) {
		i = (current - 1 - a + 1024) & 1023;
		v = values[i].value;
		color = values[i].color;
		v = v * scr_graphscale->value + scr_graphshift->value;

		if (v < 0)
			v += scr_graphheight->value * (1 +
										   (int) (-v /
												  scr_graphheight->value));
		h = (int) v % (int) scr_graphheight->value;
		Draw_Fill(x + w - 1 - a, y - h, 1, h, color);
	}
}

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/

char scr_centerstring[1024];
float scr_centertime_start;		// for slow victory printing
float scr_centertime_off;
int scr_center_lines;
int scr_erase_center;

/*
==============
SCR_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void SCR_CenterPrint(char *str)
{
	char *s;
	char line[64];
	int i, j, l;

	strncpy(scr_centerstring, str, sizeof(scr_centerstring) - 1);
	scr_centertime_off = scr_centertime->value;
	scr_centertime_start = cl.time;

	// count the number of lines for centering
	scr_center_lines = 1;
	s = str;
	while (*s) {
		if (*s == '\n')
			scr_center_lines++;
		s++;
	}

	// echo it to the console
	Com_Printf
		("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n");

	s = str;
	do {
		// scan the width of the line
		for (l = 0; l < 40; l++)
			if (s[l] == '\n' || !s[l])
				break;
		for (i = 0; i < (40 - l) * 0.5; i++)
			line[i] = ' ';

		for (j = 0; j < l; j++) {
			line[i++] = s[j];
		}

		line[i] = '\n';
		line[i + 1] = 0;

		Com_Printf("%s", line);

		while (*s && *s != '\n')
			s++;

		if (!*s)
			break;
		s++;					// skip the \n
	} while (1);
	Com_Printf
		("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n");
	Con_ClearNotify();
}


void SCR_DrawCenterString(void)
{
	char *start;
	int l;
	int j;
	int x, y;
	int remaining;

// the finale prints the characters one at a time
	remaining = 9999;

	scr_erase_center = 0;
	start = scr_centerstring;

	if (scr_center_lines <= 4)
		y = viddef.height * 0.35;
	else
		y = 48;

	do {
		// scan the width of the line
		for (l = 0; l < 40; l++)
			if (start[l] == '\n' || !start[l])
				break;
		x = (viddef.width - l * 8*cl_fontScale->value) * 0.5;
		SCR_AddDirtyPoint(x, y);
		for (j = 0; j < l; j++, x += 8*cl_fontScale->value) {
			Draw_CharScaled(x, y, cl_fontScale->value, cl_fontScale->value, start[j]);
			if (!remaining--)
				return;
		}
		SCR_AddDirtyPoint(x, y + 8*cl_fontScale->value);

		y += 8*cl_fontScale->value;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			break;
		start++;				// skip the \n
	} while (1);
}

void SCR_CheckDrawCenterString(void)
{
	scr_centertime_off -= cls.frametime;

	if (scr_centertime_off <= 0)
		return;

	SCR_DrawCenterString();
}

//=============================================================================

/*
=================
SCR_CalcVrect

Sets scr_vrect, the coordinates of the rendered window
=================
*/
static void SCR_CalcVrect(void)
{
	int size;

	// bound viewsize
	if (scr_viewsize->value < 40)
		Cvar_Set("viewsize", "40");
	if (scr_viewsize->value > 100)
		Cvar_Set("viewsize", "100");

	size = scr_viewsize->value;

	scr_vrect.width = viddef.width * size / 100;
	scr_vrect.width &= ~7;

	scr_vrect.height = viddef.height * size / 100;
	scr_vrect.height &= ~1;

	scr_vrect.x = (viddef.width - scr_vrect.width) * 0.5;
	scr_vrect.y = (viddef.height - scr_vrect.height) * 0.5;
}


/*
=================
SCR_SizeUp_f

Keybinding command
=================
*/
void SCR_SizeUp_f(void)
{
	Cvar_SetValue("viewsize", scr_viewsize->value + 10);
}


/*
=================
SCR_SizeDown_f

Keybinding command
=================
*/
void SCR_SizeDown_f(void)
{
	Cvar_SetValue("viewsize", scr_viewsize->value - 10);
}

/*
=================
SCR_Sky_f

Set a specific sky and rotation speed
=================
*/
void SCR_Sky_f(void)
{
	float rotate;
	vec3_t axis;

	if (Cmd_Argc() < 2) {
		Com_Printf("Usage: sky <basename> <rotate> <axis x y z>\n");
		return;
	}
	if (Cmd_Argc() > 2)
		rotate = atof(Cmd_Argv(2));
	else
		rotate = 0;
	if (Cmd_Argc() == 6) {
		axis[0] = atof(Cmd_Argv(3));
		axis[1] = atof(Cmd_Argv(4));
		axis[2] = atof(Cmd_Argv(5));
	} else {
		axis[0] = 0;
		axis[1] = 0;
		axis[2] = 1;
	}

	R_SetSky(Cmd_Argv(1), rotate, axis);
}

//============================================================================

/*
==================
SCR_Init
==================
*/
void SCR_Init(void)
{
	scr_viewsize = Cvar_Get("viewsize", "100", CVAR_ARCHIVE);
	scr_conspeed = Cvar_Get("scr_conspeed", "3", 0);
	scr_showturtle = Cvar_Get("scr_showturtle", "0", 0);
	scr_showpause = Cvar_Get("scr_showpause", "1", 0);
	scr_centertime = Cvar_Get("scr_centertime", "2.5", 0);
	scr_printspeed = Cvar_Get("scr_printspeed", "8", 0);
	scr_netgraph = Cvar_Get("netgraph", "0", 0);
	scr_timegraph = Cvar_Get("timegraph", "0", 0);
	scr_debuggraph = Cvar_Get("debuggraph", "0", 0);
	scr_graphheight = Cvar_Get("graphheight", "32", 0);
	scr_graphscale = Cvar_Get("graphscale", "1", 0);
	scr_graphshift = Cvar_Get("graphshift", "0", 0);
	scr_drawall = Cvar_Get("scr_drawall", "0", 0);

//
// register our commands
//
	Cmd_AddCommand("timerefresh", SCR_TimeRefresh_f);
	Cmd_AddCommand("loading", SCR_Loading_f);
	Cmd_AddCommand("sizeup", SCR_SizeUp_f);
	Cmd_AddCommand("sizedown", SCR_SizeDown_f);
	Cmd_AddCommand("sky", SCR_Sky_f);

	scr_initialized = true;
}


/*
==============
SCR_DrawNet
==============
*/
void SCR_DrawNet(void)
{
	if (cls.netchan.outgoing_sequence - cls.netchan.incoming_acknowledged
		< CMD_BACKUP - 1)
		return;

	Draw_Pic2(scr_vrect.x + 64, scr_vrect.y, i_net);
}

/*
==============
SCR_DrawPause
==============
*/
void SCR_DrawPause(void)
{
	
	if (!scr_showpause->value)	// turn off for screenshots
		return;

	if (!cl_paused->value)
		return;

	Draw_ScaledPic((viddef.width - (i_pause->width * cl_fontScale->value)) * 0.5f,
					viddef.height * 0.5f + 8.0f, 
					cl_fontScale->value, cl_fontScale->value,
					i_pause);
}

/*
==============
SCR_DrawLoading
==============
*/

void SCR_DrawLoadingBar(float percent, float scale)
{

		RE_SetColor(NULL);
		Draw_Fill(0, viddef.height - scale * 10 + 1, viddef.width, scale * 3 - 2, 256);
		RE_SetColor(colorGreen);
		Draw_Fill(2, viddef.height - scale * 10 + 3, viddef.width * percent * 0.01, scale * 3 - 6, 256);
		RE_SetColor(NULL);
	
}


void SCR_DrawLoading(void)
{
	int scaled;
	char mapfile[32];
	char *mapname;
	int	fontscale = (float)cl_fontScale->value;
	
	if (!scr_draw_loading)
		return;
	scr_draw_loading = 0;
	if (loadingMessage && cl.configstrings[CS_MODELS + 1][0]) {
		strcpy(mapfile, cl.configstrings[CS_MODELS + 1] + 5);	// skip "maps/"
		mapfile[strlen(mapfile) - 4] = 0;	// cut off ".bsp"
		if (Draw_FindPic(va("/levelshots/%s.jpg", mapfile)))
			Draw_StretchPic(0, 0, viddef.width, viddef.height,
							va("/levelshots/%s.jpg", mapfile));

		if(cl_fontScale->value>1)
			scaled = 8;
		else scaled = 4;

		SCR_DrawLoadingBar(loadingPercent, scaled);


		mapname = cl.configstrings[CS_NAME];
		
		RE_SetColor(colorGreen);
		Draw_StringScaled(0, fontscale*scaled,  fontscale, fontscale, mapname);
		RE_SetColor(colorYellow);
		Draw_StringScaled(0, 24*fontscale, fontscale, fontscale,
					va("%s", loadingMessages[0]));
		Draw_StringScaled(0, 32*fontscale, fontscale, fontscale,
					va("%s", loadingMessages[1]));
		Draw_StringScaled(0, 40*fontscale, fontscale, fontscale,
					va("%s", loadingMessages[2]));
		Draw_StringScaled(0, 48*fontscale, fontscale, fontscale,
					va("%s", loadingMessages[3]));
		RE_SetColor(NULL);

	}

}



//=============================================================================

/*
==================
SCR_RunConsole

Scroll it up or down
==================
*/
void SCR_RunConsole(void)
{
// decide on the height of the console
	if (cls.key_dest == key_console)
		scr_conlines = 0.5;		// half screen
	else
		scr_conlines = 0;		// none visible

	if (scr_conlines < scr_con_current) {
		scr_con_current -= scr_conspeed->value * cls.frametime;
		if (scr_conlines > scr_con_current)
			scr_con_current = scr_conlines;

	} else if (scr_conlines > scr_con_current) {
		scr_con_current += scr_conspeed->value * cls.frametime;
		if (scr_conlines < scr_con_current)
			scr_con_current = scr_conlines;
	}

}

/*
==================
SCR_DrawConsole
==================
*/
void SCR_DrawConsole(void)
{
	Con_CheckResize();

	if (cls.state == ca_disconnected || cls.state == ca_connecting) {	// forced
																		// full
																		// screen
																		// console
		Con_DrawConsole(1.0);
		return;
	}

	if (cls.state != ca_active || !cl.refresh_prepped) {	// connected,
															// but can't
															// render
		Con_DrawConsole(0.5);
		Draw_Fill(0, viddef.height * 0.5f, viddef.width,
				  viddef.height * 0.5f, 0);
		return;
	}

	if (scr_con_current) {
		Con_DrawConsole(scr_con_current);
	} else {
		if (cls.key_dest == key_game || cls.key_dest == key_message)
			Con_DrawNotify();	// only draw notify in game
	}
}

//=============================================================================

/*
================
SCR_BeginLoadingPlaque
================
*/

qboolean needLoadingPlaque(void)
{
	if (!cls.disable_screen || !scr_draw_loading)
		return true;
	return false;
}


void SCR_BeginLoadingPlaque(void)
{
	S_StopAllSounds();
	cl.sound_prepped = false;	// don't play ambients
	CDAudio_Stop();
	if (cls.disable_screen)
		return;
	if (developer->value)
		return;
	if (cls.state == ca_disconnected)
		return;					// if at console, don't bring up the
								// plaque
	if (cls.key_dest == key_console)
		return;
	if (cl.cinematictime > 0)
		scr_draw_loading = 2;	// clear to balack first
	else
		scr_draw_loading = 1;
	SCR_UpdateScreen();
	cls.disable_screen = Sys_Milliseconds();
	cls.disable_servercount = cl.servercount;
}

/*
================
SCR_EndLoadingPlaque
================
*/
void SCR_EndLoadingPlaque(void)
{
	cls.disable_screen = 0;
	scr_draw_loading = 0;
	Con_ClearNotify();

}

/*
================
SCR_Loading_f
================
*/
void SCR_Loading_f(void)
{
	SCR_BeginLoadingPlaque();
}

/*
================
SCR_TimeRefresh_f
================
*/
int entitycmpfnc(const entity_t * a, const entity_t * b)
{
	/*
	 ** all other models are sorted by model then skin
	 */
	if (a->model == b->model) {
		return ((int) a->skin - (int) b->skin);
	} else {
		return ((int) a->model - (int) b->model);
	}
}




void SCR_TimeRefresh_f(void)
{
	int i;
	int start, stop;
	float time;

	if (cls.state != ca_active)
		return;

	start = Sys_Milliseconds();

	if (Cmd_Argc() == 2) {		// run without page flipping
		R_BeginFrame(0);
		for (i = 0; i < 128; i++) {
			cl.refdef.viewangles[1] = i / 128.0 * 360.0;
			R_RenderFrame(&cl.refdef, false);
		}
		GLimp_EndFrame();
	} else {
		for (i = 0; i < 128; i++) {
			cl.refdef.viewangles[1] = i / 128.0 * 360.0;

			R_BeginFrame(0);
			R_RenderFrame(&cl.refdef, false);
			GLimp_EndFrame();
		}
	}

	stop = Sys_Milliseconds();
	time = (stop - start) / 1000.0;
	Com_Printf("%f seconds ("S_COLOR_YELLOW"%f"S_COLOR_WHITE" fps)\n", time, 128 / time);
}

/*
=================
SCR_AddDirtyPoint
=================
*/
void SCR_AddDirtyPoint(int x, int y)
{
	if (x < scr_dirty.x1)
		scr_dirty.x1 = x;
	if (x > scr_dirty.x2)
		scr_dirty.x2 = x;
	if (y < scr_dirty.y1)
		scr_dirty.y1 = y;
	if (y > scr_dirty.y2)
		scr_dirty.y2 = y;
}

void SCR_DirtyScreen(void)
{
	SCR_AddDirtyPoint(0, 0);
	SCR_AddDirtyPoint(viddef.width - 1, viddef.height - 1);
}

/*
==============
SCR_TileClear

Clear any parts of the tiled background that were drawn on last frame
==============
*/
void SCR_TileClear(void)
{
	int i;
	int top, bottom, left, right;
	dirty_t clear;

	if (scr_drawall->value)
		SCR_DirtyScreen();		// for power vr or broken page flippers...

	if (scr_con_current == 1.0)
		return;					// full screen console
	if (scr_viewsize->value == 100)
		return;					// full screen rendering
	if (cl.cinematictime > 0)
		return;					// full screen cinematic

	// erase rect will be the union of the past three frames
	// so tripple buffering works properly
	clear = scr_dirty;
	for (i = 0; i < 2; i++) {
		if (scr_old_dirty[i].x1 < clear.x1)
			clear.x1 = scr_old_dirty[i].x1;
		if (scr_old_dirty[i].x2 > clear.x2)
			clear.x2 = scr_old_dirty[i].x2;
		if (scr_old_dirty[i].y1 < clear.y1)
			clear.y1 = scr_old_dirty[i].y1;
		if (scr_old_dirty[i].y2 > clear.y2)
			clear.y2 = scr_old_dirty[i].y2;
	}

	scr_old_dirty[1] = scr_old_dirty[0];
	scr_old_dirty[0] = scr_dirty;

	scr_dirty.x1 = 9999;
	scr_dirty.x2 = -9999;
	scr_dirty.y1 = 9999;
	scr_dirty.y2 = -9999;

	// don't bother with anything convered by the console)
	top = scr_con_current * viddef.height;
	if (top >= clear.y1)
		clear.y1 = top;

	if (clear.y2 <= clear.y1)
		return;					// nothing disturbed

	top = scr_vrect.y;
	bottom = top + scr_vrect.height - 1;
	left = scr_vrect.x;
	right = left + scr_vrect.width - 1;

	if (clear.y1 < top) {		// clear above view screen
		i = clear.y2 < top - 1 ? clear.y2 : top - 1;
		Draw_TileClear2(clear.x1, clear.y1,
						clear.x2 - clear.x1 + 1, i - clear.y1 + 1,
						i_backtile);
		clear.y1 = top;
	}
	if (clear.y2 > bottom) {	// clear below view screen
		i = clear.y1 > bottom + 1 ? clear.y1 : bottom + 1;
		Draw_TileClear2(clear.x1, i,
						clear.x2 - clear.x1 + 1, clear.y2 - i + 1,
						i_backtile);
		clear.y2 = bottom;
	}
	if (clear.x1 < left) {		// clear left of view screen
		i = clear.x2 < left - 1 ? clear.x2 : left - 1;
		Draw_TileClear2(clear.x1, clear.y1,
						i - clear.x1 + 1, clear.y2 - clear.y1 + 1,
						i_backtile);
		clear.x1 = left;
	}
	if (clear.x2 > right) {		// clear left of view screen
		i = clear.x1 > right + 1 ? clear.x1 : right + 1;
		Draw_TileClear2(i, clear.y1,
						clear.x2 - i + 1, clear.y2 - clear.y1 + 1,
						i_backtile);
		clear.x2 = right;
	}

}


//===============================================================


#define STAT_MINUS		10		// num frame for '-' stats digit
char *sb_nums[2][11] = {
	{"num_0", "num_1", "num_2", "num_3", "num_4", "num_5",
	 "num_6", "num_7", "num_8", "num_9", "num_minus"},
	{"anum_0", "anum_1", "anum_2", "anum_3", "anum_4", "anum_5",
	 "anum_6", "anum_7", "anum_8", "anum_9", "anum_minus"}
};

#define	ICON_WIDTH	24
#define	ICON_HEIGHT	24
#define	CHAR_WIDTH	16
#define	ICON_SPACE	8



/*
================
SizeHUDString

Allow embedded \n in the string
================
*/
void SizeHUDString(char *string, int *w, int *h)
{
	int lines, width, current;

	lines = 1;
	width = 0;

	current = 0;
	while (*string) {
		if (*string == '\n') {
			lines++;
			current = 0;
		} else {
			current++;
			if (current > width)
				width = current;
		}
		string++;
	}

	*w = width * 8;
	*h = lines * 8;
}
/*
void DrawHUDString(int x, int y, int centerwidth, int xor, char *string,
				   ...)
{
	int margin;
	char line[1024];
	int width;
	int i;
	va_list argptr;
	char msg[2048], *strp = msg;

	va_start(argptr, string);
	vsprintf(msg, string, argptr);
	va_end(argptr);

	margin = x;

	while (*strp) {
		// scan out one line of text from the string
		width = 0;
		while (*strp && *strp != '\n')
			line[width++] = *strp++;
		line[width] = 0;

		if (centerwidth)
			x = margin + (centerwidth - width * 8) * 0.5;
		else
			x = margin;
		for (i = 0; i < width; i++) {
			Draw_Char(x, y, line[i] ^ xor);
			x += 8;
		}
		if (*strp) {
			strp++;				// skip the \n
			x = margin;
			y += 8;
		}
	}
}
*/

void DrawHUDString (float x, float y, float scale_x, float scale_y, int centerwidth, int xor, char *string)
{
	float	margin;
	char	line[1024];
	int		width;
	int		i;

	margin = x;

	while (*string)
	{
		// scan out one line of text from the string
		width = 0;
		while (*string && *string != '\n')
			line[width++] = *string++;
		line[width] = 0;

		if (centerwidth)
			x = margin + (centerwidth - width*8)*scale_x/2;
		else
			x = margin;
		for (i=0 ; i<width ; i++)
		{
			Draw_CharScaled (x, y, scale_x, scale_y, line[i]^xor);
			x += 8*scale_x;
		}
		if (*string)
		{
			string++;	// skip the \n
			x = margin;
			y += 8*scale_y;
		}
	}
}

/*
==============
SCR_DrawField
==============
*/
void SCR_DrawField (int x, int y, float scale_x, float scale_y, int color, int width, int value)
{
	char	num[16], *ptr;
	int		l;
	int		frame;

	if (width < 1)
		return;

	// draw number string
	if (width > 5)
		width = 5;

	SCR_AddDirtyPoint (x, y);
	SCR_AddDirtyPoint (x+(width*CHAR_WIDTH+2)*scale_x, y+23*scale_y);

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;
	x += (2 + CHAR_WIDTH*(width - l))*scale_x;

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		Draw_PicScaled (x,y,scale_x,scale_y,sb_nums[color][frame]);
		x += CHAR_WIDTH*scale_x;
		ptr++;
		l--;
	}
}



/*
===============
SCR_TouchPics

Allows rendering code to cache all needed sbar graphics
===============
*/

void SCR_TouchPics(void)
{
	int i, j;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 11; j++)
			Draw_FindPic(sb_nums[i][j]);

	if (crosshair->value) {
		if (crosshair->value > 4 || crosshair->value < 0)
			crosshair->value = 4;

		Com_sprintf(crosshair_pic, sizeof(crosshair_pic), "ch%i",
					(int) (crosshair->value));
		Draw_GetPicSize(&crosshair_width, &crosshair_height,
						crosshair_pic);



		if (!crosshair_width)
			crosshair_pic[0] = 0;



	}
}
//=======================================================

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.
==================
*/
extern cvar_t *r_speeds;
void SCR_DrawStats(qboolean hud);
void SCR_DrawLayout(void);

int c_brush_polys, 
	c_alias_polys, 
	c_flares,
	c_visible_lightmaps,
	c_visible_textures,
	c_flares,
	c_shadow_volumes,
	c_decals,
	c_shadow_tris,
	c_part_tris,
	c_decal_tris;

extern cvar_t *cl_drawfps;
int frames_this_second = 0;
extern cvar_t *cl_hudScale;

void SCR_DrawSpeeds(void){
	
	char	bsp[18], alias[18], st[18], partTris[18], flares[18], shadow[18], decals[18], dtr[18];
	int		fontscale = (float)cl_fontScale->value;

	if(!r_speeds->value)
		return;

	sprintf(bsp,		"%i w_tris",	c_brush_polys);
	sprintf(alias,		"%i m_tris",	c_alias_polys);
	sprintf(st,			"%i s_tris",	c_shadow_tris);
	sprintf(partTris,	"%i p_tris",	c_part_tris);
	sprintf(flares,		"%i flares",	c_flares);
	sprintf(shadow,		"%i shadows",	c_shadow_volumes);
	sprintf(decals,		"%i decals",	c_decals);
	sprintf(dtr,		"%i d_tris",	c_decal_tris);
	
	RE_SetColor(colorCyan);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5,	fontscale, fontscale, bsp);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+10*fontscale, fontscale, fontscale, alias);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+20*fontscale, fontscale, fontscale, st);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+30*fontscale, fontscale, fontscale, partTris);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+40*fontscale, fontscale, fontscale, dtr);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+50*fontscale, fontscale, fontscale, flares);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+60*fontscale, fontscale, fontscale, shadow);
	Draw_StringScaled(viddef.width - 95*fontscale, viddef.height*0.5+70*fontscale, fontscale, fontscale, decals);
	RE_SetColor(colorWhite);
}

void SCR_DrawFPS(void){
	
	static char	fps[8];
	static int	millis;
	int		fontscale = (float)cl_fontScale->value;
	
//	if (frames_this_second < 50)
//		Draw_ScaledPic((viddef.width - i_turtle->width*fontscale-10), (viddef.height*0.8) 
//									- (i_turtle->height*fontscale), fontscale, fontscale, i_turtle);	

	frames_this_second++;
	
	if(curtime - millis >= 1000){
			
		Com_sprintf(fps, sizeof(fps), "%4dfps", frames_this_second);
	
		millis = curtime;
		frames_this_second = 0;
		
	}
	if (cl_drawfps->value && (cls.state == ca_active))
		Draw_StringScaled(viddef.width - 57*fontscale, viddef.height*0.8, fontscale, fontscale, fps);
		
}




void SCR_DrawClock(void)
{
	char	timebuf[20];
	char	tmpbuf[24];
	char	datebuf[20];
	char	tmpdatebuf[24];
	int	fontscale = (float)cl_fontScale->value;
#ifndef _WIN32
	struct tm *tm;
	time_t aclock;

	time(&aclock);
	tm = localtime(&aclock);
	strftime(timebuf, sizeof(timebuf), "%T", tm);
	strftime(datebuf, sizeof(datebuf), "%D", tm);
#else
	_strtime(timebuf);
	_strdate(datebuf);
#endif

	sprintf(tmpbuf, "Time %s", timebuf);
	sprintf(tmpdatebuf, "Date %s", datebuf);
	
	if (!cl_drawfps->value)
		Draw_StringScaled(viddef.width - 105*fontscale, viddef.height*0.8, fontscale, fontscale, tmpbuf);
		else
		Draw_StringScaled(viddef.width - 105*fontscale, viddef.height*0.8+10*fontscale , fontscale, fontscale, tmpbuf);


	if (!cl_drawfps->value)
		Draw_StringScaled(viddef.width - 105*fontscale, viddef.height*0.8+10*fontscale, fontscale, fontscale, tmpdatebuf);
		else
		Draw_StringScaled(viddef.width - 105*fontscale, viddef.height*0.8+20*fontscale, fontscale, fontscale, tmpdatebuf);
	

}
extern qboolean menuActive;
extern cvar_t *r_mode;
void R_VCFreeFrame();

void SCR_UpdateScreen(void)
{
	int numframes;
	int i;
	float separation[2] = { 0, 0 };

	// if the screen is disabled (loading plaque is up, or vid mode
	// changing)
	// do nothing at all
	if (cls.disable_screen) {
		if (cls.download)		// Knightmare- don't time out on downloads
			cls.disable_screen = Sys_Milliseconds();
		if (Sys_Milliseconds() - cls.disable_screen > 120000 && cl.refresh_prepped && !(cl.cinematictime > 0)){
			cls.disable_screen = 0;
			Com_Printf("Loading plaque timed out.\n");
			return;
		}
		scr_draw_loading = 2;
	}

	if (!scr_initialized || !con.initialized)
		return;					// not initialized yet


	if(cl_fontScale->value >1)
		cl_fontScale->value = 2;

	if(cl_fontScale->value < 1)
		cl_fontScale->value = 1;

	if(cl_hudScale->value >1)
		Cvar_SetValue("cl_hudScale", 1);

	if(cl_hudScale->value < 0.1)
		Cvar_SetValue("cl_hudScale", 0.1);

	if (cl_stereo_separation->value > 1.0)
		Cvar_SetValue("cl_stereo_separation", 1.0);
	else if (cl_stereo_separation->value < 0)
		Cvar_SetValue("cl_stereo_separation", 0.0);
	
	if(cl_fontScale->value < 1)
		Cvar_SetValue("cl_fontScale", 1);
	else
		if(cl_fontScale->value > 2)
		Cvar_SetValue("cl_fontScale", 2);

	if (cl_stereo->value) {
		numframes = 2;
		separation[0] = -cl_stereo_separation->value / 2;
		separation[1] = cl_stereo_separation->value / 2;
	} else {
		separation[0] = 0;
		separation[1] = 0;
		numframes = 1;
	}

	for (i = 0; i < numframes; i++) {
		R_BeginFrame(separation[i]);

		if (scr_draw_loading == 2) {	// loading plaque over black
										// screen
			R_SetPalette(NULL);
			SCR_DrawLoading();

			if (cls.disable_screen)
				scr_draw_loading = 2;


			// NO FULLSCREEN CONSOLE!!!
			continue;
		}
		// if a cinematic is supposed to be running, handle menus
		// and console specially
		else if (cl.cinematictime > 0) {
			if (cls.key_dest == key_menu) {
				if (cl.cinematicpalette_active) {
					R_SetPalette(NULL);
					cl.cinematicpalette_active = false;
				}
				R_VCFreeFrame();
				M_Draw();
			} else
				SCR_DrawCinematic();
		} else {


			// make sure the game palette is active
			if (cl.cinematicpalette_active) {
				R_SetPalette(NULL);
				cl.cinematicpalette_active = false;
			}
			// do 3D refresh drawing, and then update the screen
			SCR_CalcVrect();

			// clear any dirty part of the background
			SCR_TileClear();
			
			V_RenderView(separation[i]);

		
			SCR_DrawNet();
			SCR_CheckDrawCenterString();


			if (scr_timegraph->value)
				SCR_DebugGraph(cls.frametime * 300, 0);

			if (scr_debuggraph->value || scr_timegraph->value
				|| scr_netgraph->value)
				SCR_DrawDebugGraph();

			SCR_DrawPause();
			SCR_DrawSpeeds();
			SCR_DrawFPS();
			if (cl_drawclock->value && (cls.state == ca_active))
				SCR_DrawClock();
			
			menuActive = false;
			
			SCR_DrawConsole();

			if (need_free_vbo)		// ���� ��������� ������� - �������
			R_VCFreeFrame();	// � M_Draw ����� ���������� ��������� �������
			
			M_Draw();
			
			SCR_DrawLoading();
		
			if(!menuActive)	{	
				SCR_DrawStats(false);
			if (cl.frame.playerstate.stats[STAT_LAYOUTS] & 1)
				SCR_DrawLayout();
			if (cl.frame.playerstate.stats[STAT_LAYOUTS] & 2)
				CL_DrawInventory();
				SCR_DrawStats(true);
			}
		}

	}
	GLimp_EndFrame();
}