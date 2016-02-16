/*
* =================================================
* FileName: TimeManager.cpp
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/07/12
*
* Last Edited on: 2015/09/03
* Last Edited by: DotCam
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/


#include "OceanPluginPrivatePCH.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sky/TimeManager.h"

// ATimeManager::ATimeManager(const class FObjectInitializer& PCIP) : Super(PCIP) 
// 	{
// 	PrimaryActorTick.bCanEverTick = true;
// 	}

//void ATimeManager::OnConstruction(const FTransform& Transform)
//	{
//	// Set Defaults
//
//
//
//	CurrentTime = StartTime;
//	PreviousTime = CurrentTime;
//
//	BaseDateTime = FDateTime(StartTime.Year, StartTime.Month, StartTime.Day, StartTime.Hour, StartTime.Minute, StartTime.Second, 0);
//
//	SecondsInMinute = 60;
//	MinutesInHour = 60;
//	MinutesInDay = 1440;
//	HoursInDay = 24;
//	DaysInMonth = BaseDateTime.DaysInMonth(StartTime.Year, StartTime.Month);
//	MonthsInYear = 12;
//
//
//
//	DayPhase = GetDayPhase();
//	MonthPhase = GetMonthPhase();
//	YearPhase = GetYearPhase();
//
//	DayOfYear = BaseDateTime.GetDayOfYear() + DayPhase;
//	}


/* --- Utility Functions --- */

// float ATimeManager::SinD(float input) 
// 	{
// 	return FMath::Sin((PI / 180.0f) * input);
// 	}
// 
// float ATimeManager::ASinD(float input) 
// 	{
// 	return (180.0f / PI) * FMath::Asin(input);
// 	}
// 
// float ATimeManager::CosD(float input) 
// 	{
// 	return FMath::Cos((PI / 180.0f) * input);
// 	}
// 
// float ATimeManager::ACosD(float input) 
// 	{
// 	return (180.0f / PI) * FMath::Acos(input);
// 	}
// 
// float ATimeManager::TanD(float input) 
// 	{
// 	return FMath::Tan((PI / 180.0f) * input);
// 	}
// 
// float ATimeManager::ATanD(float input) 
// 	{
// 	return (180.0f / PI) * FMath::Atan(input);
// 	}
// 
// 
// 
// 
// /* --- Time of Day --- */
// 
// float ATimeManager::GetElapsedDayInMinutes()
// 	{
// 	return ((CurrentTime.Hour * MinutesInHour) + CurrentTime.Minute + (CurrentTime.Second / SecondsInMinute));
// 	}
// 
// 
// // float ATimeManager::GetElapsedDayInMinutes(FTimeDate time) 
// // 	{
// // 	
// // 	}
// 
// 
// 	
// 
// 
// void ATimeManager::IncrementTime(float deltaTime) 
// 	{
// 	FTimeDate time = CurrentTime;
// 	PreviousTime = time;
// 
// 	float inncrement = deltaTime * TimeScaleMultiplier;
// 
// 	// Increment time
// 	if ((time.Second + inncrement) >= 60.0f) 
// 		{
// 		time.Second -= 60.0f;
// 		time.Minute++;
// 
// 		if (time.Minute >= 60) 
// 			{
// 			time.Minute = 0;
// 			time.Hour++;
// 
// 			if (time.Hour >= 24) 
// 				{
// 				time.Hour = 0;
// 				time.Day++;
// 				
// 				if (time.Day >= FDateTime::DaysInMonth(time.Year, time.Month)) 
// 					{
// 					time.Day = 1;
// 					time.Month++;
// 
// 					if (time.Month > 12) 
// 						{
// 						time.Month = 1;
// 						time.Year++;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	else 
// 		{
// 		time.Second += inncrement;
// 		}
// 
// 	ElapsedTime = (ElapsedTime + inncrement) > 1440.0f ? (ElapsedTime + inncrement) - 1440.0f : ElapsedTime + inncrement;
// 	CurrentTime = time;
// 	//SetTheCurrentTime(time);
// 	}
// 
// 
// FTimeDate ATimeManager::SetTheCurrentTime(FTimeDate time)
// 	{
// 	CurrentTime = time;
// 	PreviousTime = time;
// 	return CurrentTime;
// 	}
// 
// int32 ATimeManager::DaysInYear(int32 year)
// 	{
// 	return FDateTime::DaysInYear(year);
// 	}
// 
// int32 ATimeManager::DaysInMonth(int32 year, int32 month)
// 	{
// 	return FDateTime::DaysInMonth(year, month);
// 	}
// 
// float ATimeManager::GetDayOfYear()
// 	{
// 	BaseDateTime = FDateTime(CurrentTime.Year, CurrentTime.Month, CurrentTime.Day,
// 		CurrentTime.Hour, CurrentTime.Minute, FPlatformMath::FloorToInt(CurrentTime.Second),
// 		FPlatformMath::FloorToInt(FMath::Frac(CurrentTime.Second * MillisecondsInSecond)));
// 
// 	ElapsedDays = BaseDateTime.GetDayOfYear();
// 	return (float)ElapsedDays + GetDayPhase();
// 	}
// 
// float ATimeManager::GetDayPhase() 
// 	{
// 	//DayPhase = ((CurrentTime.Hour * 60) + (CurrentTime.Second / 60) + CurrentTime.Minute) / 1440;
// 	DayPhase = ElapsedTime / 1440;
// 	return DayPhase;
// 	}
// 
// 
// 
// float ATimeManager::GetYearPhase() 
// 	{
// 	YearPhase = GetDayOfYear() / DaysInYear(CurrentTime.Year);
// 	return YearPhase;
// 	}
// 
// float ATimeManager::GetTimeOffsetFromLocation(FLocation location)
// 	{
// 	float offset = location.Longitude >= 0 ? location.Longitude * 15 : (location.Longitude * 15) * -1.0f;
// 	HourOffset = FPlatformMath::FloorToInt(offset);
// 	return offset;
// 	}
// 
// 
// bool ATimeManager::IsLeapYear(int32 year) 
// 	{
// 	bool isLeap = false;
// 
// 	if ((year % 4) == 0) 
// 		{
// 		isLeap = (year % 100) == 0 ? (year % 400) == 0 : true;
// 		}
// 	return isLeap;
// 	}
// 
// 
// float ATimeManager::CalculateEoT(float eotBase) 
// 	{
// 	
// 	EoT = (cos(eotBase) * 0.258) - (sin(eotBase) * 7.416) - (cos(eotBase * 2) * 3.648) - (sin(eotBase * 2) * 9.228);
// 
// 	return EoT;
// 
// 	// Old Formulas
// 	//float base = (360 / 365) * (DaysElapsedThisYear - 81);
// 	//return (SinD(base * 2) * 9.87) - (CosD(base) * 7.53) - (SinD(base) * 1.5);
// 	}
// 
// void ATimeManager::SetHourAngle(float timeCorrection)
// 	{
// 	// Hour Angle (deg) = LocalSolarTime (LT + (TC / 60min)) + (TimeCorrection / 60min) - 12hr
// 	HRA = 15 * ((GetElapsedDayInMinutes() + (timeCorrection / 60)) - 12);
// 	}
// 
// float ATimeManager::GetSolarZenith(float latitude)
// 	{
// 	Zenith = 90.0 - SinD(SinD(Declination) * SinD(latitude) + CosD(Declination) * CosD(HRA) * CosD(latitude)) * -1.0f;
// 	return Zenith;
// 	}
// 
// 
// FRotator ATimeManager::GetCurrentSunRotation(FTimeDate time)
// 	{
// 	// Container floats
// 	//float saa; // Solar Altitude Angle
// 	float lat = LocalLocation.Latitude; // Latitude - shortened var name to keep the code readable
// 	float daysElapsed = GetDayOfYear();
// 
// 	// Local Standard Time Meridian = 15 * Hour Offset from GMT/UT
// 	//LSTM = 15 * HourOffset;  <- Now set through const float LSTM
// 
// 	// Time Correction factor = 4(Longitude - LSTM) + EoT
// 	//float timeCorrection = ((LocalLocation.Longitude - LSTM) * 4) + CalculateEoT();
// 
// 	float eotBase = 360 * ((ElapsedDays + GetDayPhase()) - 1) / 365.242;
// 
// 	CalculateEoT(eotBase);
// 
// 	LocalClockTime = SolarTime - (EoT / 60) + LongitudeCorrection + (int32)bIsDaylightSavingsActive;
// 
// 	SetHourAngle(LongitudeCorrection);
// 
// 	// Hour Angle (deg) = LocalSolarTime (LT + (TC / 60min)) + (TimeCorrection / 60min) - 12hr
// 	//HRA = 15 * ((GetElapsedDayInMinutes(CurrentTime) + (timeCorrection / 60)) - 12);
// 
// 	// The solar Declination angle
// 	//Declination = 23.45 * SinD((360 / 365) * (daysElapsed - 81));
// 
// 	Declination = (0.39795 * CosD(0.98563 * (eotBase - 173)));
// 
// 	// Solar Altitude Angle
// 	
// 
// 	// Zenith angle (pitch)
// 	Zenith = GetSolarZenith(lat);
// 
// 	// Azimuth angle calculation part one
// 	//float az = (SinD(Declination) * CosD(Location.Latitude)) - (CosD(Declination) * CosD(HRA) * SinD(Location.Latitude));
// 
// 	float az = CosD((SinD(Declination) * CosD(lat)) - (CosD(Declination) * CosD(HRA) * SinD(lat)) / CosD(90 + Zenith)) * -1.0f;
// 
// 	if (SinD(90 + Zenith) > 0)
// 		{
// 		Azimuth = 360 - az;
// 		}
// 	else
// 		{
// 		Azimuth = az;
// 		}
// 
// 	
// 	/*if (HRA < 0) {
// 	rot.Yaw = AcosD(yaw / CosD(rot.Pitch));
// 	}
// 	else
// 	rot.Yaw = AcosD(yaw / CosD(rot.Pitch)) * -1;*/
// 
// 	return FRotator( (Zenith), Azimuth, 0);
// 	}

