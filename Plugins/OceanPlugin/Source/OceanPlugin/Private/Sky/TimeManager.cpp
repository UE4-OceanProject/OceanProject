/*=================================================
* FileName: TimeManager.cpp
*
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/07/12
*
* Last Edited on: 2018/01/30
* Last Edited by: SaschaElble
*
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "Sky/TimeManager.h"
#include "Kismet/KismetMathLibrary.h"

ATimeManager::ATimeManager(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATimeManager::InitializeCalendar(FTimeDate time)
{
	time = ValidateTimeDate(time);

	InternalTime = ConvertToDateTime(time);
	OffsetUTC = FMath::Clamp(OffsetUTC, -12, 12);

	DayOfYear = InternalTime.GetDayOfYear();
	int32 leapDays = IsLeapYear(time.Year);

	if (DayOfYear >= (79 + leapDays) && DayOfYear < (265 + leapDays))
	{
		bDaylightSavingsActive = true;
	}

	OffsetDST = bAllowDaylightSavings && bDaylightSavingsActive ? 1 : 0;

	// Local Standard Time Meridian (degrees) = 15 * Hour Offset from UTC
	LSTM = 15 * OffsetUTC;

	SpanUTC = FTimespan((FMath::Abs(OffsetUTC) + OffsetDST), 0, 0);

	Latitude = FMath::Clamp(Latitude, -90.0f, 90.0f);
	Longitude = FMath::Clamp(Longitude, -180.0f, 180.0f);

	CurrentLocalTime = time;
	bIsCalendarInitialized = true;
}

FTimeDate ATimeManager::ValidateTimeDate(FTimeDate time)
{
	time.Year = FMath::Clamp<int32>(time.Year, 1, 9999);
	time.Month = FMath::Clamp<int32>(time.Month, 1, 12);
	time.Day = FMath::Clamp<int32>(time.Day, 1, GetDaysInMonth(time.Year, time.Month));
	time.Hour = FMath::Clamp<int32>(time.Hour, 0, 23);
	time.Minute = FMath::Clamp<int32>(time.Minute, 0, 59);
	time.Second = FMath::Clamp<int32>(time.Second, 0, 59);
	time.Millisecond = FMath::Clamp<int32>(time.Millisecond, 0, 999);

	return time;
}

/* --- Utility Functions --- */

// Float versions

float ATimeManager::SinD(float input)
{
	return FMath::Sin((PI / 180.0f) * input);
}

float ATimeManager::ASinD(float input)
{
	return (180.0f / PI) * FMath::Asin(input);
}

float ATimeManager::CosD(float input)
{
	return FMath::Cos((PI / 180.0f) * input);
}

float ATimeManager::ACosD(float input)
{
	return (180.0f / PI) * FMath::Acos(input);
}

float ATimeManager::TanD(float input)
{
	return FMath::Tan((PI / 180.0f) * input);
}

float ATimeManager::ATanD(float input)
{
	return (180.0f / PI) * FMath::Atan(input);
}

float ATimeManager::ATan2D(float A, float B)
{
	return (180.f / PI) * FMath::Atan2(A, B);
}


// Double versions

double ATimeManager::SinD(double input)
{
	return sin((PI / 180.0) * input);
}

double ATimeManager::ASinD(double input)
{
	return (180.0 / PI) * asin(input);
}

double ATimeManager::CosD(double input)
{
	return cos((PI / 180.0) * input);
}

double ATimeManager::ACosD(double input)
{
	return (180.0 / PI) * acos(input);
}

double ATimeManager::TanD(double input)
{
	return tan((PI / 180.0) * input);
}

double ATimeManager::ATanD(double input)
{
	return (180.0 / PI) * atan(input);
}

double ATimeManager::ATan2D(double A, double B)
{
	return (180.0 / PI) * atan2(A, B);
}




FTimeDate ATimeManager::ConvertToTimeDate(FDateTime dt)
{
	return FTimeDate(dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond(), dt.GetMillisecond());
}

FDateTime ATimeManager::ConvertToDateTime(FTimeDate td)
{
	return FDateTime(td.Year, td.Month, td.Day, td.Hour, td.Minute, td.Second, td.Millisecond);
}


/* --- Time of Day --- */

float ATimeManager::GetElapsedDayInMinutes()
{
	if (!bIsCalendarInitialized)
	{
		return 0.0f;
	}

	return (float)InternalTime.GetTimeOfDay().GetTotalMinutes();
}


void ATimeManager::IncrementTime(float deltaTime)
{
	if (!bIsCalendarInitialized)
	{
		return;
	}

	InternalTime += FTimespan::FromSeconds(deltaTime * TimeScaleMultiplier);

	if (CurrentLocalTime.Day != InternalTime.GetDay())
	{
		int32 leapDays = IsLeapYear(InternalTime.GetYear());
		DayOfYear = InternalTime.GetDayOfYear();

		if (DayOfYear >= (79 + leapDays) && DayOfYear < (265 + leapDays))
		{
			bDaylightSavingsActive = true;
		}
	}
	CurrentLocalTime = ConvertToTimeDate(InternalTime);
}


void ATimeManager::SetCurrentLocalTime(float time)
{
	float minute = FMath::Frac(time / 60) * 60;
	float second = FMath::Frac(minute) * 60;
	float millisec = FMath::Frac(second) * 1000;
	FTimeDate newTD = FTimeDate(InternalTime.GetYear(), InternalTime.GetMonth(), InternalTime.GetDay(),
		FPlatformMath::FloorToInt(time / 60), minute, second, millisec);

	InitializeCalendar(newTD);
}


int32 ATimeManager::GetDaysInYear(int32 year)
{
	return FDateTime::DaysInYear(year);
}


int32 ATimeManager::GetDaysInMonth(int32 year, int32 month)
{
	return FDateTime::DaysInMonth(year, month);
}


int32 ATimeManager::GetDayOfYear(FTimeDate time)
{
	return ConvertToDateTime(time).GetDayOfYear();
}


float ATimeManager::GetDayPhase()
{
	if (!bIsCalendarInitialized)
	{
		return 0.0f;
	}

	return GetElapsedDayInMinutes() / 1440.0;
}


float ATimeManager::GetYearPhase()
{
	if (!bIsCalendarInitialized)
	{
		return 0.0f;
	}

	return InternalTime.DaysInYear(InternalTime.GetYear()) / (InternalTime.GetDayOfYear() + (GetElapsedDayInMinutes() / 1440));
}


bool ATimeManager::IsLeapYear(int32 year)
{
	bool isLeap = false;

	if ((year % 4) == 0)
	{
		isLeap = (year % 100) == 0 ? (year % 400) == 0 : true;
	}
	return isLeap;
    }




// Moons mean longitude L'
//var LP = 218.3164477 + 481267.88123421*T - 0.0015786*T2 + T3 / 538841.0 - T4 / 65194000.0;
// Moons mean elongation
//var D = 297.8501921 + 445267.1114034*T - 0.0018819*T2 + T3 / 545868.0 - T4 / 113065000.0;
// Suns mean anomaly
//var M = 357.5291092 + 35999.0502909*T - 0.0001536*T2 + T3 / 24490000.0;
// Moons mean anomaly M'
//var MP = 134.9633964 + 477198.8675055*T + 0.0087414*T2 + T3 / 69699.0 - T4 / 14712000.0;
// phase angle
//var pa = 180.0 - D - 6.289*this._sind(MP) + 2.1*this._sind(M) - 1.274*this._sind(2 * D - MP)
//- 0.658*this._sind(2 * D) - 0.214*this._sind(2 * MP) - 0.11*this._sind(D);







FRotator ATimeManager::CalculateMoonAngle(float Latitude2, float Longitude2, float TimeZone, bool bIsDaylightSavingTime, int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds)
{
	if (!bIsCalendarInitialized)
	{
		return FRotator();
	}

	double lct = InternalTime.GetTimeOfDay().GetTotalHours();
	double elapsed = InternalTime.GetJulianDay() - JD2000;
	double utc;

	if ((lct + SpanUTC.GetHours()) > 24.0)
	{
		utc = (lct + SpanUTC.GetHours()) - 24.0;
		elapsed++;
	}
	else
	{
		utc = lct + SpanUTC.GetHours();
	}

	elapsed += (utc / 24.0);

	// Approximations for Mean Ecliptic Longitude (L), Mean Anomaly (M), and Mean Distance (F)
	// c0 + c1 are charted values * (d - d0) which is the value of elapsed (days since JD2000 - JD2000)
	// values are modulated to 360, we only care about the remainder, container doubles can be discarded
	// The approximations for years 1950 to 2050 provide accuracy of: 
	// ~ 2.57deg total deviation on Ecliptic Longitude (standard deviation 1.04deg)
	// ~ 0.81deg total deviation on Ecliptic Latitude (standard deviation 0.31deg)
	// ~ 7645km total deviation on Ecliptic Longitude (standard deviation 3388km)
	double partL = fmod(218.3164477 + (13.176396 * elapsed), 360.0);
	double partM = fmod(134.9633964 + (13.064993 * elapsed), 360.0);
	double partF = fmod(93.272 + (13.229350 * elapsed), 360.0);

	// Using the above approximations, calculate the Geocentric Ecliptic Coordinates (lambda, beta, large delta)
	double ecLong = partL + (6.289 * SinD(partM));
	double ecLat = 5.128 * SinD(partF);
	double ecDist = 385001 - (20905 * CosD(partM));

	// Convert to Observer->Sky coordinates (delta Declination, alpha Right Ascension) from Ecliptic Coordinates
	// ecLatitude, ecLongitude, and the obliquity of the ecliptic (epsilon 23.4397)
	double lunarDec = ASinD((SinD(ecLat) * CosD(EcObliquity)) + (CosD(ecLat) * SinD(EcObliquity) * SinD(ecLong)));
	double lunarRA = ATan2D((SinD(ecLong) * CosD(EcObliquity)) - (TanD(ecLat) * SinD(EcObliquity)), CosD(ecLong));

	// Calculate Sidereal time (theta) and the Hour Angle (tau)
	double lunarST = fmod((357.009 + 102.937) + (15 * (utc)) - Longitude, 360.0);
	double lunarHRA = lunarST - lunarRA;

	double lat = (double)Latitude;
	double lunarAA = ASinD((SinD(lat) * SinD(lunarDec)) + (CosD(lat) * CosD(lunarDec) * CosD(lunarHRA)));
	double lunarAz = ATan2D(SinD(lunarHRA), ((CosD(lunarHRA) * SinD(lat)) - (TanD(lunarDec) * CosD(lat))));

	lunarAA = lunarAA * -1;

	//This is put here because we need the data above. 
//"CalculateMoonPhase" doesn't actually do anything but return the calculated variable here...
	double sdist = 149598000; // distance from Earth to Sun in km
	double phi = acos(sin(TEMP_solarDec) * sin(lunarDec) + cos(TEMP_solarDec) * cos(lunarDec) * cos(TEMP_solarRA - lunarRA));
	double inc = atan2(sdist * sin(phi), ecDist - sdist * cos(phi));
	double angle = atan2(cos(TEMP_solarDec) * sin(TEMP_solarRA - lunarRA), sin(TEMP_solarDec) * cos(lunarDec) - cos(TEMP_solarDec) * sin(lunarDec) * cos(TEMP_solarRA - lunarRA));

	//Fraction, phase, angle
	LunarPhase_Fraction = (1 + cos(inc)) / 2;
	LunarPhase_Phase = 0.5 + 0.5 * inc * (angle < 0 ? -1 : 1) / PI;
	LunarPhase_Angle = angle;
	//END MoonPhase


	//for debug only:
	PartL = (float)partL;
	PartM = (float)partM;
	PartF = (float)partF;
	EcLongitude = (float)ecLong;
	EcLatitude = (float)ecLat;
	EcDistance = (float)ecDist;

	LunarElapsedDays = (float)elapsed;
	LocalClockTime = (float)lct;
	LunarAltAngle = (float)lunarAA;
	LunarAzimuth = (float)lunarAz;

	SiderealTime = (float)lunarST;
	LunarHRA = (float)lunarHRA;
	LunarDeclination = (float)lunarDec;
	LunarRightAsc = (float)lunarRA;


	return FRotator(LunarAltAngle, LunarAzimuth, 0);
}

//Returned as Fraction, phase, angle. YOU MUST CALL CALC SUN AND MOON POSITION BEFORE THIS DUE TO CHEATING!!!
FRotator ATimeManager::CalculateMoonPhase()
{
	//Fraction, phase, angle
	return FRotator(LunarPhase_Fraction, LunarPhase_Phase, LunarPhase_Angle);
}
