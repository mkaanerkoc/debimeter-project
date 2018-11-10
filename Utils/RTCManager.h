/*
 * RTCManager.h
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */

#ifndef RTCMANAGER_H_
#define RTCMANAGER_H_

#include "stdint.h"


/*Enums */

typedef enum TimeFormat
{
	TIME_FORMAT_1=0, /**/
	TIME_FORMAT_2=1  /**/
}TimeFormat_t;

typedef enum DateFormat
{
	DATE_FORMAT_1=0, /**/
	DATE_FORMAT_2=1  /**/
}DateFormat_t;


/*Structs */
typedef struct TimeStruct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}TimeStruct_t;

typedef struct DateStruct
{
	uint8_t day;
	uint8_t month;
	uint16_t year;
}DateStruct_t;

typedef struct DateTimeStruct
{
	TimeStruct_t time;
	DateStruct_t date;
}DateTimeStruct_t;


/*Globals */
TimeStruct_t 				timeStruct;
DateStruct_t 				dateStruct;
DateTimeStruct_t 		dateTimeStruct;




/*Function Prototypes */
void InitializeRTCManager(void);


void RTC_SetTime(uint8_t hour,uint8_t minute, uint8_t second);
void RTC_GetTime(TimeStruct_t *timeStr);
void RTC_GetTimeString(TimeFormat_t format);

void RTC_SetDate(uint8_t day,uint8_t month ,uint8_t year);
void RTC_GetDate(DateStruct_t *dateStr);
void RTC_GetDateString(DateFormat_t format);


#endif /* RTCMANAGER_H_ */
