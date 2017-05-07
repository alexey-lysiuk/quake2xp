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
// xBox 360 controller support
// based on DirectQuake by MH

#include "../client/client.h"
#include "winquake.h"
#include "xinput.h"

extern	unsigned	sys_msg_time;

qboolean	xiActive			= qfalse;
int			xiActiveController	= -1;
int			xi_oldDpadState		= 0;
int			xi_oldButtonState	= 0;

typedef struct {
	HINSTANCE xiDevice;
} xinput_t;

xinput_t xinput;

#define XINPUT_LIB	"xinput1_3.dll" // win7 support

#define XI_MAX_CONTROLLERS 4
#define XI_MAX_CONTROLLER_BUTTONS 10

typedef void	(__stdcall * _xInputEnable)(BOOL);
typedef DWORD	(__stdcall * _XInputGetCapabilities)(DWORD, DWORD, PXINPUT_CAPABILITIES);
typedef DWORD	(__stdcall * _XInputGetState)(DWORD, PXINPUT_STATE);
typedef DWORD	(__stdcall * _XInputGetBatteryInformation)(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
typedef DWORD	(__stdcall * _XInputSetState)(DWORD, XINPUT_VIBRATION*);

static void		(WINAPI * qXInputEnable)(BOOL enable);
static DWORD	(WINAPI * qXInputGetCapabilities)(DWORD dwUserIndex, DWORD dwFlags, PXINPUT_CAPABILITIES pCapabilities);
static DWORD	(WINAPI * qXInputGetState)(DWORD dwUserIndex, PXINPUT_STATE pState);
static DWORD	(WINAPI * qXInputGetBatteryInformation)(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
static DWORD	(WINAPI * qXInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);

void IN_ShutDownXinput() {

	Com_Printf("..." S_COLOR_YELLOW "shutting down xInput subsystem\n");

	if (xinput.xiDevice) {
		Com_Printf("..." S_COLOR_YELLOW "unloading " S_COLOR_GREEN "%s\n", XINPUT_LIB);
		FreeLibrary(xinput.xiDevice);
	}
	memset(&xinput, 0, sizeof(xinput_t));
}

void IN_StartupXInput(void)
{
	XINPUT_CAPABILITIES xiCaps;
	XINPUT_BATTERY_INFORMATION batteryInfo;
	char batteryLevel[64], batteryType[64];

	// reset to -1 each time as this can be called at runtime
	xiActiveController = -1;

	Com_Printf("\n======= Init xInput Devices =======\n\n");

	// Load the xInput dll
	Com_Printf("...calling LoadLibrary(" S_COLOR_GREEN "%s" S_COLOR_WHITE "):", XINPUT_LIB);

	if ((xinput.xiDevice = LoadLibrary(XINPUT_LIB)) == NULL) {
		Com_Printf(S_COLOR_RED"failed!\n");
		Com_Printf("\n-----------------------------------\n\n");
		return;
	}

	qXInputEnable						= (_xInputEnable)						GetProcAddress(xinput.xiDevice, "XInputEnable");
	qXInputGetCapabilities				= (_XInputGetCapabilities)				GetProcAddress(xinput.xiDevice, "XInputGetCapabilities");
	qXInputGetState						= (_XInputGetState)						GetProcAddress(xinput.xiDevice, "XInputGetState");
	qXInputGetBatteryInformation		= (_XInputGetBatteryInformation)		GetProcAddress(xinput.xiDevice, "XInputGetBatteryInformation");
	qXInputSetState						= (_XInputSetState)						GetProcAddress(xinput.xiDevice, "XInputSetState");

	if (!qXInputEnable || !qXInputGetCapabilities || !qXInputGetState || !qXInputGetBatteryInformation || !qXInputSetState) {
		Com_Printf(S_COLOR_RED"...can't find xInput procedures adresses.\n");
		Com_Printf("\n-----------------------------------\n\n");
		return;
	}
	Com_Printf(S_COLOR_GREEN"succeeded.\n\n");

	for (int numDev = 0; numDev < XI_MAX_CONTROLLERS; numDev++){

		memset(&xiCaps, 0, sizeof(XINPUT_CAPABILITIES));
		DWORD getCaps = qXInputGetCapabilities(numDev, XINPUT_FLAG_GAMEPAD, &xiCaps);

		memset(&batteryInfo, 0, sizeof(XINPUT_BATTERY_INFORMATION));
		DWORD battStat = qXInputGetBatteryInformation(numDev, BATTERY_DEVTYPE_GAMEPAD, &batteryInfo);

		if (getCaps == ERROR_SUCCESS)
		{
			if(batteryInfo.BatteryLevel == BATTERY_LEVEL_EMPTY)
				strcpy(batteryLevel, "Battery empity");
			else
				if (batteryInfo.BatteryLevel == BATTERY_LEVEL_LOW)
					strcpy(batteryLevel, "Battery level low");
			else
				if (batteryInfo.BatteryLevel == BATTERY_LEVEL_MEDIUM)
					strcpy(batteryLevel, "Battery level medium");
			else
				if (batteryInfo.BatteryLevel == BATTERY_LEVEL_FULL)
					strcpy(batteryLevel, "Battery level full");
			
			if (batteryInfo.BatteryType == BATTERY_TYPE_DISCONNECTED)
				goto fail;

			if (batteryInfo.BatteryType == BATTERY_TYPE_WIRED)
					strcpy(batteryType, "Controller wired");
				else
			if (batteryInfo.BatteryType == BATTERY_TYPE_ALKALINE)
					strcpy(batteryType, "Controller use Alkalyne battery");
				else
			if (batteryInfo.BatteryType == BATTERY_TYPE_NIMH)
					strcpy(batteryType, "Controller use Ni-MH battery");
				else
			if (batteryInfo.BatteryType == BATTERY_TYPE_UNKNOWN)
					strcpy(batteryType, "Controller use unknow battery type");

			// just use the first one
			Com_Printf("...found %i xInput Controller\n", numDev + 1);

			Com_Printf("...%s\n...%s\n", batteryLevel, batteryType);

			// store to global active controller
			xiActiveController = numDev;
			break;
		}
	}

	if (xiActiveController != -1)
	{
		qXInputEnable(TRUE);
		xiActive = qtrue;
	}
	else {
		fail:
		Com_Printf(S_COLOR_MAGENTA"...xInput Device disconnected or not found.\n");
		IN_ShutDownXinput();
	}
	 
	Com_Printf("\n-----------------------------------\n\n");
}

void IN_ToggleXInput()
{

	if (xi_useController->value){
		
		if (xiActive)
			return;

		if (xiActiveController != -1) {
			qXInputEnable(TRUE);
			xiActive = qtrue;
		}
		xi_oldDpadState = xi_oldButtonState = 0;
	}
	else 
	{
		if (!xiActive)
			return;

		qXInputEnable(FALSE);
		xiActive = qfalse;
		xi_oldDpadState = xi_oldButtonState = 0;
	}
}

void SetRumble(int devNum, int rumbleLow, int rumbleHigh) {

	if (!xiActive)
		return;

	if (devNum < 0 || devNum >= XI_MAX_CONTROLLERS)
		return;

	if (!xi_useController->value)
		return;

	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = clamp(rumbleLow, 0, 65535);
	vibration.wRightMotorSpeed = clamp(rumbleHigh, 0, 65535);
	DWORD err = qXInputSetState(devNum, &vibration);

	if (err != ERROR_SUCCESS)
		Com_Printf(S_COLOR_RED"XInputSetState error: 0x%x", err);
}

extern cvar_t *cl_forwardspeed;
extern cvar_t *cl_sidespeed;

extern cvar_t *cl_yawspeed;
extern cvar_t *cl_pitchspeed;

#define XI_AXIS_NONE		0
#define XI_AXIS_LOOK		1
#define XI_AXIS_MOVE		2
#define XI_AXIS_TURN		3
#define XI_AXIS_STRAFE		4
#define XI_AXIS_INVLOOK		5
#define XI_AXIS_INVMOVE		6
#define XI_AXIS_INVTURN		7
#define XI_AXIS_INVSTRAFE	8

void IN_ControllerAxisMove(usercmd_t *cmd, int axisval, int dz, int axismax, cvar_t *axisaction)
{
	// not using this axis
	if ((int)axisaction->value <= XI_AXIS_NONE)
		return;

	// unimplemented
	if ((int)axisaction->value > XI_AXIS_INVSTRAFE)
		return;

	// get the amount moved less the deadzone
	int realmove = abs(axisval) - dz;

	// move is within deadzone threshold
	if (realmove < dz)
		return;

	// 0 to 1 scale
	float fmove = (float)realmove / (axismax - dz);

	float speed;

	if ((in_speed.state & 1) ^ (int)cl_run->value)
		speed = 2;
	else
		speed = 1;

	// square it to get better scale at small moves
	fmove *= fmove;

	// go back to negative
	if (axisval < 0) 
		fmove *= -1;

	// check for inverse scale
	if ((int)axisaction->value > XI_AXIS_STRAFE) 
		fmove *= -1;
	
	float inv = 1;
	if(xi_pitchInversion->value)
		inv *= -1;

	// decode the move
	switch ((int)axisaction->value)
	{
	case XI_AXIS_LOOK:
	case XI_AXIS_INVLOOK:
		cl.viewangles_PITCH -= fmove * (cl_pitchspeed->value / cl.refdef.fov_y) * xi_sensY->value * inv;
		break;

	case XI_AXIS_MOVE:
	case XI_AXIS_INVMOVE:
		cmd->forwardmove += fmove * speed * cl_forwardspeed->value;
		break;

	case XI_AXIS_TURN:
	case XI_AXIS_INVTURN:
		// slow this down because the default cl_yawspeed is too fast here
		// invert it so that positive move = right
		cl.viewangles_YAW -= fmove * (cl_yawspeed->value / cl.refdef.fov_x) * xi_sensX->value;
		break;

	case XI_AXIS_STRAFE:
	case XI_AXIS_INVSTRAFE:
		cmd->sidemove -= fmove * speed * cl_sidespeed->value;
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
	DWORD xiResult = qXInputGetState(xiActiveController, &xiState);

	if (xiResult != ERROR_SUCCESS)
	{
		// something went wrong - we'll handle that properly later...
		return;
	}

	// check the axes (always, even if state doesn't change)
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32768, xi_axisLt);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32768, xi_axisLy);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32768, xi_axisRx);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32768, xi_axisRy);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.bLeftTrigger, 0, 255, xi_axisLt);
	IN_ControllerAxisMove(cmd, xiState.Gamepad.bRightTrigger, 0, 255, xi_axisRt);

	// fix up the command (bound/etc)
	if (cl.viewangles[0] > 80.0) 
		cl.viewangles[0] = 80.0;

	if (cl.viewangles[0] < -70.0) 
		cl.viewangles[0] = -70.0;

	// check for a change of state
	if (xiLastPacket == xiState.dwPacketNumber) 
		return;

	// store back last packet
	xiLastPacket = xiState.dwPacketNumber;

	int buttonState = 0;
	int dpadState = 0;

	if (xiState.Gamepad.bLeftTrigger >= 128)
		buttonState |= 0x10000;
	if (xiState.Gamepad.bRightTrigger >= 128)
		buttonState |= 0x20000;

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
	for (int i = 0; i < XI_MAX_CONTROLLERS; i++)
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
	for (int i = 0; i < XI_MAX_CONTROLLER_BUTTONS; i++)
	{
		if ((buttonState & (1 << i)) && !(xi_oldButtonState & (1 << i)))
			Key_Event(K_JOY1 + i, qtrue, sys_msg_time);

		if (!(buttonState & (1 << i)) && (xi_oldButtonState & (1 << i)))
			Key_Event(K_JOY1 + i, qfalse, sys_msg_time);
	}

	// store back
	xi_oldButtonState = buttonState;
}