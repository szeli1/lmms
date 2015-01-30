/*
 * aeffectx.h - simple header to allow VeSTige compilation and eventually work
 *
 * Copyright (c) 2006 Javier Serrano Polo <jasp00/at/users.sourceforge.net>
 *
 * This file is part of LMMS - http://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#ifndef AEFFECTX_H
#define AEFFECTX_H

#define CCONST(a, b, c, d)( ( ( (int) a ) << 24 ) |		\
				( ( (int) b ) << 16 ) |		\
				( ( (int) c ) << 8 ) |		\
				( ( (int) d ) << 0 ) )

const int audioMasterAutomate = 0;
const int audioMasterVersion = 1;
const int audioMasterCurrentId = 2;
const int audioMasterIdle = 3;
const int audioMasterPinConnected = 4;
// unsupported? 5
const int audioMasterWantMidi = 6;
const int audioMasterGetTime = 7;
const int audioMasterProcessEvents = 8;
const int audioMasterSetTime = 9;
const int audioMasterTempoAt = 10;
const int audioMasterGetNumAutomatableParameters = 11;
const int audioMasterGetParameterQuantization = 12;
const int audioMasterIOChanged = 13;
const int audioMasterNeedIdle = 14;
const int audioMasterSizeWindow = 15;
const int audioMasterGetSampleRate = 16;
const int audioMasterGetBlockSize = 17;
const int audioMasterGetInputLatency = 18;
const int audioMasterGetOutputLatency = 19;
const int audioMasterGetPreviousPlug = 20;
const int audioMasterGetNextPlug = 21;
const int audioMasterWillReplaceOrAccumulate = 22;
const int audioMasterGetCurrentProcessLevel = 23;
const int audioMasterGetAutomationState = 24;
const int audioMasterOfflineStart = 25;
const int audioMasterOfflineRead = 26;
const int audioMasterOfflineWrite = 27;
const int audioMasterOfflineGetCurrentPass = 28;
const int audioMasterOfflineGetCurrentMetaPass = 29;
const int audioMasterSetOutputSampleRate = 30;
// unsupported? 31
const int audioMasterGetSpeakerArrangement = 31; // deprecated in 2.4?
const int audioMasterGetVendorString = 32;
const int audioMasterGetProductString = 33;
const int audioMasterGetVendorVersion = 34;
const int audioMasterVendorSpecific = 35;
const int audioMasterSetIcon = 36;
const int audioMasterCanDo = 37;
const int audioMasterGetLanguage = 38;
const int audioMasterOpenWindow = 39;
const int audioMasterCloseWindow = 40;
const int audioMasterGetDirectory = 41;
const int audioMasterUpdateDisplay = 42;
const int audioMasterBeginEdit = 43;
const int audioMasterEndEdit = 44;
const int audioMasterOpenFileSelector = 45;
const int audioMasterCloseFileSelector = 46; // currently unused
const int audioMasterEditFile = 47; // currently unused
const int audioMasterGetChunkFile = 48; // currently unused
const int audioMasterGetInputSpeakerArrangement = 49; // currently unused

const int effFlagsHasEditor = 1;
const int effFlagsCanReplacing = 1 << 4; // very likely
const int effFlagsIsSynth = 1 << 8; // currently unused

const int effOpen = 0;
const int effClose = 1; // currently unused
const int effSetProgram = 2; // currently unused
const int effGetProgram = 3; // currently unused
const int effGetProgramName = 5; // currently unused
const int effGetParamName = 8; // currently unused
const int effSetSampleRate = 10;
const int effSetBlockSize = 11;
const int effMainsChanged = 12;
const int effEditGetRect = 13;
const int effEditOpen = 14;
const int effEditClose = 15;
const int effEditIdle = 19;
const int effEditTop = 20;
const int effProcessEvents = 25;
const int effGetEffectName = 45;
const int effGetParameterProperties = 47; // missing
const int effGetVendorString = 47;
const int effGetProductString = 48;
const int effGetVendorVersion = 49;
const int effCanDo = 51; // currently unused
const int effGetVstVersion = 58; // currently unused

const int kEffectMagic = CCONST( 'V', 's', 't', 'P' );
const int kVstLangEnglish = 1;
const int kVstMidiType = 1;

const int kVstTransportPlaying = 1 << 1;
const int kVstTransportCycleActive = 1 << 2;
const int kVstTransportChanged = 1;

/* validity flags for a VstTimeInfo structure, this info comes from the web */

const int kVstNanosValid (1 << 8);
const int kVstPpqPosValid (1 << 9);
const int kVstTempoValid (1 << 10);
const int kVstBarsValid (1 << 11);
const int kVstCyclePosValid (1 << 12);
const int kVstTimeSigValid (1 << 13);
const int kVstSmpteValid (1 << 14);
const int kVstClockValid (1 << 15);




class RemoteVstPlugin;


class VstMidiEvent
{
public:
	// 00
	int type;
	// 04
	int byteSize;
	// 08
	int deltaFrames;
	// 0c?
	int flags;
	// 10?
	int noteLength;
	// 14?
	int noteOffset;
	// 18
	char midiData[4];
	// 1c?
	char detune;
	// 1d?
	char noteOffVelocity;
	// 1e?
	char reserved1;
	// 1f?
	char reserved2;

} ;




class VstEvent
{
	char dump[sizeof( VstMidiEvent )];

} ;




class VstEvents
{
public:
	// 00
	int numEvents;
	// 04
	void *reserved;
	// 08
	VstEvent* events[1];

} ;


/* constants from http://www.rawmaterialsoftware.com/juceforum/viewtopic.php?t=3740&sid=183f74631fee71a493316735e2b9f28b */
enum Vestige2StringConstants
{
        VestigeMaxNameLen       = 64,
        VestigeMaxLabelLen      = 64,
        VestigeMaxShortLabelLen = 8,
        VestigeMaxCategLabelLen = 24,
        VestigeMaxFileNameLen   = 100
};


/* this struct taken from http://asseca.com/vst-24-specs/efGetParameterProperties.html */
struct VstParameterProperties
{
    float stepFloat;              /* float step */
    float smallStepFloat;         /* small float step */
    float largeStepFloat;         /* large float step */
    char label[VestigeMaxLabelLen];  /* parameter label */
    int32_t flags;               /* @see VstParameterFlags */
    int32_t minInteger;          /* integer minimum */
    int32_t maxInteger;          /* integer maximum */
    int32_t stepInteger;         /* integer step */
    int32_t largeStepInteger;    /* large integer step */
    char shortLabel[VestigeMaxShortLabelLen]; /* short label, recommended: 6 + delimiter */
    int16_t displayIndex;        /* index where this parameter should be displayed (starting with 0) */
    int16_t category;            /* 0: no category, else group index + 1 */
    int16_t numParametersInCategory; /* number of parameters in category */
    int16_t reserved;            /* zero */
    char categoryLabel[VestigeMaxCategLabelLen]; /* category label, e.g. "Osc 1"  */
    char future[16];              /* reserved for future use */
};


/* this enum taken from http://asseca.com/vst-24-specs/efGetParameterProperties.html */
enum VstParameterFlags
{
        kVstParameterIsSwitch                = 1 << 0,  /* parameter is a switch (on/off) */
        kVstParameterUsesIntegerMinMax       = 1 << 1,  /* minInteger, maxInteger valid */
        kVstParameterUsesFloatStep           = 1 << 2,  /* stepFloat, smallStepFloat, largeStepFloat valid */
        kVstParameterUsesIntStep             = 1 << 3,  /* stepInteger, largeStepInteger valid */
        kVstParameterSupportsDisplayIndex    = 1 << 4,  /* displayIndex valid */
        kVstParameterSupportsDisplayCategory = 1 << 5,  /* category, etc. valid */
        kVstParameterCanRamp                 = 1 << 6   /* set if parameter value can ramp up/down */
};


class AEffect
{
public:
	// Never use virtual functions!!!
	// 00-03
	int magic;
	// dispatcher 04-07
	intptr_t (* dispatcher)( AEffect * , int , int , intptr_t, void * , float );
	// process, quite sure 08-0b
	void (* process)( AEffect * , float * * , float * * , int );
	// setParameter 0c-0f
	void (* setParameter)( AEffect * , int , float );
	// getParameter 10-13
	float (* getParameter)( AEffect * , int );
	// programs 14-17
	int numPrograms;
	// Params 18-1b
	int numParams;
	// Input 1c-1f
	int numInputs;
	// Output 20-23
	int numOutputs;
	// flags 24-27
	int flags;
	// Fill somewhere 28-2b
	void *ptr1;
	void *ptr2;
	// Zeroes 2c-2f 30-33 34-37 38-3b
	char empty3[4 + 4 + 4];
	// 1.0f 3c-3f
	float unkown_float;
	// An object? pointer 40-43
	void *ptr3;
	// Zeroes 44-47
	void *user;
	// Id 48-4b
	int32_t uniqueID;
	// Don't know 4c-4f
	char unknown1[4];
	// processReplacing 50-53
	void (* processReplacing)( AEffect * , float * * , float * * , int );

} ;




class VstTimeInfo
{
public:
	// 00
	double samplePos;
	// 08
	double sampleRate;
	// unconfirmed 10
	double nanoSeconds;
	// 18
	double ppqPos;
	// 20?
	double tempo;
	// 28
	double barStartPos;
	// 30?
	double cycleStartPos;
	// 38?
	double cycleEndPos;
	// 40?
	int32_t timeSigNumerator;
	// 44?
	int32_t timeSigDenominator;

    int32_t   smpteOffset;
    int32_t   smpteFrameRate;
    int32_t   samplesToNextClock;
	// 54
	int32_t flags;

} ;



typedef intptr_t (* audioMasterCallback)( AEffect * , int32_t, int32_t, intptr_t, void * , float );


#endif
