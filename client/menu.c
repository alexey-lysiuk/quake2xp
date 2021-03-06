/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
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
#include <ctype.h>

#include "client.h"
#include "qmenu.h"
#include "snd_loc.h"
#include "../ref_gl/r_local.h"

static int m_main_cursor;
extern qboolean ru_loc;
void M_Option_Banner(image_t* banner[2]);

#define NUM_CURSOR_FRAMES 15

int convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n);

void M_Menu_Main_f(void);
void M_Menu_Game_f(void);
void M_Menu_LoadGame_f(void);
void M_Menu_SaveGame_f(void);
void M_Menu_PlayerConfig_f(void);
void M_Menu_DownloadOptions_f(void);
void M_Menu_Credits_f(void);
void M_Menu_Multiplayer_f(void);
void M_Menu_JoinServer_f(void);
void M_Menu_AddressBook_f(void);
void M_Menu_StartServer_f(void);
void M_Menu_DMOptions_f(void);
void M_Menu_Video_f(void);
void M_Menu_Options_f(void);
void M_Menu_Keys_f(void);
void M_Menu_Quit_f(void);

void M_Menu_Advanced_f(void);
void M_AdvancedInit(void);

qboolean m_entersound;			// play after drawing a frame, so caching
								// won't disrupt the sound

void(*m_drawfunc) (void);
int(*m_keyfunc) (int key);

extern cvar_t *cl_hudScale;
model_t *currentPlayerWeapon;
qboolean drawIDlogo;
struct model_s* cl_menu_id_logo;
struct model_s* cl_menu_quad_model;

void R_MenuBackGround();

//=============================================================================
/* Support Routines */

#define	MAX_MENU_DEPTH	8


typedef struct {
	void(*draw) (void);
	int(*key) (int k);
} menulayer_t;

menulayer_t m_layers[MAX_MENU_DEPTH];
uint m_menudepth;

static void M_Banner(image_t *banner[2]) {
	int w, h;

	w = banner[0]->width;
	h = banner[0]->height;

	if (cl_fontScale->value == 2) {
		Draw_ScaledPic((viddef.width / 2) - (w * 0.5), (viddef.height / 2) - (130 + (h * cl_fontScale->value)), cl_fontScale->value, cl_fontScale->value, banner[0]);
		Draw_ScaledBumpPic((viddef.width / 2) - (w * 0.5), (viddef.height / 2) - (130 + (h * cl_fontScale->value)), cl_fontScale->value, cl_fontScale->value, banner[0], banner[1]);
	}
	else if (cl_fontScale->value >= 3) {
		Draw_ScaledPic((viddef.width / 2) - (w * 0.75), (viddef.height / 2) - (170 + (h / 0.65 * cl_fontScale->value)), cl_fontScale->value, cl_fontScale->value, banner[0]);
		Draw_ScaledBumpPic((viddef.width / 2) - (w * 0.75), (viddef.height / 2) - (170 + (h / 0.65 * cl_fontScale->value)), cl_fontScale->value, cl_fontScale->value, banner[0], banner[1]);
	}
}

void M_PushMenu(void(*draw) (void), int(*key) (int k)) {
	int i;

	if (Cvar_VariableInteger("maxclients") == 1 && Com_ServerState())
		Cvar_Set("paused", "1");

	// if this menu is already present, drop back to that level
	// to avoid stacking menus by hotkeys
	for (i = 0; i < m_menudepth; i++)
		if (m_layers[i].draw == draw && m_layers[i].key == key) {
			m_menudepth = i;
		}

	if (i == m_menudepth) {
		if (m_menudepth >= MAX_MENU_DEPTH)
			Com_Error(ERR_FATAL, "M_PushMenu: MAX_MENU_DEPTH");
		m_layers[m_menudepth].draw = m_drawfunc;
		m_layers[m_menudepth].key = m_keyfunc;
		m_menudepth++;
	}

	m_drawfunc = draw;
	m_keyfunc = key;
	cls.menuActive = qtrue;

	m_entersound = qtrue;

	cls.key_dest = key_menu;
}

void M_ForceMenuOff(void) {
	m_drawfunc = 0;
	m_keyfunc = 0;
	cls.key_dest = key_game;
	m_menudepth = 0;
	Key_ClearStates();

	cls.menuActive = qfalse;

	Cvar_Set("paused", "0");
}

void M_PopMenu(void) {
	S_StartLocalSound(fastsound_descriptor[menu_out_sound]);
	if (m_menudepth < 1)
		Com_Error(ERR_FATAL, "M_PopMenu: depth < 1");
	m_menudepth--;
	
	if (m_menudepth) {
		m_drawfunc = m_layers[m_menudepth].draw;
		m_keyfunc = m_layers[m_menudepth].key;
	}

	if (!m_menudepth)
		M_ForceMenuOff();
}


int Default_MenuKey(menuframework_s * m, int key) {
	int sound = 0;
	menucommon_s *item;

	if (m) {
		if ((item = Menu_ItemAtCursor(m)) != 0) {
			if (item->type == MTYPE_FIELD) {
				if (Field_Key((menufield_s *)item, key))
					return 0;
			}
		}
	}

	switch (key) {
	case K_ESCAPE:
	case K_XPAD_B:

		M_PopMenu();
		return menu_out_sound;
	case K_KP_UPARROW:
	case K_UPARROW:
	case K_XPAD_DPAD_UP:
		if (m) {
			m->cursor--;
			Menu_AdjustCursor(m, -1);
			sound = menu_move_sound;
		}
		break;
	case K_TAB:
		if (m) {
			m->cursor++;
			Menu_AdjustCursor(m, 1);
			sound = menu_move_sound;
		}
		break;
	case K_KP_DOWNARROW:
	case K_DOWNARROW:
	case K_XPAD_DPAD_DOWN:
		if (m) {
			m->cursor++;
			Menu_AdjustCursor(m, 1);
			sound = menu_move_sound;
		}
		break;
	case K_KP_LEFTARROW:
	case K_LEFTARROW:
	case K_XPAD_DPAD_LEFT:
		if (m) {
			Menu_SlideItem(m, -1);
			sound = menu_move_sound;
		}
		break;
	case K_KP_RIGHTARROW:
	case K_RIGHTARROW:
	case K_XPAD_DPAD_RIGHT:
		if (m) {
			Menu_SlideItem(m, 1);
			sound = menu_move_sound;
		}
		break;

	case K_MOUSE1:
	case K_XPAD_A:
	case K_KP_ENTER:
	case K_ENTER:
		if (m)
			Menu_SelectItem(m);
		sound = menu_move_sound;
		break;
	}

	return sound;
}

//=============================================================================




/*
================
M_DrawCharacter

Draws one solid graphics character
cx and cy are in 320*240 coordinates, and will be centered on
higher res screens.
================
*/
void M_DrawCharacter(int cx, int cy, int num) {
	float	fontscale = cl_fontScale->value;

	Draw_CharScaled(cx + ((viddef.width - 320) >> 1), cy + ((viddef.height - 240) >> 1), fontscale, fontscale, num);
}

void M_Print(int cx, int cy, char *str) {
	while (*str) {
		M_DrawCharacter(cx, cy, (*str) + 128);
		str++;
		cx += 8 * cl_fontScale->value;
	}
}

void M_PrintWhite(int cx, int cy, char *str) {
	while (*str) {
		M_DrawCharacter(cx, cy, *str);
		str++;
		cx += 8 * cl_fontScale->value;
	}
}


void M_DrawTextBox(int x, int y, int width, int lines) {
	int cx, cy;
	int n;

	// draw left side
	cx = x;
	cy = y;
	M_DrawCharacter(cx, cy, 1);
	for (n = 0; n < lines; n++) {
		cy += 8;
		M_DrawCharacter(cx, cy, 4);
	}
	M_DrawCharacter(cx, cy + 8, 7);

	// draw middle
	cx += 8;
	while (width > 0) {
		cy = y;
		M_DrawCharacter(cx, cy, 2);
		for (n = 0; n < lines; n++) {
			cy += 8;
			M_DrawCharacter(cx, cy, 5);
		}
		M_DrawCharacter(cx, cy + 8, 8);
		width -= 1;
		cx += 8;
	}

	// draw right side
	cy = y;
	M_DrawCharacter(cx, cy, 3);
	for (n = 0; n < lines; n++) {
		cy += 8;
		M_DrawCharacter(cx, cy, 6);
	}
	M_DrawCharacter(cx, cy + 8, 9);
}


/*
=======================================================================

MAIN MENU

=======================================================================
*/
#define	MAIN_ITEMS	5

void M_Main_DrawQuad(float x, float y) {
	extern float CalcFov(float fov_x, float w, float h);
	refdef_t refdef;
	static int yaw;
	entity_t entity;

	memset(&refdef, 0, sizeof(refdef));
	memset(&entity, 0, sizeof(entity));

	refdef.x = x;
	refdef.y = y - 7 * cl_fontScale->value;
	refdef.width = 50 * cl_fontScale->value;
	refdef.height = 50 * cl_fontScale->value;
	refdef.fov_x = 45;
	refdef.fov_y = 45;
	refdef.time = cls.realtime / 1.5;
	refdef.areabits = 0;
	refdef.num_entities = 1;
	refdef.entities = &entity;
	refdef.lightstyles = 0;
	refdef.rdflags = RDF_NOWORLDMODEL | RDF_NOCLEAR;

	entity.model = menumodel.menuQuad;
	entity.flags = RF_NOSHADOW | RF_DEPTHHACK;
	entity.origin[0] = 55;
	entity.origin[1] = -3;
	entity.origin[2] = -17;
	VectorCopy(entity.origin, entity.oldorigin);

	entity.frame = 0;
	entity.oldframe = 0;
	entity.backlerp = 0.0;
	entity.angles[1] = anglemod(cl.time / 16);
	entity.angleMod = qtrue;

	R_RenderFrame(&refdef);
	refdef.num_entities++;
}


void M_Main_Draw(void) {
	int i;
	int w, h;
	int ystart;
	int xoffset;
	int offcet = 0;
	int widest = -1;
	int totalheight = 0;

	char *names[] = {
		"m_main_game",
		"m_main_multiplayer",
		"m_main_options",
		"m_main_video",
		"m_main_quit",
		0
	};

	char *namesBump[] = {
		"m_main_game_bump",
		"m_main_multiplayer_bump",
		"m_main_options_bump",
		"m_main_video_bump",
		"m_main_quit_bump",
		0
	};

	const float fontscale = cl_fontScale->value;

	for (i = 0; names[i] != 0; i++) {
		Draw_GetPicSize(&w, &h, names[i]);

		if (w > widest)
			widest = w;
		totalheight += (h + 12);
	}

	ystart = (viddef.height / 2) - 110 * cl_fontScale->value;
	xoffset = (viddef.width - widest + (70 - (cl_fontScale->value - 1) * 170)) / 2;
	offcet = (fontscale - 1) * 60;

	for (i = 0; names[i] != 0; i++) {
		if (i != m_main_cursor) {
			Draw_ScaledPic(xoffset + offcet, ystart + (i * fontscale) * 40 + 13, fontscale, fontscale, i_main_menu[i]);
			Draw_ScaledBumpPic(xoffset + offcet, ystart + (i * fontscale) * 40 + 13, fontscale, fontscale, i_main_menu[i], i_main_menu_bump[i]);
		}
	}

	Draw_ScaledPic(xoffset + offcet, ystart + (m_main_cursor * fontscale) * 40 + 13, fontscale, fontscale, i_main_menu_sel[m_main_cursor]);
	Draw_ScaledBumpPic(xoffset + offcet, ystart + (m_main_cursor * fontscale) * 40 + 13, fontscale, fontscale, i_main_menu_sel[m_main_cursor], i_main_menu_bump_sel[m_main_cursor]);

	w = i_main_plaque[0]->width;
	Draw_ScaledPic((xoffset - 30) - w, ystart, fontscale, fontscale, i_main_plaque[0]);
	Draw_ScaledBumpPic((xoffset - 30) - w, ystart, fontscale, fontscale, i_main_plaque[0], i_main_plaque[1]);

	w = i_main_logo[0]->width;
	h = i_main_logo[0]->height;
	if (cl_fontScale->value == 3) {
		Draw_ScaledPic((xoffset - 30) - w, ystart + h + (140 * fontscale), fontscale, fontscale, i_main_logo[0]);
		Draw_ScaledBumpPic((xoffset - 30) - w, ystart + h + (140 * fontscale), fontscale, fontscale, i_main_logo[0], i_main_logo[1]);
	}
	else if (cl_fontScale->value == 2) {
		Draw_ScaledPic((xoffset - 30) - w, ystart + h + 260, fontscale, fontscale, i_main_logo[0]);
		Draw_ScaledBumpPic((xoffset - 30) - w, ystart + h + 260, fontscale, fontscale, i_main_logo[0], i_main_logo[1]);

	}
	M_Main_DrawQuad(xoffset - 30, ystart + (m_main_cursor * 40 + 5)* fontscale);
	drawIDlogo = qtrue;
}


int M_Main_Key(int key) {
	switch (key) {
	case K_ESCAPE:
		//		case K_XPAD_BACK:
	case K_XPAD_B: // ??? ???????????? "B" ???????????????????????? ????????? ??????????????????????????? ???????????????
		M_PopMenu();
		break;

	case K_KP_DOWNARROW:
	case K_DOWNARROW:
	case K_XPAD_DPAD_DOWN:
		if (++m_main_cursor >= MAIN_ITEMS)
			m_main_cursor = 0;
		return menu_move_sound;

	case K_KP_UPARROW:
	case K_UPARROW:
	case K_XPAD_DPAD_UP:
		if (--m_main_cursor < 0)
			m_main_cursor = MAIN_ITEMS - 1;
		return menu_move_sound;

	case K_KP_ENTER:
		//		case K_XPAD_START:
	case K_XPAD_A: // ??? ???????????? "A" ???????????????????????? ????????? ??????????????? ?????????????????? ????????????
	case K_ENTER:
		m_entersound = qtrue;

		switch (m_main_cursor) {
		case 0:
			M_Menu_Game_f();
			break;

		case 1:
			M_Menu_Multiplayer_f();
			break;

		case 2:
			M_Menu_Options_f();
			break;

		case 3:
			M_Menu_Video_f();
			break;

		case 4:
			M_Menu_Quit_f();
			break;
		}
	}

	return 0;
}


void M_Menu_Main_f(void) {
	M_PushMenu(M_Main_Draw, M_Main_Key);
}

/*
=======================================================================

MULTIPLAYER MENU

=======================================================================
*/
static menuframework_s s_multiplayer_menu;
static menuaction_s s_join_network_server_action;
static menuaction_s s_start_network_server_action;
static menuaction_s s_player_setup_action;

static void Multiplayer_MenuDraw(void) {
	M_Banner(i_banner_multiplayer);

	Menu_AdjustCursor(&s_multiplayer_menu, 1);
	Menu_Draw(&s_multiplayer_menu);
}

static void PlayerSetupFunc(void *unused) {
	M_Menu_PlayerConfig_f();
}

static void JoinNetworkServerFunc(void *unused) {
	M_Menu_JoinServer_f();
}

static void StartNetworkServerFunc(void *unused) {
	M_Menu_StartServer_f();
}

void Multiplayer_MenuInit(void) {
	s_multiplayer_menu.x = viddef.width * 0.50 - 64 * cl_fontScale->value;
	s_multiplayer_menu.nitems = 0;
	
	drawIDlogo = qfalse;

	s_join_network_server_action.generic.type = MTYPE_ACTION;
	s_join_network_server_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_join_network_server_action.generic.x = 0;
	s_join_network_server_action.generic.y = 0;
	s_join_network_server_action.generic.name = " join network server";
	s_join_network_server_action.generic.callback = JoinNetworkServerFunc;

	s_start_network_server_action.generic.type = MTYPE_ACTION;
	s_start_network_server_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_start_network_server_action.generic.x = 0;
	s_start_network_server_action.generic.y = 10 * cl_fontScale->value;
	s_start_network_server_action.generic.name = " start network server";
	s_start_network_server_action.generic.callback = StartNetworkServerFunc;

	s_player_setup_action.generic.type = MTYPE_ACTION;
	s_player_setup_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_player_setup_action.generic.x = 0;
	s_player_setup_action.generic.y = 20 * cl_fontScale->value;
	s_player_setup_action.generic.name = " player setup";
	s_player_setup_action.generic.callback = PlayerSetupFunc;

	Menu_AddItem(&s_multiplayer_menu,
		(void *)&s_join_network_server_action);
	Menu_AddItem(&s_multiplayer_menu,
		(void *)&s_start_network_server_action);
	Menu_AddItem(&s_multiplayer_menu, (void *)&s_player_setup_action);

	Menu_SetStatusBar(&s_multiplayer_menu, NULL);

	Menu_Center(&s_multiplayer_menu);
}

int Multiplayer_MenuKey(int key) {
	return Default_MenuKey(&s_multiplayer_menu, key);
}

void M_Menu_Multiplayer_f(void) {
	Multiplayer_MenuInit();
	M_PushMenu(Multiplayer_MenuDraw, Multiplayer_MenuKey);
}

/*
=======================================================================

KEYS MENU

=======================================================================
*/
char *bindnames[][2] = {
	{ "+attack", "attack" },
	{ "weapnext", "next weapon" },
	{ "weapprev", "prev weapon" },
	{ "+forward", "walk forward" },
	{ "+back", "backpedal" },
	{ "+left", "turn left" },
	{ "+right", "turn right" },
	{ "+speed", "run" },
	{ "+moveleft", "step left" },
	{ "+moveright", "step right" },
	{ "+moveup", "up / jump" },
	{ "+movedown", "down / crouch" },
	{ "inven", "inventory" },
	{ "invuse", "use item" },
	{ "invdrop", "drop item" },
	{ "invprev", "prev item" },
	{ "invnext", "next item" },
	{ "+zoom", "zoom" },     // ??????????????????????????? ??????????????? ????????????, ????????? ??????????????? ??? ???????????? ?????????????????????????????? ?????????????????????!
	{ "+flashlight", "flashlight" },
	{ "cmd help", "help computer" },
	{ 0, 0 }
};

int keys_cursor;
static int bind_grab;

static menuframework_s s_keys_menu;
static menuaction_s s_keys_attack_action;
static menuaction_s s_keys_change_weapon_action;
static menuaction_s s_keys_change1_weapon_action;
static menuaction_s s_keys_walk_forward_action;
static menuaction_s s_keys_backpedal_action;
static menuaction_s s_keys_turn_left_action;
static menuaction_s s_keys_turn_right_action;
static menuaction_s s_keys_run_action;
static menuaction_s s_keys_step_left_action;
static menuaction_s s_keys_step_right_action;
static menuaction_s s_keys_move_up_action;
static menuaction_s s_keys_move_down_action;
static menuaction_s s_keys_inventory_action;
static menuaction_s s_keys_inv_use_action;
static menuaction_s s_keys_inv_drop_action;
static menuaction_s s_keys_inv_prev_action;
static menuaction_s s_keys_inv_next_action;
static menuaction_s s_keys_zoom_action; //zoom
static menuaction_s s_keys_flashlight_action; //flashlight
static menuaction_s s_keys_help_computer_action;

static void M_UnbindCommand(char *command) {
	int j;
	size_t l;
	char *b;

	l = strlen(command);

	for (j = 0; j < 256; j++) {
		b = keybindings[j];
		if (!b)
			continue;
		if (!strncmp(b, command, l))
			Key_SetBinding(j, "");
	}
}

static void M_FindKeysForCommand(char *command, int *twokeys) {
	int count;
	int j;
	size_t l;
	char *b;

	twokeys[0] = twokeys[1] = -1;
	l = strlen(command);
	count = 0;

	for (j = 0; j < 256; j++) {
		b = keybindings[j];
		if (!b)
			continue;
		if (!strncmp(b, command, l)) {
			twokeys[count] = j;
			count++;
			if (count == 2)
				break;
		}
	}
}

static void KeyCursorDrawFunc(menuframework_s * menu) {
	Draw_CharScaled(menu->x,
		menu->y + menu->cursor * 9 * cl_fontScale->value,
		cl_fontScale->value, cl_fontScale->value,
		bind_grab ? '=' : 12 + ((int)(Sys_Milliseconds() / 250) & 1));
}

static void DrawKeyBindingFunc(void *self) {
	int keys[2];
	menuaction_s *a = (menuaction_s *)self;

	M_FindKeysForCommand(bindnames[a->generic.localdata[0]][0], keys);

	RE_SetColor(colorWhite);

	if (keys[0] == -1) {
		Menu_DrawStringScaled(a->generic.x + a->generic.parent->x + 16 * cl_fontScale->value,
			a->generic.y + a->generic.parent->y,
			cl_fontScale->value, cl_fontScale->value,
			"???");
	}
	else {
		size_t x;
		char *name = Key_KeynumToString(keys[0]);
		RE_SetColor(colorWhite);

		if (strstr(name, "XPAD_"))
			RE_SetColor(colorGold);
		if (strstr(name, "XPAD_A"))
			RE_SetColor(colorGreen);
		if (!strncmp(name, "XPAD_B", 7))
			RE_SetColor(colorRed);
		if (strstr(name, "XPAD_X"))
			RE_SetColor(colorBlue);
		if (strstr(name, "XPAD_Y"))
			RE_SetColor(colorYellow);


		Menu_DrawStringScaled(a->generic.x + a->generic.parent->x + 16 * cl_fontScale->value,
			a->generic.y + a->generic.parent->y,
			cl_fontScale->value, cl_fontScale->value,
			(const char *)name);

		x = strlen(name) * 8 * cl_fontScale->value;

		if (keys[1] != -1) {
			char *name2 = Key_KeynumToString(keys[1]);

			RE_SetColor(colorWhite);

			Menu_DrawStringScaled(a->generic.x + a->generic.parent->x + 24 + x,
				a->generic.y + a->generic.parent->y,
				cl_fontScale->value, cl_fontScale->value,
				"  ");

			if (strstr(name2, "XPAD_"))
				RE_SetColor(colorGold);
			if (strstr(name2, "XPAD_A"))
				RE_SetColor(colorGreen);
			if (!strncmp(name2, "XPAD_B", 7))
				RE_SetColor(colorRed);
			if (strstr(name2, "XPAD_X"))
				RE_SetColor(colorBlue);
			if (strstr(name2, "XPAD_Y"))
				RE_SetColor(colorYellow);

			Menu_DrawStringScaled(a->generic.x + a->generic.parent->x + 48 + x,
				a->generic.y + a->generic.parent->y,
				cl_fontScale->value, cl_fontScale->value,
				name2);
		}
	}
	RE_SetColor(colorWhite);
}

static void KeyBindingFunc(void *self) {
	menuaction_s *a = (menuaction_s *)self;
	int keys[2];

	M_FindKeysForCommand(bindnames[a->generic.localdata[0]][0], keys);

	if (keys[1] != -1)
		M_UnbindCommand(bindnames[a->generic.localdata[0]][0]);

	bind_grab = qtrue;

	Menu_SetStatusBar(&s_keys_menu,
		"press a key or button for this action");
}

static void Keys_MenuInit(void) {
	int y = 0;
	int i = 0;

	drawIDlogo = qfalse;

	s_keys_menu.x = viddef.width * 0.50;
	s_keys_menu.nitems = 0;
	s_keys_menu.cursordraw = KeyCursorDrawFunc;

	s_keys_attack_action.generic.type = MTYPE_ACTION;
	s_keys_attack_action.generic.flags = QMF_GRAYED;
	s_keys_attack_action.generic.x = 0;
	s_keys_attack_action.generic.y = y;
	s_keys_attack_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_attack_action.generic.localdata[0] = i;
	s_keys_attack_action.generic.name =
		bindnames[s_keys_attack_action.generic.localdata[0]][1];

	s_keys_change_weapon_action.generic.type = MTYPE_ACTION;
	s_keys_change_weapon_action.generic.flags = QMF_GRAYED;
	s_keys_change_weapon_action.generic.x = 0;
	s_keys_change_weapon_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_change_weapon_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_change_weapon_action.generic.localdata[0] = ++i;
	s_keys_change_weapon_action.generic.name =
		bindnames[s_keys_change_weapon_action.generic.localdata[0]][1];

	s_keys_change1_weapon_action.generic.type = MTYPE_ACTION;
	s_keys_change1_weapon_action.generic.flags = QMF_GRAYED;
	s_keys_change1_weapon_action.generic.x = 0;
	s_keys_change1_weapon_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_change1_weapon_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_change1_weapon_action.generic.localdata[0] = ++i;
	s_keys_change1_weapon_action.generic.name =
		bindnames[s_keys_change1_weapon_action.generic.localdata[0]][1];


	s_keys_walk_forward_action.generic.type = MTYPE_ACTION;
	s_keys_walk_forward_action.generic.flags = QMF_GRAYED;
	s_keys_walk_forward_action.generic.x = 0;
	s_keys_walk_forward_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_walk_forward_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_walk_forward_action.generic.localdata[0] = ++i;
	s_keys_walk_forward_action.generic.name =
		bindnames[s_keys_walk_forward_action.generic.localdata[0]][1];

	s_keys_backpedal_action.generic.type = MTYPE_ACTION;
	s_keys_backpedal_action.generic.flags = QMF_GRAYED;
	s_keys_backpedal_action.generic.x = 0;
	s_keys_backpedal_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_backpedal_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_backpedal_action.generic.localdata[0] = ++i;
	s_keys_backpedal_action.generic.name =
		bindnames[s_keys_backpedal_action.generic.localdata[0]][1];

	s_keys_turn_left_action.generic.type = MTYPE_ACTION;
	s_keys_turn_left_action.generic.flags = QMF_GRAYED;
	s_keys_turn_left_action.generic.x = 0;
	s_keys_turn_left_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_turn_left_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_turn_left_action.generic.localdata[0] = ++i;
	s_keys_turn_left_action.generic.name =
		bindnames[s_keys_turn_left_action.generic.localdata[0]][1];

	s_keys_turn_right_action.generic.type = MTYPE_ACTION;
	s_keys_turn_right_action.generic.flags = QMF_GRAYED;
	s_keys_turn_right_action.generic.x = 0;
	s_keys_turn_right_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_turn_right_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_turn_right_action.generic.localdata[0] = ++i;
	s_keys_turn_right_action.generic.name =
		bindnames[s_keys_turn_right_action.generic.localdata[0]][1];

	s_keys_run_action.generic.type = MTYPE_ACTION;
	s_keys_run_action.generic.flags = QMF_GRAYED;
	s_keys_run_action.generic.x = 0;
	s_keys_run_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_run_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_run_action.generic.localdata[0] = ++i;
	s_keys_run_action.generic.name =
		bindnames[s_keys_run_action.generic.localdata[0]][1];

	s_keys_step_left_action.generic.type = MTYPE_ACTION;
	s_keys_step_left_action.generic.flags = QMF_GRAYED;
	s_keys_step_left_action.generic.x = 0;
	s_keys_step_left_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_step_left_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_step_left_action.generic.localdata[0] = ++i;
	s_keys_step_left_action.generic.name =
		bindnames[s_keys_step_left_action.generic.localdata[0]][1];

	s_keys_step_right_action.generic.type = MTYPE_ACTION;
	s_keys_step_right_action.generic.flags = QMF_GRAYED;
	s_keys_step_right_action.generic.x = 0;
	s_keys_step_right_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_step_right_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_step_right_action.generic.localdata[0] = ++i;
	s_keys_step_right_action.generic.name =
		bindnames[s_keys_step_right_action.generic.localdata[0]][1];

	s_keys_move_up_action.generic.type = MTYPE_ACTION;
	s_keys_move_up_action.generic.flags = QMF_GRAYED;
	s_keys_move_up_action.generic.x = 0;
	s_keys_move_up_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_move_up_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_move_up_action.generic.localdata[0] = ++i;
	s_keys_move_up_action.generic.name =
		bindnames[s_keys_move_up_action.generic.localdata[0]][1];

	s_keys_move_down_action.generic.type = MTYPE_ACTION;
	s_keys_move_down_action.generic.flags = QMF_GRAYED;
	s_keys_move_down_action.generic.x = 0;
	s_keys_move_down_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_move_down_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_move_down_action.generic.localdata[0] = ++i;
	s_keys_move_down_action.generic.name =
		bindnames[s_keys_move_down_action.generic.localdata[0]][1];

	s_keys_inventory_action.generic.type = MTYPE_ACTION;
	s_keys_inventory_action.generic.flags = QMF_GRAYED;
	s_keys_inventory_action.generic.x = 0;
	s_keys_inventory_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_inventory_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_inventory_action.generic.localdata[0] = ++i;
	s_keys_inventory_action.generic.name =
		bindnames[s_keys_inventory_action.generic.localdata[0]][1];

	s_keys_inv_use_action.generic.type = MTYPE_ACTION;
	s_keys_inv_use_action.generic.flags = QMF_GRAYED;
	s_keys_inv_use_action.generic.x = 0;
	s_keys_inv_use_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_inv_use_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_inv_use_action.generic.localdata[0] = ++i;
	s_keys_inv_use_action.generic.name =
		bindnames[s_keys_inv_use_action.generic.localdata[0]][1];

	s_keys_inv_drop_action.generic.type = MTYPE_ACTION;
	s_keys_inv_drop_action.generic.flags = QMF_GRAYED;
	s_keys_inv_drop_action.generic.x = 0;
	s_keys_inv_drop_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_inv_drop_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_inv_drop_action.generic.localdata[0] = ++i;
	s_keys_inv_drop_action.generic.name =
		bindnames[s_keys_inv_drop_action.generic.localdata[0]][1];

	s_keys_inv_prev_action.generic.type = MTYPE_ACTION;
	s_keys_inv_prev_action.generic.flags = QMF_GRAYED;
	s_keys_inv_prev_action.generic.x = 0;
	s_keys_inv_prev_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_inv_prev_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_inv_prev_action.generic.localdata[0] = ++i;
	s_keys_inv_prev_action.generic.name =
		bindnames[s_keys_inv_prev_action.generic.localdata[0]][1];

	s_keys_inv_next_action.generic.type = MTYPE_ACTION;
	s_keys_inv_next_action.generic.flags = QMF_GRAYED;
	s_keys_inv_next_action.generic.x = 0;
	s_keys_inv_next_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_inv_next_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_inv_next_action.generic.localdata[0] = ++i;
	s_keys_inv_next_action.generic.name =
		bindnames[s_keys_inv_next_action.generic.localdata[0]][1];

	s_keys_zoom_action.generic.type = MTYPE_ACTION;
	s_keys_zoom_action.generic.flags = QMF_GRAYED;
	s_keys_zoom_action.generic.x = 0;
	s_keys_zoom_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_zoom_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_zoom_action.generic.localdata[0] = ++i;
	s_keys_zoom_action.generic.name = bindnames[i][1];

	s_keys_flashlight_action.generic.type = MTYPE_ACTION;
	s_keys_flashlight_action.generic.flags = QMF_GRAYED;
	s_keys_flashlight_action.generic.x = 0;
	s_keys_flashlight_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_flashlight_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_flashlight_action.generic.localdata[0] = ++i;
	s_keys_flashlight_action.generic.name = bindnames[i][1];

	s_keys_help_computer_action.generic.type = MTYPE_ACTION;
	s_keys_help_computer_action.generic.flags = QMF_GRAYED;
	s_keys_help_computer_action.generic.x = 0;
	s_keys_help_computer_action.generic.y = y += 9 * cl_fontScale->value;
	s_keys_help_computer_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_help_computer_action.generic.localdata[0] = ++i;
	s_keys_help_computer_action.generic.name =
		bindnames[s_keys_help_computer_action.generic.localdata[0]][1];

	Menu_AddItem(&s_keys_menu, (void *)&s_keys_attack_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_change_weapon_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_change1_weapon_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_walk_forward_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_backpedal_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_turn_left_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_turn_right_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_run_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_step_left_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_step_right_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_move_up_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_move_down_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_inventory_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_inv_use_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_inv_drop_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_inv_prev_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_inv_next_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_zoom_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_flashlight_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_help_computer_action);

	Menu_SetStatusBar(&s_keys_menu, "enter to change, backspace to clear");
	Menu_Center(&s_keys_menu);
}

static void Keys_MenuDraw(void) {

	M_Option_Banner(i_banner_options);
	Menu_AdjustCursor(&s_keys_menu, 1);
	Menu_Draw(&s_keys_menu);
}

int Keys_MenuKey(int key) {
	menuaction_s *item = (menuaction_s *)Menu_ItemAtCursor(&s_keys_menu);

	if (bind_grab) {
		if (key != K_ESCAPE && key != '`' && key != K_XPAD_START) {
			char cmd[1024];

			Com_sprintf(cmd, sizeof(cmd), "bind \"%s\" \"%s\"\n",
				Key_KeynumToString(key),
				bindnames[item->generic.localdata[0]][0]);
			Cbuf_InsertText(cmd);
		}

		Menu_SetStatusBar(&s_keys_menu,
			"enter to change, backspace to clear");
		bind_grab = qfalse;
		return menu_out_sound;
	}

	switch (key) {
	case K_KP_ENTER:
	case K_ENTER:
	case K_XPAD_A:
		KeyBindingFunc(item);
		return menu_in_sound;
	case K_BACKSPACE:			// delete bindings
	case K_DEL:				// delete bindings
	case K_KP_DEL:
		M_UnbindCommand(bindnames[item->generic.localdata[0]][0]);
		return menu_out_sound;
	default:
		return Default_MenuKey(&s_keys_menu, key);
	}
}

void M_Menu_Keys_f(void) {

	Keys_MenuInit();
	M_PushMenu(Keys_MenuDraw, Keys_MenuKey);
}

void M_Menu_Advanced_f(void) {
	M_Option_Banner(i_banner_options);
	M_AdvancedInit();

}

/*
=======================================================================

CONTROLS MENU

=======================================================================
*/
static cvar_t *win_noalttab;


static menuframework_s s_options_menu;
static menuaction_s s_options_defaults_action;
static menuaction_s s_options_customize_options_action;
static menuaction_s s_options_advanced_options_action;
static menuslider_s s_options_sensitivity_slider;
static menulist_s s_options_noalttab_box;
static menulist_s s_options_alwaysrun_box;
static menulist_s s_options_invertmouse_box;
static menulist_s s_options_crosshair_box;
static menulist_s s_options_gamepad_box;
static menuslider_s s_options_effectsVolume_slider;
static menuslider_s s_options_musicvolume_slider;
static menulist_s s_options_musicsrc_list;
static menulist_s s_options_useEFX_list;
static menulist_s s_options_unlimited_ambient_list;
static menulist_s s_options_aldev_box;
static menulist_s s_options_alResempler_box;
static menulist_s s_options_alquality_list;
static menulist_s s_options_hrtf;
static menulist_s s_options_console_action;
static menulist_s s_options_cpuUtil_box;
static menulist_s s_options_fps_box;
static menulist_s s_options_time_box;

extern cvar_t	*cl_drawhud;
extern cvar_t	*cl_3dhud;
extern cvar_t	*m_inversion;
#ifdef _WIN32
extern cvar_t	*in_useXInput;
#endif
static void CrosshairFunc(void *unused) {
	Cvar_SetValue("crosshair", s_options_crosshair_box.curvalue);
}



static void AdvancedSettingsFunc(void *unused) {
	M_Menu_Advanced_f();

}

static void CustomizeControlsFunc(void *unused) {
	M_Menu_Keys_f();
}



static void AlwaysRunFunc(void *unused) {
	Cvar_SetValue("cl_run", s_options_alwaysrun_box.curvalue);
}

static void MouseSpeedFunc(void *unused) {
	Cvar_SetValue("sensitivity",
		s_options_sensitivity_slider.curvalue / 2.0F);
}

static void NoAltTabFunc(void *unused) {
	Cvar_SetValue("win_noalttab", s_options_noalttab_box.curvalue);
}
#ifdef _WIN32
static void GamePadFunc(void* unused) {
	Cvar_SetValue("in_useXInput", s_options_gamepad_box.curvalue);
}
#endif
static void FpsFunc(void *unused) {
	Cvar_SetValue("cl_drawFPS", s_options_fps_box.curvalue);
}

static void CpuUtilFunc(void* unused) {
	Cvar_SetValue("sys_cpuUtilization", s_options_cpuUtil_box.curvalue);
}

static void TimeFunc(void *unused) {
	Cvar_SetValue("cl_drawTime", s_options_time_box.curvalue);
}

static void ControlsSetMenuItemValues(void) {

	s_options_effectsVolume_slider.curvalue = Cvar_VariableValue("s_effectsVolume") * 10;
	s_options_musicvolume_slider.curvalue = Cvar_VariableValue("s_musicVolume") * 10;
	s_options_musicsrc_list.curvalue = Cvar_VariableValue("s_musicSrc");
	s_options_useEFX_list.curvalue = Cvar_VariableValue("s_useEfx");
	s_options_hrtf.curvalue = Cvar_VariableValue("s_useHRTF");
	

	s_options_sensitivity_slider.curvalue = (sensitivity->value) * 2;

	Cvar_SetValue("cl_run", ClampCvar(0, 1, cl_run->value));
	s_options_alwaysrun_box.curvalue = cl_run->value;

#ifdef _WIN32
	s_options_invertmouse_box.curvalue = m_inversion->value > 0;
#else
	s_options_invertmouse_box.curvalue = m_pitch->value < 0;
#endif

	Cvar_SetValue("crosshair", ClampCvarInteger(0, 13, crosshair->integer));
	s_options_crosshair_box.curvalue = crosshair->value;
#ifdef _WIN32
	Cvar_SetValue("in_useXInput", ClampCvarInteger(0, 1, in_useXInput->integer));
	s_options_gamepad_box.curvalue = in_useXInput->integer;
#else
	s_options_gamepad_box.curvalue = 0;
#endif

	s_options_noalttab_box.curvalue = win_noalttab->value;

	Cvar_SetValue("sys_cpuUtilization", ClampCvarInteger(0, 1, sys_cpuUtilization->integer));
	s_options_cpuUtil_box.curvalue = sys_cpuUtilization->value;

	Cvar_SetValue("cl_drawFPS", ClampCvarInteger(0, 2, cl_drawFPS->integer));
	s_options_fps_box.curvalue = cl_drawFPS->value;

	Cvar_SetValue("cl_drawTime", ClampCvarInteger(0, 1, cl_drawTime->integer));
	s_options_time_box.curvalue = cl_drawTime->value;

}

static void ControlsResetDefaultsFunc(void *unused) {
	Cbuf_AddText("exec default.cfg\n");
	Cbuf_Execute();

	ControlsSetMenuItemValues();
}

static void InvertMouseFunc(void *unused) {

#ifdef _WIN32
	Cvar_SetValue("m_inversion", !(m_inversion->value));
#else
	Cvar_SetValue("m_pitch", -m_pitch->value);
#endif
}

static void UpdateVolumeFunc(void *unused) {
	
	float vol = s_options_effectsVolume_slider.curvalue / 10.0;
	Cvar_SetValue("s_effectsVolume", vol);
}

static void UpdateMusicVolumeFunc(void *unused) {
	Cvar_SetValue("s_musicVolume", s_options_musicvolume_slider.curvalue / 10);
}

char *al_device[] = {
	"default",
	0,
	0,
	0,
	0,
	0,
	0
};
char* playback1251[] = {
0,
0,
0,
0,
0,
0,
0
};
char* al_resemplers[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

static void AlDevice(void *unused) {
	if (s_options_aldev_box.curvalue > 0)
		Cvar_Set("s_device", al_device[s_options_aldev_box.curvalue]);
	else
		Cvar_Set("s_device", "");

	CL_Snd_Restart_f();
}

static void AlResempler(void* unused) {
		Cvar_SetValue("s_resamplerQuality", s_options_alResempler_box.curvalue);
		CL_Snd_Restart_f();
}

static void UpdateMusicSrcFunc(void *unused) {
	Cvar_SetValue("s_musicSrc", s_options_musicsrc_list.curvalue);
}

static void UpdateEFX(void *unused) {
	Cvar_SetValue("s_useEfx", s_options_useEFX_list.curvalue);
	CL_Snd_Restart_f();
}


static void UpdateHRTF(void *unused) {
	Cvar_SetValue("s_useHRTF", s_options_hrtf.curvalue);
	CL_Snd_Restart_f();
}

static void ConsoleFunc(void *unused) {
	/*
	** the proper way to do this is probably to have ToggleConsole_f accept a parameter
	*/
	extern void Key_ClearTyping(void);

	if (cl.attractloop) {
		Cbuf_AddText("killserver\n");
		return;
	}

	Key_ClearTyping();
	Con_ClearNotify();

	M_ForceMenuOff();
	cls.key_dest = key_console;
}

//Q2xp stuff

static menuslider_s s_aoptions_railCoreRed_slider;
static menuslider_s s_aoptions_railCoreGreen_slider;
static menuslider_s s_aoptions_railCoreBlue_slider;
static menuslider_s s_aoptions_railSpiralRed_slider;
static menuslider_s s_aoptions_railSpiralGreen_slider;
static menuslider_s s_aoptions_railSpiralBlue_slider;

static menuslider_s s_aoptions_hudScale_slider;
static menuslider_s s_aoptions_fontScale_slider;

static menulist_s s_aoptions_blood_box;
static menulist_s s_aoptions_decals_box;
static menulist_s s_aoptions_3dcam_box;

static menulist_s s_aoptions_drawHud_box;
static menulist_s s_aoptions_3dhud_box;

static void UpdateRailCoreRedFunc(void *unused) {
	Cvar_SetValue("cl_railcore_red", s_aoptions_railCoreRed_slider.curvalue / 10);
}

static void UpdateRailCoreGreenFunc(void *unused) {
	Cvar_SetValue("cl_railcore_green", s_aoptions_railCoreGreen_slider.curvalue / 10);
}

static void UpdateRailCoreBlueFunc(void *unused) {
	Cvar_SetValue("cl_railcore_blue", s_aoptions_railCoreBlue_slider.curvalue / 10);
}

static void UpdateRailSpiralRedFunc(void *unused) {
	Cvar_SetValue("cl_railspiral_red", s_aoptions_railSpiralRed_slider.curvalue / 10);
}

static void UpdateRailSpiralGreenFunc(void *unused) {
	Cvar_SetValue("cl_railspiral_green", s_aoptions_railSpiralGreen_slider.curvalue / 10);
}

static void UpdateRailSpiralBlueFunc(void *unused) {
	Cvar_SetValue("cl_railspiral_blue", s_aoptions_railSpiralBlue_slider.curvalue / 10);
}

static void UpdateHudScaleFunc(void *unused) {
	Cvar_SetValue("cl_hudScale", s_aoptions_hudScale_slider.curvalue / 10);
}

static void UpdateFontScaleFunc(void *unused) {
	Cvar_SetValue("cl_fontScale", s_aoptions_fontScale_slider.curvalue / 10 + 1);
}

static void UpdateBloodFunc(void *unused) {
	Cvar_SetValue("cl_blood", s_aoptions_blood_box.curvalue);
}

static void UpdateDecalsFunc(void *unused) {
	Cvar_SetValue("cl_decals", s_aoptions_decals_box.curvalue);
}

static void Update3dCamFunc(void *unused) {
	Cvar_SetValue("cl_thirdPerson", s_aoptions_3dcam_box.curvalue);
}

static void UpdateHud(void *unused) {
	Cvar_SetValue("cl_drawhud", s_aoptions_drawHud_box.curvalue);
}

static void Update3dHud(void *unused) {
	Cvar_SetValue("cl_3dhud", s_aoptions_3dhud_box.curvalue);
}



void M_AdvancedInit(void) {
	static char *yesno_names[] = {
		"no",
		"yes",
		0
	};

	unsigned  menu_y = 0;

	drawIDlogo = qfalse;

	/*
	** configure controls menu and menu items
	*/
	s_options_menu.x = viddef.width >> 1;
	s_options_menu.y = (viddef.height >> 1) - 100 * cl_fontScale->value;
	s_options_menu.nitems = 0;

	s_aoptions_railCoreRed_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_railCoreRed_slider.generic.x = 0;
	s_aoptions_railCoreRed_slider.generic.y = menu_y;
	s_aoptions_railCoreRed_slider.generic.name = "Railgun Core Red";
	s_aoptions_railCoreRed_slider.generic.callback = UpdateRailCoreRedFunc;
	s_aoptions_railCoreRed_slider.minvalue = 0;
	s_aoptions_railCoreRed_slider.maxvalue = 10;
	s_aoptions_railCoreRed_slider.curvalue = Cvar_VariableValue("cl_railcore_red") * 10;
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_railCoreGreen_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_railCoreGreen_slider.generic.x = 0;
	s_aoptions_railCoreGreen_slider.generic.y = menu_y;
	s_aoptions_railCoreGreen_slider.generic.name = "Railgun Core Green";
	s_aoptions_railCoreGreen_slider.generic.callback = UpdateRailCoreGreenFunc;
	s_aoptions_railCoreGreen_slider.minvalue = 0;
	s_aoptions_railCoreGreen_slider.maxvalue = 10;
	s_aoptions_railCoreGreen_slider.curvalue = Cvar_VariableValue("cl_railcore_green") * 10;
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_railCoreBlue_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_railCoreBlue_slider.generic.x = 0;
	s_aoptions_railCoreBlue_slider.generic.y = menu_y;
	s_aoptions_railCoreBlue_slider.generic.name = "Railgun Core Blue";
	s_aoptions_railCoreBlue_slider.generic.callback = UpdateRailCoreBlueFunc;
	s_aoptions_railCoreBlue_slider.minvalue = 0;
	s_aoptions_railCoreBlue_slider.maxvalue = 10;
	s_aoptions_railCoreBlue_slider.curvalue = Cvar_VariableValue("cl_railcore_blue") * 10;
	menu_y += 20 * cl_fontScale->value;

	s_aoptions_railSpiralRed_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_railSpiralRed_slider.generic.x = 0;
	s_aoptions_railSpiralRed_slider.generic.y = menu_y;
	s_aoptions_railSpiralRed_slider.generic.name = "Railgun Spiral Red";
	s_aoptions_railSpiralRed_slider.generic.callback = UpdateRailSpiralRedFunc;
	s_aoptions_railSpiralRed_slider.minvalue = 0;
	s_aoptions_railSpiralRed_slider.maxvalue = 10;
	s_aoptions_railSpiralRed_slider.curvalue = Cvar_VariableValue("cl_railspiral_red") * 10;
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_railSpiralGreen_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_railSpiralGreen_slider.generic.x = 0;
	s_aoptions_railSpiralGreen_slider.generic.y = menu_y;
	s_aoptions_railSpiralGreen_slider.generic.name = "Railgun Spiral Green";
	s_aoptions_railSpiralGreen_slider.generic.callback = UpdateRailSpiralGreenFunc;
	s_aoptions_railSpiralGreen_slider.minvalue = 0;
	s_aoptions_railSpiralGreen_slider.maxvalue = 10;
	s_aoptions_railSpiralGreen_slider.curvalue = Cvar_VariableValue("cl_railspiral_green") * 10;
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_railSpiralBlue_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_railSpiralBlue_slider.generic.x = 0;
	s_aoptions_railSpiralBlue_slider.generic.y = menu_y;
	s_aoptions_railSpiralBlue_slider.generic.name = "Railgun Spiral Blue";
	s_aoptions_railSpiralBlue_slider.generic.callback = UpdateRailSpiralBlueFunc;
	s_aoptions_railSpiralBlue_slider.minvalue = 0;
	s_aoptions_railSpiralBlue_slider.maxvalue = 10;
	s_aoptions_railSpiralBlue_slider.curvalue = Cvar_VariableValue("cl_railspiral_blue") * 10;
	menu_y += 20 * cl_fontScale->value;

	s_aoptions_blood_box.generic.type = MTYPE_SPINCONTROL;
	s_aoptions_blood_box.generic.x = 0;
	s_aoptions_blood_box.generic.y = menu_y;
	s_aoptions_blood_box.generic.name = "Enable Blood and Gore";
	s_aoptions_blood_box.generic.callback = UpdateBloodFunc;
	s_aoptions_blood_box.itemnames = yesno_names;
	s_aoptions_blood_box.curvalue = Cvar_VariableValue("cl_blood");
	menu_y += 20 * cl_fontScale->value;

	s_aoptions_decals_box.generic.type = MTYPE_SPINCONTROL;
	s_aoptions_decals_box.generic.x = 0;
	s_aoptions_decals_box.generic.y = menu_y;
	s_aoptions_decals_box.generic.name = "Enable Decals";
	s_aoptions_decals_box.generic.callback = UpdateDecalsFunc;
	s_aoptions_decals_box.itemnames = yesno_names;
	s_aoptions_decals_box.curvalue = Cvar_VariableValue("cl_decals");
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_3dcam_box.generic.type = MTYPE_SPINCONTROL;
	s_aoptions_3dcam_box.generic.x = 0;
	s_aoptions_3dcam_box.generic.y = menu_y;
	s_aoptions_3dcam_box.generic.name = "Third Person view";
	s_aoptions_3dcam_box.generic.callback = Update3dCamFunc;
	s_aoptions_3dcam_box.itemnames = yesno_names;
	s_aoptions_3dcam_box.curvalue = Cvar_VariableValue("cl_thirdPepson");
	menu_y += 20 * cl_fontScale->value;

	s_aoptions_drawHud_box.generic.type = MTYPE_SPINCONTROL;
	s_aoptions_drawHud_box.generic.x = 0;
	s_aoptions_drawHud_box.generic.y = menu_y;
	s_aoptions_drawHud_box.generic.name = "Draw Hud";
	s_aoptions_drawHud_box.generic.callback = UpdateHud;
	s_aoptions_drawHud_box.itemnames = yesno_names;
	s_aoptions_drawHud_box.curvalue = Cvar_VariableValue("cl_drawhud");
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_hudScale_slider.generic.type = MTYPE_SLIDER;
	s_aoptions_hudScale_slider.generic.x = 0;
	s_aoptions_hudScale_slider.generic.y = menu_y;
	s_aoptions_hudScale_slider.generic.name = "Hud Scale";
	s_aoptions_hudScale_slider.generic.callback = UpdateHudScaleFunc;
	s_aoptions_hudScale_slider.minvalue = 0;
	s_aoptions_hudScale_slider.maxvalue = 10;
	s_aoptions_hudScale_slider.curvalue = Cvar_VariableValue("cl_hudScale") * 10;
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_3dhud_box.generic.type = MTYPE_SPINCONTROL;
	s_aoptions_3dhud_box.generic.x = 0;
	s_aoptions_3dhud_box.generic.y = menu_y;
	s_aoptions_3dhud_box.generic.name = "Draw hud models";
	s_aoptions_3dhud_box.generic.callback = Update3dHud;
	s_aoptions_3dhud_box.itemnames = yesno_names;
	s_aoptions_3dhud_box.curvalue = Cvar_VariableValue("cl_3dhud");
	menu_y += 10 * cl_fontScale->value;

	s_aoptions_railCoreRed_slider.curvalue = cl_railcore_red->value * 10;
	s_aoptions_railCoreGreen_slider.curvalue = cl_railcore_green->value * 10;
	s_aoptions_railCoreBlue_slider.curvalue = cl_railcore_blue->value * 10;

	s_aoptions_railSpiralRed_slider.curvalue = cl_railspiral_red->value * 10;
	s_aoptions_railSpiralGreen_slider.curvalue = cl_railspiral_green->value * 10;
	s_aoptions_railSpiralBlue_slider.curvalue = cl_railspiral_blue->value * 10;

	s_aoptions_blood_box.curvalue = cl_blood->value;
	s_aoptions_decals_box.curvalue = cl_decals->value;
	s_aoptions_3dcam_box.curvalue = cl_thirdPerson->value;

	s_aoptions_drawHud_box.curvalue = cl_drawhud->value;
	s_aoptions_hudScale_slider.curvalue = cl_hudScale->value * 10;


	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_railCoreRed_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_railCoreGreen_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_railCoreBlue_slider);

	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_railSpiralRed_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_railSpiralGreen_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_railSpiralBlue_slider);

	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_blood_box);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_decals_box);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_3dcam_box);

	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_drawHud_box);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_hudScale_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_aoptions_3dhud_box);

}





void Options_MenuInit(void) {
	static char *s_musicsrc_items[] = {
		"off",
		"cd-audio",
		"ogg-audio",
		0
	};

	static char *yesno_names[] = {
		"no",
		"yes",
		0
	};

	static char *fps_names[] = {
		"off",
		"average",
		"full",
		0
	};

	static char *crosshair_names[] = {
		"none",
		"dot",
		"dot in circle",
		"cross",
		"cross1",
		"q3 style",
		"cross2",
		"cross3",
		"cross4",
		"cross5",
		"celtic cross",
		"wp cross",
		"sun cross",
		"wolfsangel cross",
		0

	};

	static char* not_found[] = {
	"unsupported",
	"unsupported",
	0
	};

	unsigned i;
	extern alConfig_t alConfig;


	win_noalttab = Cvar_Get("win_noalttab", "0", CVAR_ARCHIVE);
	
	drawIDlogo = qfalse;

	/*
	** configure controls menu and menu items
	*/
	s_options_menu.x = viddef.width >> 1;
	s_options_menu.y = (viddef.height >> 1) - 110 * cl_fontScale->value;
	s_options_menu.nitems = 0;

	s_options_effectsVolume_slider.generic.type = MTYPE_SLIDER;
	s_options_effectsVolume_slider.generic.x = 0;
	s_options_effectsVolume_slider.generic.y = 10 * cl_fontScale->value;
	s_options_effectsVolume_slider.generic.name = "Effects Volume";
	s_options_effectsVolume_slider.generic.callback = UpdateVolumeFunc;
	s_options_effectsVolume_slider.minvalue = 0;
	s_options_effectsVolume_slider.maxvalue = 10;
	s_options_effectsVolume_slider.curvalue = s_effectsVolume->value * 10.0;


	s_options_musicvolume_slider.generic.type = MTYPE_SLIDER;
	s_options_musicvolume_slider.generic.x = 0;
	s_options_musicvolume_slider.generic.y = 20 * cl_fontScale->value;
	s_options_musicvolume_slider.generic.name = "Music Volume";
	s_options_musicvolume_slider.generic.callback = UpdateMusicVolumeFunc;
	s_options_musicvolume_slider.minvalue = 0;
	s_options_musicvolume_slider.maxvalue = 10;
	s_options_musicvolume_slider.curvalue = Cvar_VariableValue("s_musicVolume") * 10;


	s_options_musicsrc_list.generic.type = MTYPE_SPINCONTROL;
	s_options_musicsrc_list.generic.x = 0;
	s_options_musicsrc_list.generic.y = 30 * cl_fontScale->value;
	s_options_musicsrc_list.generic.name = "Music Source";
	s_options_musicsrc_list.generic.callback = UpdateMusicSrcFunc;
	s_options_musicsrc_list.itemnames = s_musicsrc_items;
	s_options_musicsrc_list.curvalue = Cvar_VariableValue("s_musicSrc");


	s_options_aldev_box.generic.type = MTYPE_SPINCONTROL;
	s_options_aldev_box.generic.x = 0;
	s_options_aldev_box.generic.y = 50 * cl_fontScale->value;
	s_options_aldev_box.generic.name = "Sound Device";
	s_options_aldev_box.generic.callback = AlDevice;

#ifdef _WIN32
	extern LPALGETSTRINGISOFT alGetStringiSOFT;
#endif

	if (alGetStringiSOFT) {
		
		if (ru_loc) {
		#define MAX_LEN 512
			for (i = 0; i <= alConfig.device_count; i++) {
				playback1251[i] = malloc(sizeof(char) * MAX_LEN);
				convert_utf8_to_windows1251((const char*)al_device[i], playback1251[i], MAX_LEN);
			}
			s_options_aldev_box.itemnames = playback1251;
		}
		else
			s_options_aldev_box.itemnames = al_device;
	}
	else 
		s_options_aldev_box.itemnames = al_device;

	s_options_aldev_box.curvalue = 0;
	for (i = 1; i <= alConfig.device_count; i++)
		if (!Q_strcasecmp(s_device->string, al_device[i])) {
			s_options_aldev_box.curvalue = i;
			break;
		}
	
	s_options_alResempler_box.generic.type = MTYPE_SPINCONTROL;
	s_options_alResempler_box.generic.x = 0;
	s_options_alResempler_box.generic.y = 60 * cl_fontScale->value;
	s_options_alResempler_box.generic.name = "Sound Resampler";
	s_options_alResempler_box.generic.callback = AlResempler;

	if (alGetStringiSOFT)
		s_options_alResempler_box.itemnames = al_resemplers;
	else
		s_options_alResempler_box.itemnames = not_found;

	s_options_alResempler_box.generic.statusbar = "Sources Resampling Algorithms";

	s_options_alResempler_box.curvalue = 0;
	for (i = 1; i <= alConfig.numResamplers; i++)
		if (s_resamplerQuality->integer == i) {
			s_options_alResempler_box.curvalue = i;
			break;
		}

	s_options_hrtf.generic.type = MTYPE_SPINCONTROL;
	s_options_hrtf.generic.x = 0;
	s_options_hrtf.generic.y = 70 * cl_fontScale->value;
	s_options_hrtf.generic.name = "Use HRTF";
	s_options_hrtf.generic.callback = UpdateHRTF;
	if (alGetStringiSOFT)
		s_options_hrtf.itemnames = yesno_names;
	else
		s_options_hrtf.itemnames = not_found;

	s_options_hrtf.curvalue = Cvar_VariableValue("s_useHRTF");
	s_options_hrtf.generic.statusbar = "Enable HRTF function";

	s_options_useEFX_list.generic.type = MTYPE_SPINCONTROL;
	s_options_useEFX_list.generic.x = 0;
	s_options_useEFX_list.generic.y = 80 * cl_fontScale->value;
	s_options_useEFX_list.generic.name = "Use EFX Reverbation";
	s_options_useEFX_list.generic.callback = UpdateEFX;
	s_options_useEFX_list.itemnames = yesno_names;
	s_options_useEFX_list.curvalue = Cvar_VariableValue("s_openal_efx");
	s_options_useEFX_list.generic.statusbar = "Enable room and underwater reverberation effects";

	s_options_sensitivity_slider.generic.type = MTYPE_SLIDER;
	s_options_sensitivity_slider.generic.x = 0;
	s_options_sensitivity_slider.generic.y = 100 * cl_fontScale->value;
	s_options_sensitivity_slider.generic.name = "Mouse Speed";
	s_options_sensitivity_slider.generic.callback = MouseSpeedFunc;
	s_options_sensitivity_slider.minvalue = 2;
	s_options_sensitivity_slider.maxvalue = 22;


	s_options_alwaysrun_box.generic.type = MTYPE_SPINCONTROL;
	s_options_alwaysrun_box.generic.x = 0;
	s_options_alwaysrun_box.generic.y = 110 * cl_fontScale->value;
	s_options_alwaysrun_box.generic.name = "Always Run";
	s_options_alwaysrun_box.generic.callback = AlwaysRunFunc;
	s_options_alwaysrun_box.itemnames = yesno_names;


	s_options_invertmouse_box.generic.type = MTYPE_SPINCONTROL;
	s_options_invertmouse_box.generic.x = 0;
	s_options_invertmouse_box.generic.y = 120 * cl_fontScale->value;
	s_options_invertmouse_box.generic.name = "Invert Mouse";
	s_options_invertmouse_box.generic.callback = InvertMouseFunc;
	s_options_invertmouse_box.itemnames = yesno_names;

	s_options_crosshair_box.generic.type = MTYPE_SPINCONTROL;
	s_options_crosshair_box.generic.x = 0;
	s_options_crosshair_box.generic.y = 130 * cl_fontScale->value;
	s_options_crosshair_box.generic.name = "Crosshair";
	s_options_crosshair_box.generic.callback = CrosshairFunc;
	s_options_crosshair_box.itemnames = crosshair_names;

	/*
	s_options_noalttab_box.generic.type = MTYPE_SPINCONTROL;
	s_options_noalttab_box.generic.x	= 0;
	s_options_noalttab_box.generic.y	= menu_y;
	s_options_noalttab_box.generic.name	= "disable alt-tab";
	s_options_noalttab_box.generic.callback = NoAltTabFunc;
	s_options_noalttab_box.itemnames = yesno_names;
	menu_y+=10;
	*/

	s_options_gamepad_box.generic.type = MTYPE_SPINCONTROL;
	s_options_gamepad_box.generic.x = 0;
	s_options_gamepad_box.generic.y = 140 * cl_fontScale->value;;
	s_options_gamepad_box.generic.name = "Gamepad";
#ifdef _WIN32
	s_options_gamepad_box.generic.callback = GamePadFunc;
	s_options_gamepad_box.itemnames = yesno_names;
#else
	s_options_gamepad_box.generic.callback = NULL;
	s_options_gamepad_box.itemnames = not_found;
#endif
	s_options_gamepad_box.generic.statusbar = "Enable xBox Controller";

	s_options_cpuUtil_box.generic.type = MTYPE_SPINCONTROL;
	s_options_cpuUtil_box.generic.x = 0;
	s_options_cpuUtil_box.generic.y = 160 * cl_fontScale->value;
	s_options_cpuUtil_box.generic.name = "Draw CPU Utilization";
	s_options_cpuUtil_box.generic.callback = CpuUtilFunc;
	s_options_cpuUtil_box.itemnames = yesno_names;

	s_options_fps_box.generic.type = MTYPE_SPINCONTROL;
	s_options_fps_box.generic.x = 0;
	s_options_fps_box.generic.y = 170 * cl_fontScale->value;
	s_options_fps_box.generic.name = "Draw FPS";
	s_options_fps_box.generic.callback = FpsFunc;
	s_options_fps_box.itemnames = fps_names;


	s_options_time_box.generic.type = MTYPE_SPINCONTROL;
	s_options_time_box.generic.x = 0;
	s_options_time_box.generic.y = 180 * cl_fontScale->value;
	s_options_time_box.generic.name = "Draw Date / Time";
	s_options_time_box.generic.callback = TimeFunc;
	s_options_time_box.itemnames = yesno_names;
	//-------------------------

	s_options_advanced_options_action.generic.type = MTYPE_ACTION;
	s_options_advanced_options_action.generic.x = 0;
	s_options_advanced_options_action.generic.y = 200 * cl_fontScale->value;
	s_options_advanced_options_action.generic.name = "Advanced Settings";
	s_options_advanced_options_action.generic.callback = AdvancedSettingsFunc;


	s_options_customize_options_action.generic.type = MTYPE_ACTION;
	s_options_customize_options_action.generic.x = 0;
	s_options_customize_options_action.generic.y = 210 * cl_fontScale->value;
	s_options_customize_options_action.generic.name = "Customize Controls";
	s_options_customize_options_action.generic.callback = CustomizeControlsFunc;
	//-------------------------

	s_options_defaults_action.generic.type = MTYPE_ACTION;
	s_options_defaults_action.generic.x = 0;
	s_options_defaults_action.generic.y = 230 * cl_fontScale->value;
	s_options_defaults_action.generic.name = "Reset Defaults";
	s_options_defaults_action.generic.callback = ControlsResetDefaultsFunc;


	s_options_console_action.generic.type = MTYPE_ACTION;
	s_options_console_action.generic.x = 0;
	s_options_console_action.generic.y = 240 * cl_fontScale->value;
	s_options_console_action.generic.name = "go to console";
	s_options_console_action.generic.callback = ConsoleFunc;

	ControlsSetMenuItemValues();

	Menu_AddItem(&s_options_menu, (void *)&s_options_effectsVolume_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_options_musicvolume_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_options_musicsrc_list);
	Menu_AddItem(&s_options_menu, (void *)&s_options_aldev_box);
	Menu_AddItem(&s_options_menu, (void*)&s_options_alResempler_box);
	
	Menu_AddItem(&s_options_menu, (void *)&s_options_hrtf);
	Menu_AddItem(&s_options_menu, (void *)&s_options_useEFX_list);

	Menu_AddItem(&s_options_menu, (void *)&s_options_sensitivity_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_options_alwaysrun_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_invertmouse_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_crosshair_box);
	Menu_AddItem(&s_options_menu, (void*)&s_options_gamepad_box);

	Menu_AddItem(&s_options_menu, (void *)&s_options_cpuUtil_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_fps_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_time_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_advanced_options_action);
	Menu_AddItem(&s_options_menu, (void *)&s_options_customize_options_action);
	Menu_AddItem(&s_options_menu, (void *)&s_options_defaults_action);
	Menu_AddItem(&s_options_menu, (void *)&s_options_console_action);
}

void M_Option_Banner(image_t *banner[2]) {
	int w, h;
	int move;

	move = 170 + (cl_fontScale->value - 1) * 100;

	w = banner[0]->width;
	h = banner[0]->height;

	move += h;
	if (cl_fontScale->value == 2) {
		Draw_ScaledPic(viddef.width / 2 - (w * 0.5), viddef.height / 2 - move, cl_fontScale->value, cl_fontScale->value, banner[0]);
		Draw_ScaledBumpPic(viddef.width / 2 - (w * 0.5), viddef.height / 2 - move, cl_fontScale->value, cl_fontScale->value, banner[0], banner[1]);
	}
	else 
		if (cl_fontScale->value == 3) {
			Draw_ScaledPic(viddef.width / 2 - (w * 0.75), viddef.height / 2 - move, cl_fontScale->value, cl_fontScale->value, banner[0]);
			Draw_ScaledBumpPic(viddef.width / 2 - (w * 0.75), viddef.height / 2 - move, cl_fontScale->value, cl_fontScale->value, banner[0], banner[1]);
	}
}

void Options_MenuDraw(void) {
	M_Option_Banner(i_banner_options);

	Menu_AdjustCursor(&s_options_menu, 1);
	Menu_Draw(&s_options_menu);
}

int Options_MenuKey(int key) {
	return Default_MenuKey(&s_options_menu, key);
}

void M_Menu_Options_f(void) {
	Options_MenuInit();
	M_PushMenu(Options_MenuDraw, Options_MenuKey);
}

/*
=======================================================================

VIDEO MENU

=======================================================================
*/

void M_Menu_Video_f(void) {
	VID_MenuInit();
	M_PushMenu(VID_MenuDraw, VID_MenuKey);
}

/*
=============================================================================

END GAME MENU

=============================================================================
*/
static int credits_start_time;
static char **credits;
static char *creditsIndex[256];
static char *creditsBuffer;

static char *idcredits[] = {
	"+QUAKE II XP",
	"",
	"+PROGRAMMING",
	"Kirk Barnes",
	"KriGSSv1N",
	"Pavel 'VorteX' Timofeyev",
	"Willow",
	"Linux port by Alejandro Pulver",
	"FreeBSD port by Alexey 'danfe' Dokuchaev",
	"",
	"+BETA TESTERS",
	"Serge 'Berserker' Borodulin",
	"ViPeR_2540 - beta testing and new hardware",
	"Sergei V. Sh",
	"",
	"+MEDIA ART",
	"Kirk Barnes",
	"KriGSSv1N",
	"Turic",
	"Atex",
	"DeltaDesignRus",
	"Jim 'Just_Jim' Waurzyniak",
	"Serge 'Berserker' Borodulin",
	"Focator",
	"Pieter Verhoeven",
	"Stefan 'evilreflex' Lautner",
	"Kurtis Smith",
	"",
	"+USE CODE",
	"MrG - quake2xp based on beefquake r5",
	"Serge 'Berserker' Borodulin",
	"Vic",
	"Echon",
	"Knightmare",
	"Berserk",
	"Discoloda",
	"SulQ2",
	"",
	"+SPECIAL THANKS",
	"",
	"To all the guys from quakesrc.org, quakedev.com, inside3d.com",
	"",
	"+QUAKE II BY ID SOFTWARE",
	"",
	"+PROGRAMMING",
	"John Carmack",
	"John Cash",
	"Brian Hook",
	"",
	"+ART",
	"Adrian Carmack",
	"Kevin Cloud",
	"Paul Steed",
	"",
	"+LEVEL DESIGN",
	"Tim Willits",
	"American McGee",
	"Christian Antkow",
	"Paul Jaquays",
	"Brandon James",
	"",
	"+BIZ",
	"Todd Hollenshead",
	"Barrett (Bear) Alexander",
	"Donna Jackson",
	"",
	"",
	"+SPECIAL THANKS",
	"Ben Donges for beta testing",
	"",
	"",
	"",
	"",
	"",
	"",
	"+ADDITIONAL SUPPORT",
	"",
	"+LINUX PORT AND CTF",
	"Dave \"Zoid\" Kirsch",
	"",
	"+CINEMATIC SEQUENCES",
	"Ending Cinematic by Blur Studio - ",
	"Venice, CA",
	"",
	"Environment models for Introduction",
	"Cinematic by Karl Dolgener",
	"",
	"Assistance with environment design",
	"by Cliff Iwai",
	"",
	"+SOUND EFFECTS AND MUSIC",
	"Sound Design by Soundelux Media Labs.",
	"Music Composed and Produced by",
	"Soundelux Media Labs.  Special thanks",
	"to Bill Brown, Tom Ozanich, Brian",
	"Celano, Jeff Eisner, and The Soundelux",
	"Players.",
	"",
	"\"Level Music\" by Sonic Mayhem",
	"www.sonicmayhem.com",
	"",
	"\"Quake II Theme Song\"",
	"(C) 1997 Rob Zombie. All Rights",
	"Reserved.",
	"",
	"Track 10 (\"Climb\") by Jer Sypult",
	"",
	"Voice of computers by",
	"Carly Staehlin-Taylor",
	"",
	"+THANKS TO ACTIVISION",
	"+IN PARTICULAR:",
	"",
	"John Tam",
	"Steve Rosenthal",
	"Marty Stratton",
	"Henk Hartong",
	"",
	"Quake II(tm) (C)1997 Id Software, Inc.",
	"All Rights Reserved.  Distributed by",
	"Activision, Inc. under license.",
	"Quake II(tm), the Id Software name,",
	"the \"Q II\"(tm) logo and id(tm)",
	"logo are trademarks of Id Software,",
	"Inc. Activision(R) is a registered",
	"trademark of Activision, Inc. All",
	"other trademarks and trade names are",
	"properties of their respective owners.",
	0
};

static char *xatcredits[] = {
	"+QUAKE II MISSION PACK: THE RECKONING",
	"+BY",
	"+XATRIX ENTERTAINMENT, INC.",
	"",
	"+DESIGN AND DIRECTION",
	"Drew Markham",
	"",
	"+PRODUCED BY",
	"Greg Goodrich",
	"",
	"+PROGRAMMING",
	"Rafael Paiz",
	"",
	"+LEVEL DESIGN / ADDITIONAL GAME DESIGN",
	"Alex Mayberry",
	"",
	"+LEVEL DESIGN",
	"Mal Blackwell",
	"Dan Koppel",
	"",
	"+ART DIRECTION",
	"Michael \"Maxx\" Kaufman",
	"",
	"+COMPUTER GRAPHICS SUPERVISOR AND",
	"+CHARACTER ANIMATION DIRECTION",
	"Barry Dempsey",
	"",
	"+SENIOR ANIMATOR AND MODELER",
	"Jason Hoover",
	"",
	"+CHARACTER ANIMATION AND",
	"+MOTION CAPTURE SPECIALIST",
	"Amit Doron",
	"",
	"+ART",
	"Claire Praderie-Markham",
	"Viktor Antonov",
	"Corky Lehmkuhl",
	"",
	"+INTRODUCTION ANIMATION",
	"Dominique Drozdz",
	"",
	"+ADDITIONAL LEVEL DESIGN",
	"Aaron Barber",
	"Rhett Baldwin",
	"",
	"+3D CHARACTER ANIMATION TOOLS",
	"Gerry Tyra, SA Technology",
	"",
	"+ADDITIONAL EDITOR TOOL PROGRAMMING",
	"Robert Duffy",
	"",
	"+ADDITIONAL PROGRAMMING",
	"Ryan Feltrin",
	"",
	"+PRODUCTION COORDINATOR",
	"Victoria Sylvester",
	"",
	"+SOUND DESIGN",
	"Gary Bradfield",
	"",
	"+MUSIC BY",
	"Sonic Mayhem",
	"",
	"",
	"",
	"+SPECIAL THANKS",
	"+TO",
	"+OUR FRIENDS AT ID SOFTWARE",
	"",
	"John Carmack",
	"John Cash",
	"Brian Hook",
	"Adrian Carmack",
	"Kevin Cloud",
	"Paul Steed",
	"Tim Willits",
	"Christian Antkow",
	"Paul Jaquays",
	"Brandon James",
	"Todd Hollenshead",
	"Barrett (Bear) Alexander",
	"Dave \"Zoid\" Kirsch",
	"Donna Jackson",
	"",
	"",
	"",
	"+THANKS TO ACTIVISION",
	"+IN PARTICULAR:",
	"",
	"Marty Stratton",
	"Henk \"The Original Ripper\" Hartong",
	"Kevin Kraff",
	"Jamey Gottlieb",
	"Chris Hepburn",
	"",
	"+AND THE GAME TESTERS",
	"",
	"Tim Vanlaw",
	"Doug Jacobs",
	"Steven Rosenthal",
	"David Baker",
	"Chris Campbell",
	"Aaron Casillas",
	"Steve Elwell",
	"Derek Johnstone",
	"Igor Krinitskiy",
	"Samantha Lee",
	"Michael Spann",
	"Chris Toft",
	"Juan Valdes",
	"",
	"+THANKS TO INTERGRAPH COMPUTER SYTEMS",
	"+IN PARTICULAR:",
	"",
	"Michael T. Nicolaou",
	"",
	"",
	"Quake II Mission Pack: The Reckoning",
	"(tm) (C)1998 Id Software, Inc. All",
	"Rights Reserved. Developed by Xatrix",
	"Entertainment, Inc. for Id Software,",
	"Inc. Distributed by Activision Inc.",
	"under license. Quake(R) is a",
	"registered trademark of Id Software,",
	"Inc. Quake II Mission Pack: The",
	"Reckoning(tm), Quake II(tm), the Id",
	"Software name, the \"Q II\"(tm) logo",
	"and id(tm) logo are trademarks of Id",
	"Software, Inc. Activision(R) is a",
	"registered trademark of Activision,",
	"Inc. Xatrix(R) is a registered",
	"trademark of Xatrix Entertainment,",
	"Inc. All other trademarks and trade",
	"names are properties of their",
	"respective owners.",
	0
};

static char *roguecredits[] = {
	"+QUAKE II MISSION PACK 2: GROUND ZERO",
	"+BY",
	"+ROGUE ENTERTAINMENT, INC.",
	"",
	"+PRODUCED BY",
	"Jim Molinets",
	"",
	"+PROGRAMMING",
	"Peter Mack",
	"Patrick Magruder",
	"",
	"+LEVEL DESIGN",
	"Jim Molinets",
	"Cameron Lamprecht",
	"Berenger Fish",
	"Robert Selitto",
	"Steve Tietze",
	"Steve Thoms",
	"",
	"+ART DIRECTION",
	"Rich Fleider",
	"",
	"+ART",
	"Rich Fleider",
	"Steve Maines",
	"Won Choi",
	"",
	"+ANIMATION SEQUENCES",
	"Creat Studios",
	"Steve Maines",
	"",
	"+ADDITIONAL LEVEL DESIGN",
	"Rich Fleider",
	"Steve Maines",
	"Peter Mack",
	"",
	"+SOUND",
	"James Grunke",
	"",
	"+GROUND ZERO THEME",
	"+AND",
	"+MUSIC BY",
	"Sonic Mayhem",
	"",
	"+VWEP MODELS",
	"Brent \"Hentai\" Dill",
	"",
	"",
	"",
	"+SPECIAL THANKS",
	"+TO",
	"+OUR FRIENDS AT ID SOFTWARE",
	"",
	"John Carmack",
	"John Cash",
	"Brian Hook",
	"Adrian Carmack",
	"Kevin Cloud",
	"Paul Steed",
	"Tim Willits",
	"Christian Antkow",
	"Paul Jaquays",
	"Brandon James",
	"Todd Hollenshead",
	"Barrett (Bear) Alexander",
	"Katherine Anna Kang",
	"Donna Jackson",
	"Dave \"Zoid\" Kirsch",
	"",
	"",
	"",
	"+THANKS TO ACTIVISION",
	"+IN PARTICULAR:",
	"",
	"Marty Stratton",
	"Henk Hartong",
	"Mitch Lasky",
	"Steve Rosenthal",
	"Steve Elwell",
	"",
	"+AND THE GAME TESTERS",
	"",
	"The Ranger Clan",
	"Dave \"Zoid\" Kirsch",
	"Nihilistic Software",
	"Robert Duffy",
	"",
	"And Countless Others",
	"",
	"",
	"",
	"Quake II Mission Pack 2: Ground Zero",
	"(tm) (C)1998 Id Software, Inc. All",
	"Rights Reserved. Developed by Rogue",
	"Entertainment, Inc. for Id Software,",
	"Inc. Distributed by Activision Inc.",
	"under license. Quake(R) is a",
	"registered trademark of Id Software,",
	"Inc. Quake II Mission Pack 2: Ground",
	"Zero(tm), Quake II(tm), the Id",
	"Software name, the \"Q II\"(tm) logo",
	"and id(tm) logo are trademarks of Id",
	"Software, Inc. Activision(R) is a",
	"registered trademark of Activision,",
	"Inc. Rogue(R) is a registered",
	"trademark of Rogue Entertainment,",
	"Inc. All other trademarks and trade",
	"names are properties of their",
	"respective owners.",
	0
};


void M_Credits_MenuDraw(void) {
	int i, x, y;
	int i6s = 6 * cl_fontScale->value;

	// draw the credits
	
	drawIDlogo = qfalse;

	for (i = 0, y = viddef.height - ((cls.realtime - credits_start_time) / 30.0F); credits[i] && y < (int)viddef.height; y += 10 * cl_fontScale->value, i++)    /// Berserker' FIX: was y < viddef.height
	{
		int j, stringoffset = 0;
		int bold;

		if (y <= -i6s)
			continue;

		if (credits[i][0] == '+') {
			bold = 128;
			stringoffset = 1;
		}
		else {
			bold = 0;
			stringoffset = 0;
		}

		for (j = 0; credits[i][j + stringoffset]; j++)
		{
			x = (viddef.width - strlen(credits[i]) * i6s - stringoffset * i6s) / 2 + (j + stringoffset) * i6s;

			Draw_CharScaled(x, y, cl_fontScale->value, cl_fontScale->value, credits[i][j + stringoffset] + bold);
		}
	}

	if (y < 0)
		credits_start_time = cls.realtime;

}

int M_Credits_Key(int key) {
	switch (key) {
	case K_ESCAPE:
	case K_XPAD_B: // ?????????????????? ?????? credits ?????? ???????????????????????? "B"
		if (creditsBuffer)
			FS_FreeFile(creditsBuffer);
		
		M_PopMenu();
		break;
	}

	return menu_out_sound;
}

extern int Developer_searchpath(int who);

void M_Menu_Credits_f(void) {
	int n;
	int count;
	char *p;
	int isdeveloper = 0;

	creditsBuffer = NULL;
	count = FS_LoadFile("credits", (void**)&creditsBuffer);
	if (count != -1) {
		p = creditsBuffer;
		for (n = 0; n < 255; n++) {
			creditsIndex[n] = p;
			while (*p != '\r' && *p != '\n') {
				p++;
				if (--count == 0)
					break;
			}
			if (*p == '\r') {
				*p++ = 0;
				if (--count == 0)
					break;
			}
			*p++ = 0;
			if (--count == 0)
				break;
		}
		creditsIndex[++n] = 0;
		credits = creditsIndex;
	}
	else {
		isdeveloper = Developer_searchpath(1);

		if (isdeveloper == 1)	// xatrix
			credits = xatcredits;
		else if (isdeveloper == 2)	// ROGUE
			credits = roguecredits;
		else {
			credits = idcredits;
		}

	}

	credits_start_time = cls.realtime;
	M_PushMenu(M_Credits_MenuDraw, M_Credits_Key);
}

/*
=============================================================================

GAME MENU

=============================================================================
*/

static int m_game_cursor;

static menuframework_s s_game_menu;
static menuaction_s s_easy_game_action;
static menuaction_s s_medium_game_action;
static menuaction_s s_hard_game_action;
static menuaction_s s_nightmare_game_action;
static menuaction_s s_load_game_action;
static menuaction_s s_save_game_action;
static menuaction_s s_credits_action;
static menuseparator_s s_blankline;

static void StartGame(void) {
	// disable updates and start the cinematic going
	cl.servercount = -1;
	M_ForceMenuOff();
	Cvar_SetValue("deathmatch", 0);
	Cvar_SetValue("coop", 0);

	Cvar_SetValue("gamerules", 0);	// PGM

	Cbuf_AddText("loading ; killserver ; wait ; newgame\n");
	cls.key_dest = key_game;
}

static void EasyGameFunc(void *data) {
	Cvar_ForceSet("skill", "0");
	StartGame();
}

static void MediumGameFunc(void *data) {
	Cvar_ForceSet("skill", "1");
	StartGame();
}

static void HardGameFunc(void *data) {
	Cvar_ForceSet("skill", "2");
	StartGame();
}

static void NightmareGameFunc(void *data) {
	Cvar_ForceSet("skill", "3");
	StartGame();
}

static void LoadGameFunc(void *unused) {
	M_Menu_LoadGame_f();
}

static void SaveGameFunc(void *unused) {
	M_Menu_SaveGame_f();
}

static void CreditsFunc(void *unused) {
	M_Menu_Credits_f();
}

#define	MAX_MODS			48
#define	MAX_MOD_NAMELEN		24
menuframework_s	s_mods_menu;
menuaction_s	s_mods_actions[MAX_MODS];
menuaction_s	s_mod_game_action;
char			m_mod_names[MAX_MODS][MAX_MOD_NAMELEN + 1];
void FS_InitFilesystem(void);

void ModCallback(void* self)
{
	menuaction_s* a = (menuaction_s*)self;

	if (s_mods_menu.cursor < MAX_MODS) {
		Cbuf_AddText("killserver\n");
		Cbuf_AddText(va("game %s\n", m_mod_names[s_mods_menu.cursor]));
		Cbuf_AddText("exec default.cfg\n");
		Cbuf_AddText("exec xpconfig.cfg\n");
	}
	M_ForceMenuOff();
}

void DrawModShot(void* m)
{
	int	w = 0, h = 0, size = 0, scale;
	menuaction_s* menu = (menuaction_s*)m;
	
	if (cl_fontScale->integer >= 3)
		scale = 250;
	else 
		scale = 150;

	w = viddef.width * 0.50 + 10 * cl_fontScale->integer;
	h = viddef.height / 2 - scale;
	size = min(viddef.width - w, viddef.height - h);
	size *= 0.75;

	if (Draw_FindPic(m_mod_names[s_mods_menu.cursor])) {
		Draw_Fill(w - 3, h - 3, size + 6, size + 6, 0.3, 0.3, 0.3, 1.0);
		Draw_StretchPic(w, h, size, size, m_mod_names[s_mods_menu.cursor]);
	}
	else {
		Draw_Fill(w - 3, h - 3, size + 6, size + 6, 0.3, 0.3, 0.3, 1.0);
		Draw_StretchPic(w, h, size, size, "idlog");
	}
}

qboolean Mods_MenuInit()
{
	char	*path = NULL;
	char	**dirnames = NULL;
	char	findname[MAX_OSPATH];
	int		i, l, ndirs = 0, t = 0, len = 0, scale;
	static char	modinfo[MAX_MODS][MAX_CLIENTS];
	FILE	*f;

	path = FS_NextPath(NULL);
	if (path)
	{
		char* tmp = findname;
		Com_sprintf(findname, sizeof(findname), "%s/../*.*", path);
		while (*tmp != 0)
		{
			if (*tmp == '\\')
				*tmp = '/';
			tmp++;
		}
		l = strlen(findname) - 3;	// cut "*.*"
		dirnames = FS_ListFiles(findname, &ndirs, SFF_SUBDIR, 0);

		if (dirnames != 0)
		{
			if (ndirs > MAX_MODS)
				ndirs = MAX_MODS;

			for (i = 0; i < ndirs; i++){

				strcpy(m_mod_names[i], &dirnames[i][l]);
				free(dirnames[i]);
			}
			free(dirnames);
		}
	}

	if (!ndirs)
		return qfalse;

	if (cl_fontScale->integer >= 3)
		scale = 250;
	else
		scale = 150;

	s_mods_menu.x = viddef.width * 0.50 - 20 * cl_fontScale->value;
	s_mods_menu.y = viddef.height / 2 - scale;

	s_mods_menu.nitems = 0;

	for (i = 0; i < MAX_MODS; i++){

		if (t == ndirs)
			break;

		s_mods_actions[t].generic.name = m_mod_names[t];
		s_mods_actions[t].generic.flags = QMF_LEFT_JUSTIFY;
		s_mods_actions[t].generic.localdata[0] = t;
		s_mods_actions[t].generic.callback = ModCallback;
		s_mods_actions[t].generic.y = i * 10 * cl_fontScale->value;

		s_mods_actions[t].generic.type = MTYPE_ACTION;
		s_mods_actions[t].generic.statusbarfunc = DrawModShot;

		if (!Q_strcasecmp(m_mod_names[t], "baseq2"))
			s_mods_actions[t].generic.statusbar = "Quake II";
		else
		if (!Q_strcasecmp(m_mod_names[t], "ctf"))
			s_mods_actions[t].generic.statusbar = "Quake II: Capture The Flag";
		else
			if (!Q_strcasecmp(m_mod_names[t], "rogue"))
				s_mods_actions[t].generic.statusbar = "Quake II: Ground Zero";
		else
			if (!Q_strcasecmp(m_mod_names[t], "xatrix"))
				s_mods_actions[t].generic.statusbar = "Quake II: The Reckoning";
		else
			if (!Q_strcasecmp(m_mod_names[t], "gladiator"))
				s_mods_actions[t].generic.statusbar = "Gladiator Bot";
		else
			if (!Q_strcasecmp(m_mod_names[t], "3zb2"))
				s_mods_actions[t].generic.statusbar = "3rd Zigock Bot";
		else
			if (!Q_strcasecmp(m_mod_names[t], "eraser"))
				s_mods_actions[t].generic.statusbar = "Eraser Bot";
			else 				
				if (!Q_strcasecmp(m_mod_names[t], "zaero"))
					s_mods_actions[t].generic.statusbar = "Quake II: Zaero";
			else {
				f = fopen(va("%s/info.md", m_mod_names[t]), "rb");
				if (!f) {
					s_mods_actions[t].generic.statusbar = m_mod_names[t];
				}
				else {
					fseek(f, 0, SEEK_END);
					len = min(255, ftell(f));
					fseek(f, 0, SEEK_SET);
					memset(modinfo[t], 0, 256);
					fread(modinfo[t], len, 1, f);
					s_mods_actions[t].generic.statusbar = &modinfo[t][0];
					fclose(f);
				}
		
			}

			Menu_AddItem(&s_mods_menu, &s_mods_actions[t]);

		t++;
	}

	return qtrue;
}


void Mods_MenuDraw(void)
{
	M_Banner(i_banner_game);
	Menu_Draw(&s_mods_menu);
}

int Mods_MenuKey(int key)
{
	return Default_MenuKey(&s_mods_menu, key);
}

void SelectModFunc(void* data)
{
	if (Mods_MenuInit())
		M_PushMenu(Mods_MenuDraw, Mods_MenuKey);
}


void Game_MenuInit(void) {
	static const char *difficulty_names[] = {
		"Easy",
		"Medium",
		"Hard",
		"Nightmare!",
		0
	};

	drawIDlogo = qfalse;
	
	s_game_menu.x = viddef.width * 0.50 - 20 * cl_fontScale->value;
	s_game_menu.nitems = 0;

	s_easy_game_action.generic.type = MTYPE_ACTION;
	s_easy_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_easy_game_action.generic.x = 0;
	s_easy_game_action.generic.y = 0;
	s_easy_game_action.generic.name = "Easy";
	s_easy_game_action.generic.callback = EasyGameFunc;

	s_medium_game_action.generic.type = MTYPE_ACTION;
	s_medium_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_medium_game_action.generic.x = 0;
	s_medium_game_action.generic.y = 10 * cl_fontScale->value;
	s_medium_game_action.generic.name = "Medium";
	s_medium_game_action.generic.callback = MediumGameFunc;

	s_hard_game_action.generic.type = MTYPE_ACTION;
	s_hard_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_hard_game_action.generic.x = 0;
	s_hard_game_action.generic.y = 20 * cl_fontScale->value;
	s_hard_game_action.generic.name = "Hard";
	s_hard_game_action.generic.callback = HardGameFunc;

	s_nightmare_game_action.generic.type = MTYPE_ACTION;
	s_nightmare_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_nightmare_game_action.generic.x = 0;
	s_nightmare_game_action.generic.y = 30 * cl_fontScale->value;
	s_nightmare_game_action.generic.name = "Nightmare!";
	s_nightmare_game_action.generic.callback = NightmareGameFunc;

	s_blankline.generic.type = MTYPE_SEPARATOR;

	s_load_game_action.generic.type = MTYPE_ACTION;
	s_load_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_load_game_action.generic.x = 0;
	s_load_game_action.generic.y = 50 * cl_fontScale->value;
	s_load_game_action.generic.name = "Load Game";
	s_load_game_action.generic.callback = LoadGameFunc;

	s_save_game_action.generic.type = MTYPE_ACTION;
	s_save_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_save_game_action.generic.x = 0;
	s_save_game_action.generic.y = 60 * cl_fontScale->value;
	s_save_game_action.generic.name = "Save Game";
	s_save_game_action.generic.callback = SaveGameFunc;

	s_blankline.generic.type = MTYPE_SEPARATOR;

	s_mod_game_action.generic.type = MTYPE_ACTION;
	s_mod_game_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_mod_game_action.generic.x = 0;
	s_mod_game_action.generic.y = 80 * cl_fontScale->value;
	s_mod_game_action.generic.name = "Select Mod";
	s_mod_game_action.generic.callback = SelectModFunc;

	s_blankline.generic.type = MTYPE_SEPARATOR;

	s_credits_action.generic.type = MTYPE_ACTION;
	s_credits_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_credits_action.generic.x = 0;
	s_credits_action.generic.y = 100 * cl_fontScale->value;
	s_credits_action.generic.name = "Credits";
	s_credits_action.generic.callback = CreditsFunc;

	Menu_AddItem(&s_game_menu, (void *)&s_easy_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_medium_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_hard_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_nightmare_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_blankline);
	Menu_AddItem(&s_game_menu, (void *)&s_load_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_save_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_mod_game_action);
	Menu_AddItem(&s_game_menu, (void *)&s_blankline);
	Menu_AddItem(&s_game_menu, (void *)&s_credits_action);

	Menu_Center(&s_game_menu);
}

void Game_MenuDraw(void) {
	M_Banner(i_banner_game);
	Menu_AdjustCursor(&s_game_menu, 1);
	Menu_Draw(&s_game_menu);
}

int Game_MenuKey(int key) {
	return Default_MenuKey(&s_game_menu, key);
}

void M_Menu_Game_f(void) {
	Game_MenuInit();
	M_PushMenu(Game_MenuDraw, Game_MenuKey);
	m_game_cursor = 1;
}

/*
=============================================================================

LOADGAME MENU

=============================================================================
*/

#define	MAX_SAVEGAMES	15
#define MAX_SAVESTRING 512
static menuframework_s s_savegame_menu;

static menuframework_s s_loadgame_menu;
static menuaction_s s_loadgame_actions[MAX_SAVEGAMES];

static menuaction_s s_quickLoadGame_actions;
static menuaction_s s_quickSaveGame_actions;

char m_savestrings[MAX_SAVEGAMES][MAX_OSPATH];
char m_savesInfos[MAX_SAVEGAMES][MAX_OSPATH];
char m_savemapnames[MAX_SAVEGAMES][MAX_OSPATH];

static char m_quicksavestring[32];
static qboolean m_quicksavevalid;
char m_quickSavesInfos[MAX_OSPATH];

qboolean m_savevalid[MAX_SAVEGAMES];

void Create_MapNamesList()
{
	int		i, j;
	FILE* f;
	char	name[MAX_OSPATH];

	for (i = 0; i < MAX_SAVEGAMES; i++)
	{
		Com_sprintf(name, sizeof(name), "%s/savexp/save%i/server.ssv", FS_Gamedir(), i);
		f = fopen(name, "rb");
		if (!f)
		{
			strcpy(m_savemapnames[i], "<EMPTY>");
			m_savevalid[i] = qfalse;
		}
		else
		{
			fseek(f, 0x20, SEEK_SET); //move to map name
			fread(m_savemapnames[i], sizeof(m_savemapnames[i]), 1, f);

			if (m_savemapnames[i][0] == '*')
			{
				for (j = 0; j < MAX_OSPATH - 1; j++)
					m_savemapnames[i][j] = m_savemapnames[i][j + 1];
			}

			for (j = 0; j < strlen(m_savemapnames[i]); j++)
			{
				if (m_savemapnames[i][j] == '$' || m_savemapnames[i][j] == 0)
				{
					m_savemapnames[i][j] = 0;
					break;
				}
			}
			fclose(f);
			m_savevalid[i] = qtrue;
		}
	}
}

void Create_QuickSavesList(qboolean load)
{
	FILE* f;
	char	name[MAX_OSPATH], tmp[32];

	Com_sprintf(name, sizeof(name), "%s/savexp/quick/comment.sav", FS_Gamedir());
	f = fopen(name, "rb");
	if (!f)
	{
		strcpy(m_quicksavestring, "QUICKSAVE <EMPTY>");
		m_quicksavevalid = qfalse;
	}
	else
	{
		fseek(f, 12, SEEK_SET); //move to map name
		fread(tmp, sizeof(tmp), 1, f);
		if(load)
			Com_sprintf(m_quicksavestring, sizeof(m_quicksavestring), "QUICKLOAD: %s", tmp);
		else
			Com_sprintf(m_quicksavestring, sizeof(m_quicksavestring), "QUICKSAVE: %s", tmp);
		fclose(f);
		m_quicksavevalid = qtrue;
	}

}


void Create_Savestrings(void) {
	int i;
	FILE *f;
	char name[MAX_OSPATH];

	for (i = 0; i < MAX_SAVEGAMES; i++) {
		Com_sprintf(name, sizeof(name), "%s/savexp/save%i/comment.sav", FS_Gamedir(), i);
		f = fopen(name, "rb");
		if (!f) {
			strcpy(m_savestrings[i], "<EMPTY>");
			m_savevalid[i] = qfalse;
		}
		else {
			fseek(f, 9, SEEK_SET); // try find victory screen
			fread(m_savestrings[i], sizeof(m_savestrings[i]), 1, f);

			if (!Q_stricmp(m_savestrings[i], "victory.pcx")) {
				strcpy(m_savestrings[i], "Victory Screen");
			}
			else {
				fseek(f, 0, SEEK_SET);
				fread(m_savestrings[i], sizeof(m_savestrings[i]), 1, f);
			}
			fclose(f);
			m_savevalid[i] = qtrue;
		}
	}
}

void Create_SavesInfoss(void) {
	int i;
	FILE *f;
	char name[MAX_OSPATH];

	for (i = 0; i < MAX_SAVEGAMES; i++) {
		Com_sprintf(name, sizeof(name), "%s/savexp/save%i/saveinfo.sav",
			FS_Gamedir(), i);
		f = fopen(name, "rb");
		if (!f) {
			strcpy(m_savesInfos[i], "");
			m_savevalid[i] = qfalse;
		}
		else {
			fseek(f, 9, SEEK_SET); // try find victory screen
			fread(m_savesInfos[i], sizeof(m_savesInfos[i]), 1, f);

			if (!Q_stricmp(m_savesInfos[i], "victory.pcx")) {
				strcpy(m_savesInfos[i], "Victory Screen");
			}
			else {
				fseek(f, 0, SEEK_SET);
				fread(m_savesInfos[i], sizeof(m_savesInfos[i]), 1, f);
			}
			fclose(f);
			m_savevalid[i] = qtrue;
		}
	}
}

void Create_QuickSavesInfoss(void) {
	FILE* f;
	char name[MAX_OSPATH];

		Com_sprintf(name, sizeof(name), "%s/savexp/quick/saveinfo.sav", FS_Gamedir());
		f = fopen(name, "rb");
		if (!f) {
			strcpy(m_quickSavesInfos, "");
			m_quicksavevalid = qfalse;
		}
		else {
			fseek(f, 0, SEEK_SET);
			fread(m_quickSavesInfos, sizeof(m_quickSavesInfos), 1, f);
			fclose(f);
			m_quicksavevalid = qtrue;
		}
}

void DrawSavedShot(void* m)
{
	int				i, w, h, picWidth, center;
	menuaction_s	*menu = (menuaction_s*)m;
	char			savePic[MAX_QPATH];
	float			aspect;

	Draw_GetPicSize(&w, &h, "m_banner_load_game");
	picWidth = (viddef.width * 0.5) - (w * 0.25) + 10 * (int)cl_fontScale->value;
	picWidth += 90;

	// fucking hack, lol
	int wtf;
	if (cl_fontScale->integer >= 3)
		wtf = 14;
	else
		wtf = 16;

	i = menu->generic.localdata[0];

	if (m_savevalid[i])
	{
	if (i){
		strcpy(savePic, va("/savexp/save%i/shot.jpg", i));
		Draw_GetPicSize(&w, &h, savePic);
		aspect = (float)w / (float)h;

		R_FreePic(savePic); // update pic cache
		Draw_Fill(viddef.width * 0.5 - 5, (viddef.height * 0.5 - (picWidth / aspect) * 0.5) - 5, picWidth + 10, (picWidth / aspect) + (wtf * (int)cl_fontScale->value), 0.3, 0.3, 0.3, 1.0);
		Draw_StretchPic(viddef.width * 0.5, viddef.height * 0.5 - (picWidth / aspect) * 0.5, picWidth, picWidth / aspect, savePic);
		Draw_Fill(viddef.width * 0.5, (viddef.height * 0.5 + (picWidth / aspect) * 0.5) + (cl_fontScale->integer - 1), picWidth, 10 * cl_fontScale->value, 0.0, 0.5, 0.0, 1.0);

		center = (viddef.width * 0.5) + (picWidth * 0.5) - ((int)strlen(m_savesInfos[i]) * (int)cl_fontScale->value * 6) * 0.5;
		Draw_StringScaled(center, (viddef.height * 0.5 + (picWidth / aspect) * 0.5) + 2, cl_fontScale->value, cl_fontScale->value, m_savesInfos[i]);
		}
	else {

		if(!Q_stricmp(m_savemapnames[i], "victory.pcx")) {
			strcpy(savePic, va("/pics/victory.jpg", m_savemapnames[i]));
			Draw_GetPicSize(&w, &h, savePic);
			aspect = (float)w / (float)h;
			Draw_Fill(viddef.width * 0.5 - 5, (viddef.height * 0.5 - (picWidth / aspect) * 0.5) - 5, picWidth + 10, (picWidth / aspect) + (wtf * (int)cl_fontScale->value), 0.3, 0.3, 0.3, 1.0);
			Draw_StretchPic(viddef.width * 0.5, viddef.height * 0.5 - (picWidth / aspect) * 0.5, picWidth, picWidth / aspect, savePic);
		}
	else
		{
		strcpy(savePic, va("/levelshots/%s.jpg", m_savemapnames[i]));
		Draw_GetPicSize(&w, &h, savePic);

		if (w == -1 && h == -1) { //custom map no levelshot
			strcpy(savePic, "/gfx/defshot.jpg");
			Draw_GetPicSize(&w, &h, savePic);
		}

		aspect = (float)w / (float)h;
		Draw_Fill(viddef.width * 0.5 - 5, (viddef.height * 0.5 - (picWidth / aspect) * 0.5) - 5, picWidth + 10, (picWidth / aspect) + (wtf * (int)cl_fontScale->value), 0.3, 0.3, 0.3, 1.0);
		Draw_StretchPic(viddef.width * 0.5, viddef.height * 0.5 - (picWidth / aspect) * 0.5, picWidth, picWidth / aspect, savePic);
		}
				
		Draw_Fill(viddef.width * 0.5, (viddef.height * 0.5 + (picWidth / aspect) * 0.5), picWidth, 10 * cl_fontScale->value, 0.0, 0.5, 0.0, 1.0);

		center = (viddef.width * 0.5) + (picWidth * 0.5) - ((int)strlen(m_savesInfos[i]) * (int)cl_fontScale->value * 6) * 0.5;
		Draw_StringScaled(center, (viddef.height * 0.5 + (picWidth / aspect) * 0.5) + 2, cl_fontScale->value, cl_fontScale->value, m_savesInfos[i]);
		}
	}
	else {
		Draw_GetPicSize(&w, &h, "nosaveshot");
		aspect = (float)w / (float)h;
		Draw_StretchPic(viddef.width * 0.5, viddef.height * 0.5 - (picWidth / aspect) * 0.5, picWidth, picWidth / aspect, "nosaveshot");
	}

}

void DrawQuickSavedShot(void* m)
{
	int				w, h, picWidth, center;
	menuaction_s*	menu = (menuaction_s*)m;
	char			savePic[MAX_QPATH];
	float			aspect;

	Draw_GetPicSize(&w, &h, "m_banner_load_game");
	picWidth = (viddef.width * 0.5) - (w * 0.25) + 10 * (int)cl_fontScale->value;
	picWidth += 90;

	// fucking hack, lol
	int wtf;
	if (cl_fontScale->integer >= 3)
		wtf = 14;
	else
		wtf = 16;

	strcpy(savePic, va("/savexp/quick/shot.jpg"));
	Draw_GetPicSize(&w, &h, savePic);
	aspect = (float)w / (float)h;

	if (m_quicksavevalid){

		R_FreePic(savePic); // update pic cache
		Draw_Fill(viddef.width * 0.5 - 5, (viddef.height * 0.5 - (picWidth / aspect) * 0.5) - 5, picWidth + 10, (picWidth / aspect) + (wtf * (int)cl_fontScale->value), 0.3, 0.3, 0.3, 1.0);
		Draw_StretchPic(viddef.width * 0.5, viddef.height * 0.5 - (picWidth / aspect) * 0.5, picWidth, picWidth / aspect, savePic);
		Draw_Fill(viddef.width * 0.5, (viddef.height * 0.5 + (picWidth / aspect) * 0.5) + (cl_fontScale->integer - 1), picWidth, 10 * cl_fontScale->value, 0.0, 0.5, 0.0, 1.0);

		center = (viddef.width * 0.5) + (picWidth * 0.5) - ((int)strlen(m_quickSavesInfos) * (int)cl_fontScale->value * 6) * 0.5;
		Draw_StringScaled(center, (viddef.height * 0.5 + (picWidth / aspect) * 0.5) + 2, cl_fontScale->value, cl_fontScale->value, m_quickSavesInfos);
	}
	else {
		Draw_GetPicSize(&w, &h, "nosaveshot");
		aspect = (float)w / (float)h;
		Draw_StretchPic(viddef.width * 0.5, viddef.height * 0.5 - (picWidth / aspect) * 0.5, picWidth, picWidth / aspect, "nosaveshot");
	}
}	

void LoadGameCallback(void *self) {
	menuaction_s *a = (menuaction_s *)self;

	if (m_savevalid[a->generic.localdata[0]])
		Cbuf_AddText(va("load save%i\n", a->generic.localdata[0]));
	M_ForceMenuOff();
}

void QuickLoadGameCallback(void* self) {
	menuaction_s* a = (menuaction_s*)self;

	Cbuf_AddText(va("load quick\n"));
	M_ForceMenuOff();
}

void QuickSaveGameCallback(void* self) {
	menuaction_s* a = (menuaction_s*)self;

	Cbuf_AddText(va("save quick\n"));
	M_ForceMenuOff();
}

void LoadGame_MenuInit(void) {
	int i, w, h;

	drawIDlogo = qfalse;
	
	Draw_GetPicSize(&w, &h, "m_banner_load_game");

	s_loadgame_menu.x = viddef.width * 0.5 - w * 0.25 - 16 * cl_fontScale->value;
	s_loadgame_menu.y = viddef.height * 0.2 + h * 0.5 + 8 * cl_fontScale->value;
	s_loadgame_menu.x *= 0.5;

	s_loadgame_menu.nitems = 0;

	// The quickload slot...
	Create_QuickSavesList(qtrue);
	Create_QuickSavesInfoss();
	
	s_quickLoadGame_actions.generic.type = MTYPE_ACTION;
	s_quickLoadGame_actions.generic.name = m_quicksavestring;
	s_quickLoadGame_actions.generic.flags = QMF_LEFT_JUSTIFY;
	s_quickLoadGame_actions.generic.x = 0;
	s_quickLoadGame_actions.generic.y = 10 * cl_fontScale->value;
	s_quickLoadGame_actions.generic.localdata[0] = 0;
	s_quickLoadGame_actions.generic.statusbarfunc = DrawQuickSavedShot;

	if (!m_quicksavevalid)
	{
		s_quickLoadGame_actions.generic.callback = NULL;
	}
	else
	{
		s_quickLoadGame_actions.generic.callback = QuickLoadGameCallback;
	}

	Menu_AddItem(&s_loadgame_menu, &s_quickLoadGame_actions);
	
	Create_Savestrings();
	Create_SavesInfoss();
	Create_MapNamesList();

	for (i = 0; i < MAX_SAVEGAMES; i++) {
		
		s_loadgame_actions[i].generic.name = m_savestrings[i];
		s_loadgame_actions[i].generic.flags = QMF_LEFT_JUSTIFY;
		s_loadgame_actions[i].generic.localdata[0] = i;
		s_loadgame_actions[i].generic.callback = LoadGameCallback;

		s_loadgame_actions[i].generic.x = 0;
		s_loadgame_actions[i].generic.y = (i+3) * 10 * cl_fontScale->value;
		if (i > 0)				// separate from autosave
			s_loadgame_actions[i].generic.y += 10 * cl_fontScale->value;

		s_loadgame_actions[i].generic.type = MTYPE_ACTION;
		s_loadgame_actions[i].generic.statusbarfunc = DrawSavedShot;

		Menu_AddItem(&s_loadgame_menu, &s_loadgame_actions[i]);
	}
	
}

void LoadGame_MenuDraw(void) {
	M_Banner(i_banner_load_game);
	//  Menu_AdjustCursor( &s_loadgame_menu, 1 );
	Menu_Draw(&s_loadgame_menu);
}

int LoadGame_MenuKey(int key) {
	if (key == K_ESCAPE || key == K_ENTER) {
		s_savegame_menu.cursor = s_loadgame_menu.cursor - 1;
		if (s_savegame_menu.cursor < 0)
			s_savegame_menu.cursor = 0;
	}
	return Default_MenuKey(&s_loadgame_menu, key);
}

void M_Menu_LoadGame_f(void) {
	LoadGame_MenuInit();
	M_PushMenu(LoadGame_MenuDraw, LoadGame_MenuKey);
}


/*
=============================================================================

SAVEGAME MENU

=============================================================================
*/
static menuframework_s s_savegame_menu;
static menuaction_s s_savegame_actions[MAX_SAVEGAMES];

void SaveGameCallback(void *self) {
	menuaction_s *a = (menuaction_s *)self;

	Cbuf_AddText(va("save save%i\n", a->generic.localdata[0]));
	M_ForceMenuOff();
}

void SaveGame_MenuDraw(void) {
	M_Banner(i_banner_save_game);
	Menu_AdjustCursor(&s_savegame_menu, 1);
	Menu_Draw(&s_savegame_menu);
}

void SaveGame_MenuInit(void) {
	int i, w, h;

	Draw_GetPicSize(&w, &h, "m_banner_save_game");

	s_savegame_menu.x = viddef.width * 0.5 - w * 0.25 - 16 * cl_fontScale->value;
	s_savegame_menu.y = viddef.height * 0.2 + h * 0.5 + 8 * cl_fontScale->value;
	s_savegame_menu.x *= 0.5;
	s_savegame_menu.nitems = 0;


	// The quicksave slot...
	Create_QuickSavesList(qfalse);
	Create_QuickSavesInfoss();

	s_quickSaveGame_actions.generic.type = MTYPE_ACTION;
	s_quickSaveGame_actions.generic.name = m_quicksavestring;
	s_quickSaveGame_actions.generic.flags = QMF_LEFT_JUSTIFY;
	s_quickSaveGame_actions.generic.x = 0;
	s_quickSaveGame_actions.generic.y = 10 * cl_fontScale->value;
	s_quickSaveGame_actions.generic.localdata[0] = 0;
	s_quickSaveGame_actions.generic.statusbarfunc = DrawQuickSavedShot;
	s_quickSaveGame_actions.generic.callback = QuickSaveGameCallback;

	Menu_AddItem(&s_savegame_menu, &s_quickSaveGame_actions);


	Create_Savestrings();
	Create_SavesInfoss();
	Create_MapNamesList();

	drawIDlogo = qfalse;

	// don't include the autosave slot
	for (i = 0; i < MAX_SAVEGAMES - 1; i++) {
		s_savegame_actions[i].generic.name = m_savestrings[i + 1];
		s_savegame_actions[i].generic.localdata[0] = i + 1;
		s_savegame_actions[i].generic.flags = QMF_LEFT_JUSTIFY;
		s_savegame_actions[i].generic.callback = SaveGameCallback;

		s_savegame_actions[i].generic.x = 0;
		s_savegame_actions[i].generic.y = (i+3) * 10 * cl_fontScale->value;

		s_savegame_actions[i].generic.type = MTYPE_ACTION;
		s_savegame_actions[i].generic.statusbarfunc = DrawSavedShot;

		Menu_AddItem(&s_savegame_menu, &s_savegame_actions[i]);
	}
}

int SaveGame_MenuKey(int key) {
	if (key == K_ENTER || key == K_ESCAPE) {
		s_loadgame_menu.cursor = s_savegame_menu.cursor - 1;
		if (s_loadgame_menu.cursor < 0)
			s_loadgame_menu.cursor = 0;
	}
	return Default_MenuKey(&s_savegame_menu, key);
}

void M_Menu_SaveGame_f(void) {
	if (!Com_ServerState())
		return;					// not playing a game

	SaveGame_MenuInit();
	M_PushMenu(SaveGame_MenuDraw, SaveGame_MenuKey);
	Create_Savestrings();
}


/*
=============================================================================

JOIN SERVER MENU

=============================================================================
*/
#define MAX_LOCAL_SERVERS 8

static menuframework_s s_joinserver_menu;
static menuseparator_s s_joinserver_server_title;
static menuaction_s s_joinserver_search_action;
static menuaction_s s_joinserver_address_book_action;
static menuaction_s s_joinserver_server_actions[MAX_LOCAL_SERVERS];

int m_num_servers;
#define	NO_SERVER_STRING	"<no server>"

// user readable information
static char local_server_names[MAX_LOCAL_SERVERS][80];

// network address
static netadr_t local_server_netadr[MAX_LOCAL_SERVERS];

void M_AddToServerList(netadr_t adr, char *info) {
	int i;

	if (m_num_servers == MAX_LOCAL_SERVERS)
		return;
	while (*info == ' ')
		info++;

	// ignore if duplicated
	for (i = 0; i < m_num_servers; i++)
		if (!strcmp(info, local_server_names[i]))
			return;

	local_server_netadr[m_num_servers] = adr;
	strncpy(local_server_names[m_num_servers], info,
		sizeof(local_server_names[0]) - 1);
	m_num_servers++;
}


void JoinServerFunc(void *self) {
	char buffer[128];
	int index;

	index = (menuaction_s *)self - s_joinserver_server_actions;

	if (Q_stricmp(local_server_names[index], NO_SERVER_STRING) == 0)
		return;

	if (index >= m_num_servers)
		return;

	Com_sprintf(buffer, sizeof(buffer), "connect %s\n",
		NET_AdrToString(local_server_netadr[index]));
	Cbuf_AddText(buffer);
	M_ForceMenuOff();
}

void AddressBookFunc(void *self) {
	M_Menu_AddressBook_f();
}

void NullCursorDraw(void *self) {}

void SearchLocalGames(void) {
	int		i;
	float	fontscale = cl_fontScale->value;

	m_num_servers = 0;
	for (i = 0; i < MAX_LOCAL_SERVERS; i++)
		strcpy(local_server_names[i], NO_SERVER_STRING);

	M_DrawTextBox(8, 120 - 48, 36 * fontscale, 3 * fontscale);

	M_Print(16 + 16, 120 - 48 + 8 * fontscale, "Searching for local servers, this");
	M_Print(16 + 16, 120 - 48 + 16 * fontscale, "could take up to a minute, so");
	M_Print(16 + 16, 120 - 48 + 24 * fontscale, "please be patient.");

	// the text box won't show up unless we do a buffer swap
	GLimp_EndFrame();

	// send out info packets
	CL_PingServers_f();
}

void SearchLocalGamesFunc(void *self) {
	SearchLocalGames();
}

void JoinServer_MenuInit(void) {
	int i, shift;

	shift = 60 * (cl_fontScale->value - 1);

	drawIDlogo = qfalse;

	s_joinserver_menu.x = viddef.width * 0.50 - 120;

	s_joinserver_menu.nitems = 0;

	s_joinserver_address_book_action.generic.type = MTYPE_ACTION;
	s_joinserver_address_book_action.generic.name = "address book";
	s_joinserver_address_book_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_joinserver_address_book_action.generic.x = 0;
	s_joinserver_address_book_action.generic.y = shift;
	s_joinserver_address_book_action.generic.callback = AddressBookFunc;

	s_joinserver_search_action.generic.type = MTYPE_ACTION;
	s_joinserver_search_action.generic.name = "refresh server list";
	s_joinserver_search_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_joinserver_search_action.generic.x = 0;
	s_joinserver_search_action.generic.y = shift + 10 * cl_fontScale->value;
	s_joinserver_search_action.generic.callback = SearchLocalGamesFunc;
	s_joinserver_search_action.generic.statusbar = "search for servers";

	s_joinserver_server_title.generic.type = MTYPE_SEPARATOR;
	s_joinserver_server_title.generic.name = "connect to...";
	s_joinserver_server_title.generic.x = 80;
	s_joinserver_server_title.generic.y = shift + 30 * cl_fontScale->value;

	for (i = 0; i < MAX_LOCAL_SERVERS; i++) {
		s_joinserver_server_actions[i].generic.type = MTYPE_ACTION;
		strcpy(local_server_names[i], NO_SERVER_STRING);
		s_joinserver_server_actions[i].generic.name = local_server_names[i];
		s_joinserver_server_actions[i].generic.flags = QMF_LEFT_JUSTIFY;
		s_joinserver_server_actions[i].generic.x = 0;
		s_joinserver_server_actions[i].generic.y = shift + 40 * cl_fontScale->value + i * 10 * cl_fontScale->value;

		s_joinserver_server_actions[i].generic.callback = JoinServerFunc;
		s_joinserver_server_actions[i].generic.statusbar = "press ENTER to connect";
	}

	Menu_AddItem(&s_joinserver_menu, &s_joinserver_address_book_action);
	Menu_AddItem(&s_joinserver_menu, &s_joinserver_server_title);
	Menu_AddItem(&s_joinserver_menu, &s_joinserver_search_action);

	for (i = 0; i < 8; i++)
		Menu_AddItem(&s_joinserver_menu, &s_joinserver_server_actions[i]);

	Menu_Center(&s_joinserver_menu);

	SearchLocalGames();
}

void JoinServer_MenuDraw(void) {
	M_Banner(i_banner_join_server);
	Menu_Draw(&s_joinserver_menu);
}


int JoinServer_MenuKey(int key) {
	return Default_MenuKey(&s_joinserver_menu, key);
}

void M_Menu_JoinServer_f(void) {
	JoinServer_MenuInit();
	M_PushMenu(JoinServer_MenuDraw, JoinServer_MenuKey);
}


/*
=============================================================================

START SERVER MENU

=============================================================================
*/
static menuframework_s s_startserver_menu;
static char **mapnames;
static int nummaps;

static menuaction_s s_startserver_start_action;
static menuaction_s s_startserver_dmoptions_action;
static menufield_s s_timelimit_field;
static menufield_s s_fraglimit_field;
static menufield_s s_maxclients_field;
static menufield_s s_hostname_field;
static menulist_s s_startmap_list;
static menulist_s s_rules_box;

void DMOptionsFunc(void *self) {
	if (s_rules_box.curvalue == 1)
		return;
	M_Menu_DMOptions_f();
}

void RulesChangeFunc(void *self) {
	// DM
	if (s_rules_box.curvalue == 0) {
		s_maxclients_field.generic.statusbar = NULL;
		s_startserver_dmoptions_action.generic.statusbar = NULL;
	}
	else if (s_rules_box.curvalue == 1)	// coop // PGM
	{
		s_maxclients_field.generic.statusbar = "4 maximum for cooperative";
		if (atoi(s_maxclients_field.buffer) > 4)
			strcpy(s_maxclients_field.buffer, "4");
		s_startserver_dmoptions_action.generic.statusbar =
			"N/A for cooperative";
	}
	//=====
	//PGM
	// ROGUE GAMES
	else if (Developer_searchpath(2) == 2) {
		if (s_rules_box.curvalue == 2)	// tag
		{
			s_maxclients_field.generic.statusbar = NULL;
			s_startserver_dmoptions_action.generic.statusbar = NULL;
		}
		/*
		else if(s_rules_box.curvalue == 3)		// deathball
		{
		s_maxclients_field.generic.statusbar = NULL;
		s_startserver_dmoptions_action.generic.statusbar = NULL;
		}
		*/
	}
	//PGM
	//=====
}



void StartServerActionFunc(void *self) {
	char	startmap[1024];
	int		timelimit;
	int		fraglimit;
	int		maxclients;
	char	*spot;

	strcpy(startmap, strchr(mapnames[s_startmap_list.curvalue], '\n') + 1);

	maxclients = atoi(s_maxclients_field.buffer);
	timelimit = atoi(s_timelimit_field.buffer);
	fraglimit = atoi(s_fraglimit_field.buffer);

	Cvar_SetValue("maxclients", ClampCvar(0, maxclients, maxclients));
	Cvar_SetValue("timelimit", ClampCvar(0, timelimit, timelimit));
	Cvar_SetValue("fraglimit", ClampCvar(0, fraglimit, fraglimit));
	Cvar_Set("hostname", s_hostname_field.buffer);
	//	Cvar_SetValue ("deathmatch", !s_rules_box.curvalue );
	//	Cvar_SetValue ("coop", s_rules_box.curvalue );

	//PGM
	if ((s_rules_box.curvalue < 2) || (Developer_searchpath(2) != 2)) {
		Cvar_SetValue("deathmatch", !s_rules_box.curvalue);
		Cvar_SetValue("coop", s_rules_box.curvalue);
		Cvar_SetValue("gamerules", 0);
	}
	else {
		Cvar_SetValue("deathmatch", 1);	// deathmatch is always qtrue for rogue games, right?
		Cvar_SetValue("coop", 0);			// FIXME - this might need to depend on which game we're running
		Cvar_SetValue("gamerules", s_rules_box.curvalue);
	}
	//PGM

	spot = NULL;
	if (s_rules_box.curvalue == 1)		// PGM
	{
		if (Q_stricmp(startmap, "bunk1") == 0)
			spot = "start";
		else if (Q_stricmp(startmap, "mintro") == 0)
			spot = "start";
		else if (Q_stricmp(startmap, "fact1") == 0)
			spot = "start";
		else if (Q_stricmp(startmap, "power1") == 0)
			spot = "pstart";
		else if (Q_stricmp(startmap, "biggun") == 0)
			spot = "bstart";
		else if (Q_stricmp(startmap, "hangar1") == 0)
			spot = "unitstart";
		else if (Q_stricmp(startmap, "city1") == 0)
			spot = "unitstart";
		else if (Q_stricmp(startmap, "boss1") == 0)
			spot = "bosstart";
	}

	if (spot) {
		if (Com_ServerState())
			Cbuf_AddText("disconnect\n");
		Cbuf_AddText(va("gamemap \"*%s$%s\"\n", startmap, spot));
	}
	else {
		Cbuf_AddText(va("map %s\n", startmap));
	}

	M_ForceMenuOff();
}

void StartServer_MenuInit(void) {
	static char *dm_coop_names[] =
	{
		"deathmatch",
		"cooperative",
		0
	};
	//=======
	//PGM
	static char *dm_coop_names_rogue[] =
	{
		"deathmatch",
		"cooperative",
		"tag",
		//		"deathball",
		0
	};
	//PGM
	//=======
	char *buffer;
	char  mapsname[1024];
	char *s;
	int length;
	int i;
	FILE *fp;

	/*
	** load the list of map names
	*/
	Com_sprintf(mapsname, sizeof(mapsname), "%s/maps.lst", FS_Gamedir());
	if ((fp = fopen(mapsname, "rb")) == 0) {
		if ((length = FS_LoadFile("maps.lst", (void **)&buffer)) == -1)
			Com_Error(ERR_DROP, "couldn't find maps.lst\n");
	}
	else {
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		buffer = (char*)malloc(length);
		fread(buffer, length, 1, fp);
	}

	s = buffer;

	i = 0;
	while (i < length) {
		if (s[i] == '\r')
			nummaps++;
		i++;
	}

	if (nummaps == 0)
		Com_Error(ERR_DROP, "no maps in maps.lst\n");

	mapnames = malloc(sizeof(char *)* (nummaps + 1));
	memset(mapnames, 0, sizeof(char *)* (nummaps + 1));

	s = buffer;

	for (i = 0; i < nummaps; i++) {
		char  shortname[MAX_TOKEN_CHARS];
		char  longname[MAX_TOKEN_CHARS];
		char  scratch[200];
		int		j, l;

		strcpy(shortname, COM_Parse(&s));
		l = strlen(shortname);
		for (j = 0; j < l; j++)
			shortname[j] = toupper(shortname[j]);
		strcpy(longname, COM_Parse(&s));
		Com_sprintf(scratch, sizeof(scratch), "%s\n%s", longname, shortname);

		mapnames[i] = (char*)malloc(strlen(scratch) + 1);
		strcpy(mapnames[i], scratch);
	}
	mapnames[nummaps] = 0;

	if (fp != 0) {
		fp = 0;
		free(buffer);
	}
	else {
		FS_FreeFile(buffer);
	}

	drawIDlogo = qfalse;

	/*
	** initialize the menu stuff
	*/
	s_startserver_menu.x = viddef.width * 0.50;
	s_startserver_menu.nitems = 0;

	s_startmap_list.generic.type = MTYPE_SPINCONTROL;
	s_startmap_list.generic.x = 0;
	s_startmap_list.generic.y = 0;
	s_startmap_list.generic.name = "initial map";
	s_startmap_list.itemnames = mapnames;

	s_rules_box.generic.type = MTYPE_SPINCONTROL;
	s_rules_box.generic.x = 0;
	s_rules_box.generic.y = 20 * cl_fontScale->value;
	s_rules_box.generic.name = "rules";

	//PGM - rogue games only available with rogue DLL.
	if (Developer_searchpath(2) == 2)
		s_rules_box.itemnames = dm_coop_names_rogue;
	else
		s_rules_box.itemnames = dm_coop_names;
	//PGM

	if (Cvar_VariableInteger("coop"))
		s_rules_box.curvalue = 1;
	else
		s_rules_box.curvalue = 0;
	s_rules_box.generic.callback = RulesChangeFunc;

	s_timelimit_field.generic.type = MTYPE_FIELD;
	s_timelimit_field.generic.name = "time limit";
	s_timelimit_field.generic.flags = QMF_NUMBERSONLY;
	s_timelimit_field.generic.x = 0;
	s_timelimit_field.generic.y = 36 * cl_fontScale->value;
	s_timelimit_field.generic.statusbar = "0 = no limit";
	s_timelimit_field.length = 3;
	s_timelimit_field.visible_length = 3;
	strcpy(s_timelimit_field.buffer, Cvar_VariableString("timelimit"));

	s_fraglimit_field.generic.type = MTYPE_FIELD;
	s_fraglimit_field.generic.name = "frag limit";
	s_fraglimit_field.generic.flags = QMF_NUMBERSONLY;
	s_fraglimit_field.generic.x = 0;
	s_fraglimit_field.generic.y = 54 * cl_fontScale->value;
	s_fraglimit_field.generic.statusbar = "0 = no limit";
	s_fraglimit_field.length = 3;
	s_fraglimit_field.visible_length = 3;
	strcpy(s_fraglimit_field.buffer, Cvar_VariableString("fraglimit"));

	/*
	** maxclients determines the maximum number of players that can join
	** the game.  If maxclients is only "1" then we should default the menu
	** option to 8 players, otherwise use whatever its current value is.
	** Clamping will be done when the server is actually started.
	*/
	s_maxclients_field.generic.type = MTYPE_FIELD;
	s_maxclients_field.generic.name = "max players";
	s_maxclients_field.generic.flags = QMF_NUMBERSONLY;
	s_maxclients_field.generic.x = 0;
	s_maxclients_field.generic.y = 72 * cl_fontScale->value;
	s_maxclients_field.generic.statusbar = NULL;
	s_maxclients_field.length = 3;
	s_maxclients_field.visible_length = 3;
	if (Cvar_VariableInteger("maxclients") == 1)
		strcpy(s_maxclients_field.buffer, "8");
	else
		strcpy(s_maxclients_field.buffer, Cvar_VariableString("maxclients"));

	s_hostname_field.generic.type = MTYPE_FIELD;
	s_hostname_field.generic.name = "hostname";
	s_hostname_field.generic.flags = 0;
	s_hostname_field.generic.x = 0;
	s_hostname_field.generic.y = 90 * cl_fontScale->value;
	s_hostname_field.generic.statusbar = NULL;
	s_hostname_field.length = 12;
	s_hostname_field.visible_length = 12;
	strcpy(s_hostname_field.buffer, Cvar_VariableString("hostname"));

	s_startserver_dmoptions_action.generic.type = MTYPE_ACTION;
	s_startserver_dmoptions_action.generic.name = " deathmatch flags";
	s_startserver_dmoptions_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_startserver_dmoptions_action.generic.x = 24;
	s_startserver_dmoptions_action.generic.y = 108 * cl_fontScale->value;
	s_startserver_dmoptions_action.generic.statusbar = NULL;
	s_startserver_dmoptions_action.generic.callback = DMOptionsFunc;

	s_startserver_start_action.generic.type = MTYPE_ACTION;
	s_startserver_start_action.generic.name = " begin";
	s_startserver_start_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_startserver_start_action.generic.x = 24;
	s_startserver_start_action.generic.y = 128 * cl_fontScale->value;
	s_startserver_start_action.generic.callback = StartServerActionFunc;

	Menu_AddItem(&s_startserver_menu, &s_startmap_list);
	Menu_AddItem(&s_startserver_menu, &s_rules_box);
	Menu_AddItem(&s_startserver_menu, &s_timelimit_field);
	Menu_AddItem(&s_startserver_menu, &s_fraglimit_field);
	Menu_AddItem(&s_startserver_menu, &s_maxclients_field);
	Menu_AddItem(&s_startserver_menu, &s_hostname_field);
	Menu_AddItem(&s_startserver_menu, &s_startserver_dmoptions_action);
	Menu_AddItem(&s_startserver_menu, &s_startserver_start_action);

	Menu_Center(&s_startserver_menu);

	// call this now to set proper inital state
	RulesChangeFunc(NULL);
}




void StartServer_MenuDraw(void) {
	Menu_Draw(&s_startserver_menu);
}

int StartServer_MenuKey(int key) {
	if (key == K_ESCAPE) {
		if (mapnames) {
			int i;

			for (i = 0; i < nummaps; i++)
				free(mapnames[i]);
			free(mapnames);
		}
		mapnames = 0;
		nummaps = 0;
	}

	return Default_MenuKey(&s_startserver_menu, key);
}

void M_Menu_StartServer_f(void) {
	StartServer_MenuInit();
	M_PushMenu(StartServer_MenuDraw, StartServer_MenuKey);
}

/*
=============================================================================

DMOPTIONS BOOK MENU

=============================================================================
*/
static char dmoptions_statusbar[128];

static menuframework_s s_dmoptions_menu;
static menulist_s s_friendlyfire_box;
static menulist_s s_falls_box;
static menulist_s s_weapons_stay_box;
static menulist_s s_instant_powerups_box;
static menulist_s s_powerups_box;
static menulist_s s_health_box;
static menulist_s s_spawn_farthest_box;
static menulist_s s_teamplay_box;
static menulist_s s_samelevel_box;
static menulist_s s_force_respawn_box;
static menulist_s s_armor_box;
static menulist_s s_allow_exit_box;
static menulist_s s_infinite_ammo_box;
static menulist_s s_fixed_fov_box;
static menulist_s s_quad_drop_box;
static menulist_s s_predator_box;
static menulist_s s_ut_dj_box;

//ROGUE
static menulist_s s_no_mines_box;
static menulist_s s_no_nukes_box;
static menulist_s s_stack_double_box;
static menulist_s s_no_spheres_box;
//ROGUE

static void DMFlagCallback(void *self) {
	menulist_s *f = (menulist_s *)self;
	int flags;
	int bit = 0;

	flags = Cvar_VariableValue("dmflags");

	if (f == &s_friendlyfire_box) {
		if (f->curvalue)
			flags &= ~DF_NO_FRIENDLY_FIRE;
		else
			flags |= DF_NO_FRIENDLY_FIRE;
		goto setvalue;
	}
	else if (f == &s_falls_box) {
		if (f->curvalue)
			flags &= ~DF_NO_FALLING;
		else
			flags |= DF_NO_FALLING;
		goto setvalue;
	}
	else if (f == &s_weapons_stay_box) {
		bit = DF_WEAPONS_STAY;
	}
	else if (f == &s_instant_powerups_box) {
		bit = DF_INSTANT_ITEMS;
	}
	else if (f == &s_allow_exit_box) {
		bit = DF_ALLOW_EXIT;
	}
	else if (f == &s_powerups_box) {
		if (f->curvalue)
			flags &= ~DF_NO_ITEMS;
		else
			flags |= DF_NO_ITEMS;
		goto setvalue;
	}
	else if (f == &s_health_box) {
		if (f->curvalue)
			flags &= ~DF_NO_HEALTH;
		else
			flags |= DF_NO_HEALTH;
		goto setvalue;
	}
	else if (f == &s_spawn_farthest_box) {
		bit = DF_SPAWN_FARTHEST;
	}
	else if (f == &s_teamplay_box) {
		if (f->curvalue == 1) {
			flags |= DF_SKINTEAMS;
			flags &= ~DF_MODELTEAMS;
		}
		else if (f->curvalue == 2) {
			flags |= DF_MODELTEAMS;
			flags &= ~DF_SKINTEAMS;
		}
		else {
			flags &= ~(DF_MODELTEAMS | DF_SKINTEAMS);
		}

		goto setvalue;
	}
	else if (f == &s_samelevel_box) {
		bit = DF_SAME_LEVEL;
	}
	else if (f == &s_force_respawn_box) {
		bit = DF_FORCE_RESPAWN;
	}
	else if (f == &s_armor_box) {
		if (f->curvalue)
			flags &= ~DF_NO_ARMOR;
		else
			flags |= DF_NO_ARMOR;
		goto setvalue;
	}
	else if (f == &s_infinite_ammo_box) {
		bit = DF_INFINITE_AMMO;
	}
	else if (f == &s_fixed_fov_box) {
		bit = DF_FIXED_FOV;
	}
	else if (f == &s_quad_drop_box) {
		bit = DF_QUAD_DROP;
	}
	else if (f == &s_predator_box) {
		bit = DF_FLASHLIGHT;
	}
	else if (f == &s_ut_dj_box) {
		bit = DF_UT_DOUBLE_JUMP;

	}

	//=======
	//ROGUE
	else if (Developer_searchpath(2) == 2) {
		if (f == &s_no_mines_box) {
			bit = DF_NO_MINES;
		}
		else if (f == &s_no_nukes_box) {
			bit = DF_NO_NUKES;
		}
		else if (f == &s_stack_double_box) {
			bit = DF_NO_STACK_DOUBLE;
		}
		else if (f == &s_no_spheres_box) {
			bit = DF_NO_SPHERES;
		}
	}
	//ROGUE
	//=======

	if (f) {
		if (f->curvalue == 0)
			flags &= ~bit;
		else
			flags |= bit;
	}

setvalue:
	Cvar_SetValue("dmflags", flags);

	Com_sprintf(dmoptions_statusbar, sizeof(dmoptions_statusbar),
		"dmflags = %d", flags);

}

void DMOptions_MenuInit(void) {
	static char *yes_no_names[] = {
		"no", "yes", 0
	};
	static char *teamplay_names[] = {
		"disabled", "by skin", "by model", 0
	};
	int dmflags = Cvar_VariableInteger("dmflags");
	int y = 0;
	
	drawIDlogo = qfalse;

	s_dmoptions_menu.x = viddef.width * 0.50;
	s_dmoptions_menu.nitems = 0;

	s_falls_box.generic.type = MTYPE_SPINCONTROL;
	s_falls_box.generic.x = 0;
	s_falls_box.generic.y = y;
	s_falls_box.generic.name = "falling damage";
	s_falls_box.generic.callback = DMFlagCallback;
	s_falls_box.itemnames = yes_no_names;
	s_falls_box.curvalue = (dmflags & DF_NO_FALLING) == 0;

	s_weapons_stay_box.generic.type = MTYPE_SPINCONTROL;
	s_weapons_stay_box.generic.x = 0;
	s_weapons_stay_box.generic.y = y += 10 * cl_fontScale->value;
	s_weapons_stay_box.generic.name = "weapons stay";
	s_weapons_stay_box.generic.callback = DMFlagCallback;
	s_weapons_stay_box.itemnames = yes_no_names;
	s_weapons_stay_box.curvalue = (dmflags & DF_WEAPONS_STAY) != 0;

	s_instant_powerups_box.generic.type = MTYPE_SPINCONTROL;
	s_instant_powerups_box.generic.x = 0;
	s_instant_powerups_box.generic.y = y += 10 * cl_fontScale->value;
	s_instant_powerups_box.generic.name = "instant powerups";
	s_instant_powerups_box.generic.callback = DMFlagCallback;
	s_instant_powerups_box.itemnames = yes_no_names;
	s_instant_powerups_box.curvalue = (dmflags & DF_INSTANT_ITEMS) != 0;

	s_powerups_box.generic.type = MTYPE_SPINCONTROL;
	s_powerups_box.generic.x = 0;
	s_powerups_box.generic.y = y += 10 * cl_fontScale->value;
	s_powerups_box.generic.name = "allow powerups";
	s_powerups_box.generic.callback = DMFlagCallback;
	s_powerups_box.itemnames = yes_no_names;
	s_powerups_box.curvalue = (dmflags & DF_NO_ITEMS) == 0;

	s_health_box.generic.type = MTYPE_SPINCONTROL;
	s_health_box.generic.x = 0;
	s_health_box.generic.y = y += 10 * cl_fontScale->value;
	s_health_box.generic.callback = DMFlagCallback;
	s_health_box.generic.name = "allow health";
	s_health_box.itemnames = yes_no_names;
	s_health_box.curvalue = (dmflags & DF_NO_HEALTH) == 0;

	s_armor_box.generic.type = MTYPE_SPINCONTROL;
	s_armor_box.generic.x = 0;
	s_armor_box.generic.y = y += 10 * cl_fontScale->value;
	s_armor_box.generic.name = "allow armor";
	s_armor_box.generic.callback = DMFlagCallback;
	s_armor_box.itemnames = yes_no_names;
	s_armor_box.curvalue = (dmflags & DF_NO_ARMOR) == 0;

	s_spawn_farthest_box.generic.type = MTYPE_SPINCONTROL;
	s_spawn_farthest_box.generic.x = 0;
	s_spawn_farthest_box.generic.y = y += 10 * cl_fontScale->value;
	s_spawn_farthest_box.generic.name = "spawn farthest";
	s_spawn_farthest_box.generic.callback = DMFlagCallback;
	s_spawn_farthest_box.itemnames = yes_no_names;
	s_spawn_farthest_box.curvalue = (dmflags & DF_SPAWN_FARTHEST) != 0;

	s_samelevel_box.generic.type = MTYPE_SPINCONTROL;
	s_samelevel_box.generic.x = 0;
	s_samelevel_box.generic.y = y += 10 * cl_fontScale->value;
	s_samelevel_box.generic.name = "same map";
	s_samelevel_box.generic.callback = DMFlagCallback;
	s_samelevel_box.itemnames = yes_no_names;
	s_samelevel_box.curvalue = (dmflags & DF_SAME_LEVEL) != 0;

	s_force_respawn_box.generic.type = MTYPE_SPINCONTROL;
	s_force_respawn_box.generic.x = 0;
	s_force_respawn_box.generic.y = y += 10 * cl_fontScale->value;
	s_force_respawn_box.generic.name = "force respawn";
	s_force_respawn_box.generic.callback = DMFlagCallback;
	s_force_respawn_box.itemnames = yes_no_names;
	s_force_respawn_box.curvalue = (dmflags & DF_FORCE_RESPAWN) != 0;

	s_teamplay_box.generic.type = MTYPE_SPINCONTROL;
	s_teamplay_box.generic.x = 0;
	s_teamplay_box.generic.y = y += 10 * cl_fontScale->value;
	s_teamplay_box.generic.name = "teamplay";
	s_teamplay_box.generic.callback = DMFlagCallback;
	s_teamplay_box.itemnames = teamplay_names;

	s_allow_exit_box.generic.type = MTYPE_SPINCONTROL;
	s_allow_exit_box.generic.x = 0;
	s_allow_exit_box.generic.y = y += 10 * cl_fontScale->value;
	s_allow_exit_box.generic.name = "allow exit";
	s_allow_exit_box.generic.callback = DMFlagCallback;
	s_allow_exit_box.itemnames = yes_no_names;
	s_allow_exit_box.curvalue = (dmflags & DF_ALLOW_EXIT) != 0;

	s_infinite_ammo_box.generic.type = MTYPE_SPINCONTROL;
	s_infinite_ammo_box.generic.x = 0;
	s_infinite_ammo_box.generic.y = y += 10 * cl_fontScale->value;
	s_infinite_ammo_box.generic.name = "infinite ammo";
	s_infinite_ammo_box.generic.callback = DMFlagCallback;
	s_infinite_ammo_box.itemnames = yes_no_names;
	s_infinite_ammo_box.curvalue = (dmflags & DF_INFINITE_AMMO) != 0;

	s_fixed_fov_box.generic.type = MTYPE_SPINCONTROL;
	s_fixed_fov_box.generic.x = 0;
	s_fixed_fov_box.generic.y = y += 10 * cl_fontScale->value;
	s_fixed_fov_box.generic.name = "fixed FOV";
	s_fixed_fov_box.generic.callback = DMFlagCallback;
	s_fixed_fov_box.itemnames = yes_no_names;
	s_fixed_fov_box.curvalue = (dmflags & DF_FIXED_FOV) != 0;

	s_quad_drop_box.generic.type = MTYPE_SPINCONTROL;
	s_quad_drop_box.generic.x = 0;
	s_quad_drop_box.generic.y = y += 10 * cl_fontScale->value;
	s_quad_drop_box.generic.name = "quad drop";
	s_quad_drop_box.generic.callback = DMFlagCallback;
	s_quad_drop_box.itemnames = yes_no_names;
	s_quad_drop_box.curvalue = (dmflags & DF_QUAD_DROP) != 0;

	s_friendlyfire_box.generic.type = MTYPE_SPINCONTROL;
	s_friendlyfire_box.generic.x = 0;
	s_friendlyfire_box.generic.y = y += 10 * cl_fontScale->value;
	s_friendlyfire_box.generic.name = "friendly fire";
	s_friendlyfire_box.generic.callback = DMFlagCallback;
	s_friendlyfire_box.itemnames = yes_no_names;
	s_friendlyfire_box.curvalue = (dmflags & DF_NO_FRIENDLY_FIRE) == 0;

	s_predator_box.generic.type = MTYPE_SPINCONTROL;
	s_predator_box.generic.x = 0;
	s_predator_box.generic.y = y += 10 * cl_fontScale->value;
	s_predator_box.generic.name = "Flash Light";
	s_predator_box.generic.callback = DMFlagCallback;
	s_predator_box.itemnames = yes_no_names;
	s_predator_box.curvalue = (dmflags & DF_FLASHLIGHT) != 0;

	s_ut_dj_box.generic.type = MTYPE_SPINCONTROL;
	s_ut_dj_box.generic.x = 0;
	s_ut_dj_box.generic.y = y += 10 * cl_fontScale->value;
	s_ut_dj_box.generic.name = "UT double jump";
	s_ut_dj_box.generic.callback = DMFlagCallback;
	s_ut_dj_box.itemnames = yes_no_names;
	s_ut_dj_box.curvalue = (dmflags & DF_UT_DOUBLE_JUMP) != 0;

	//============
	//ROGUE
	if (Developer_searchpath(2) == 2) {
		s_no_mines_box.generic.type = MTYPE_SPINCONTROL;
		s_no_mines_box.generic.x = 0;
		s_no_mines_box.generic.y = y += 10 * cl_fontScale->value;
		s_no_mines_box.generic.name = "remove mines";
		s_no_mines_box.generic.callback = DMFlagCallback;
		s_no_mines_box.itemnames = yes_no_names;
		s_no_mines_box.curvalue = (dmflags & DF_NO_MINES) != 0;

		s_no_nukes_box.generic.type = MTYPE_SPINCONTROL;
		s_no_nukes_box.generic.x = 0;
		s_no_nukes_box.generic.y = y += 10 * cl_fontScale->value;
		s_no_nukes_box.generic.name = "remove nukes";
		s_no_nukes_box.generic.callback = DMFlagCallback;
		s_no_nukes_box.itemnames = yes_no_names;
		s_no_nukes_box.curvalue = (dmflags & DF_NO_NUKES) != 0;

		s_stack_double_box.generic.type = MTYPE_SPINCONTROL;
		s_stack_double_box.generic.x = 0;
		s_stack_double_box.generic.y = y += 10 * cl_fontScale->value;
		s_stack_double_box.generic.name = "2x/4x stacking off";
		s_stack_double_box.generic.callback = DMFlagCallback;
		s_stack_double_box.itemnames = yes_no_names;
		s_stack_double_box.curvalue = (dmflags & DF_NO_STACK_DOUBLE) != 0;

		s_no_spheres_box.generic.type = MTYPE_SPINCONTROL;
		s_no_spheres_box.generic.x = 0;
		s_no_spheres_box.generic.y = y += 10 * cl_fontScale->value;
		s_no_spheres_box.generic.name = "remove spheres";
		s_no_spheres_box.generic.callback = DMFlagCallback;
		s_no_spheres_box.itemnames = yes_no_names;
		s_no_spheres_box.curvalue = (dmflags & DF_NO_SPHERES) != 0;

	}
	//ROGUE
	//============

	Menu_AddItem(&s_dmoptions_menu, &s_falls_box);
	Menu_AddItem(&s_dmoptions_menu, &s_weapons_stay_box);
	Menu_AddItem(&s_dmoptions_menu, &s_instant_powerups_box);
	Menu_AddItem(&s_dmoptions_menu, &s_powerups_box);
	Menu_AddItem(&s_dmoptions_menu, &s_health_box);
	Menu_AddItem(&s_dmoptions_menu, &s_armor_box);
	Menu_AddItem(&s_dmoptions_menu, &s_spawn_farthest_box);
	Menu_AddItem(&s_dmoptions_menu, &s_samelevel_box);
	Menu_AddItem(&s_dmoptions_menu, &s_force_respawn_box);
	Menu_AddItem(&s_dmoptions_menu, &s_teamplay_box);
	Menu_AddItem(&s_dmoptions_menu, &s_allow_exit_box);
	Menu_AddItem(&s_dmoptions_menu, &s_infinite_ammo_box);
	Menu_AddItem(&s_dmoptions_menu, &s_fixed_fov_box);
	Menu_AddItem(&s_dmoptions_menu, &s_quad_drop_box);
	Menu_AddItem(&s_dmoptions_menu, &s_friendlyfire_box);
	Menu_AddItem(&s_dmoptions_menu, &s_predator_box);
	Menu_AddItem(&s_dmoptions_menu, &s_ut_dj_box);

	//=======
	//ROGUE
	if (Developer_searchpath(2) == 2) {
		Menu_AddItem(&s_dmoptions_menu, &s_no_mines_box);
		Menu_AddItem(&s_dmoptions_menu, &s_no_nukes_box);
		Menu_AddItem(&s_dmoptions_menu, &s_stack_double_box);
		Menu_AddItem(&s_dmoptions_menu, &s_no_spheres_box);
	}
	//ROGUE
	//=======

	Menu_Center(&s_dmoptions_menu);

	// set the original dmflags statusbar
	DMFlagCallback(0);
	Menu_SetStatusBar(&s_dmoptions_menu, dmoptions_statusbar);
}

void DMOptions_MenuDraw(void) {
	Menu_Draw(&s_dmoptions_menu);
}

int DMOptions_MenuKey(int key) {
	return Default_MenuKey(&s_dmoptions_menu, key);
}

void M_Menu_DMOptions_f(void) {
	DMOptions_MenuInit();
	M_PushMenu(DMOptions_MenuDraw, DMOptions_MenuKey);
}

/*
=============================================================================

DOWNLOADOPTIONS BOOK MENU

=============================================================================
*/
static menuframework_s s_downloadoptions_menu;

static menuseparator_s s_download_title;
static menulist_s s_allow_download_box;
static menulist_s s_allow_download_maps_box;
static menulist_s s_allow_download_models_box;
static menulist_s s_allow_download_players_box;
static menulist_s s_allow_download_sounds_box;

static void DownloadCallback(void *self) {
	menulist_s *f = (menulist_s *)self;

	if (f == &s_allow_download_box) {
		Cvar_SetValue("allow_download", f->curvalue);
	}

	else if (f == &s_allow_download_maps_box) {
		Cvar_SetValue("allow_download_maps", f->curvalue);
	}

	else if (f == &s_allow_download_models_box) {
		Cvar_SetValue("allow_download_models", f->curvalue);
	}

	else if (f == &s_allow_download_players_box) {
		Cvar_SetValue("allow_download_players", f->curvalue);
	}

	else if (f == &s_allow_download_sounds_box) {
		Cvar_SetValue("allow_download_sounds", f->curvalue);
	}
}

void DownloadOptions_MenuInit(void) {
	static char *yes_no_names[] = {
		"no", "yes", 0
	};
	int y = 0;

	drawIDlogo = qfalse;
	
	s_downloadoptions_menu.x = viddef.width * 0.50;
	s_downloadoptions_menu.nitems = 0;

	s_download_title.generic.type = MTYPE_SEPARATOR;
	s_download_title.generic.name = "Download Options";
	s_download_title.generic.x = 48;
	s_download_title.generic.y = y;

	s_allow_download_box.generic.type = MTYPE_SPINCONTROL;
	s_allow_download_box.generic.x = 0;
	s_allow_download_box.generic.y = y += 20 * cl_fontScale->value;
	s_allow_download_box.generic.name = "allow downloading";
	s_allow_download_box.generic.callback = DownloadCallback;
	s_allow_download_box.itemnames = yes_no_names;
	s_allow_download_box.curvalue =
		(Cvar_VariableInteger("allow_download") != 0);

	s_allow_download_maps_box.generic.type = MTYPE_SPINCONTROL;
	s_allow_download_maps_box.generic.x = 0;
	s_allow_download_maps_box.generic.y = y += 20 * cl_fontScale->value;
	s_allow_download_maps_box.generic.name = "maps";
	s_allow_download_maps_box.generic.callback = DownloadCallback;
	s_allow_download_maps_box.itemnames = yes_no_names;
	s_allow_download_maps_box.curvalue =
		(Cvar_VariableInteger("allow_download_maps") != 0);

	s_allow_download_players_box.generic.type = MTYPE_SPINCONTROL;
	s_allow_download_players_box.generic.x = 0;
	s_allow_download_players_box.generic.y = y += 10 * cl_fontScale->value;
	s_allow_download_players_box.generic.name = "player models/skins";
	s_allow_download_players_box.generic.callback = DownloadCallback;
	s_allow_download_players_box.itemnames = yes_no_names;
	s_allow_download_players_box.curvalue =
		(Cvar_VariableInteger("allow_download_players") != 0);

	s_allow_download_models_box.generic.type = MTYPE_SPINCONTROL;
	s_allow_download_models_box.generic.x = 0;
	s_allow_download_models_box.generic.y = y += 10 * cl_fontScale->value;
	s_allow_download_models_box.generic.name = "models";
	s_allow_download_models_box.generic.callback = DownloadCallback;
	s_allow_download_models_box.itemnames = yes_no_names;
	s_allow_download_models_box.curvalue =
		(Cvar_VariableInteger("allow_download_models") != 0);

	s_allow_download_sounds_box.generic.type = MTYPE_SPINCONTROL;
	s_allow_download_sounds_box.generic.x = 0;
	s_allow_download_sounds_box.generic.y = y += 10 * cl_fontScale->value;
	s_allow_download_sounds_box.generic.name = "sounds";
	s_allow_download_sounds_box.generic.callback = DownloadCallback;
	s_allow_download_sounds_box.itemnames = yes_no_names;
	s_allow_download_sounds_box.curvalue =
		(Cvar_VariableInteger("allow_download_sounds") != 0);

	Menu_AddItem(&s_downloadoptions_menu, &s_download_title);
	Menu_AddItem(&s_downloadoptions_menu, &s_allow_download_box);
	Menu_AddItem(&s_downloadoptions_menu, &s_allow_download_maps_box);
	Menu_AddItem(&s_downloadoptions_menu, &s_allow_download_players_box);
	Menu_AddItem(&s_downloadoptions_menu, &s_allow_download_models_box);
	Menu_AddItem(&s_downloadoptions_menu, &s_allow_download_sounds_box);

	Menu_Center(&s_downloadoptions_menu);

	// skip over title
	if (s_downloadoptions_menu.cursor == 0)
		s_downloadoptions_menu.cursor = 1;
}

void DownloadOptions_MenuDraw(void) {
	Menu_Draw(&s_downloadoptions_menu);
}

int DownloadOptions_MenuKey(int key) {
	return Default_MenuKey(&s_downloadoptions_menu, key);
}

void M_Menu_DownloadOptions_f(void) {
	DownloadOptions_MenuInit();
	M_PushMenu(DownloadOptions_MenuDraw, DownloadOptions_MenuKey);
}

/*
=============================================================================

ADDRESS BOOK MENU

=============================================================================
*/
#define NUM_ADDRESSBOOK_ENTRIES 9

static menuframework_s s_addressbook_menu;
static menufield_s s_addressbook_fields[NUM_ADDRESSBOOK_ENTRIES];

void AddressBook_MenuInit(void) {
	int i;
	int w = i_banner_addressbook[0]->width;
	int h = i_banner_addressbook[0]->height;

	s_addressbook_menu.x = viddef.width / 2 - ((w + 8) * cl_fontScale->value) / 4;
	s_addressbook_menu.y = viddef.height / 4 + (h * cl_fontScale->value);
	s_addressbook_menu.nitems = 0;
	
	drawIDlogo = qfalse;

	for (i = 0; i < NUM_ADDRESSBOOK_ENTRIES; i++) {
		cvar_t *adr;
		char buffer[20];

		Com_sprintf(buffer, sizeof(buffer), "adr%d", i);

		adr = Cvar_Get(buffer, "", CVAR_ARCHIVE);

		s_addressbook_fields[i].generic.type = MTYPE_FIELD;
		s_addressbook_fields[i].generic.name = 0;
		s_addressbook_fields[i].generic.callback = 0;
		s_addressbook_fields[i].generic.x = 0;
		s_addressbook_fields[i].generic.y = i * 18 * cl_fontScale->value + 0;
		s_addressbook_fields[i].generic.localdata[0] = i;
		s_addressbook_fields[i].cursor = 0;
		s_addressbook_fields[i].length = 60;
		s_addressbook_fields[i].visible_length = 30;

		strcpy(s_addressbook_fields[i].buffer, adr->string);

		Menu_AddItem(&s_addressbook_menu, &s_addressbook_fields[i]);
	}
}

int AddressBook_MenuKey(int key) {
	if (key == K_ESCAPE) {
		int index;
		char buffer[20];

		for (index = 0; index < NUM_ADDRESSBOOK_ENTRIES; index++) {
			Com_sprintf(buffer, sizeof(buffer), "adr%d", index);
			Cvar_Set(buffer, s_addressbook_fields[index].buffer);
		}
	}
	return Default_MenuKey(&s_addressbook_menu, key);
}

void AddressBook_MenuDraw(void) {
	M_Banner(i_banner_addressbook);
	Menu_Draw(&s_addressbook_menu);
}

void M_Menu_AddressBook_f(void) {
	AddressBook_MenuInit();
	M_PushMenu(AddressBook_MenuDraw, AddressBook_MenuKey);
}

/*
=============================================================================

PLAYER CONFIG MENU

=============================================================================
*/
static menuframework_s s_player_config_menu;
static menufield_s s_player_name_field;
static menulist_s s_player_model_box;
static menulist_s s_player_skin_box;
static menulist_s s_player_handedness_box;
static menulist_s s_player_rate_box;
static menuseparator_s s_player_skin_title;
static menuseparator_s s_player_model_title;
static menuseparator_s s_player_hand_title;
static menuseparator_s s_player_rate_title;
static menuaction_s s_player_download_action;

#define MAX_DISPLAYNAME 16
#define MAX_PLAYERMODELS 1024

typedef struct {
	int nskins;
	char **skindisplaynames;
	char displayname[MAX_DISPLAYNAME];
	char directory[MAX_QPATH];
} playermodelinfo_s;

static playermodelinfo_s s_pmi[MAX_PLAYERMODELS];
static char *s_pmnames[MAX_PLAYERMODELS];
static int s_numplayermodels;

static int rate_tbl[] = { 2500, 3200, 5000, 10000, 25000, 0 };
static char *rate_names[] =
{ "28.8 Modem", "33.6 Modem", "Single ISDN",
"Dual ISDN/Cable", "T1/LAN", "User defined", 0
};

void DownloadOptionsFunc(void *self) {
	M_Menu_DownloadOptions_f();
}

static void HandednessCallback(void *unused) {
	Cvar_SetValue("hand", s_player_handedness_box.curvalue);
}

static void RateCallback(void *unused) {
	if (s_player_rate_box.curvalue !=
		sizeof(rate_tbl) / sizeof(*rate_tbl) - 1)
		Cvar_SetValue("rate", rate_tbl[s_player_rate_box.curvalue]);
}

static void ModelCallback(void *unused) {
	s_player_skin_box.itemnames =
		s_pmi[s_player_model_box.curvalue].skindisplaynames;
	s_player_skin_box.curvalue = 0;
	currentPlayerWeapon = NULL;
}

static qboolean IconOfSkinExists(char *skin, char **pcxfiles,
	int npcxfiles) {
	int i;
	char scratch[1024];

	strcpy(scratch, skin);
	*strrchr(scratch, '.') = 0;
	strcat(scratch, "_i.pcx");

	for (i = 0; i < npcxfiles; i++) {
		if (strcmp(pcxfiles[i], scratch) == 0)
			return qtrue;
	}

	return qfalse;
}

static qboolean
PlayerConfig_ScanDirectories(void) {
	char		path[MAX_OSPATH];
	char           *ptr;
	char          **dirnames;
	char          **pcxnames;
	char          **skinnames;
	int		i, j, k;
	int		ndirs, npms;
	int		npcxfiles;
	int		nskins;

	s_numplayermodels = 0;
	ndirs = npms = 0;

	/* Get a list of directories. */
	dirnames = FS_ListFilesAll("players/*", &ndirs, SFF_SUBDIR, 0);

	if (dirnames == NULL)
		return (qfalse);

	/* Go through the subdirectories. */
	npms = ndirs;
	if (npms > MAX_PLAYERMODELS)
		npms = MAX_PLAYERMODELS;

	for (i = 0; i < npms; i++) {
		if (*dirnames[i] == '\0')
			continue;

		/* Verify the existence of tris.md2. */
		if (!FS_FileExists(va("%s/tris.md2", dirnames[i]))) {
			*dirnames[i] = '\0';
			continue;
		}

		/* Verify the existence of at least one pcx skin. */
		pcxnames = FS_ListFilesAll(va("%s/*.pcx", dirnames[i]),
			&npcxfiles, 0, SFF_SUBDIR | SFF_HIDDEN | SFF_SYSTEM);

		if (pcxnames == NULL) {
			*dirnames[i] = '\0';
			continue;
		}

		/*
		* Count valid skins, which consist of a skin with a matching
		* "_i" icon.
		*/
		for (j = 0, nskins = 0; j < npcxfiles; j++)
			if (strstr(pcxnames[j], "_i.pcx") == 0 &&
				IconOfSkinExists(pcxnames[j], pcxnames, npcxfiles))
				nskins++;

		if (nskins == 0)
			continue;

		skinnames = malloc(sizeof(char *)* (nskins + 1));
		memset(skinnames, 0, sizeof(char *)* (nskins + 1));

		/* Copy the valid skins. */
		for (k = 0, j = 0; j < npcxfiles; j++) {
			if (strstr(pcxnames[j], "_i.pcx") == 0 &&
				IconOfSkinExists(pcxnames[j], pcxnames, npcxfiles)) {
				COM_FileBase(pcxnames[j], path);
				skinnames[k++] = strdup(path);
			}
		}

		/* At this point we have a valid player model. */
		s_pmi[s_numplayermodels].nskins = nskins;
		s_pmi[s_numplayermodels].skindisplaynames = skinnames;

		/* Make short name for the model. */
		ptr = strrchr(dirnames[i], '/');
		Q_strncpyz(s_pmi[s_numplayermodels].displayname, ptr + 1, MAX_DISPLAYNAME);
		Q_strncpyz(s_pmi[s_numplayermodels].directory, ptr + 1, MAX_QPATH);
		s_numplayermodels++;

		FS_FreeList(pcxnames, npcxfiles);
	}

	FS_FreeList(dirnames, ndirs);

	return (qtrue);
}

static int pmicmpfnc(const void *_a, const void *_b) {
	const playermodelinfo_s *a = (const playermodelinfo_s *)_a;
	const playermodelinfo_s *b = (const playermodelinfo_s *)_b;

	/*
	** sort by male, female, then alphabetical
	*/
	if (strcmp(a->directory, "male") == 0)
		return -1;
	else if (strcmp(b->directory, "male") == 0)
		return 1;

	if (strcmp(a->directory, "female") == 0)
		return -1;
	else if (strcmp(b->directory, "female") == 0)
		return 1;

	return strcmp(a->directory, b->directory);
}


qboolean PlayerConfig_MenuInit(void) {
	extern cvar_t *name;
	extern cvar_t *skin;
	char currentdirectory[1024];
	char currentskin[1024];
	int i = 0;
	int offcet = 0;
	int currentdirectoryindex = 0;
	int currentskinindex = 0;

	cvar_t *hand = Cvar_Get("hand", "0", CVAR_USERINFO | CVAR_ARCHIVE);

	static char *handedness[] = { "right", "left", "center", 0 };

	PlayerConfig_ScanDirectories();

	offcet = (cl_fontScale->value - 1) * 16;

	if (s_numplayermodels == 0)
		return qfalse;

	if (hand->value < 0 || hand->value > 2)
		Cvar_SetValue("hand", 0);

	strcpy(currentdirectory, skin->string);

	if (strchr(currentdirectory, '/')) {
		strcpy(currentskin, strchr(currentdirectory, '/') + 1);
		*strchr(currentdirectory, '/') = 0;
	}
	else if (strchr(currentdirectory, '\\')) {
		strcpy(currentskin, strchr(currentdirectory, '\\') + 1);
		*strchr(currentdirectory, '\\') = 0;
	}
	else {
		strcpy(currentdirectory, "male");
		strcpy(currentskin, "grunt");
	}

	qsort(s_pmi, s_numplayermodels, sizeof(s_pmi[0]), pmicmpfnc);

	memset(s_pmnames, 0, sizeof(s_pmnames));
	for (i = 0; i < s_numplayermodels; i++) {
		s_pmnames[i] = s_pmi[i].displayname;
		if (Q_stricmp(s_pmi[i].directory, currentdirectory) == 0) {
			int j;

			currentdirectoryindex = i;

			for (j = 0; j < s_pmi[i].nskins; j++) {
				if (Q_stricmp(s_pmi[i].skindisplaynames[j], currentskin) ==
					0) {
					currentskinindex = j;
					break;
				}
			}
		}
	}
	
	drawIDlogo = qfalse;

	s_player_config_menu.x = viddef.width / 2 - 95 * cl_fontScale->value; //3
	s_player_config_menu.y = viddef.height / 2 - 97 * cl_fontScale->value; //1
	s_player_config_menu.nitems = 0;

	s_player_name_field.generic.type = MTYPE_FIELD;
	s_player_name_field.generic.name = "name";
	s_player_name_field.generic.callback = 0;
	s_player_name_field.generic.x = 0;
	s_player_name_field.generic.y = 0;
	s_player_name_field.length = 26;
	s_player_name_field.visible_length = 26;
	strcpy(s_player_name_field.buffer, name->string);
	s_player_name_field.cursor = strlen(name->string);

	s_player_model_title.generic.type = MTYPE_SEPARATOR;
	s_player_model_title.generic.name = "model";
	s_player_model_title.generic.x = -8 * cl_fontScale->value;
	s_player_model_title.generic.y = 60 * cl_fontScale->value;

	s_player_model_box.generic.type = MTYPE_SPINCONTROL;
	s_player_model_box.generic.x = (-56 * cl_fontScale->value) + offcet;
	s_player_model_box.generic.y = 70 * cl_fontScale->value;
	s_player_model_box.generic.callback = ModelCallback;
	s_player_model_box.generic.cursor_offset = -48;
	s_player_model_box.curvalue = currentdirectoryindex;
	s_player_model_box.itemnames = s_pmnames;

	s_player_skin_title.generic.type = MTYPE_SEPARATOR;
	s_player_skin_title.generic.name = "skin";
	s_player_skin_title.generic.x = -16 * cl_fontScale->value;
	s_player_skin_title.generic.y = 84 * cl_fontScale->value;

	s_player_skin_box.generic.type = MTYPE_SPINCONTROL;
	s_player_skin_box.generic.x = (-56 * cl_fontScale->value) + offcet;
	s_player_skin_box.generic.y = 94 * cl_fontScale->value;
	s_player_skin_box.generic.name = 0;
	s_player_skin_box.generic.callback = 0;
	s_player_skin_box.generic.cursor_offset = -48;
	s_player_skin_box.curvalue = currentskinindex;
	s_player_skin_box.itemnames =
		s_pmi[currentdirectoryindex].skindisplaynames;

	s_player_hand_title.generic.type = MTYPE_SEPARATOR;
	s_player_hand_title.generic.name = "handedness";
	s_player_hand_title.generic.x = 32 * cl_fontScale->value;
	s_player_hand_title.generic.y = 108 * cl_fontScale->value;

	s_player_handedness_box.generic.type = MTYPE_SPINCONTROL;
	s_player_handedness_box.generic.x = (-56 * cl_fontScale->value) + offcet;
	s_player_handedness_box.generic.y = 118 * cl_fontScale->value;
	s_player_handedness_box.generic.name = 0;
	s_player_handedness_box.generic.cursor_offset = -48;
	s_player_handedness_box.generic.callback = HandednessCallback;
	s_player_handedness_box.curvalue = Cvar_VariableValue("hand");
	s_player_handedness_box.itemnames = handedness;

	for (i = 0; i < sizeof(rate_tbl) / sizeof(*rate_tbl) - 1; i++)
		if (Cvar_VariableInteger("rate") == rate_tbl[i])
			break;

	s_player_rate_title.generic.type = MTYPE_SEPARATOR;
	s_player_rate_title.generic.name = "connect speed";
	s_player_rate_title.generic.x = 56 * cl_fontScale->value;
	s_player_rate_title.generic.y = 156 * cl_fontScale->value;

	s_player_rate_box.generic.type = MTYPE_SPINCONTROL;
	s_player_rate_box.generic.x = (-56 * cl_fontScale->value) + offcet;
	s_player_rate_box.generic.y = 166 * cl_fontScale->value;
	s_player_rate_box.generic.name = 0;
	s_player_rate_box.generic.cursor_offset = -48;
	s_player_rate_box.generic.callback = RateCallback;
	s_player_rate_box.curvalue = i;
	s_player_rate_box.itemnames = rate_names;

	s_player_download_action.generic.type = MTYPE_ACTION;
	s_player_download_action.generic.name = "download options";
	s_player_download_action.generic.flags = QMF_LEFT_JUSTIFY;
	s_player_download_action.generic.x = -24 * cl_fontScale->value;
	s_player_download_action.generic.y = 186 * cl_fontScale->value;
	s_player_download_action.generic.statusbar = NULL;
	s_player_download_action.generic.callback = DownloadOptionsFunc;

	Menu_AddItem(&s_player_config_menu, &s_player_name_field);
	Menu_AddItem(&s_player_config_menu, &s_player_model_title);
	Menu_AddItem(&s_player_config_menu, &s_player_model_box);
	if (s_player_skin_box.itemnames) {
		Menu_AddItem(&s_player_config_menu, &s_player_skin_title);
		Menu_AddItem(&s_player_config_menu, &s_player_skin_box);
	}
	Menu_AddItem(&s_player_config_menu, &s_player_hand_title);
	Menu_AddItem(&s_player_config_menu, &s_player_handedness_box);
	Menu_AddItem(&s_player_config_menu, &s_player_rate_title);
	Menu_AddItem(&s_player_config_menu, &s_player_rate_box);
	Menu_AddItem(&s_player_config_menu, &s_player_download_action);

	return qtrue;
}

#include "m_frames.h"
#include <time.h>
int	pose = 8;//0;
const char *pose_names[] = { "stand", "run", "attack", "pain1", "pain2", "pain3", "jump", "flip", "salute", "taunt", "wave", "point", "crouch stand", "crouch walk", "crouch attack", "crouch pain", 0 };
int	pose_start[] = { PLAYER_FRAME_stand01, PLAYER_FRAME_run1, PLAYER_FRAME_attack1, PLAYER_FRAME_pain101, PLAYER_FRAME_pain201, PLAYER_FRAME_pain301, PLAYER_FRAME_jump1, PLAYER_FRAME_flip01, PLAYER_FRAME_salute01, PLAYER_FRAME_taunt01, PLAYER_FRAME_wave01, PLAYER_FRAME_point01, PLAYER_FRAME_crstnd01, PLAYER_FRAME_crwalk1, PLAYER_FRAME_crattak1, PLAYER_FRAME_crpain1 };
int	pose_end[] = { PLAYER_FRAME_stand40, PLAYER_FRAME_run6, PLAYER_FRAME_attack8, PLAYER_FRAME_pain104, PLAYER_FRAME_pain204, PLAYER_FRAME_pain304, PLAYER_FRAME_jump6, PLAYER_FRAME_flip12, PLAYER_FRAME_salute11, PLAYER_FRAME_taunt17, PLAYER_FRAME_wave11, PLAYER_FRAME_point12, PLAYER_FRAME_crstnd19, PLAYER_FRAME_crwalk6, PLAYER_FRAME_crattak9, PLAYER_FRAME_crpain4 };
int	pose_rot = 7;
const char *pose_rot_names[] = { "0 degrees", "30 degrees", "60 degrees", "90 degrees", "120 degrees", "150 degrees", "180 degrees", "210 degrees", "240 degrees", "270 degrees", "300 degrees", "330 degrees", "auto rotate", 0 };
int	pose_rot_angle[] = { 0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, -1 };

struct image_s *R_RegisterPlayerBump(char *name);



void PlayerConfig_MenuDraw(void) {
	extern float CalcFov(float fov_x, float w, float h);
	refdef_t refdef;
	char scratch[MAX_QPATH];
	int x2, y2;

	srand(time(0));

	memset(&refdef, 0, sizeof(refdef));

	refdef.x = viddef.width / 2;
	x2 = 320 * (cl_fontScale->value - 1);

	refdef.y = (viddef.height / 2) - (350 * cl_fontScale->value) / 2;
	y2 = viddef.height / 2 - 72 * cl_fontScale->value;

	refdef.width = 270 * cl_fontScale->value;
	refdef.height = 350 * cl_fontScale->value;
	refdef.fov_x = 40;
	refdef.fov_y = CalcFov(refdef.fov_x, refdef.width, refdef.height);
	refdef.time = cls.realtime * 0.001;

	pose = rand() % 16;

	if (s_pmi[s_player_model_box.curvalue].skindisplaynames) {
		entity_t entity[2];
		refdef.num_entities = 1;

		// player model
		memset(&entity[0], 0, sizeof(entity[0]));

		Com_sprintf(scratch, sizeof(scratch), "players/%s/tris.md2",
			s_pmi[s_player_model_box.curvalue].directory);
		entity[0].model = R_RegisterModel(scratch);
		Com_sprintf(scratch, sizeof(scratch), "players/%s/%s.pcx",
			s_pmi[s_player_model_box.curvalue].directory,
			s_pmi[s_player_model_box.curvalue].
			skindisplaynames[s_player_skin_box.curvalue]);

		entity[0].skin = R_RegisterSkin(scratch);
		entity[0].bump = R_RegisterPlayerBump(scratch);
		entity[0].origin[0] = 90;
		entity[0].origin[1] = 0;
		entity[0].origin[2] = -8;
		entity[0].flags = RF_NOSHADOW | RF_DEPTHHACK;

		VectorCopy(entity[0].origin, entity[0].oldorigin);

		if (pose_rot_angle[pose_rot] == -1) {
			entity[0].angles[1] = anglemod(cls.realtime >> 4);
			if (entity[0].angles[1] > 360)
				entity[0].angles[1] -= 360;
		}
		else
			entity[0].angles[1] = pose_rot_angle[pose_rot];

		entity[0].frame = pose_start[pose] + ((cls.realtime >> 8) % (pose_end[pose] - pose_start[pose] + 1));
		entity[0].oldframe = pose_start[pose] + (((cls.realtime >> 8) - 1) % (pose_end[pose] - pose_start[pose] + 1));
		entity[0].backlerp = 1.0 - ((float)(cls.realtime & 0xff)) / 255.0;

		// player weapon model

		memset(&entity[1], 0, sizeof(entity[1]));

		if (currentPlayerWeapon) {
			entity[1].model = currentPlayerWeapon;
			entity[1].skin = currentPlayerWeapon->skins[0];
			entity[1].bump = currentPlayerWeapon->skins_normal[0];
		}
		else {
			Com_sprintf(scratch, sizeof(scratch), "players/%s/w_sshotgun.md2", s_pmi[s_player_model_box.curvalue].directory); //force default player weapon
			entity[1].model = R_RegisterModel(scratch);
			entity[1].skin = R_RegisterSkin(scratch);
			entity[1].bump = R_RegisterPlayerBump(scratch);
		}

		if (entity[1].model) {

			entity[1].origin[0] = 90;
			entity[1].origin[1] = 0;
			entity[1].origin[2] = -8;

			VectorCopy(entity[1].origin, entity[1].oldorigin);

			entity[1].angles[1] = entity[0].angles[1];
			entity[1].frame = entity[0].frame;
			entity[1].oldframe = entity[0].oldframe;
			entity[1].backlerp = entity[0].backlerp;
			entity[1].flags = RF_NOSHADOW | RF_DEPTHHACK;
			refdef.num_entities++;
		}

		refdef.areabits = 0;
		refdef.entities = &entity[0];
		refdef.rdflags = RDF_NOWORLDMODEL | RDF_NOCLEAR;

		Menu_Draw(&s_player_config_menu);

		R_RenderFrame(&refdef);

		Com_sprintf(scratch, sizeof(scratch), "/players/%s/%s_i.pcx",
					s_pmi[s_player_model_box.curvalue].directory,
					s_pmi[s_player_model_box.curvalue].
					skindisplaynames[s_player_skin_box.curvalue]);

		Draw_PicScaled(s_player_config_menu.x - 40 * cl_fontScale->value, y2, cl_fontScale->value, cl_fontScale->value, scratch);

	}
}

int PlayerConfig_MenuKey(int key) {
	int i;

	if (key == K_ESCAPE) {
		char scratch[1024];

		Cvar_Set("name", s_player_name_field.buffer);

		Com_sprintf(scratch, sizeof(scratch), "%s/%s",
			s_pmi[s_player_model_box.curvalue].directory,
			s_pmi[s_player_model_box.curvalue].
			skindisplaynames[s_player_skin_box.curvalue]);

		Cvar_Set("skin", scratch);

		for (i = 0; i < s_numplayermodels; i++) {
			int j;

			for (j = 0; j < s_pmi[i].nskins; j++) {
				if (s_pmi[i].skindisplaynames[j])
					free(s_pmi[i].skindisplaynames[j]);
				s_pmi[i].skindisplaynames[j] = 0;
			}
			free(s_pmi[i].skindisplaynames);
			s_pmi[i].skindisplaynames = 0;
			s_pmi[i].nskins = 0;
		}
	}
	return Default_MenuKey(&s_player_config_menu, key);
}


void M_Menu_PlayerConfig_f(void) {
	if (!PlayerConfig_MenuInit()) {
		Menu_SetStatusBar(&s_multiplayer_menu,
			"No valid player models found");
		return;
	}
	Menu_SetStatusBar(&s_multiplayer_menu, NULL);
	M_PushMenu(PlayerConfig_MenuDraw, PlayerConfig_MenuKey);
}


/*
=======================================================================

GALLERY MENU

=======================================================================
*/
#if 0
void M_Menu_Gallery_f(void) {
	extern void Gallery_MenuDraw(void);
	extern const char *Gallery_MenuKey(int key);

	M_PushMenu(Gallery_MenuDraw, Gallery_MenuKey);
}
#endif

/*
=======================================================================

QUIT MENU

=======================================================================
*/

int M_Quit_Key(int key) {
	switch (key) {
	case K_ESCAPE:
	case 'n':
	case 'N':
	case K_XPAD_B: // ?????????????????? ?????????????????? ?????? ???????????????????????? "B"
		M_PopMenu();
		break;

	case 'Y':
	case 'y':
	case K_XPAD_A: // ??????????????????????????????????????? ?????????????????? ?????? ???????????????????????? "A"
		cls.key_dest = key_console;
		CL_Quit_f();
		break;

	default:
		break;
	}

	return 0;
}

void M_Quit_Draw(void) {
	int w = 320;
	int h = 240;
	drawIDlogo = qfalse;
//	Draw_GetPicSize(&w, &h, "quit");
	Draw_ScaledPic((viddef.width - w * cl_fontScale->value) / 2, (viddef.height - h * cl_fontScale->value) / 2, cl_fontScale->value, cl_fontScale->value, i_quit[0]);
	Draw_ScaledBumpPic((viddef.width - w * cl_fontScale->value) / 2, (viddef.height - h * cl_fontScale->value) / 2, cl_fontScale->value, cl_fontScale->value, i_quit[0], i_quit[1]);

}


void M_Menu_Quit_f(void) {
	M_PushMenu(M_Quit_Draw, M_Quit_Key);
}



//=============================================================================
/* Menu Subsystem */


/*
=================
M_Init
=================
*/
void M_Init(void) {
	Cmd_AddCommand("menu_main", M_Menu_Main_f);
	Cmd_AddCommand("menu_game", M_Menu_Game_f);
	Cmd_AddCommand("menu_loadgame", M_Menu_LoadGame_f);
	Cmd_AddCommand("menu_savegame", M_Menu_SaveGame_f);
	Cmd_AddCommand("menu_joinserver", M_Menu_JoinServer_f);
	Cmd_AddCommand("menu_addressbook", M_Menu_AddressBook_f);
	Cmd_AddCommand("menu_startserver", M_Menu_StartServer_f);
	Cmd_AddCommand("menu_dmoptions", M_Menu_DMOptions_f);
	Cmd_AddCommand("menu_playerconfig", M_Menu_PlayerConfig_f);
	Cmd_AddCommand("menu_downloadoptions", M_Menu_DownloadOptions_f);
	Cmd_AddCommand("menu_credits", M_Menu_Credits_f);
	Cmd_AddCommand("menu_multiplayer", M_Menu_Multiplayer_f);
	Cmd_AddCommand("menu_video", M_Menu_Video_f);
	Cmd_AddCommand("menu_options", M_Menu_Options_f);
	Cmd_AddCommand("menu_keys", M_Menu_Keys_f);
	Cmd_AddCommand("menu_advanced", M_Menu_Advanced_f);
	Cmd_AddCommand("menu_quit", M_Menu_Quit_f);
}

void SCR_TileClear(void);

void M_DrawBackgroundModel() {
	refdef_t	refdef;
	vec3_t		center, rad;
	entity_t	entity;
	int			pos;

	if (!drawIDlogo)
		return;

	memset(&refdef, 0, sizeof(refdef));
	memset(&entity, 0, sizeof(entity));

	entity.model = menumodel.menuLogo;

	R_ModelRadius(entity.model, rad);
	R_ModelCenter(entity.model, center);

	pos = (viddef.width - viddef.height) / 2;
	refdef.x = pos;
	refdef.y = 0;
	refdef.width = viddef.height;
	refdef.height = viddef.height;
	refdef.fov_x = 45;
	refdef.fov_y = 45;
	refdef.time = cls.realtime;
	refdef.viewangles[0] = 30;
	refdef.areabits = 0;
	refdef.num_entities = 1;
	refdef.entities = &entity;
	refdef.lightstyles = 0;
	refdef.rdflags = RDF_NOWORLDMODEL | RDF_NOCLEAR | RDF_NOWORLDMODEL2;
	VectorSet(refdef.vieworg, -rad[0] * 1.5, 0, rad[0] * 0.8);

	entity.flags = RF_NOSHADOW  | RF_DEPTHHACK;
	entity.frame = 0;
	entity.oldframe = 0;
	entity.backlerp = 0.0;

	entity.angles[1] = anglemod(cl.time / 32);
	entity.angleMod = qtrue;

	VectorNegate(center, entity.origin);

	// Draw it
	R_RenderFrame(&refdef);
	refdef.num_entities++;
}

/*
=================
M_Draw
=================
*/
void M_Draw(void) {
	if (cls.key_dest != key_menu)
		return;

	// repaint everything next frame
	SCR_DirtyScreen();	
	
	if (cls.state != ca_active || !cl.refresh_prepped) {
		Draw_StretchPic2(0, 0, viddef.width, viddef.height, i_menuBackground);
		
		if (!drawIDlogo)
			R_MenuBackGround();

		M_DrawBackgroundModel();
	} 
	else
		R_MenuBackGround();
	m_drawfunc();

	// delay playing the enter sound until after the
	// menu has been drawn, to avoid delay while
	// caching images
	if (m_entersound) {
		S_StartLocalSound(fastsound_descriptor[menu_in_sound]);
		m_entersound = qfalse;
	}
}


/*
=================
M_Keydown
=================
*/
void M_Keydown(int key) {
	if (m_keyfunc) {
		int s = m_keyfunc(key);
		if (s == menu_in_sound)
			S_StartLocalSound(fastsound_descriptor[s]);
		if (s == menu_move_sound)
			S_StartLocalSound(fastsound_descriptor[s]);
		if (s == menu_out_sound)
			S_StartLocalSound(fastsound_descriptor[s]);
	}
}
