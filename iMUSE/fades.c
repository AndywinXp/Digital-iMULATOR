#include "fades.h"
#include "commands.h"
#include <string.h>
#include <stdio.h>

int fades_moduleInit()
{
	for (int l = 0; l < MAX_FADES; l++) {
		fades[l].status = 0;
	}
	fadesOn = 0;
	return 0;
}

int fades_moduleDeinit()
{
	return 0;
}

int fades_save(unsigned char *buffer, int sizeLeft)
{
	// We're saving 640 bytes:
	// which means 10 ints (4 bytes each) for 16 times (number of fades)
	if (sizeLeft < 640)
		return -5;
	memcpy(buffer, fades, 640);
	return 640;
}

int fades_restore(unsigned char *buffer)
{
	memcpy(fades, buffer, 640);
	return 640;
}

int fades_fadeParam(int soundId, int opcode, int destinationValue, int fadeLength)
{
	if (!soundId || fadeLength < 0)
		return -5;
	if (opcode != 0x500 && opcode != 0x600 && opcode != 0x700 && opcode != 0x800 && opcode != 0xf00 && opcode != 17)
		return -5;
	for (int l = 0; l < MAX_FADES; l++) {
		if (!fades[l].status || fades[l].sound != soundId)
			continue;
		if (fades[l].param == opcode || opcode == -1) {
			fades[l].status = 0;
		}
	}
	if (fadeLength == 0) {
		if (opcode != 0x600 || destinationValue) {
			// IMUSE_CMDS_SetParam
			handleCmds(12, soundId, opcode, destinationValue);
			return 0;
		}
		else {
			// IMUSE_CMDS_StopSound
			handleCmds(9, soundId);
			return 0;
		}
	}
	for (int l = 0; l < MAX_FADES; l++) {
		if (fades[l].status == 0)
			fades[l].sound = soundId;
		fades[l].param = opcode;
		// IMUSE_CMDS_GetParam
		fades[l].currentVal = handleCmds(13, soundId, opcode);
		fades[l].length = fadeLength;
		fades[l].counter = fadeLength;
		fades[l].slope = (destinationValue - fades[l].currentVal) / fadeLength;
			
		if ((destinationValue - fades[l].currentVal) < 0) {
			fades[l].nudge = -1;
		} else {
			fades[l].nudge = 1;
		}

		if ((destinationValue - fades[l].currentVal) < 0) {
			fades[l].slopeMod = -(fades[l].slope % fadeLength);
		} else {
			fades[l].slopeMod = fades[l].slope % fadeLength;
		}

		fades[l].modOvfloCounter = 0;
		fades[l].status = 1;
		fadesOn = 1;
	}
	printf("DigitalIMUSEFades: ERROR: fd unable to alloc fade...\n");
	return -6;
}

void fades_clearFadeStatus(int soundId, int param)
{
	for (int l = 0; l < MAX_FADES; l++) {
		if (fades[l].status == 0)
			continue;
		if (fades[l].sound != soundId)
			continue;
		if (fades[l].param == param || param == -1) {
			fades[l].status = 0;
		}
	}
}

void fades_loop()
{
	if (!fadesOn)
		return;
	fadesOn = 0;

	for (int l = 0; l < MAX_FADES; l++) {
		if (!fades[l].status)
			continue;

		fadesOn = 1;
		if (--fades[l].counter == 0) {
			fades[l].status = 0;
		}

		int currentVolume = fades[l].currentVal + fades[l].slope;
		int currentSlopeMod = fades[l].modOvfloCounter + fades[l].slopeMod;
		fades[l].modOvfloCounter += fades[l].slopeMod;

		if (fades[l].length <= currentSlopeMod) {
			fades[l].modOvfloCounter = currentSlopeMod - fades[l].length;
			currentVolume += fades[l].nudge;
		}

		if (fades[l].currentVal == currentVolume)
			continue;

		fades[l].currentVal = currentVolume;

		if (!(fades[l].counter % 6))
			continue;

		if ((fades[l].param != 0x600) || currentVolume) {
			// IMUSE_CMDS_SetParam
			handleCmds(12, fades[l].sound, fades[l].param, currentVolume);
			continue;
		} else {
			// IMUSE_CMDS_StopSound
			handleCmds(9, fades[l].sound);
		}
	}
}

void fades_moduleFree()
{
	for (int l = 0; l < MAX_FADES; l++) {
		fades[l].status = 0;
	}
	fadesOn = 0;
}

int fades_moduleDebug()
{
	printf("fadesOn: %d", fadesOn);

	for (int l = 0; l < MAX_FADES; l++) {
		printf("\n");
		printf("fades[%d]: \n", l);
		printf("status: %d \n", fades[l].status);
		printf("sound: %d \n", fades[l].sound);
		printf("param: %d \n", fades[l].param);
		printf("currentVal: %d \n", fades[l].currentVal);
		printf("counter: %d \n", fades[l].counter);
		printf("length: %d \n", fades[l].length);
		printf("slope: %d \n", fades[l].slope);
		printf("slopeMod: %d \n", fades[l].slopeMod);
		printf("modOvfloCounter: %d \n", fades[l].modOvfloCounter);
		printf("nudge: %d \n", fades[l].nudge);
	}	
	return 0;
}