#include "files.h"
#include "imuse.h"
#include <stdio.h>

// Validated
int files_moduleInit(iMUSEInitData *initDataPtr) {
	files_initDataPtr = initDataPtr;
	if (!initDataPtr)
		return -1;
	return 0;
}

// Validated
int files_moduleDeinit() {
	return 0;
}

// Validated 
// In the assembly this is written as "return files_initDataPtr(soundId)", but it's actually
// pointing at the getSoundDataAddr function which is the first thing in the struct
int files_getSoundAddrData(int soundId) {
	if ((soundId != 0 && soundId < 0xFFFFFFF0) && files_initDataPtr != NULL) {
		if (files_initDataPtr->getSoundDataAddr) {
			return files_initDataPtr->getSoundDataAddr(soundId);
		}
	}
	printf("ERR: soundAddrFunc failure");
	return 0;
}

// TODO: what is this func_some1?
int files_some1(soundId) {
	if ((soundId != 0 && soundId < 0xFFFFFFF0) && files_initDataPtr != NULL) {
		if (files_initDataPtr->func_some1) {
			return files_initDataPtr->func_some1(soundId);
		}
	}
	return 0;
}


// Validated
int files_getNextSound(int soundId) {
	int foundSoundId = 0;
	do {
		foundSoundId = wave_getNextSound(foundSoundId);
		if (!foundSoundId)
			return -1;
	} while (foundSoundId != soundId);
	return 2;
}

// Validated
int files_checkRange(int soundId) {
	return (soundId != 0 && soundId < 0xFFFFFFF0);
}

// Validated
int files_seek(int soundId, __int32 offset, int mode) {
	if (soundId != 0 && soundId < 0xFFFFFFF0) {
		if (files_initDataPtr->seekFunc != NULL) {
			return files_initDataPtr->seekFunc(soundId, offset, mode);
		}
	}
	printf("ERR: seekFunc failure");
	return 0;
}

// Validated
int files_read(int soundId, unsigned char *buf, int size) {
	if (soundId != 0 && soundId < 0xFFFFFFF0) {
		if (files_initDataPtr->readFunc != NULL) {
			return files_initDataPtr->readFunc(soundId, buf, size);
		}
	}
	printf("ERR: readFunc failure");
	return 0;
}

// Validated
int files_getBufInfo(int bufId) {
	if (bufId && files_initDataPtr->bufInfoFunc != NULL) {
		return files_initDataPtr->bufInfoFunc(bufId);
	}
	printf("ERR: bufInfoFunc failure");
	return 0;
}
