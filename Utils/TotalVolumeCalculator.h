#ifndef TOTAL_VOLUME_CALCULATOR_H
#define TOTAL_VOLUME_CALCULATOR_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define MAX_FILE_NAME_LEN 	256


void tvm_Update( void *self, double value );
void tvm_OpenFile( void *self );
void tvm_ParseFile( void *self );
void tvm_ResetValue( void *self );
void tvm_CloseFile( void *self );


typedef struct totalVolumeManager
{
	time_t 	lastUpdate;
	time_t 	lastReset;
	double 	 value;
	FILE 	*file;
  char filename[MAX_FILE_NAME_LEN];
	void(*Update)(void* self,double addValue);
	void(*OpenFile)(void* self);
	void(*ParseFile)(void* self);
	void(*ResetValue)( void *self);
	void(*CloseFile)(void* self);
}totalVolumeManager;


void init_totalVolumeManager(totalVolumeManager *self, const char* filenameParam );



#endif
