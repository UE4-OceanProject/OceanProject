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
	time.Year = FMath::Clamp(time.Year, 1, 9999);
	time.Month = FMath::Clamp(time.Month, 1, 12);
	time.Day = FMath::Clamp(time.Day, 1, GetDaysInMonth(time.Year, time.Month));
	time.Hour = FMath::Clamp(time.Hour, 0, 23);
	time.Minute = FMath::Clamp(time.Minute, 0, 59);
	time.Second = FMath::Clamp(time.Second, 0, 59);
	time.Millisecond = FMath::Clamp(time.Millisecond, 0, 999);

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



FRotator ATimeManager::CalculateSunAngle()
    {
	if (!bIsCalendarInitialized)
	    {
		return FRotator();
	    }

	DayOfYear = InternalTime.GetDayOfYear() - 1;
	LSTM -= bAllowDaylightSavings && bDaylightSavingsActive ? 1 : 0;
	double lct = InternalTime.GetTimeOfDay().GetTotalHours();

	double eotBase = (DayOfYear - 81) * (360.0 / 365.242);
	double eot = (9.87 * SinD(eotBase * 2)) - (7.53 * CosD(eotBase)) - (1.5 * SinD(eotBase));

	double tcf = ((Longitude - LSTM) * 4) + eot;
	double solTime = lct + (tcf / 60);

	double hra = (solTime - 12) * 15;
	double decl = 23.452294 * SinD((360.0 / 365.242) * (DayOfYear - 81));

	double lat = (double)Latitude;
	double saa = ASinD((SinD(decl) * SinD(lat)) + (CosD(decl) * CosD(lat) * CosD(hra)));
	double saz = ACosD(((SinD(decl) * CosD(lat)) - (CosD(decl) * SinD(lat) * CosD(hra))) / CosD(saa));

	if (hra >= 0.0)
	    {
		saz = saz * -1;
	    }

	// TEMPORARY - For debug only
	LocalClockTime = (float)lct;
	EoT = (float)eot;
	TimeCorrection = (float)tcf;
	SolarTime = (float)solTime;
	SolarHRA = (float)hra;
	SolarDeclination = (float)decl;
	SolarAltAngle = (float)saa;
	SolarAzimuth = (float)saz;

	return FRotator(SolarAltAngle - 180, SolarAzimuth, 0);
    }



FRotator ATimeManager::CalculateMoonAngle()
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
	double partL = fmod(218.316 + (13.176396 * elapsed), 360.0);
	double partM = fmod(134.963 + (13.064993 * elapsed), 360.0);
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


	return FRotator(LunarAltAngle, LunarAzimuth, 0);
    }


float ATimeManager::CalculateMoonPhase()
    {
	// Last time Lunar year start = solar year start:
	double elapsed = InternalTime.GetJulianDay() - JD1900;

	double cycles = elapsed / 29.530588853;
	int32 count = FPlatformMath::FloorToInt(cycles);
	cycles -= count;
	return cycles;
    }



/* ------------------------------------------------------- */
/* --- Variations of the Lunar Calculations - Not used --- */
/* ------------------------------------------------------- */


// FRotator ATimeManager::CalculateMoonAngle()
// {
// 	double lct = (InternalTime + SpanUTC).GetTimeOfDay().GetTotalHours();
// 	double elapsedDays = InternalTime.GetJulianDay() - JD2000;  // T
// 	
// 	double sunPH = fmod(282.9404 + (4.70935E-5 * elapsedDays), 360.0);
// 	double sunMA = fmod(356.0470 + (0.9856002585 * elapsedDays), 360.0);
// 	double sunML = sunPH + sunMA;
// 
// 	// lunar orbital elements
// 	double ascNodeLong = fmod(125.1228 - (0.0529538083 * elapsedDays), 360.0);   // N
// 	double inclination = 5.1454;                                                 // i
// 	double argOfPerigee = fmod(318.0634 + (0.1643573223 * elapsedDays), 360.0);  // w
// 	double meanDistance = 60.2666;  // Earth equatorial radii                    // a
// 	double eccentricity = 0.054900;                                              // e
// 	double meanAnomaly = fmod(115.3654 + (13.0649929509 * elapsedDays), 360.0);  // M
// 
// 	//EcObliquity =
// 
// 	// Eccentric anomaly (E)
// 	//  - First iteration e0
// 	double e0 = meanAnomaly + (180.0 / PI) * eccentricity * SinD(meanAnomaly) * (1 + (eccentricity * CosD(meanAnomaly)));
// 	//  - Second iteration e1 (set e0 = e1 then repeat e1 calculation multiple times for greater accuracy, once is enough for now)
// 	double e1 = e0 - ((e0 - (180.0 / PI) * eccentricity * SinD(e0) - meanAnomaly) / (1 - (eccentricity * CosD(e0))));
// 
// 	// Compute x/y coordinates in the plane of the lunar orbit
// 	double loX = meanDistance * (CosD(e1) - eccentricity);
// 	double loY = meanDistance * sqrt(1 - (eccentricity * eccentricity)) * SinD(e1);
// 
// 	// distance and true anomaly
// 	double distance = sqrt((loX * loX) + (loY * loY));  // r
// 	double trueAnom = ATan2D(loY, loX);                 // v
// 	
// 	// need a positive result for v
// 	if (trueAnom < 0)
// 		trueAnom += 360.0;
// 	
// 	// convert to ecliptic coordinates
// 	double ecX = distance * ((CosD(ascNodeLong) * CosD(trueAnom + argOfPerigee))
// 		- (SinD(ascNodeLong) * SinD(trueAnom + argOfPerigee) * CosD(inclination)));
// 	double ecY = distance * ((SinD(ascNodeLong) * CosD(trueAnom + argOfPerigee))
// 		+ (CosD(ascNodeLong) * SinD(trueAnom + argOfPerigee) * CosD(inclination)));
// 	double ecZ = distance * SinD(trueAnom + argOfPerigee) * SinD(inclination);
// 
// 	// rotate to equatorial coordinates to account for earths tilt (x axis remains the same)
// 	double rotY = (ecY * CosD(EcObliquity)) - (ecZ * SinD(EcObliquity));
// 	double rotZ = (ecY * SinD(EcObliquity)) + (ecZ * CosD(EcObliquity));
// 	
// 	// convert to ecliptic latitude & longitude
// 	//double ecLat = 
// 	//double ecLon = ;
// 
// 	// convert to ecliptic coordinates
// 	//double  = x;
// 	//double  = (y * CosD(EcObliquity * -1)) - (z * SinD(EcObliquity * -1));
// 	//double  = (y * SinD(EcObliquity * -1)) + (z * CosD(EcObliquity * -1));
// 
// 	// convert to Right Ascension and Declination
// 	double ra = ATan2D(rotY, ecX);
// 	double decl = ASinD(rotY / distance);
// 
// 	double meanLong = fmod(argOfPerigee + meanAnomaly, 360.0);
// 
// 	// calculate Sidereal time
// // 	double gmst0 = (sunML / 15) + 12;
// // 	double sidereal = gmst0 + lct + (Longitude / 15);
// 	double moEarth = 357.529 + (0.985608 * (elapsedDays + (lct / 24)));
// 	double sidereal = fmod(moEarth + 102.937 - Longitude, 360.0);
// 	double hra = fmod(sidereal - (15 * ra), 360.0);
// 
// 	// force the following equations to use the double overloaded function
// 	double lat = (double)Latitude;
// 
// 	// convert to topocentric
// // 	double mpar = ASinD(1 / distance);
// // 	double rho = 0.99833 + (0.00167 * CosD(2 * lat));
// // 	double gcLat = lat - (0.1924 * SinD(2 * lat));
// // 	double geoHRA = sidereal - ra;
// // 	double auxG = ATanD(TanD(gcLat) / CosD(geoHRA));
// // 	double tRA = ra - (mpar * rho * CosD(gcLat) * SinD(geoHRA) / CosD(decl));
// // 	double tDecl = decl - ((mpar * rho * SinD(gcLat) * SinD(auxG - decl)) / (SinD(auxG)));
// // 	//double tHRA = 
// // 
// 	// convert HRA and Declination to x/y/z (celestial)
// 	double cX = CosD(hra) * CosD(decl);
// 	double cY = SinD(hra) * CosD(decl);
// 	double cZ = SinD(decl);
// 	
// 	// rotate to east -> west orientation, while keeping the direction & celestial zenith
// 	double xhor = (cX * SinD(lat)) - (cZ * CosD(lat));
// 	double yhor = cY;
// 	double zhor = (cX * CosD(lat)) + ((cZ * SinD(lat)));
// 	
// 	// Altitude Angle and Azimuth
// 
// 	double altitude = ASinD(zhor);
// 	double azimuth = ATan2D(yhor, xhor) + 180.0;
// 
// 	// TEMP - for debug only:
// 	LocalClockTime = (float)lct;	
// 	LunarElapsedDays = (float)elapsedDays;
// 	LunarDeclination = (float)decl;
// 	LunarRightAsc = (float)ra;
// 	SiderealTime = (float)sidereal;
// 	LunarHRA = (float)hra;
// 	LunarAltAngle = (float)altitude;
// 	LunarAzimuth = (float)azimuth;
// 
// 	return FRotator(LunarAltAngle, LunarAzimuth, 0);
// }





// FRotator ATimeManager::CalculateMoonAngle()
// {
// 	double lct = InternalTime.GetTimeOfDay().GetTotalMinutes();
// 	double elapsed = InternalTime.GetJulianDay() + (lct / 1440) - JD2000; // -0.5) -1;
// 	double ejc = elapsed / 36525.0;
// 	//double eps = 23.43929 + ((46.8150 * ejc) + ((0.00059 * ejc) * ejc)) - ((((0.001813 * ejc) * ejc) * ejc) / 3600);
// 	//double eps = 23.43929 - ((46.8150 * ejc) + (0.00059 * pow(ejc, 2)) - (0.001813 * pow(ejc, 3)));
// 	double eps = 23.43929 - ((46.8150 * ejc) + (0.00059 * pow(ejc, 2)) - (0.001813 * pow(ejc, 3)) / 3600.0);
// 
// 	// for debug only:
// 	LunarElapsedDays = (float)elapsed;
// 	//LocalClockTime = (float)lct;
// 
// 	// Approximations for Mean Ecliptic Longitude (L), Mean Anomaly (M), and Mean Distance (F)
// 	// c0 + c1 are charted values * (d - d0) which is the value of elapsed (days since JD2000 - JD2000)
// 	// values are modulated to 360, we only care about the remainder, container doubles can be discarded
// 	// The approximations for years 1950 to 2050 provide accuracy of: 
// 	// ~ 2.57deg total deviation on Ecliptic Longitude (standard deviation 1.04deg)
// 	// ~ 0.81deg total deviation on Ecliptic Latitude (standard deviation 0.31deg)
// 	// ~ 7645km total deviation on Ecliptic Longitude (standard deviation 3388km)
// 	double partL = fmod(218.316 + (13.176396 * elapsed), 360.0);
// 	double partM = fmod(134.963 + (13.064993 * elapsed), 360.0);
// 	double partF = fmod(93.272 + (13.229350 * elapsed), 360.0);
// 
// 	// TEMP - for debug only:
// 	PartL = (float)partL;
// 	PartM = (float)partM;
// 	PartF = (float)partF;
// 
// 	// Using the above approximations, calculate the Geocentric Ecliptic Coordinates (lambda, beta, large delta)
// 	double ecLong = partL + (6.289 * SinD(partM));
// 	double ecLat = 5.128 * SinD(partF);
// 	double ecDist = 385001 - (20905 * CosD(partM));
// 
// 	// TEMP - for debug only:
// 	EcLongitude = (float)ecLong;
// 	EcLatitude = (float)ecLat;
// 	EcDistance = (float)ecDist;
// 
// 	double partX = CosD(ecLat) * CosD(ecLong);
// 	double partY = (CosD(eps) * CosD(ecLat) * SinD(ecLong)) - (SinD(eps) * SinD(ecLat));
// 	double partZ = (SinD(eps) * CosD(ecLat) * SinD(ecLong)) - (CosD(eps) * SinD(ecLat));
// 	double partR = sqrt(1.0 - (partZ * partZ));
// 
// 	// Convert to Observer->Sky coordinates (delta, alpha) using the obliquity of the ecliptic (eps)
// 	double lunarDec = fmod((180.0 / PI) * ATanD(partZ / partR), 360);
// 	double lunarRA = fmod((24.0 / PI) * ATanD(partY / (partX + partR)), 360);
// 
// 	double theta0 = fmod(280.46061837 + (360.98564736629 * elapsed) + (0.000387933 * pow(ejc, 2)) - (pow(ejc, 3) / 38710000.0), 360);
// 
// 	double lunarST = theta0 + Longitude;
// 	//double lunarST = (357.529 + 102.937) (134.963 + 13.064993)
// 	double lunarHRA = lunarST - lunarRA;
// 
// 	// TEMP - for debug only:
// 	LunarDeclination = (float)lunarDec;
// 	LunarRightAsc = (float)lunarRA;
// 	SiderealTime = (float)lunarST;
// 	LunarHRA = (float)lunarHRA;
// 
// 
// 	double lat = (double)Latitude;
// 	double lunarAA = SinD((SinD(lat) * SinD(lunarDec)) + (CosD(lat) * CosD(lunarDec) * CosD(lunarHRA)));
// 	double lunarAz = TanD(SinD(lunarHRA) / ((CosD(lat) * TanD(lunarDec)) - (SinD(lat) * CosD(LunarHRA))));
// 
// 	LunarAltAngle = (float)lunarAA;
// 	LunarAzimuth = (float)lunarAz;
// 
// 	return FRotator(LunarAltAngle, LunarAzimuth, 0);
// }


// FRotator ATimeManager::CalculateMoonAngle()
// {
// 	ElapsedJD1900 = (InternalTime.GetJulianDay() - JD1900) / 36525;
// 	double actualEcObl = EcObliquity - (0.0130125 * ElapsedJD1900) - (0.00000164 * pow(ElapsedJD1900, 2)) - (0.000000503 * pow(ElapsedJD1900, 3));
// 
// 	double lonMoon = fmod(270.434164 + (481267.8831 * ElapsedJD1900), 360.0);
// 	double anomSun = fmod(358.475833 + (35999.0498 * ElapsedJD1900), 360.0);
// 	double anomMoon = fmod(296.104608 + (477198.8491 * ElapsedJD1900), 360.0);
// 	double elonMoon = fmod(350.737486 + (445267.1142 * ElapsedJD1900), 360.0);
// 	double distMoon = 11.250889 + (483202.0251 * ElapsedJD1900);  //fmod( , 360.0);
// 
// 	double ecLon = lonMoon + (6.288750 * SinD(anomMoon)) + (1.274018 * SinD((2 * elonMoon) - anomSun))
// 		+ (0.658309 * SinD(2 * elonMoon)) + (0.211316 * SinD(2 * anomMoon))
// 		- (0.185596 * SinD(anomSun)) - (0.114336 * SinD(2 * distMoon));
// 
// 	double ecLat = (5.128189 * SinD(distMoon)) + (0.280606 * SinD(lonMoon + distMoon))
// 		+ (0.277693 * SinD(anomMoon - distMoon)) + (0.173238 * SinD((2 * elonMoon) - distMoon))
// 		+ (0.055413 * SinD((2 * elonMoon) + distMoon - anomMoon))
// 		+ (0.046272 * SinD((2 * elonMoon) - distMoon - anomMoon));
// 
// 	double ecHorizPara = 0.950724 + (0.051818 * CosD(anomMoon)) + (0.009531 * CosD((2 * elonMoon) - anomMoon))
// 		+ (0.007843 * CosD(2 * elonMoon)) + (0.002824 * CosD(2 * anomMoon)) + (0.000857 * CosD((2 * elonMoon) + anomMoon));
// 
// 
// 	double lct = InternalTime.GetTimeOfDay().GetTotalMinutes();
// 	double elapsed = ((InternalTime.GetJulianDay() - 1) + (lct / 1440)) - (JD2000 - 0.5);
// 
// 	LocalClockTime = (float)lct;
// 	// for debug only:
// 	LunarElapsedDays = (float)ElapsedJD1900;
// 
// 	// TEMP - for debug only:
// 	PartL = (float)lonMoon;
// 	PartM = (float)anomMoon;
// 	PartF = (float)distMoon;
// 
// 	// TEMP - for debug only:
// 	EcLongitude = (float)ecLon;
// 	EcLatitude = (float)ecLat;
// 	EcDistance = (float)distMoon;
// 
// 	// Convert to Observer->Sky coordinates (delta, alpha) using the obliquity of the ecliptic (epsilon = 23.4397)
// 	double lunarDec = SinD((SinD(ecLat) * CosD(actualEcObl)) + (CosD(ecLat) * SinD(actualEcObl) * SinD(ecLon)));
// 	double lunarRA = TanD(((SinD(ecLon) * CosD(actualEcObl)) - (TanD(ecLat) * SinD(actualEcObl))) / CosD(ecLon));
// 
// 	double lunarST = fmod((357.529 + 102.937) + (15 * ((LocalClockTime / 60) + OffsetUTC)) - Longitude, 360.0);
// 	//double lunarST = fmod((134.963 + 13.064993) + (15 * ((LocalClockTime / 60) + OffsetUTC)) - Longitude, 360.0);
// 	double lunarHRA = lunarST - lunarRA;
// 
// 	// TEMP - for debug only:
// 	LunarDeclination = (float)lunarDec;
// 	LunarRightAsc = (float)lunarRA;
// 	SiderealTime = (float)lunarST;
// 	LunarHRA = (float)lunarHRA;
// 
// 	double lunarAA = ASinD((SinD(Latitude) * SinD(lunarDec)) + (CosD(Latitude) * CosD(lunarDec) * CosD(lunarHRA)));
// 	double lunarAz = ATan2D(SinD(lunarHRA), ((CosD(lunarHRA) * SinD(Latitude)) - (TanD(lunarDec) * CosD(Latitude))));
// 
// 	LunarAltAngle = (float)lunarAA;
// 	LunarAzimuth = (float)lunarAz;
// 
// 	return FRotator(LunarAltAngle, LunarAzimuth - 180, 0);
// }




// UN-Modified Original

// 	FRotator ATimeManager::CalculateMoonAngle()
// 	{
// 		double lct = InternalTime.GetTimeOfDay().GetTotalMinutes();
// 		double elapsed = ((InternalTime.GetJulianDay() - 1) + (lct / 1440)) - (JD2000 - 0.5);
// 
// 		LocalClockTime = (float)lct;
// 		// for debug only:
// 		LunarElapsedDays = (float)elapsed;
// 
// 		// Approximations for Mean Ecliptic Longitude (L), Mean Anomaly (M), and Mean Distance (F)
// 		// c0 + c1 are charted values * (d - d0) which is the value of elapsed (days since JD2000 - JD2000)
// 		// values are modulated to 360, we only care about the remainder, container doubles can be discarded
// 		// The approximations for years 1950 to 2050 provide accuracy of: 
// 		// ~ 2.57deg total deviation on Ecliptic Longitude (standard deviation 1.04deg)
// 		// ~ 0.81deg total deviation on Ecliptic Latitude (standard deviation 0.31deg)
// 		// ~ 7645km total deviation on Ecliptic Longitude (standard deviation 3388km)
// 		double partL = fmod(218.316 + (13.176396 * elapsed), 360.0);
// 		double partM = fmod(134.963 + (13.064993 * elapsed), 360.0);
// 		double partF = fmod(93.272 + (13.229350 * elapsed), 360.0);
// 
// 		// TEMP - for debug only:
// 		PartL = (float)partL;
// 		PartM = (float)partM;
// 		PartF = (float)partF;
// 
// 		// Using the above approximations, calculate the Geocentric Ecliptic Coordinates (lambda, beta, large delta)
// 		double ecLong = partL + (6.289 * SinD(partM));
// 		double ecLat = 5.128 * SinD(partF);
// 		double ecDist = 385001 - (20905 * CosD(partM));
// 
// 		// TEMP - for debug only:
// 		EcLongitude = (float)ecLong;
// 		EcLatitude = (float)ecLat;
// 		EcDistance = (float)ecDist;
// 
// 		// Convert to Observer->Sky coordinates (delta, alpha) using the obliquity of the ecliptic (epsilon = 23.4397)
// 		double lunarDec = ASinD((SinD(ecLat) * CosD(23.4397)) + (CosD(ecLat) * SinD(23.4397) * SinD(ecLong)));
// 		double lunarRA = 15 * ATan2D((SinD(ecLong) * CosD(23.4397)) - (TanD(ecLat) * SinD(23.4397)), CosD(ecLong));
// 
// 		//double lunarDec = SinD((SinD(ecLat) * CosD(actualEcObl)) + (CosD(ecLat) * SinD(actualEcObl) * SinD(ecLon)));
// 		//double lunarRA = TanD(((SinD(ecLon) * CosD(actualEcObl)) - (TanD(ecLat) * SinD(actualEcObl))) / CosD(ecLon));
// 
// 		double lunarST = fmod((357.529 + 102.937) + (15 * ((LocalClockTime / 60) + OffsetUTC)) - Longitude, 360.0);
// 		//double lunarST = fmod((134.963 + 13.064993) + (15 * ((LocalClockTime / 60) + OffsetUTC)) - Longitude, 360.0);
// 		double lunarHRA = lunarST - lunarRA;
// 
// 		// TEMP - for debug only:
// 		LunarDeclination = (float)lunarDec;
// 		LunarRightAsc = (float)lunarRA;
// 		SiderealTime = (float)lunarST;
// 		LunarHRA = (float)lunarHRA;
// 
// 
// 		double lunarAA = ASinD((SinD(Latitude) * SinD(lunarDec)) + (CosD(Latitude) * CosD(lunarDec) * CosD(lunarHRA)));
// 		double lunarAz = ATan2D(SinD(lunarHRA), ((CosD(lunarHRA) * SinD(Latitude)) - (TanD(lunarDec) * CosD(Latitude))));
// 
// 		LunarAltAngle = (float)lunarAA;
// 		LunarAzimuth = (float)lunarAz;
// 
// 		return FRotator(LunarAltAngle, LunarAzimuth - 180, 0);
// 	}

