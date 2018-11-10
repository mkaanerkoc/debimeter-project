/*
 * RTCManager.c
 *
 *  Created on: Jan 14, 2018
 *      Author: Kaan
 */


#include "RTCManager.h"

#ifdef GATEWAY_CODE

#include <stdio.h>
#include <time.h>

#endif

void InitializeRTCManager(void)
{

}

#ifdef GATEWAY_CODE

time_t now;
struct tm *tm;

#endif 

void RTC_SetTime(uint8_t hour,uint8_t minute, uint8_t second)
{
	#ifndef GATEWAY_CODE

	RTC_TimeTypeDef 		sTime;
	RTC_DateTypeDef 		sDate;

	if ( hour > HOURS_LIMIT )
	{

	}
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.SecondFraction = second;
	sTime.SubSeconds = 0;

	HAL_RTC_SetTime(&hrtc,&sTime,FORMAT_BIN);
	#else // gateway code

	printf("<RTCManager> RTC_SetTime() not implemented yet..\n");

	#endif 
}
void RTC_GetTime(TimeStruct_t *timeStr)
{
	#ifndef GATEWAY_CODE
	RTC_TimeTypeDef 		sTime;
	RTC_DateTypeDef 		sDate;

	//NOTE :  There is some bug in HAL_RTC_Get functions , we must call two of this functions
	HAL_RTC_GetTime(&hrtc,&sTime,FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,FORMAT_BIN);
	timeStr->hour = sTime.Hours;
	timeStr->minute = sTime.Minutes;
	timeStr->second = sTime.Seconds;
	return;
	#else // gateway code

	now = time(0);
	if((tm = localtime (&now)) != NULL)
	{
		timeStr->hour = tm->tm_hour;
		timeStr->minute = tm->tm_min;
		timeStr->second = tm->tm_sec;
	}


	return;
	#endif
}

void RTC_GetTimeString(TimeFormat_t format)
{
	#ifndef GATEWAY_CODE
	RTC_TimeTypeDef 		sTime;
	RTC_DateTypeDef 		sDate;

	HAL_RTC_GetTime(&hrtc,&sTime,FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,FORMAT_BIN);
	return;
	#else // gateway code
	return;
	#endif 
}
void RTC_SetDate(uint8_t day,uint8_t month ,uint8_t year)
{
	#ifndef GATEWAY_CODE
	RTC_TimeTypeDef 		sTime;
	RTC_DateTypeDef 		sDate;
	// aq calendar'i ya

	sDate.Date = day;
	sDate.Month = month;
	sDate.Year = year;
	HAL_RTC_SetDate(&hrtc,&sDate,FORMAT_BIN);
	return;
	#else // gateway code
	return;
	#endif 
}
void RTC_GetDate(DateStruct_t *dateStr)
{
	#ifndef GATEWAY_CODE
	RTC_TimeTypeDef 		sTime;
	RTC_DateTypeDef 		sDate;

	//NOTE :  There is some bug in HAL_RTC_Get functions , we must call two of this functions
	HAL_RTC_GetTime(&hrtc,&sTime,FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,FORMAT_BIN);
	dateStr->day = sDate.Date;
	dateStr->month = sDate.Month;
	dateStr->year = sDate.Year;
	return;
	return;
	#else // gateway code

	now = time(0);
	if((tm = localtime (&now)) != NULL)
	{
		dateStr->day = tm->tm_mday;
		dateStr->month = tm->tm_mon + 1;
		dateStr->year = tm->tm_year + 1900;
	}

	return;
	#endif 
}
void RTC_GetDateString(DateFormat_t format)
{
	#ifndef GATEWAY_CODE
	RTC_TimeTypeDef 		sTime;
	RTC_DateTypeDef 		sDate;

	HAL_RTC_GetTime(&hrtc,&sTime,FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,FORMAT_BIN);



	if(format == DATE_FORMAT_1)
	{

	}
	else if(format == DATE_FORMAT_1)
	{

	}
	return;
	#else // gateway code
	return;
	#endif 
}


