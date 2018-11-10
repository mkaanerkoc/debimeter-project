#include "TotalVolumeCalculator.h"

#include <string.h>


void tvm_Update( void *self, double value);
void tvm_OpenFile( void *self );
void tvm_ParseFile( void *self );
void tvm_ResetValue( void *self );
void tvm_CloseFile( void *self );


void init_totalVolumeManager( totalVolumeManager *self, const char* filenameParam )
{
	(*self) = (totalVolumeManager){
		0,
		0,
		0,
		NULL,
		"",
		tvm_Update,
		tvm_OpenFile,
		tvm_ParseFile,
		tvm_ResetValue,
		tvm_CloseFile
	};
	strcpy( self->filename, "/home/pi/debimeter/");
	strcat( self->filename, filenameParam );
	printf("file location  : %s\n", self->filename);
}

void tvm_Update( void *self, double value)
{
	//printf("<TotalVolumeManager> Update with %f \n",value);
	time_t updateTime = (unsigned)time(NULL);
	unsigned int timeDiffInSeconds;
	double timeDiffInDay;
	double volumeToAdd = 0;
	totalVolumeManager *this = ( totalVolumeManager* )self; 
	this->OpenFile(this);
	if( this->file != NULL )
	{
		this->ParseFile(this);
		//printf("<TVM Update> Before : Value : %lf, Reset : %d , Last Update : %d \n", this->value,this->lastReset, this->lastUpdate );
		timeDiffInSeconds = updateTime - this->lastUpdate; // DT in seconds 
		timeDiffInDay = (double)timeDiffInSeconds / 86400; // DT in Days
		volumeToAdd = timeDiffInDay * value; // m3/day * dday -> dm3
		//printf("<TVM Update> VolumeToAdd : %lf\n", volumeToAdd );
		this->value = this->value + volumeToAdd; // burada kesinlikle timeDiff in birimi ile Value nin paydasi AYNI OLMAK ZORUNDADIR..
		this->lastUpdate = updateTime;
		//printf("<TVM Update> After : Value : %lf, Reset : %d , Last Update : %d \n", this->value, this->lastReset, this->lastUpdate );
		fseek( this->file, 0, SEEK_SET );
		fprintf( this->file, "%lf;%d;%d",this->value,this->lastReset,this->lastUpdate );
	}
	this->CloseFile(this);
}

void tvm_OpenFile( void *self )
{
	totalVolumeManager *this = ( totalVolumeManager* )self; 
	this->file = fopen(this->filename,"r+");
	if( this->file == NULL )
	{
		printf("Cannot open the file with r+!! \n");
		this->file = fopen(this->filename,"w+");
		if( this->file == NULL )
		{
			printf("Cannot open the file with w+!! \n");
		}
	}
}

void tvm_ParseFile( void *self )
{
	totalVolumeManager *this = ( totalVolumeManager* )self;
	char *resetDate;
	char *lastUpdate;
	char *volumeValue;
	char lineBuffer[128];
	char lineIndex = 0;
	time_t readTime = (unsigned)time(NULL);
	struct tm * timeinfo;
	time_t tempLastReset, tempLastUpdate;
	double tempLastValue;
	if( this->file != NULL )
	{
		fseek( this->file, 0, SEEK_SET );
		fgets( lineBuffer,64, this->file );
//		printf("<TVM> parseFile : line : %s, length is: %d  \n", lineBuffer,strlen(lineBuffer) );	
		if( strlen( lineBuffer ) < 7 )
		{
			fseek( this->file, 0, SEEK_SET );
			fprintf( this->file, "0.0;%d;%d", readTime, readTime );
//			printf("First Line : 0.0;%d;%d\n",readTime,readTime);
			this->lastReset = readTime;
			this->lastUpdate = readTime;
			this->value = 0.0F;
		}
		else
		{
			if( EOF != sscanf(lineBuffer, "%lf;%d;%d", &tempLastValue, &tempLastReset, &tempLastUpdate))
			{
				this->value = tempLastValue;
				this->lastUpdate = tempLastUpdate;
				this->lastReset = tempLastReset;
			}
		}
	}
}

void tvm_ResetValue( void *self )
{
	totalVolumeManager *this = ( totalVolumeManager* )self; 
	this->file = fopen(this->filename,"w+");
	if( this->file != NULL )
	{
		fseek( this->file, 0, SEEK_SET );	
		time_t resetTime = (unsigned)time(NULL);
		fprintf( this->file, "0.0;%d;%d", resetTime, resetTime );
		fclose(this->file);	
	}
}

void tvm_CloseFile( void *self )
{
	totalVolumeManager *this = ( totalVolumeManager* )self;
	if( this->file != NULL )
	{
		fclose( this->file );
	}
}
