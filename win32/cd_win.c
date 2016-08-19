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
// Quake is a trademark of Id Software, Inc., (c) 1996 Id Software, Inc. All
// rights reserved.

#include <windows.h>
#include <process.h>
#include "../client/client.h"
#define MCI_NULL 0

extern	HWND	cl_hwnd;

static qboolean cdValid = qfalse;
static qboolean	playing = qfalse;
static qboolean	wasPlaying = qfalse;
static qboolean	initialized = qfalse;
static qboolean	enabled = qfalse;
static qboolean playLooping = qfalse;
static byte 	remap[100];
static byte		cdrom;
static byte		playTrack;
static byte		maxTrack;

typedef struct {
	int					cdVolume;
	HMIXER				hMixer;
	MIXERLINE			mixerLine;
	MIXERLINECONTROLS	mixerLineControl;
	MIXERCONTROL		mixerControl;
} cdAudio_t;

static cdAudio_t	cdAudio;

cvar_t *cd_nocd;
cvar_t *cd_drive;
cvar_t *cd_loopcount;
cvar_t *cd_looptrack;
cvar_t	*cd_volume;

UINT	wDeviceID;
int		loopcounter;


/*
 =================
 CDAudio_MixerShutdown
 =================
 */
void CDAudio_MixerShutdown (void) {

	DWORD	dwResult;

	if (!cdAudio.hMixer)
		return;

	if (dwResult = mixerClose (cdAudio.hMixer))
		Com_DPrintf ("CDAudio: mixerClose() failed (%i)\n", dwResult);

	cdAudio.hMixer = NULL;
}

/*
 =================
 CDAudio_MixerInit
 =================
 */
static void CDAudio_MixerInit (void) {

	DWORD	dwResult;

	if (!mixerGetNumDevs ()) {
		Com_DPrintf ("CDAudio: no mixer devices found\n");
		return;
	}

	// Open the device
	if (dwResult = mixerOpen (&cdAudio.hMixer, 0, 0, 0, MIXER_OBJECTF_MIXER)) {
		Com_DPrintf ("CDAudio: mixerOpen() failed (%i)\n", dwResult);
		return;
	}

	memset (&cdAudio.mixerLine, 0, sizeof(MIXERLINE));
	cdAudio.mixerLine.cbStruct = sizeof(MIXERLINE);
	cdAudio.mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;

	if (dwResult = mixerGetLineInfo ((HMIXEROBJ)cdAudio.hMixer, &cdAudio.mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE)) {
		Com_DPrintf ("CDAudio: mixerGetLineInfo() failed (%i)\n", dwResult);
		CDAudio_MixerShutdown ();
		return;
	}

	memset (&cdAudio.mixerLineControl, 0, sizeof(MIXERLINECONTROLS));
	cdAudio.mixerLineControl.cbStruct = sizeof(MIXERLINECONTROLS);
	cdAudio.mixerLineControl.dwLineID = cdAudio.mixerLine.dwLineID;
	cdAudio.mixerLineControl.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	cdAudio.mixerLineControl.cControls = 1;
	cdAudio.mixerLineControl.cbmxctrl = sizeof(cdAudio.mixerControl);
	cdAudio.mixerLineControl.pamxctrl = &cdAudio.mixerControl;

	if (dwResult = mixerGetLineControls ((HMIXEROBJ)cdAudio.hMixer, &cdAudio.mixerLineControl, MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
		Com_DPrintf ("CDAudio: mixerGetLineControls() failed (%i)\n", dwResult);
		CDAudio_MixerShutdown ();
		return;
	}
}
/*
 =================
 CDAudio_GetMixerVolume
 =================
 */
static int CDAudio_GetMixerVolume (void) {

	DWORD							dwResult;
	MIXERCONTROLDETAILS				mxControlDetails;
	MIXERCONTROLDETAILS_UNSIGNED	mxValue[2];
	MIXERCONTROL					*mxControl;
	MIXERLINE						*mxLine;
	int								value;

	if (!cdAudio.hMixer)
		return 0;

	mxLine = &cdAudio.mixerLine;
	mxControl = &cdAudio.mixerControl;

	memset (&mxControlDetails, 0, sizeof(MIXERCONTROLDETAILS));
	mxControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxControlDetails.dwControlID = mxControl->dwControlID;
	mxControlDetails.cChannels = mxLine->cChannels;
	mxControlDetails.cbDetails = sizeof(mxValue);
	mxControlDetails.paDetails = &mxValue;

	if (dwResult = mixerGetControlDetails ((HMIXEROBJ)cdAudio.hMixer, &mxControlDetails, MIXER_GETCONTROLDETAILSF_VALUE)) {
		Com_DPrintf ("CDAudio: mixerGetControlDetails() failed (%i)\n", dwResult);
		return 0;
	}

	value = mxValue[(mxValue[0].dwValue > mxValue[1].dwValue) ? 0 : 1].dwValue;
	return (255 * (value - mxControl->Bounds.dwMinimum)) / (mxControl->Bounds.dwMaximum - mxControl->Bounds.dwMinimum);
}

/*
 =================
 CDAudio_SetMixerVolume
 =================
 */
static void CDAudio_SetMixerVolume (float volume) {

	DWORD							dwResult;
	MIXERCONTROLDETAILS				mxControlDetails;
	MIXERCONTROLDETAILS_UNSIGNED	mxValue[2];
	MIXERCONTROL					*mxControl;
	MIXERLINE						*mxLine;

	if (!cdAudio.hMixer)
		return;

	mxLine = &cdAudio.mixerLine;
	mxControl = &cdAudio.mixerControl;

	memset (&mxControlDetails, 0, sizeof(MIXERCONTROLDETAILS));
	mxControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxControlDetails.dwControlID = mxControl->dwControlID;
	mxControlDetails.cChannels = mxLine->cChannels;
	mxControlDetails.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxControlDetails.paDetails = &mxValue;

	mxValue[0].dwValue = mxValue[1].dwValue = (volume * (mxControl->Bounds.dwMaximum - mxControl->Bounds.dwMinimum)) + mxControl->Bounds.dwMinimum;

	if (dwResult = mixerSetControlDetails ((HMIXEROBJ)cdAudio.hMixer, &mxControlDetails, MIXER_SETCONTROLDETAILSF_VALUE))
		Com_DPrintf ("CDAudio: mixerSetControlDetails() failed (%i)\n", dwResult);
}


void CDAudio_Pause (void);

static void CDAudio_Eject (void) {
	DWORD	dwReturn;

	if (dwReturn = mciSendCommand (wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, MCI_NULL))
		Com_DPrintf ("MCI_SET_DOOR_OPEN failed (%i)\n", dwReturn);
}


static void CDAudio_CloseDoor (void) {
	DWORD	dwReturn;

	if (dwReturn = mciSendCommand (wDeviceID, MCI_SET, MCI_SET_DOOR_CLOSED, MCI_NULL))
		Com_DPrintf ("MCI_SET_DOOR_CLOSED failed (%i)\n", dwReturn);
}


static int CDAudio_GetAudioDiskInfo (void) {
	DWORD				dwReturn;
	MCI_STATUS_PARMS	mciStatusParms;


	cdValid = qfalse;

	mciStatusParms.dwItem = MCI_STATUS_READY;
	dwReturn = mciSendCommand (wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&mciStatusParms);
	if (dwReturn) {
		Com_Printf ("CDAudio: drive ready test - get status failed\n");
		return -1;
	}
	if (!mciStatusParms.dwReturn) {
		Com_Printf ("CDAudio: drive not ready\n");
		return -1;
	}

	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	dwReturn = mciSendCommand (wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&mciStatusParms);
	if (dwReturn) {
		Com_Printf ("CDAudio: get tracks - status failed\n");
		return -1;
	}
	if (mciStatusParms.dwReturn < 1) {
		Com_Printf ("CDAudio: no music tracks\n");
		return -1;
	}

	cdValid = qtrue;
	maxTrack = mciStatusParms.dwReturn;

	return 0;
}

void CDAudio_Stop (void) {
	DWORD	dwReturn;

	if (!enabled)
		return;

	if (!playing)
		return;

	if (dwReturn = mciSendCommand (wDeviceID, MCI_STOP, 0, MCI_NULL))
		Com_Printf ("MCI_STOP failed (%i)", dwReturn);

	wasPlaying = qfalse;
	playing = qfalse;
}

void CDAudio_Play2 (int track, qboolean looping) {
	DWORD				dwReturn;
	MCI_PLAY_PARMS		mciPlayParms;
	MCI_STATUS_PARMS	mciStatusParms;

	if (!enabled)
		return;

	if (!cdValid) {
		CDAudio_GetAudioDiskInfo ();
		if (!cdValid)
			return;
	}

	track = remap[track];

	if (track < 1 || track > maxTrack) {
		CDAudio_Stop ();
		return;
	}

	// don't try to play a non-audio track
	mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
	mciStatusParms.dwTrack = track;
	dwReturn = mciSendCommand (wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD_PTR)&mciStatusParms);
	if (dwReturn) {
		Com_Printf ("MCI_STATUS failed (%i)\n", dwReturn);
		return;
	}
	if (mciStatusParms.dwReturn != MCI_CDA_TRACK_AUDIO) {
		Com_Printf ("CDAudio: track %i is not audio\n", track);
		return;
	}

	// get the length of the track to be played
	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
	mciStatusParms.dwTrack = track;
	dwReturn = mciSendCommand (wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD_PTR)&mciStatusParms);
	if (dwReturn) {
		Com_Printf ("MCI_STATUS failed (%i)\n", dwReturn);
		return;
	}

	if (playing) {
		if (playTrack == track)
			return;
		CDAudio_Stop ();
	}

	mciPlayParms.dwFrom = MCI_MAKE_TMSF (track, 0, 0, 0);
	mciPlayParms.dwTo = (mciStatusParms.dwReturn << 8) | track;
	mciPlayParms.dwCallback = (DWORD_PTR)cl_hwnd;
	dwReturn = mciSendCommand (wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM | MCI_TO, (DWORD_PTR)&mciPlayParms);
	if (dwReturn) {
		Com_Printf ("CDAudio: MCI_PLAY failed (%i)\n", dwReturn);
		return;
	}

	playLooping = looping;
	playTrack = track;
	playing = qtrue;

	if (Cvar_VariableValue ("cd_nocd"))
		CDAudio_Pause ();
}


void CDAudio_Play (int track, qboolean looping) {
	// set a loop counter so that this track will change to the
	// looptrack later
	loopcounter = 0;
	CDAudio_Play2 (track, looping);
}


void CDAudio_Pause (void) {
	DWORD				dwReturn;
	MCI_GENERIC_PARMS	mciGenericParms;

	if (!enabled)
		return;

	if (!playing)
		return;

	mciGenericParms.dwCallback = (DWORD_PTR)cl_hwnd;
	if (dwReturn = mciSendCommand (wDeviceID, MCI_PAUSE, 0, (DWORD_PTR)&mciGenericParms))
		Com_DPrintf ("MCI_PAUSE failed (%i)", dwReturn);

	wasPlaying = playing;
	playing = qfalse;
}


void CDAudio_Resume (void) {
	DWORD			dwReturn;
	MCI_PLAY_PARMS	mciPlayParms;

	if (!enabled)
		return;

	if (!cdValid)
		return;

	if (!wasPlaying)
		return;

	mciPlayParms.dwFrom = MCI_MAKE_TMSF (playTrack, 0, 0, 0);
	mciPlayParms.dwTo = MCI_MAKE_TMSF (playTrack + 1, 0, 0, 0);
	mciPlayParms.dwCallback = (DWORD_PTR)cl_hwnd;
	dwReturn = mciSendCommand (wDeviceID, MCI_PLAY, MCI_TO | MCI_NOTIFY, (DWORD_PTR)&mciPlayParms);
	if (dwReturn) {
		Com_DPrintf ("CDAudio: MCI_PLAY failed (%i)\n", dwReturn);
		return;
	}
	playing = qtrue;
}


static void CD_f (void) {
	char	*command;
	int		ret;
	int		n;

	if (Cmd_Argc () < 2)
		return;

	command = Cmd_Argv (1);

	if (Q_strcasecmp (command, "on") == 0) {
		enabled = qtrue;
		return;
	}

	if (Q_strcasecmp (command, "off") == 0) {
		if (playing)
			CDAudio_Stop ();
		enabled = qfalse;
		return;
	}

	if (Q_strcasecmp (command, "reset") == 0) {
		enabled = qtrue;
		if (playing)
			CDAudio_Stop ();
		for (n = 0; n < 100; n++)
			remap[n] = n;
		CDAudio_GetAudioDiskInfo ();
		return;
	}

	if (Q_strcasecmp (command, "remap") == 0) {
		ret = Cmd_Argc () - 2;
		if (ret <= 0) {
			for (n = 1; n < 100; n++)
			if (remap[n] != n)
				Com_Printf ("  %u -> %u\n", n, remap[n]);
			return;
		}
		for (n = 1; n <= ret; n++)
			remap[n] = atoi (Cmd_Argv (n + 1));
		return;
	}

	if (Q_strcasecmp (command, "close") == 0) {
		CDAudio_CloseDoor ();
		return;
	}

	if (!cdValid) {
		CDAudio_GetAudioDiskInfo ();
		if (!cdValid) {
			Com_Printf ("No CD in player.\n");
			return;
		}
	}

	if (Q_strcasecmp (command, "play") == 0) {
		CDAudio_Play (atoi (Cmd_Argv (2)), qfalse);
		return;
	}

	if (Q_strcasecmp (command, "loop") == 0) {
		CDAudio_Play (atoi (Cmd_Argv (2)), qtrue);
		return;
	}

	if (Q_strcasecmp (command, "stop") == 0) {
		CDAudio_Stop ();
		return;
	}

	if (Q_strcasecmp (command, "pause") == 0) {
		CDAudio_Pause ();
		return;
	}

	if (Q_strcasecmp (command, "resume") == 0) {
		CDAudio_Resume ();
		return;
	}

	if (Q_strcasecmp (command, "eject") == 0) {
		if (playing)
			CDAudio_Stop ();
		CDAudio_Eject ();
		cdValid = qfalse;
		return;
	}

	if (Q_strcasecmp (command, "info") == 0) {
		Com_Printf ("%u tracks\n", maxTrack);
		if (playing)
			Com_Printf ("Currently %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		else if (wasPlaying)
			Com_Printf ("Paused %s track %u\n", playLooping ? "looping" : "playing", playTrack);
		return;
	}
}


LONG CDAudio_MessageHandler (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (lParam != wDeviceID)
		return 1;

	switch (wParam) {
		case MCI_NOTIFY_SUCCESSFUL:
			if (playing) {
				playing = qfalse;
				if (playLooping) {
					// if the track has played the given number of times,
					// go to the ambient track
					if (++loopcounter >= cd_loopcount->value)
						CDAudio_Play2 (cd_looptrack->value, qtrue);
					else
						CDAudio_Play2 (playTrack, qtrue);
				}
			}
			break;

		case MCI_NOTIFY_ABORTED:
		case MCI_NOTIFY_SUPERSEDED:
			break;

		case MCI_NOTIFY_FAILURE:
			Com_DPrintf ("MCI_NOTIFY_FAILURE\n");
			CDAudio_Stop ();
			cdValid = qfalse;
			break;

		default:
			Com_DPrintf ("Unexpected MM_MCINOTIFY type (%i)\n", wParam);
			return 1;
	}

	return 0;
}


void CDAudio_Update (void) {

	if (cd_nocd->value) {
		CDAudio_Stop ();
		enabled = qfalse;
	}
	else {
		enabled = qtrue;
		CDAudio_Resume ();
	}

	if (cd_volume->modified) {
		if (cd_volume->value > 1.0)
			Cvar_SetValue ("cd_volume", 1.0);
		else if (cd_volume->value < 0.0)
			Cvar_SetValue ("cd_volume", 0.0);

		CDAudio_SetMixerVolume (cd_volume->value);

		cd_volume->modified = qfalse;
	}

}

// Berserker audioCD latter override

char q2cd_letter[3] = "-:";

int CDAudio_Init (void) {
	DWORD			dwReturn;
	MCI_OPEN_PARMS	mciOpenParms;
	MCI_SET_PARMS	mciSetParms;
	int				n;
	unsigned		flags;

	cd_drive = Cvar_Get ("cd_drive", "h", CVAR_ARCHIVE);
	cd_drive->help = "override the automatic assigned CD letter.";
	cd_nocd = Cvar_Get ("cd_nocd", "0", CVAR_ARCHIVE);
	cd_loopcount = Cvar_Get ("cd_loopcount", "25", CVAR_ARCHIVE); //was 4
	cd_looptrack = Cvar_Get ("cd_looptrack", "11", 0);
	cd_volume = Cvar_Get ("cd_volume", "1.0", CVAR_ARCHIVE);

	if (cd_nocd->value)
		return -1;

	if (cd_drive->string && cd_drive->string[0])
		q2cd_letter[0] = cd_drive->string[0];		// Berserker: override
	memset (&mciOpenParms, 0, sizeof(MCI_OPEN_PARMS));
	if (q2cd_letter[0]) {
		mciOpenParms.lpstrElementName = q2cd_letter;
		flags = MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT;
		mciOpenParms.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_CD_AUDIO;
	}
	else {
		flags = 0;		// Berserker: auto select
		mciOpenParms.lpstrDeviceType = "cdaudio";
	}
	if (dwReturn = mciSendCommand (0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE | flags, (DWORD)(LPVOID)&mciOpenParms)) {
		Com_Printf ("CDAudio_Init: MCI_OPEN failed (%i)\n", dwReturn);
		return -1;
	}
	wDeviceID = mciOpenParms.wDeviceID;

	// Set the time format to track/minute/second/frame (TMSF).
	mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
	if (dwReturn = mciSendCommand (wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&mciSetParms)) {
		Com_Printf ("MCI_SET_TIME_FORMAT failed (%i)\n", dwReturn);
		mciSendCommand (wDeviceID, MCI_CLOSE, 0, MCI_NULL);
		return -1;
	}

	for (n = 0; n < 100; n++)
		remap[n] = n;
	initialized = qtrue;
	enabled = qtrue;

	if (CDAudio_GetAudioDiskInfo ()) {
		//		Com_Printf("CDAudio_Init: No CD in player.\n");
		cdValid = qfalse;
		enabled = qfalse;
	}

	Cmd_AddCommand ("cd", CD_f);

	Com_Printf ("CD Audio Initialized\n");

	CDAudio_MixerInit ();
	cdAudio.cdVolume = CDAudio_GetMixerVolume ();



	return 0;
}


void CDAudio_Shutdown (void) {
	if (!initialized)
		return;
	CDAudio_Stop ();
	if (mciSendCommand (wDeviceID, MCI_CLOSE, MCI_WAIT, MCI_NULL))
		Com_DPrintf ("CDAudio_Shutdown: MCI_CLOSE failed\n");
	CDAudio_MixerShutdown ();
}


/*
===========
CDAudio_Activate

Called when the main window gains or loses focus.
The window have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void CDAudio_Activate (qboolean active) {
	if (active)
		CDAudio_Resume ();
	else
		CDAudio_Pause ();
}
