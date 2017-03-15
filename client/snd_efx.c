/*
 Copyright (C) 2006-2007 Team Blur.
 Copyright (C) 2007, team Q2XP.
 */

// Note:
// The EFX reverb editor was from Quake II Evolved / OverDose / Quantum engine,
// but wasn't integrated into Quake2XP so only equivalents of snd_eax.c are here.
//
// TODO: maybe we can do some basic room detection from map data?

#include "client.h"
#include "snd_loc.h"

// Most Linux distributions don't ship OpenAL Soft 1.14 yet,
// so for now we have it here.
#include "AL/efx-presets.h"

EFXEAXREVERBPROPERTIES rvb_generic			= EFX_REVERB_PRESET_GENERIC;
EFXEAXREVERBPROPERTIES rvb_room				= EFX_REVERB_PRESET_ROOM;
EFXEAXREVERBPROPERTIES rvb_underwater		= EFX_REVERB_PRESET_UNDERWATER;
EFXEAXREVERBPROPERTIES rvb_level			= EFX_REVERB_PRESET_CITY;

EFXEAXREVERBPROPERTIES rvb_small_room		= EFX_REVERB_PRESET_FACTORY_SMALLROOM;
EFXEAXREVERBPROPERTIES rvb_medium_room		= EFX_REVERB_PRESET_FACTORY_MEDIUMROOM;
EFXEAXREVERBPROPERTIES rvb_large_room		= EFX_REVERB_PRESET_FACTORY_LARGEROOM;

EFXEAXREVERBPROPERTIES rvb_alcove			= EFX_REVERB_PRESET_FACTORY_ALCOVE;
EFXEAXREVERBPROPERTIES rvb_short_passege	= EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE;
EFXEAXREVERBPROPERTIES rvb_long_passege		= EFX_REVERB_PRESET_FACTORY_LONGPASSAGE;
EFXEAXREVERBPROPERTIES rvb_hall				= EFX_REVERB_PRESET_FACTORY_HALL;
EFXEAXREVERBPROPERTIES rvb_cour_yard		= EFX_REVERB_PRESET_FACTORY_COURTYARD;

extern cvar_t *s_dynamicReverberation;

typedef struct {
	qboolean on;
	ALuint rvbGenericEffect;
	ALuint rvbRoomEffect;
	ALuint rvbUnderwaterEffect;
	ALuint rvbLevelEffect;
	ALuint rvbAuxSlot;

	ALuint rvbSmallRoomEffect;
	ALuint rvbMediumRoomEffect;
	ALuint rvbLargeRoomEffect;
	ALuint rvbAlcoveEffect;
} efx_t;

efx_t efx;

ALuint EFXEAX_RvbCreate (EFXEAXREVERBPROPERTIES *rvb);
ALuint EFX_RvbCreate (EFXEAXREVERBPROPERTIES *rvb);

void EFX_RvbInit (void) {

	if (efx.on)
		return;

	efx.rvbGenericEffect = EFX_RvbCreate (&rvb_generic);
	efx.rvbRoomEffect = EFX_RvbCreate (&rvb_room);
	efx.rvbUnderwaterEffect = EFX_RvbCreate (&rvb_underwater);
	efx.rvbLevelEffect = EFX_RvbCreate (&rvb_level);

	efx.rvbSmallRoomEffect = EFX_RvbCreate(&rvb_small_room);
	efx.rvbMediumRoomEffect = EFX_RvbCreate(&rvb_medium_room);
	efx.rvbLargeRoomEffect = EFX_RvbCreate(&rvb_large_room);
	efx.rvbAlcoveEffect = EFX_RvbCreate(&rvb_alcove);

	alGenAuxiliaryEffectSlots (1, &efx.rvbAuxSlot);
	alAuxiliaryEffectSloti (efx.rvbAuxSlot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE);

	if (alGetError () == AL_NO_ERROR) {
		Com_Printf (S_COLOR_YELLOW "EFX initialized\n");
		efx.on = qtrue;
	}
	else {
		Com_Printf (S_COLOR_RED "EFX init failed\n");
		efx.on = qfalse;
	}
}

trace_t CL_PMTraceWorld(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int mask, qboolean checkAliases);

void EFX_GetRoomSize() {
	vec3_t forward, right, up;
	vec3_t end, tmp;
	trace_t trace;
	float sum, frontL, backL, leftL, rightL, upL, downL;

	if (s_dynamicReverberation->value) {
		alAuxiliaryEffectSloti(efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbLevelEffect);
		return;
	}

	VectorSet (forward,	1, 0, 0);
	VectorSet (right,	0, 1, 0);
	VectorSet (up,		0, 0, 1);

	// trace to forward
	VectorMA(cl.refdef.vieworg, 2048.0, forward, end);
	trace = CL_PMTraceWorld(cl.refdef.vieworg, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction > 0 && trace.fraction < 1) {
		VectorSubtract(trace.endpos, cl.refdef.vieworg, tmp);
		frontL = VectorLength(tmp);
	}

	// trace to back
	VectorMA(cl.refdef.vieworg, -2048.0, forward, end);
	trace = CL_PMTraceWorld(cl.refdef.vieworg, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction > 0 && trace.fraction < 1) {
		VectorSubtract(trace.endpos, cl.refdef.vieworg, tmp);
		backL = VectorLength(tmp);
	}

	// trace to right
	VectorMA(cl.refdef.vieworg, 2048.0, right, end);
	trace = CL_PMTraceWorld(cl.refdef.vieworg, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction > 0 && trace.fraction < 1) {
		VectorSubtract(trace.endpos, cl.refdef.vieworg, tmp);
		rightL = VectorLength(tmp);
	}

	// trace to left
	VectorMA(cl.refdef.vieworg, -2048.0, right, end);
	trace = CL_PMTraceWorld(cl.refdef.vieworg, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction > 0 && trace.fraction < 1) {
		VectorSubtract(trace.endpos, cl.refdef.vieworg, tmp);
		leftL = VectorLength(tmp);
	}

	// trace to up
	VectorMA(cl.refdef.vieworg, 2048.0, up, end);
	trace = CL_PMTraceWorld(cl.refdef.vieworg, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction > 0 && trace.fraction < 1) {
		VectorSubtract(trace.endpos, cl.refdef.vieworg, tmp);
		upL = VectorLength(tmp);
	}

	// trace to down
	VectorMA(cl.refdef.vieworg, -2048.0, up, end);
	trace = CL_PMTraceWorld(cl.refdef.vieworg, vec3_origin, vec3_origin, end, MASK_SOLID, qfalse);
	if (trace.fraction > 0 && trace.fraction < 1) {
		VectorSubtract(trace.endpos, cl.refdef.vieworg, tmp);
		downL = VectorLength(tmp);
	}

	sum = ( frontL + backL + leftL + rightL + upL + downL) / 6.0;

	if (sum <= 128.0)
		alAuxiliaryEffectSloti(efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbAlcoveEffect);

	if(sum > 128.0 && sum < 256)
		alAuxiliaryEffectSloti(efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbSmallRoomEffect);
	
	if (sum < 512.0 && sum > 256.0)
		alAuxiliaryEffectSloti(efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbMediumRoomEffect);

	if (sum >= 512.0)
		alAuxiliaryEffectSloti(efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbLargeRoomEffect);
}



void EFX_RvbUpdate (vec3_t listener_position) {
	if (!efx.on)
		return;

	// If we are not playing, use default preset
	if (cls.state != ca_active) {
		alAuxiliaryEffectSloti (efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbGenericEffect);
	}
	else if (CL_PMpointcontents (listener_position) & MASK_WATER) {
		// Check if we are underwater and update data
		alAuxiliaryEffectSloti (efx.rvbAuxSlot, AL_EFFECTSLOT_EFFECT, efx.rvbUnderwaterEffect);
	}
	else {
		EFX_GetRoomSize();
	}

	if (alGetError () != AL_NO_ERROR)
		Com_Printf (S_COLOR_RED "EFX update failed\n");
}

void EFX_RvbShutdown (void) {
	if (!efx.on)
		return;

	Com_Printf ("EFX shutdown\n");
	alDeleteAuxiliaryEffectSlots (1, &efx.rvbAuxSlot);
	alDeleteEffects (1, &efx.rvbGenericEffect);
	alDeleteEffects (1, &efx.rvbRoomEffect);
	alDeleteEffects (1, &efx.rvbUnderwaterEffect);
	alDeleteEffects (1, &efx.rvbLevelEffect);

	alDeleteEffects(1, &efx.rvbLargeRoomEffect);
	alDeleteEffects(1, &efx.rvbMediumRoomEffect);
	alDeleteEffects(1, &efx.rvbSmallRoomEffect);
	alDeleteEffects(1, &efx.rvbAlcoveEffect);

	efx.on = qfalse;
}

void EFX_RvbProcSrc (openal_channel_t *ch, ALuint source, qboolean enabled) {
	if (!enabled)
		alSource3i (source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
	else
		alSource3i (source, AL_AUXILIARY_SEND_FILTER, efx.rvbAuxSlot, 0, AL_FILTER_NULL);
}

ALuint EFX_RvbCreate (EFXEAXREVERBPROPERTIES *rvb) {
	ALuint effect;

	alGenEffects (1, &effect);

	alEffecti (effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
	alEffectf (effect, AL_EAXREVERB_DENSITY, rvb->flDensity);
	alEffectf (effect, AL_EAXREVERB_DIFFUSION, rvb->flDiffusion);
	alEffectf (effect, AL_EAXREVERB_GAIN, rvb->flGain);
	alEffectf (effect, AL_EAXREVERB_GAINHF, rvb->flGainHF);
	alEffectf (effect, AL_EAXREVERB_GAINLF, rvb->flGainLF);
	alEffectf (effect, AL_EAXREVERB_DECAY_TIME, rvb->flDecayTime);
	alEffectf (effect, AL_EAXREVERB_DECAY_HFRATIO, rvb->flDecayHFRatio);
	alEffectf (effect, AL_EAXREVERB_DECAY_LFRATIO, rvb->flDecayLFRatio);
	alEffectf (effect, AL_EAXREVERB_REFLECTIONS_GAIN, rvb->flReflectionsGain);
	alEffectf (effect, AL_EAXREVERB_REFLECTIONS_DELAY, rvb->flReflectionsDelay);
	alEffectfv (effect, AL_EAXREVERB_REFLECTIONS_PAN, rvb->flReflectionsPan);
	alEffectf (effect, AL_EAXREVERB_LATE_REVERB_GAIN, rvb->flLateReverbGain);
	alEffectf (effect, AL_EAXREVERB_LATE_REVERB_DELAY, rvb->flLateReverbDelay);
	alEffectfv (effect, AL_EAXREVERB_LATE_REVERB_PAN, rvb->flLateReverbPan);
	alEffectf (effect, AL_EAXREVERB_ECHO_TIME, rvb->flEchoTime);
	alEffectf (effect, AL_EAXREVERB_ECHO_DEPTH, rvb->flEchoDepth);
	alEffectf (effect, AL_EAXREVERB_MODULATION_TIME, rvb->flModulationTime);
	alEffectf (effect, AL_EAXREVERB_MODULATION_DEPTH, rvb->flModulationDepth);
	alEffectf (effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, rvb->flAirAbsorptionGainHF);
	alEffectf (effect, AL_EAXREVERB_HFREFERENCE, rvb->flHFReference);
	alEffectf (effect, AL_EAXREVERB_LFREFERENCE, rvb->flLFReference);
	alEffectf (effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, rvb->flRoomRolloffFactor);
	alEffecti (effect, AL_EAXREVERB_DECAY_HFLIMIT, rvb->iDecayHFLimit);

	if (alGetError () != AL_NO_ERROR)
		Com_Printf (S_COLOR_RED "EFX create filter failed\n");

	return effect;
}
