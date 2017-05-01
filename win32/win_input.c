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
// in_win.c -- windows 95 mouse and joystick code
// 02/21/97 JCB Added extended DirectInput code to support external controllers.

#include "../client/client.h"
#include "winquake.h"

extern	unsigned	sys_msg_time;

cvar_t	*m_inversion;
cvar_t	*v_centermove;
cvar_t	*v_centerspeed;
cvar_t	*in_useJoystic;

qboolean	in_appactive;

// joystick defines and variables
// where should defines be moved?
#define JOY_ABSOLUTE_AXIS	0x00000000		// control like a joystick
#define JOY_RELATIVE_AXIS	0x00000010		// control like a mouse, spinner, trackball
#define	JOY_MAX_AXES		6				// X, Y, Z, R, U, V
#define JOY_AXIS_X			0
#define JOY_AXIS_Y			1
#define JOY_AXIS_Z			2
#define JOY_AXIS_R			3
#define JOY_AXIS_U			4
#define JOY_AXIS_V			5

enum _ControlList
{
	AxisNada = 0, AxisForward, AxisLook, AxisSide, AxisTurn, AxisUp
};

DWORD	dwAxisFlags[JOY_MAX_AXES] =
{
	JOY_RETURNX, JOY_RETURNY, JOY_RETURNZ, JOY_RETURNR, JOY_RETURNU, JOY_RETURNV
};

DWORD	dwAxisMap[JOY_MAX_AXES];
DWORD	dwControlMap[JOY_MAX_AXES];
PDWORD	pdwRawValue[JOY_MAX_AXES];

// none of these cvars are saved over a session
// this means that advanced controller configuration needs to be executed
// each time.  this avoids any problems with getting back to a default usage
// or when changing from one controller to another.  this way at least something
// works.
cvar_t	*joy_name;
cvar_t	*joy_advanced;
cvar_t	*joy_advaxisx;
cvar_t	*joy_advaxisy;
cvar_t	*joy_advaxisz;
cvar_t	*joy_advaxisr;
cvar_t	*joy_advaxisu;
cvar_t	*joy_advaxisv;
cvar_t	*joy_forwardthreshold;
cvar_t	*joy_sidethreshold;
cvar_t	*joy_pitchthreshold;
cvar_t	*joy_yawthreshold;
cvar_t	*joy_forwardsensitivity;
cvar_t	*joy_sidesensitivity;
cvar_t	*joy_pitchsensitivity;
cvar_t	*joy_yawsensitivity;
cvar_t	*joy_upthreshold;
cvar_t	*joy_upsensitivity;

qboolean	joy_avail, joy_advancedinit, joy_haspov;
DWORD		joy_oldbuttonstate, joy_oldpovstate;

int			joy_id;
DWORD		joy_flags;
DWORD		joy_numbuttons;

static JOYINFOEX	ji;

// forward-referenced functions
void IN_StartupJoystick(void);
void Joy_AdvancedUpdate_f(void);
void IN_JoyMove(usercmd_t *cmd);


// ======== xbox360 controller============
#include "xinput.h"

int xiActiveController = -1;
qboolean xiActive = qfalse;
int xi_oldDpadState = 0;
int xi_oldButtonState = 0;
void IN_StartupXInput(void);
void IN_ToggleXInput(cvar_t *var);
void IN_ControllerMove(usercmd_t *cmd);

cvar_t *xi_axisLx;
cvar_t *xi_axisLy;

cvar_t *xi_axisRx;
cvar_t *xi_axisRy;

cvar_t *xi_axisLt;
cvar_t *xi_axisRt;

cvar_t *xi_dpadArrowMap;
cvar_t *xi_useController;

// set cvars to these values to decode the action for a given axis
// if these are changed the menu strings in menu_other.cpp should also be changed!!!
#define XI_AXIS_NONE		0
#define XI_AXIS_LOOK		1
#define XI_AXIS_MOVE		2
#define XI_AXIS_TURN		3
#define XI_AXIS_STRAFE		4
#define XI_AXIS_INVLOOK		5
#define XI_AXIS_INVMOVE		6
#define XI_AXIS_INVTURN		7
#define XI_AXIS_INVSTRAFE	8

/*
============================================================

MOUSE CONTROL

============================================================
*/

qboolean	mlooking;

void IN_MLookDown (void) {
	mlooking = qtrue;
}

void IN_MLookUp (void) {
	mlooking = qfalse;
	if (!freelook->value && lookspring->value)
		IN_CenterView ();
}

int			mouse_buttons;
int			mouse_oldbuttonstate;
POINT		current_pos;
int			mouse_x, mouse_y, old_mouse_x, old_mouse_y, mx_accum, my_accum;

int			old_x, old_y;

qboolean	mouseactive;	// qfalse when not focus app

qboolean	restore_spi;
qboolean	mouseinitialized;
qboolean	mouseparmsvalid;

int			originalmouseparms[3], newmouseparms[3] = { 0, 0, 1 };
int			window_center_x, window_center_y;
RECT		window_rect;


/*
===========
IN_ActivateMouse

Called when the window gains focus or changes in some way
===========
*/
void IN_ActivateMouse (void) {
	int		width, height;

	if (mouseactive)
		return;

	/// Berserker's fix
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	GetWindowRect(cl_hwnd, &window_rect);
	if (window_rect.left < 0)
		window_rect.left = 0;
	if (window_rect.top < 0)
		window_rect.top = 0;
	if (window_rect.right >= width)
		window_rect.right = width - 1;
	if (window_rect.bottom >= height - 1)
		window_rect.bottom = height - 1;

	window_center_x = (window_rect.right + window_rect.left) * 0.5;
	window_center_y = (window_rect.top + window_rect.bottom) * 0.5;

	old_x = window_center_x;
	old_y = window_center_y;

	SetCursorPos(window_center_x, window_center_y);
	SetCapture(cl_hwnd);
	ClipCursor(&window_rect);

	mouseactive = qtrue;
	while (ShowCursor(FALSE) >= 0);
}


/*
===========
IN_DeactivateMouse

Called when the window loses focus
===========
*/
void IN_DeactivateMouse (void) {
	
	if (!mouseactive)
		return;

	ClipCursor (NULL);
	ReleaseCapture ();

	mouseactive = qfalse;
	
	while (ShowCursor(TRUE) < 0);
}



/*
===========
IN_StartupMouse
===========
*/
void IN_StartupMouse (void) {
	cvar_t		*cv;

	cv = Cvar_Get ("in_initmouse", "1", CVAR_NOSET);

	if (!cv->value)
		return;

	mouseinitialized = qtrue;
	mouseparmsvalid = SystemParametersInfo (SPI_GETMOUSE, 0, originalmouseparms, 0);
	mouse_buttons = 5;
}

/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent (int mstate) {
	int		i;

	if (!mouseinitialized)
		return;

	// perform button actions
	for (i = 0; i < mouse_buttons; i++) {
		if ((mstate & (1 << i)) &&
			!(mouse_oldbuttonstate & (1 << i))) {
			Key_Event (K_MOUSE1 + i, qtrue, sys_msg_time);
		}

		if (!(mstate & (1 << i)) &&
			(mouse_oldbuttonstate & (1 << i))) {
			Key_Event (K_MOUSE1 + i, qfalse, sys_msg_time);
		}
	}

	mouse_oldbuttonstate = mstate;
}


/*
===========
IN_MouseMove
===========
*/

qboolean FindRawDevices()
{
	PRAWINPUTDEVICELIST g_pRawInputDeviceList;
	UINT nDevices;

	Com_Printf("====== Init RAW Input Devices ======\n\n");

	// Get Number of devices attached
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
	{
		Com_Printf("\n" S_COLOR_RED "No RawInput devices attached\n");
		return qfalse;
	}
	else
		Com_Printf("" S_COLOR_YELLOW "... Found " S_COLOR_GREEN "%i" S_COLOR_YELLOW " RAW input devices.\n", nDevices);

	// Create list large enough to hold all RAWINPUTDEVICE structs
	if ((g_pRawInputDeviceList = (PRAWINPUTDEVICELIST)Z_Malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL)
	{
		Com_Printf("" S_COLOR_RED "Error mallocing RAWINPUTDEVICELIST\n");
		return qfalse;
	}
	// Now get the data on the attached devices
	if (GetRawInputDeviceList(g_pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1)
	{
		Com_Printf("" S_COLOR_RED "1Error from GetRawInputDeviceList\n");
		Z_Free(g_pRawInputDeviceList);
		return qfalse;
	}

	PRAWINPUTDEVICE g_pRawInputDevices = (PRAWINPUTDEVICE)Z_Malloc(nDevices * sizeof(RAWINPUTDEVICE));

	for (UINT i = 0; i<nDevices; i++)
	{
		if (g_pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			UINT nchars = 300;
			TCHAR deviceName[300];
			if (GetRawInputDeviceInfo(g_pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, deviceName, &nchars) >= 0)
				Com_DPrintf("Device[%d]:\n handle=0x%x\n name = %s\n\n", i, g_pRawInputDeviceList[i].hDevice, deviceName);
			RID_DEVICE_INFO dinfo;
			UINT sizeofdinfo = sizeof(dinfo);
			dinfo.cbSize = sizeofdinfo;
			if (GetRawInputDeviceInfo(g_pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &dinfo, &sizeofdinfo) >= 0)
			{
				if (dinfo.dwType == RIM_TYPEMOUSE)
				{
					RID_DEVICE_INFO_MOUSE *pMouseInfo = &dinfo.mouse;
					Com_DPrintf("ID = 0x%x\n", pMouseInfo->dwId);
					Com_DPrintf("Number of buttons = %i\n", pMouseInfo->dwNumberOfButtons);
					Com_DPrintf("Sample Rate = %i\n", pMouseInfo->dwSampleRate);
					Com_DPrintf("Has Horizontal Wheel: %s\n", pMouseInfo->fHasHorizontalWheel ? "Yes" : "No");
				}
			}
		}
	}
	Z_Free(g_pRawInputDevices);
	Z_Free(g_pRawInputDeviceList);

	Com_Printf("\n------------------------------------\n");
	
	return qtrue;
}

/*
===========
IN_Init
===========
*/
void IN_Init (void) {
	// mouse variables
	m_inversion = Cvar_Get ("m_inversion", "0", CVAR_ARCHIVE);

	// centering
	v_centermove = Cvar_Get ("v_centermove", "0.15", 0);
	v_centerspeed = Cvar_Get ("v_centerspeed", "500", 0);

	// joystick variables
	joy_name = Cvar_Get("joy_name", "joystick", 0);
	joy_advanced = Cvar_Get("joy_advanced", "0", 0);
	joy_advaxisx = Cvar_Get("joy_advaxisx", "0", 0);
	joy_advaxisy = Cvar_Get("joy_advaxisy", "0", 0);
	joy_advaxisz = Cvar_Get("joy_advaxisz", "0", 0);
	joy_advaxisr = Cvar_Get("joy_advaxisr", "0", 0);
	joy_advaxisu = Cvar_Get("joy_advaxisu", "0", 0);
	joy_advaxisv = Cvar_Get("joy_advaxisv", "0", 0);
	joy_forwardthreshold = Cvar_Get("joy_forwardthreshold", "0.15", 0);
	joy_sidethreshold = Cvar_Get("joy_sidethreshold", "0.15", 0);
	joy_upthreshold = Cvar_Get("joy_upthreshold", "0.15", 0);
	joy_pitchthreshold = Cvar_Get("joy_pitchthreshold", "0.15", 0);
	joy_yawthreshold = Cvar_Get("joy_yawthreshold", "0.15", 0);
	joy_forwardsensitivity = Cvar_Get("joy_forwardsensitivity", "-1", 0);
	joy_sidesensitivity = Cvar_Get("joy_sidesensitivity", "-1", 0);
	joy_upsensitivity = Cvar_Get("joy_upsensitivity", "-1", 0);
	joy_pitchsensitivity = Cvar_Get("joy_pitchsensitivity", "1", 0);
	joy_yawsensitivity = Cvar_Get("joy_yawsensitivity", "-1", 0);

	in_useJoystic = Cvar_Get("in_useJoystic", "0", CVAR_ARCHIVE);

	xi_axisLx = Cvar_Get("xi_axisLx", "3", CVAR_ARCHIVE);
	xi_axisLy = Cvar_Get("xi_axisLy", "2", CVAR_ARCHIVE);
	xi_axisRx = Cvar_Get("xi_axisRx", "3", CVAR_ARCHIVE);
	xi_axisRy = Cvar_Get("xi_axisRy", "1", CVAR_ARCHIVE);
	xi_axisLt = Cvar_Get("xi_axisLt", "8", CVAR_ARCHIVE);
	xi_axisRt = Cvar_Get("xi_axisRt", "4", CVAR_ARCHIVE);
	xi_dpadArrowMap = Cvar_Get("xi_dpadArrowMap", "1", CVAR_ARCHIVE);
	xi_useController = Cvar_Get("xi_useController", "1", CVAR_ARCHIVE);

	Cmd_AddCommand ("+mlook", IN_MLookDown);
	Cmd_AddCommand ("-mlook", IN_MLookUp);
	Cmd_AddCommand ("joy_advancedupdate", Joy_AdvancedUpdate_f);

	FindRawDevices();
	IN_StartupXInput();
	IN_StartupJoystick();
}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown (void) {
	IN_DeactivateMouse ();
}


/*
===========
IN_Activate

Called when the main window gains or loses focus.
The window may have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void IN_Activate (qboolean active) {
	in_appactive = active;
	mouseactive = !active;		// force a new window check or turn off
}


/*
==================
IN_Frame

Called every frame, even if not generating commands
==================
*/
void IN_Frame (void) {

	if (!in_appactive)
	{
		IN_DeactivateMouse();
		return;
	}

	if (!cl.refresh_prepped
		|| cls.key_dest == key_console
		|| cls.key_dest == key_menu) {
		// temporarily deactivate if in fullscreen
		if (Cvar_VariableValue ("r_fullScreen") == 0) {
			IN_DeactivateMouse ();
			return;
		}
	}

	IN_ActivateMouse ();
}

/*
===========
IN_Move
===========
*/
void IN_Move (usercmd_t *cmd) {
	
	if (xi_useController->modified) {
		xi_useController->modified = qfalse;
		IN_ToggleXInput(xi_useController);
	}

	if (ActiveApp)
		IN_JoyMove(cmd);
}


/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates (void) {
	mx_accum = 0;
	my_accum = 0;
	mouse_oldbuttonstate = 0;
}

/*
=========================================================================

JOYSTICK

=========================================================================
*/

/*
===============
IN_StartupJoystick
===============
*/
void IN_StartupJoystick(void)
{
	int			numdevs;
	JOYCAPS		jc;
	MMRESULT	mmr;
	cvar_t		*cv;

	// assume no joystick
	joy_avail = qfalse;

	// abort startup if user requests no joystick
	cv = Cvar_Get("in_initjoy", "1", CVAR_NOSET);
	if (!cv->value)
		return;
	
	Com_Printf("\n========== Init Joystick ==========\n\n");

	// verify joystick driver is present
	if ((numdevs = joyGetNumDevs()) == 0)
	{
		Com_Printf ("" S_COLOR_RED "joystick not found -- driver not present\n");
		return;
	}

	// cycle through the joystick ids for the first valid one
	for (joy_id = 0; joy_id<numdevs; joy_id++)
	{
		memset(&ji, 0, sizeof(ji));
		ji.dwSize = sizeof(ji);
		ji.dwFlags = JOY_RETURNCENTERED;

		if ((mmr = joyGetPosEx(joy_id, &ji)) == JOYERR_NOERROR)
			break;
	}

	// abort startup if we didn't find a valid joystick
	if (mmr != JOYERR_NOERROR)
	{
		Com_Printf("..." S_COLOR_YELLOW "Joystick Not Found " S_COLOR_WHITE "(%x)\n", mmr);
		Com_Printf("\n-----------------------------------\n\n");
		return;
	}

	// get the capabilities of the selected joystick
	// abort startup if command fails
	memset(&jc, 0, sizeof(jc));
	if ((mmr = joyGetDevCaps(joy_id, &jc, sizeof(jc))) != JOYERR_NOERROR)
	{
		Com_Printf("..." S_COLOR_RED "Invalid Joystick Capabilities (%x)\n", mmr);
		Com_Printf("\n-----------------------------------\n\n");
		return;
	}

	// save the joystick's number of buttons and POV status
	joy_numbuttons = jc.wNumButtons;
	joy_haspov = jc.wCaps & JOYCAPS_HASPOV;

	// old button and POV states default to no buttons pressed
	joy_oldbuttonstate = joy_oldpovstate = 0;

	// mark the joystick as available and advanced initialization not completed
	// this is needed as cvars are not available during initialization

	joy_avail = qtrue;
	joy_advancedinit = qfalse;

	Com_Printf("Found: " S_COLOR_GREEN "%s\n", jc.szPname);
	Com_Printf("Num Buttons: " S_COLOR_GREEN "%i / %i\n", jc.wNumButtons, jc.wMaxButtons);
	Com_Printf("Axis: " S_COLOR_GREEN "%i / %i\n", jc.wNumAxes, jc.wMaxAxes);
	Com_Printf("\n-----------------------------------\n\n");
}


/*
===========
RawValuePointer
===========
*/
PDWORD RawValuePointer(int axis)
{
	switch (axis)
	{
	case JOY_AXIS_X:
		return &ji.dwXpos;
	case JOY_AXIS_Y:
		return &ji.dwYpos;
	case JOY_AXIS_Z:
		return &ji.dwZpos;
	case JOY_AXIS_R:
		return &ji.dwRpos;
	case JOY_AXIS_U:
		return &ji.dwUpos;
	default:
	case JOY_AXIS_V:
		return &ji.dwVpos;
	}
}


/*
===========
Joy_AdvancedUpdate_f
===========
*/
void Joy_AdvancedUpdate_f(void)
{

	// called once by IN_ReadJoystick and by user whenever an update is needed
	// cvars are now available
	int	i;
	DWORD dwTemp;

	// initialize all the maps
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		dwAxisMap[i] = AxisNada;
		dwControlMap[i] = JOY_ABSOLUTE_AXIS;
		pdwRawValue[i] = RawValuePointer(i);
	}

	if (joy_advanced->value == 0.0)
	{
		// default joystick initialization
		// 2 axes only with joystick control
		dwAxisMap[JOY_AXIS_X] = AxisTurn;
		// dwControlMap[JOY_AXIS_X] = JOY_ABSOLUTE_AXIS;
		dwAxisMap[JOY_AXIS_Y] = AxisForward;
		// dwControlMap[JOY_AXIS_Y] = JOY_ABSOLUTE_AXIS;
	}
	else
	{
		if (strcmp(joy_name->string, "joystick") != 0)
		{
			// notify user of advanced controller
			Com_Printf("\n%s configured\n\n", joy_name->string);
		}

		// advanced initialization here
		// data supplied by user via joy_axisn cvars
		dwTemp = (DWORD)joy_advaxisx->value;
		dwAxisMap[JOY_AXIS_X] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_X] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisy->value;
		dwAxisMap[JOY_AXIS_Y] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Y] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisz->value;
		dwAxisMap[JOY_AXIS_Z] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Z] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisr->value;
		dwAxisMap[JOY_AXIS_R] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_R] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisu->value;
		dwAxisMap[JOY_AXIS_U] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_U] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisv->value;
		dwAxisMap[JOY_AXIS_V] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_V] = dwTemp & JOY_RELATIVE_AXIS;
	}

	// compute the axes to collect from DirectInput
	joy_flags = JOY_RETURNCENTERED | JOY_RETURNBUTTONS | JOY_RETURNPOV;
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		if (dwAxisMap[i] != AxisNada)
		{
			joy_flags |= dwAxisFlags[i];
		}
	}
}

void IN_Commands(void)
{
	int		i, key_index;
	DWORD	buttonstate, povstate;

	if (!joy_avail)
	{
		return;
	}

	// loop through the joystick buttons
	// key a joystick event or auxillary event for higher number buttons for each state change
	buttonstate = ji.dwButtons;
	for (i = 0; i < joy_numbuttons; i++)
	{
		if ((buttonstate & (1 << i)) && !(joy_oldbuttonstate & (1 << i)))
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event(key_index + i, qtrue, 0);
		}

		if (!(buttonstate & (1 << i)) && (joy_oldbuttonstate & (1 << i)))
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event(key_index + i, qfalse, 0);
		}
	}
	joy_oldbuttonstate = buttonstate;

	if (joy_haspov)
	{
		// convert POV information into 4 bits of state information
		// this avoids any potential problems related to moving from one
		// direction to another without going through the center position
		povstate = 0;
		if (ji.dwPOV != JOY_POVCENTERED)
		{
			if (ji.dwPOV == JOY_POVFORWARD)
				povstate |= 0x01;
			if (ji.dwPOV == JOY_POVRIGHT)
				povstate |= 0x02;
			if (ji.dwPOV == JOY_POVBACKWARD)
				povstate |= 0x04;
			if (ji.dwPOV == JOY_POVLEFT)
				povstate |= 0x08;
		}
		// determine which bits have changed and key an auxillary event for each change
		for (i = 0; i < 4; i++)
		{
			if ((povstate & (1 << i)) && !(joy_oldpovstate & (1 << i)))
			{
				Key_Event(K_AUX29 + i, qtrue, 0);
			}

			if (!(povstate & (1 << i)) && (joy_oldpovstate & (1 << i)))
			{
				Key_Event(K_AUX29 + i, qfalse, 0);
			}
		}
		joy_oldpovstate = povstate;
	}
}


/*
===============
IN_ReadJoystick
===============
*/
qboolean IN_ReadJoystick(void)
{

	memset(&ji, 0, sizeof(ji));
	ji.dwSize = sizeof(ji);
	ji.dwFlags = joy_flags;

	if (joyGetPosEx(joy_id, &ji) == JOYERR_NOERROR)
	{
		return qtrue;
	}
	else
	{
		// read error occurred
		// turning off the joystick seems too harsh for 1 read error,\
				// but what should be done?
// Com_Printf ("IN_ReadJoystick: no response\n");
// joy_avail = false;
		return qfalse;
	}
}


/*
===========
IN_JoyMove
===========
*/
void IN_JoyMove(usercmd_t *cmd)
{
	float	speed, aspeed;
	float	fAxisValue;
	int		i;

	if (xiActive) {
		IN_ControllerMove(cmd);
		return;
	}

	// complete initialization if first time in
	// this is needed as cvars are not available at initialization time
	if (joy_advancedinit != qtrue)
	{
		Joy_AdvancedUpdate_f();
		joy_advancedinit = qtrue;
	}

	// verify joystick is available and that the user wants to use it
	if (!joy_avail || !in_useJoystic->value)
	{
		return;
	}

	// collect the joystick data, if possible
	if (IN_ReadJoystick() != qtrue)
	{
		return;
	}

	if ((in_speed.state & 1) ^ (int)cl_run->value)
		speed = 2;
	else
		speed = 1;
	aspeed = speed * cls.frametime;

	// loop through the axes
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		// get the floating point zero-centered, potentially-inverted data for the current axis
		fAxisValue = (float)*pdwRawValue[i];
		// move centerpoint to zero
		fAxisValue -= 32768.0;

		// convert range from -32768..32767 to -1..1 
		fAxisValue /= 32768.0;

		switch (dwAxisMap[i])
		{
		case AxisForward:
			if ((joy_advanced->value == 0.0) && mlooking)
			{
				// user wants forward control to become look control
				if (fabs(fAxisValue) > joy_pitchthreshold->value)
				{
					// if mouse invert is on, invert the joystick pitch value
					// only absolute control support here (joy_advanced is false)
					if (m_pitch->value < 0.0)
					{
						cl.viewangles_PITCH -= (fAxisValue * joy_pitchsensitivity->value * (cl.refdef.fov_x / 90.0)) * aspeed * cl_pitchspeed->value;
					}
					else
					{
						cl.viewangles_PITCH += (fAxisValue * joy_pitchsensitivity->value * (cl.refdef.fov_x / 90.0)) * aspeed * cl_pitchspeed->value;
					}
				}
			}
			else
			{
				// user wants forward control to be forward control
				if (fabs(fAxisValue) > joy_forwardthreshold->value)
				{
					cmd->forwardmove += (fAxisValue * joy_forwardsensitivity->value) * speed * cl_forwardspeed->value;
				}
			}
			break;

		case AxisSide:
			if (fabs(fAxisValue) > joy_sidethreshold->value)
			{
				cmd->sidemove += (fAxisValue * joy_sidesensitivity->value) * speed * cl_sidespeed->value;
			}
			break;

		case AxisUp:
			if (fabs(fAxisValue) > joy_upthreshold->value)
			{
				cmd->upmove += (fAxisValue * joy_upsensitivity->value) * speed * cl_upspeed->value;
			}
			break;

		case AxisTurn:
			if ((in_strafe.state & 1) || (lookstrafe->value && mlooking))
			{
				// user wants turn control to become side control
				if (fabs(fAxisValue) > joy_sidethreshold->value)
				{
					cmd->sidemove -= (fAxisValue * joy_sidesensitivity->value) * speed * cl_sidespeed->value;
				}
			}
			else
			{
				// user wants turn control to be turn control
				if (fabs(fAxisValue) > joy_yawthreshold->value)
				{
					if (dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						cl.viewangles_YAW += (fAxisValue * joy_yawsensitivity->value * (cl.refdef.fov_x / 90.0)) * aspeed * cl_yawspeed->value;
					}
					else
					{
						cl.viewangles_YAW += (fAxisValue * joy_yawsensitivity->value) * speed * 180.0;
					}

				}
			}
			break;

		case AxisLook:
			if (mlooking)
			{
				if (fabs(fAxisValue) > joy_pitchthreshold->value)
				{
					// pitch movement detected and pitch movement desired by user
					if (dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						cl.viewangles_PITCH += (fAxisValue * joy_pitchsensitivity->value * (cl.refdef.fov_x / 90.0)) * aspeed * cl_pitchspeed->value;
					}
					else
					{
						cl.viewangles_PITCH += (fAxisValue * joy_pitchsensitivity->value * (cl.refdef.fov_x / 90.0)) * speed * 180.0;

					}
				}
			}
			break;

		default:
			break;
		}
	}
}

void IN_StartupXInput(void)
{
	XINPUT_CAPABILITIES xiCaps;

	// reset to -1 each time as this can be called at runtime
	xiActiveController = -1;
	
	Com_Printf("\n======= Init xInput Devices =======\n\n");

	if (xiActive)
	{
		XInputEnable(FALSE);
		xiActive = qfalse;
	}

	// only support up to 4 controllers (in a PC scenario usually just one will be attached)
	for (int c = 0; c < 4; c++)
	{
		memset(&xiCaps, 0, sizeof(XINPUT_CAPABILITIES));
		DWORD gc = XInputGetCapabilities(c, XINPUT_FLAG_GAMEPAD, &xiCaps);

		if (gc == ERROR_SUCCESS)
		{
			// just use the first one
			Com_Printf("Using xInput Device on Port %i\n", c);

			// store to global active controller
			xiActiveController = c;
			break;
		}
	}

	Com_Printf(S_COLOR_YELLOW "...No xInput Devices Found\n");
	Com_Printf("\n-----------------------------------\n\n");
	XInputEnable(FALSE);
	xiActive = qfalse;
}

void IN_ToggleXInput(cvar_t *var)
{
	if (var->value && !xiActive)
	{
		IN_StartupXInput();

		if (xiActiveController != -1)
		{
			XInputEnable(TRUE);
			xiActive = qtrue;
		}

		xi_oldDpadState = xi_oldButtonState = 0;
	}
	else if (!var->value && xiActive)
	{
		XInputEnable(FALSE);
		xiActive = qfalse;
		xi_oldDpadState = xi_oldButtonState = 0;
	}
}


extern cvar_t *cl_forwardspeed;
extern cvar_t *cl_sidespeed;

extern cvar_t *cl_yawspeed;
extern cvar_t *cl_pitchspeed;


void IN_ControllerAxisMove(usercmd_t *cmd, int axisval, int dz, int axismax, cvar_t *axisaction)
{
	// not using this axis
	if ((int)axisaction->value <= XI_AXIS_NONE) return;

	// unimplemented
	if ((int)axisaction->value > XI_AXIS_INVSTRAFE) return;

	// get the amount moved less the deadzone
	int realmove = abs(axisval) - dz;

	// move is within deadzone threshold
	if (realmove < dz) return;

	// 0 to 1 scale
	float fmove = (float)realmove / (axismax - dz);

	// square it to get better scale at small moves
	fmove *= fmove;

	// go back to negative
	if (axisval < 0) fmove *= -1;

	// check for inverse scale
	if ((int)axisaction->value > XI_AXIS_STRAFE) fmove *= -1;

	// decode the move
	switch ((int)axisaction->value)
	{
	case XI_AXIS_LOOK:
	case XI_AXIS_INVLOOK:
		// inverted by default (positive = look down)
		cl.viewangles[0] += fmove * cl_pitchspeed->value / 20.0f;
		break;

	case XI_AXIS_MOVE:
	case XI_AXIS_INVMOVE:
		cmd->forwardmove += fmove * cl_forwardspeed->value;
		break;

	case XI_AXIS_TURN:
	case XI_AXIS_INVTURN:
		// slow this down because the default cl_yawspeed is too fast here
		// invert it so that positive move = right
		cl.viewangles[1] += fmove * cl_yawspeed->value / 20.0f * -1;
		break;

	case XI_AXIS_STRAFE:
	case XI_AXIS_INVSTRAFE:
		cmd->sidemove += fmove * cl_sidespeed->value;
		break;

	default:
		// unimplemented
		break;
	}
}

void IN_ControllerMove(usercmd_t *cmd)
{
	// no controller to use
	if (!xiActive) 
		return;
	
	if (xiActiveController < 0) 
		return;
	
	if (!xi_useController->value) 
		return;

	XINPUT_STATE xiState;
	static DWORD xiLastPacket = 666;

	// get current state
	DWORD xiResult = XInputGetState(xiActiveController, &xiState);

	if (xiResult != ERROR_SUCCESS)
	{
		// something went wrong - we'll handle that properly later...
		return;
	}

	// check the axes (always, even if state doesn't change)
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32768,	 xi_axisLx);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32768,  xi_axisLy);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32768, xi_axisRx);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32768, xi_axisRy);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.bLeftTrigger, 0, 255,  xi_axisLt);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.bRightTrigger, 0, 255, xi_axisRt);

	// fix up the command (bound/etc)
	if (cl.viewangles[0] > 80.0) cl.viewangles[0] = 80.0;
	if (cl.viewangles[0] < -70.0) cl.viewangles[0] = -70.0;

	// check for a change of state
	if (xiLastPacket == xiState.dwPacketNumber) return;

	// store back last packet
	xiLastPacket = xiState.dwPacketNumber;

	int buttonState = 0;
	int dpadState = 0;

	if ((int)xi_dpadArrowMap->value)
	{
		// check dpad (same order as arrow keys)
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)		dpadState |= 1;
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)	dpadState |= 2;
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)	dpadState |= 4;
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)	dpadState |= 8;
	}
	else
	{
		// check dpad (same order as joystick pov hats)
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)		dpadState |= 1;
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)	dpadState |= 2;
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)	dpadState |= 4;
		if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)	dpadState |= 8;
	}

	// check for event changes
	for (int i = 0; i < 4; i++)
	{
		if ((int)xi_dpadArrowMap->value)
		{
			// map dpad to arrow keys
			if ((dpadState & (1 << i)) && !(xi_oldDpadState & (1 << i))) 
				Key_Event(K_UPARROW + i, qtrue, sys_msg_time);
			if (!(dpadState & (1 << i)) && (xi_oldDpadState & (1 << i))) 
				Key_Event(K_UPARROW + i, qfalse, sys_msg_time);
		}
		else
		{
			// map dpad to POV keys
			if ((dpadState & (1 << i)) && !(xi_oldDpadState & (1 << i))) 
				Key_Event(K_POV1 + i, qtrue, sys_msg_time);
			if (!(dpadState & (1 << i)) && (xi_oldDpadState & (1 << i))) 
				Key_Event(K_POV1 + i, qfalse, sys_msg_time);
		}
	}

	// store back
	xi_oldDpadState = dpadState;

	// check other buttons - map these to K_JOY buttons
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_START)			buttonState |= 1;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)				buttonState |= 2;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)		buttonState |= 4;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)		buttonState |= 8;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)	buttonState |= 16;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)	buttonState |= 32;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_A)				buttonState |= 64;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_B)				buttonState |= 128;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_X)				buttonState |= 256;
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)				buttonState |= 512;

	// check for event changes
	for (int i = 0; i < 10; i++)
	{
		if ((buttonState & (1 << i)) && !(xi_oldButtonState & (1 << i))) 
			Key_Event(K_JOY1 + i, qtrue, sys_msg_time);
		if (!(buttonState & (1 << i)) && (xi_oldButtonState & (1 << i))) 
			Key_Event(K_JOY1 + i, qfalse, sys_msg_time);
	}

	// store back
	xi_oldButtonState = buttonState;
}

