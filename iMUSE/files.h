#ifndef DIGITAL_IMUSE_FILES
#define DIGITAL_IMUSE_FILES

#include "imuse.h"

iMUSEInitData * files_initDataPtr;

int files_moduleInit(iMUSEInitData *initDataPtr);
int files_moduleDeinit();
int files_getSoundAddrData(int soundId);
int files_some1(soundId);
int files_getNextSound(int soundId);
int files_checkRange(int soundId);
int files_seek(int soundId, __int32 offset, int mode);
int files_read(int soundId, unsigned char *buf, int size);
int files_getBufInfo(int bufId);

#endif