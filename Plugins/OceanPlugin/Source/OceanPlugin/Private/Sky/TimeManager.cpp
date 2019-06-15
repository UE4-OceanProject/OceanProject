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



//
// Trigonometric functions working in degrees - this just
// makes implementing the formulas in books easier at the
// cost of some wasted multiplications.
// The 'range' function brings angles into range 0 to 360,
// and an atan2(x,y) function returns arctan in correct
// quadrant. ipart(x) returns smallest integer nearest zero
//


double ATimeManager::datan2(double y, double x) {
	double a;
	if ((x == 0) && (y == 0)) {
		return 0;
	}
	else {
		a = ATanD(y / x);
		if (x < 0) {
			a = a + 180;
		}
		if (y < 0 && x > 0) {
			a = a + 360;
		}
		return a;
	}
}


double ATimeManager::ipart(double x) {
	double a;
	if (x > 0) {
		a = FMath::FloorToFloat(x);
	}
	else {
		a = FMath::CeilToFloat(x);
	}
	return a;
}

double ATimeManager::range(double x) {
	double a;
	double b = x / 360;
	a = 360 * (b - ipart(b));
	if (a < 0) {
		a = a + 360;
	}
	return a;
}

const double RAD_TO_DEG = 180.0 / PI;
const double DEG_TO_RAD = PI / 180.0;

/** Radians to hours. */
const double RAD_TO_HOUR = 180.0 / (15.0 * PI);

/** Radians to days. */
const double RAD_TO_DAY = RAD_TO_HOUR / 24.0;

/** Astronomical Unit in km. As defined by JPL. */
const double AU = 149597870.691;

/** Earth equatorial radius in km. IERS 2003 Conventions. */
const double EARTH_RADIUS = 6378.1366;

const double TWO_PI = PI * 2.0;

/** The inverse of two times Pi. */
const double TWO_PI_INVERSE = 1.0 / TWO_PI;

/** Four times Pi. */
const double FOUR_PI = 2.0 * TWO_PI;

/** Pi divided by two. */
const double PI_OVER_TWO = PI / 2.0;

/** Length of a sidereal day in days according to IERS Conventions. */
const double SIDEREAL_DAY_LENGTH = 1.00273781191135448;

/** Julian century conversion constant = 100 * days per year. */
const double JULIAN_DAYS_PER_CENTURY = 36525.0;

/** Seconds in one day. */
const double SECONDS_PER_DAY = 86400;

/** Our default epoch. The Julian Day which represents noon on 2000-01-01. */
const double J2000 = 2451545.0;

/** Lunar cycle length in days*/
const double LUNAR_CYCLE_DAYS = 29.530588853;
/**
 * Reduces an angle in radians to the range (0 - 2 Pi).
 */
double ATimeManager::normalizeRadians(double r) {
	if (r < 0 && r >= -TWO_PI) return r + TWO_PI;
	if (r >= TWO_PI && r < FOUR_PI) return r - TWO_PI;
	if (r >= 0 && r < TWO_PI) return r;

	r -= TWO_PI * floor(r * TWO_PI_INVERSE);
	if (r < 0.) r += TWO_PI;

	return r;
}

double ATimeManager::calculateTwilightAdjustment(double moonangularRadius) const {
	double adjustment = 0.0;
	switch (twilight) {
	case HORIZON_34arcmin:
		// Rise, set, transit times, taking into account Sun/Moon angular radius (position[3]).
		// The 34' factor is the standard refraction at horizon.
		// Removing angular radius will do calculations for the center of the disk instead
		// of the upper limb.
		adjustment = -(34.0 / 60.0) * DEG_TO_RAD - moonangularRadius;
		break;
	case TWILIGHT_CIVIL:
		adjustment = -6 * DEG_TO_RAD;
		break;
	case TWILIGHT_NAUTICAL:
		adjustment = -12 * DEG_TO_RAD;
		break;
	case TWILIGHT_ASTRONOMICAL:
		adjustment = -18 * DEG_TO_RAD;
		break;
	}
	return adjustment;
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
// the analog of latitude is the declination


//Good for 1600-2299 years
FRotator ATimeManager::CalculateMoonPhase(float Latitude2, float Longitude2, float TimeZone, bool bIsDaylightSavingTime, int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds)
{
	//21Selenographic longitudes towards Mare Crisium are taken as positive.All figures here are based on geocentric coordinates, and position angles relate to the North Celestial Pole.
	//
// This page works out some values of interest to lunar-tics
// like me. The central formula for Moon position is approximate.
// Finer details like physical (as opposed to optical)
// libration and the nutation have been neglected. Formulas have
// been simplified from Meeus 'Astronomical Algorithms' (1st Ed)
// Chapter 51 (sub-earth and sub-solar points, PA of pole and
// Bright Limb, Illuminated fraction). The libration figures
// are usually 0.05 to 0.2 degree different from the results 
// given by Harry Jamieson's 'Lunar Observer's Tool Kit' DOS 
// program. Some of the code is adapted from a BASIC program 
// by George Rosenberg (ALPO).
//
// I have coded this page in a 'BASIC like' way - I intend to make 
// far more use of appropriately defined global objects when I 
// understand how they work!
//
// Written while using Netscape Gold 3.04 to keep cross-platform,
// Tested on Navigator Gold 2.02, Communicator 4.6, MSIE 5
//
// Round doesn't seem to work on Navigator 2 - and if you use
// too many nested tables, you get the text boxes not showing
// up in Netscape 3, and you get 'undefined' errors for formbox
// names on Netscape 2. Current layout seems OK.
//
// You must put all the form.name.value = variable statements
// together at the _end_ of the function, as the order of these
// statements seems to be significant.
//
// Keith Burnett
// http://www.xylem.demon.co.uk/kepler/
// kburnett@btinternet.com
// 

//
// doCalcs is written like a BASIC program - most of the calculation
// occurs in this function, although a few things are split off into
// separate functions. This function also reads the date, does some
// basic error checking, and writes all the results!
//


		//check input is valid date/time and lat/long and utc

		//
		//	Get date and time code from user, isolate the year, month, day and hours
		//	and minutes, and do some basic error checking! This only works for AD years
		//
		//g = form.num.value;
		//y = Math.floor(g / 10000);
		//m = Math.floor((g - y * 10000) / 100);
		//d = Math.floor(g - y * 10000 - m * 100);
		//bit = (g - Math.floor(g)) * 100;
		//h = Math.floor(bit);
		//min = Math.floor(bit * 100 - h * 100 + 0.5);
		//
		//	primative error checking - accounting for right number of
		//	days per month including leap years. Using bk variable to 
		//	prevent multiple alerts. See functions isleap(y) 
		//	and goodmonthday(y, m, d).
		//

	if (Year < 1600) {

		return FRotator(0, 0, 0); //"Routines are not accurate enough to work back that far - answers are meaningless!"
	}
	if (Year > 2299) {

		return FRotator(0, 0, 0); //"Routines are not accurate enough to work far into the future answers are meaningless!"
	}


	//if time is valid
//
   //		return FRotator(0,0,0); //"Wrong number of days for the month or not a leap year - type date again");
	   //}

	   //
	   //	Get the number of days since J2000.0 using day2000() function
	   //
	FDateTime CalcTime(Year, Month, Day, Hours, Minutes, Seconds);
	double JulianDay = CalcTime.GetJulianDay() + (CalcTime.GetTimeOfDay().GetTotalHours() - TimeZone) / 24.0;
	//double JulianDay = 2460476.163888888888;

	double lct = InternalTime.GetTimeOfDay().GetTotalMinutes() - CalcTime.GetTimeOfDay().FromHours(TimeZone).GetTotalMinutes() + 1;
	double elapsed = InternalTime.GetJulianDay() + (lct / 1440) - JD2000; // -0.5) -1;
	JulianDateonTTscale = JulianDay;
	double days = elapsed;
	double t = days / 36525;
	///////////////////////TEMP MASTER DEBUGGING:
	//double days = 7122.316666666667;
	//double t = 0.19499840292037418;

	//
	//	Sun formulas
	//
	//	L1	- Mean longitude
	//	M1	- Mean anomaly
	//	C1	- Equation of centre
	//	V1	- True anomaly
	//	Ec1	- Eccentricity 
	//	R1	- Sun distance
	//	Th1	- Theta (true longitude)
	//	Om1	- Long Asc Node (Omega)
	//	Lam1- Lambda (apparent longitude)
	//	Obl	- Obliquity of ecliptic
	//	Ra1	- Right Ascension
	//	Dec1- Declination
	//

	double L1 = range(280.466 + 36000.8 * t);
	double M1 = range(357.529 + 35999 * t - 0.0001536* t*t + t * t*t / 24490000);
	double C1 = (1.915 - 0.004817* t - 0.000014* t * t)* SinD(M1);
	C1 = C1 + (0.01999 - 0.000101 * t)* SinD(2 * M1);
	C1 = C1 + 0.00029 * SinD(3 * M1);
	double V1 = M1 + C1;
	double Ec1 = 0.01671 - 0.00004204 * t - 0.0000001236 * t*t;
	double R1 = 0.99972 / (1 + Ec1 * CosD(V1));
	double Th1 = L1 + C1;
	double Om1 = range(125.04 - 1934.1 * t);
	double Lam1 = Th1 - 0.00569 - 0.00478 * SinD(Om1);
	double Obl = (84381.448 - 46.815 * t) / 3600;
	double Ra1 = datan2(SinD(Th1) * CosD(Obl) - TanD(0.0)* SinD(Obl), CosD(Th1));
	double Dec1 = ASinD(SinD(0.0)* CosD(Obl) + CosD(0.0)*SinD(Obl)*SinD(Th1));

	//
	//	Moon formulas
	//
	//	F 	- Argument of latitude (F)
	//	L2 	- Mean longitude (L')
	//	Om2 - Long. Asc. Node (Om')
	//	M2	- Mean anomaly (M')
	//	D	- Mean elongation (D)
	//	D2	- 2 * D
	//	R2	- Lunar distance (Earth - Moon distance)
	//	R3	- Distance ratio (Sun / Moon)
	//	Bm	- Geocentric Latitude of Moon
	//	Lm	- Geocentric Longitude of Moon
	//	HLm	- Heliocentric longitude
	//	HBm	- Heliocentric latitude
	//	Ra2	- Lunar Right Ascension
	//	Dec2- Declination
	//

	double F = range(93.2721 + 483202 * t - 0.003403 * t* t - t * t * t / 3526000);
	double L2 = range(218.316 + 481268 * t);
	double Om2 = range(125.045 - 1934.14 * t + 0.002071 * t * t + t * t * t / 450000);
	double M2 = range(134.963 + 477199 * t + 0.008997 * t * t + t * t * t / 69700);
	double D = range(297.85 + 445267 * t - 0.00163 * t * t + t * t * t / 545900);
	double D2 = 2 * D;
	//More accurate
	double R2 = 1 + (-20905.355 * CosD(M2) - 3699.111 * CosD(D2 - M2) - 2955.968 * CosD(D2)) / 385000.56;
	//double R2 = 1 + (-20954 * CosD(M2) - 3699 * CosD(D2 - M2) - 2956 * CosD(D2)) / 385000;
	double R3 = (R2 / R1) / 379.168831168831;
	double Bm = 5.128 * SinD(F) + 0.2806 * SinD(M2 + F);
	Bm = Bm + 0.2777 * SinD(M2 - F) + 0.1732 * SinD(D2 - F);
	double Lm = 6.289 * SinD(M2) + 1.274 * SinD(D2 - M2) + 0.6583 * SinD(D2);
	Lm = Lm + 0.2136 * SinD(2 * M2) - 0.1851 * SinD(M1) - 0.1143 * SinD(2 * F);
	Lm = Lm + 0.0588 * SinD(D2 - 2 * M2);
	Lm = Lm + 0.0572* SinD(D2 - M1 - M2) + 0.0533* SinD(D2 + M2);
	Lm = Lm + L2;
	double Ra2 = datan2(SinD(Lm) * CosD(Obl) - TanD(Bm)* SinD(Obl), CosD(Lm));
	double Dec2 = ASinD(SinD(Bm)* CosD(Obl) + CosD(Bm)*SinD(Obl)*SinD(Lm));
	double HLm = range(Lam1 + 180 + (180 / PI) * R3 * CosD(Bm) * SinD(Lam1 - Lm));
	double HBm = R3 * Bm;


	//
	//	Selenographic coords of the sub Earth point
	//	This gives you the (geocentric) libration 
	//	approximating to that listed in most almanacs
	//	Topocentric libration can be up to a degree
	//	different either way
	//
	//	Physical libration ignored, as is nutation.
	//
	//	I	- Inclination of (mean) lunar orbit to ecliptic
	//	EL	- Selenographic longitude of sub Earth point
	//	EB	- Sel Lat of sub Earth point
	//	W	- angle variable
	//	X	- Rectangular coordinate
	//	Y	- Rectangular coordinate
	//	A	- Angle variable (see Meeus ch 51 for notation)
	//
	double I = 1.54242;
	double W = Lm - Om2;
	double Y = CosD(W) * CosD(Bm);
	double X = SinD(W) * CosD(Bm) * CosD(I) - SinD(Bm) * SinD(I);
	double A = datan2(X, Y);
	double EL = A - F;
	double EB = ASinD(-SinD(W) * CosD(Bm) * SinD(I) - SinD(Bm) * CosD(I));

	//
	//	Selenographic coords of sub-solar point. This point is
	//	the 'pole' of the illuminated hemisphere of the Moon
	//  and so describes the position of the terminator on the 
	//  lunar surface. The information is communicated through
	//	numbers like the colongitude, and the longitude of the
	//	terminator.
	//
	//	SL	- Sel Long of sub-solar point
	//	SB	- Sel Lat of sub-solar point
	//	W, Y, X, A	- temporary variables as for sub-Earth point
	//	Co	- Colongitude of the Sun
	//	SLt	- Selenographic longitude of terminator 
	//	riset - Lunar sunrise or set
	//
	W = range(HLm - Om2);
	Y = CosD(W) * CosD(HBm);
	X = SinD(W) * CosD(HBm) * CosD(I) - SinD(HBm) * SinD(I);
	A = datan2(X, Y);
	double SL = range(A - F);
	double SB = ASinD(-SinD(W) * CosD(HBm) * SinD(I) - SinD(HBm) * CosD(I));

	double Co = 0;
	double SLt = 0;
	if (SL < 90) {
		Co = 90 - SL;
	}
	else {
		Co = 450 - SL;
	}

	if ((Co > 90) && (Co < 270)) {
		SLt = 180 - Co;
	}
	else {
		if (Co < 90) {
			SLt = 0 - Co;
		}
		else {
			SLt = 360 - Co;
		}
	}

	//
	//	Calculate the illuminated fraction, the position angle of the bright
	//	limb, and the position angle of the Moon's rotation axis. All position
	//	angles relate to the North Celestial Pole - you need to work out the
	//  'Parallactic angle' to calculate the orientation to your local zenith.
	//

	//	Iluminated fraction
	A = CosD(Bm) * CosD(Lm - Lam1);
	double Psi = 90 - ATanD(A / FMath::Sqrt(1 - A * A));
	X = R1 * SinD(Psi);
	Y = R3 - R1 * A;
	double Il = datan2(X, Y);
	double K = (1 + CosD(Il)) / 2;

	//	PA bright limb
	X = SinD(Dec1) * CosD(Dec2) - CosD(Dec1) * SinD(Dec2) * CosD(Ra1 - Ra2);
	Y = CosD(Dec1) * SinD(Ra1 - Ra2);
	double P1 = datan2(Y, X);

	//	PA Moon's rotation axis
	//	Neglects nutation and physical libration, so Meeus' angle
	//	V is just Om2
	X = SinD(I) * SinD(Om2);
	Y = SinD(I) * CosD(Om2) * CosD(Obl) - CosD(I) * SinD(Obl);
	W = datan2(X, Y);
	A = FMath::Sqrt(X*X + Y * Y) * CosD(Ra2 - W);
	double P2 = ASinD(A / CosD(EB));



	//////////////////////////////////
	////////////OUTPUT!!!!////////////
	//////////////////////////////////

	//Sun numbers
	double daynumber = days;
	double julday = JulianDay;
	LunarElapsedDays = (float)days;

	double SunDistance = R1;
	double SunRa = Ra1 / 15;
	double SunDec = Dec1;
	//Moon numbers

	EcLongitude = (float)Lm;
	EcLatitude = (float)Bm;

	/* Lunar distance */
	double MoonDist = R2 * 60.268511;
	EcDistanceRadii = MoonDist;
	EcDistance = MoonDist * EARTH_RADIUS;

	double MoonRa = Ra2 / 15;
	LunarRightAsc = (float)MoonRa;

	double MoonDec = Dec2;
	LunarDeclination = (float)MoonDec;


	//Libration numbers

	double SelLatEarth = EB;
	double SelLongEarth = EL;

	//Sub-solar numbers
	double SelLatSun = SB;
	double SelLongSun = SL;
	double SelColongSun = Co;
	double SelLongTerm = SLt;

	//position angles and illuminated fraction
	double SelIlum = K;
	double SelPaBl = P1;
	double SelPaPole = P2;

	double test = daynumber + julday + SunDistance + SunRa + SunDec + MoonDist + MoonRa + MoonDec + SelLatEarth + SelLongEarth + SelLatSun + SelLongSun + SelColongSun + SelLongTerm + SelIlum + SelPaBl + SelPaPole;

	
	///////////////////////////
/////////////////////untested////////////////
	////////////////////////////
	
	double lat = FMath::DegreesToRadians(Latitude2);
	double lon = FMath::DegreesToRadians(Longitude2);
	double jd = JulianDay;
	//	L1	- Mean longitude
//	M1	- Mean anomaly
	double sa = L1;
	double slongitude = M1;

	///////Moon Position:
	// MOON PARAMETERS (Formulae from "Calendrical Calculations")
	double phase = normalizeRadians(
		(297.8502042 + 445267.1115168 * t - 0.00163 * t * t + t * t * t / 538841 -
			t * t * t * t / 65194000) * DEG_TO_RAD);

	// Anomalistic phase
	double anomaly = (134.9634114 + 477198.8676313 * t + .008997 * t * t +
		t * t * t / 69699 - t * t * t * t / 14712000);
	anomaly = anomaly * DEG_TO_RAD;

	// Degrees from ascending node
	double node = (93.2720993 + 483202.0175273 * t - 0.0034029 * t * t -
		t * t * t / 3526000 + t * t * t * t / 863310000);
	node = node * DEG_TO_RAD;

	double E = 1.0 - (.002495 + 7.52E-06 * (t + 1.0)) * (t + 1.0);

	// Now longitude, with the three main correcting terms of evection,
	// variation, and equation of year, plus other terms (error<0.01 deg)
	// P. Duffet's MOON program taken as reference
	double l = (218.31664563 + 481267.8811958 * t - .00146639 * t * t +
		t * t * t / 540135.03 - t * t * t * t / 65193770.4);
	l += 6.28875 * sin(anomaly) + 1.274018 * sin(2 * phase - anomaly) + .658309 * sin(2 * phase);
	l += 0.213616 * sin(2 * anomaly) - E * .185596 * sin(sa) - 0.114336 * sin(2 * node);
	l += .058793 * sin(2 * phase - 2 * anomaly) + .057212 * E * sin(2 * phase - anomaly - sa) +
		.05332 * sin(2 * phase + anomaly);
	l += .045874 * E * sin(2 * phase - sa) + .041024 * E * sin(anomaly - sa) -
		.034718 * sin(phase) -
		E * .030465 * sin(sa + anomaly);
	l += .015326 * sin(2 * (phase - node)) - .012528 * sin(2 * node + anomaly) - .01098 * sin(2 * node - anomaly) +
		.010674 * sin(4 * phase - anomaly);
	l += .010034 * sin(3 * anomaly) + .008548 * sin(4 * phase - 2 * anomaly);
	l += -E * .00791 * sin(sa - anomaly + 2 * phase) - E * .006783 * sin(2 * phase + sa) +
		.005162 * sin(anomaly - phase) + E * .005 * sin(sa + phase);
	l += .003862 * sin(4 * phase) + E * .004049 * sin(anomaly - sa + 2 * phase) +
		.003996 * sin(2 * (anomaly + phase)) + .003665 * sin(2 * phase - 3 * anomaly);
	l += E * 2.695E-3 * sin(2 * anomaly - sa) + 2.602E-3 * sin(anomaly - 2 * (node + phase));
	l += E * 2.396E-3 * sin(2 * (phase - anomaly) - sa) - 2.349E-3 * sin(anomaly + phase);
	l += E * E * 2.249E-3 * sin(2 * (phase - sa)) - E * 2.125E-3 * sin(2 * anomaly + sa);
	l += -E * E * 2.079E-3 * sin(2 * sa) + E * E * 2.059E-3 * sin(2 * (phase - sa) - anomaly);
	l += -1.773E-3 * sin(anomaly + 2 * (phase - node)) - 1.595E-3 * sin(2 * (node + phase));
	l += E * 1.22E-3 * sin(4 * phase - sa - anomaly) - 1.11E-3 * sin(2 * (anomaly + node));
	double longitude = l;

	// Let's add nutation here also
	double M1_1 = (124.90 - 1934.134 * t + 0.002063 * t * t) * DEG_TO_RAD;
	double M2_1 = (201.11 + 72001.5377 * t + 0.00057 * t * t) * DEG_TO_RAD;
	double d_0 = -.0047785 * sin(M1_1) - .0003667 * sin(M2_1);
	longitude += d_0;

	// Get accurate Moon age
	double Psin = LUNAR_CYCLE_DAYS;
	double moonAge = normalizeRadians((longitude - slongitude) * DEG_TO_RAD) * Psin / TWO_PI;

	// Now Moon parallax
	double parallax = .950724 + .051818 * cos(anomaly) + .009531 * cos(2 * phase - anomaly);
	parallax += .007843 * cos(2 * phase) + .002824 * cos(2 * anomaly);
	parallax += 0.000857 * cos(2 * phase + anomaly) + E * .000533 * cos(2 * phase - sa);
	parallax += E * .000401 * cos(2 * phase - anomaly - sa) + E * .00032 * cos(anomaly - sa) -
		.000271 * cos(phase);
	parallax += -E * .000264 * cos(sa + anomaly) - .000198 * cos(2 * node - anomaly);
	parallax += 1.73E-4 * cos(3 * anomaly) + 1.67E-4 * cos(4 * phase - anomaly);

	// So Moon distance in Earth radii is, more or less,
	double distance = 1.0 / sin(parallax * DEG_TO_RAD);
	

	// Ecliptic latitude with nodal phase (error<0.01 deg)
	l = 5.128189 * sin(node) + 0.280606 * sin(node + anomaly) + 0.277693 * sin(anomaly - node);
	l += .173238 * sin(2 * phase - node) + .055413 * sin(2 * phase + node - anomaly);
	l += .046272 * sin(2 * phase - node - anomaly) + .032573 * sin(2 * phase + node);
	l += .017198 * sin(2 * anomaly + node) + .009267 * sin(2 * phase + anomaly - node);
	l += .008823 * sin(2 * anomaly - node) + E * .008247 * sin(2 * phase - sa - node) +
		.004323 * sin(2 * (phase - anomaly) - node);
	l += .0042 * sin(2 * phase + node + anomaly) + E * .003372 * sin(node - sa - 2 * phase);
	l += E * 2.472E-3 * sin(2 * phase + node - sa - anomaly);
	l += E * 2.222E-3 * sin(2 * phase + node - sa);
	l += E * 2.072E-3 * sin(2 * phase - node - sa - anomaly);
	double latitude = l;

	double moonlongitude = longitude;
	double moonlatitude = latitude;
	
	double moondistance = distance * EARTH_RADIUS / AU;
	double moonangularRadius = atan(1737.4 / (distance * EARTH_RADIUS));

	//////////END MOON POSITION

	///////start moon posistion calc


	// Ecliptic to equatorial coordinates
	double t2 = t / 100.0;
	double tmp = t2 * (27.87 + t2 * (5.79 + t2 * 2.45));
	tmp = t2 * (-249.67 + t2 * (-39.05 + t2 * (7.12 + tmp)));
	tmp = t2 * (-1.55 + t2 * (1999.25 + t2 * (-51.38 + tmp)));
	tmp = (t2 * (-4680.93 + tmp)) / 3600.0;
	double angle = (23.4392911111111 + tmp) * DEG_TO_RAD; // obliquity

	// Add nutation in obliquity
	double d_1= .002558 * cos(M1_1) - .00015339 * cos(M2_1);

	angle += d_1 * DEG_TO_RAD;

	moonlongitude *= DEG_TO_RAD;
	moonlatitude *= DEG_TO_RAD;
	double cl = cos(moonlatitude);
	double x = moondistance * cos(moonlongitude) * cl;
	double y = moondistance * sin(moonlongitude) * cl;
	double z = moondistance * sin(moonlatitude);
	tmp = y * cos(angle) - z * sin(angle);
	z = y * sin(angle) + z * cos(angle);
	y = tmp;

	// Obtain local apparent sidereal time
	double jd0 = floor(jd - 0.5) + 0.5;
	double T0 = (jd0 - J2000) / JULIAN_DAYS_PER_CENTURY;
	double secs = (jd - jd0) * SECONDS_PER_DAY;
	double gmst = (((((-6.2e-6 * T0) + 9.3104e-2) * T0) + 8640184.812866) * T0) + 24110.54841;
	double msday =
		1.0 + (((((-1.86e-5 * T0) + 0.186208) * T0) + 8640184.812866) / (SECONDS_PER_DAY * JULIAN_DAYS_PER_CENTURY));
	gmst = (gmst + msday * secs) * (15.0 / 3600.0) * DEG_TO_RAD;
	double lst = gmst + lon;

	// Obtain topocentric rectangular coordinates
	// Set radiusAU = 0 for geocentric calculations
	// (rise/set/transit will have no sense in this case)
	double radiusAU = EARTH_RADIUS / AU;
	double correction[3] = {
			radiusAU * cos(lat) * cos(lst),
			radiusAU * cos(lat) * sin(lst),
			radiusAU * sin(lat) };
	double xtopo = x - correction[0];
	double ytopo = y - correction[1];
	double ztopo = z - correction[2];

	// Obtain topocentric equatorial coordinates
	double ra = 0.0;
	double dec = PI_OVER_TWO;
	if (ztopo < 0.0) {
		dec = -dec;
	}
	if (ytopo != 0.0 || xtopo != 0.0) {
		ra = atan2(ytopo, xtopo);
		dec = atan2(ztopo / sqrt(xtopo * xtopo + ytopo * ytopo), 1.0);
	}
	double dist = sqrt(xtopo * xtopo + ytopo * ytopo + ztopo * ztopo);

	// Hour angle
	double angh = lst - ra;

	// Obtain azimuth and geometric alt
	double sinlat = sin(lat);
	double coslat = cos(lat);
	double sindec = sin(dec), cosdec = cos(dec);
	double h = sinlat * sindec + coslat * cosdec * cos(angh);
	double alt = asin(h);
	double azy = sin(angh);
	double azx = cos(angh) * sinlat - sindec * coslat / cosdec;
	double azi = PI + atan2(azy, azx); // 0 = north

	// Get apparent elevation
	if (alt > -3 * DEG_TO_RAD) {
		double r = 0.016667 * DEG_TO_RAD * fabs(tan(PI_OVER_TWO - (alt * RAD_TO_DEG + 7.31 / (alt * RAD_TO_DEG + 4.4)) * DEG_TO_RAD));
		double refr = r * (0.28 * 1010 / (10 + 273.0)); // Assuming pressure of 1010 mb and T = 10 C
		alt = fmin(alt + refr, PI_OVER_TWO); // This is not accurate, but acceptable
	}

	tmp = calculateTwilightAdjustment(moonangularRadius);

	// Compute cosine of hour angle
	tmp = (sin(tmp) - sin(lat) * sin(dec)) / (cos(lat) * cos(dec));
	double celestialHoursToEarthTime = RAD_TO_DAY / SIDEREAL_DAY_LENGTH;

	// Make calculations for the meridian
	double transit_time1 = celestialHoursToEarthTime * normalizeRadians(ra - lst);
	double transit_time2 = celestialHoursToEarthTime * (normalizeRadians(ra - lst) - TWO_PI);
	double transit_alt = asin(sin(dec) * sin(lat) + cos(dec) * cos(lat));
	if (transit_alt > -3 * DEG_TO_RAD) {
		double r = 0.016667 * DEG_TO_RAD * fabs(tan(PI_OVER_TWO - (transit_alt * RAD_TO_DEG + 7.31 / (transit_alt * RAD_TO_DEG + 4.4)) * DEG_TO_RAD));
		double refr = r * (0.28 * 1010 / (10 + 273.0)); // Assuming pressure of 1010 mb and T = 10 C
		transit_alt = fmin(transit_alt + refr, PI_OVER_TWO); // This is not accurate, but acceptable
	}

	// Obtain the current event in time
	double transit_time = transit_time1;
	double jdToday = floor(jd - 0.5) + 0.5;
	double transitToday2 = floor(jd + transit_time2 - 0.5) + 0.5;
	// Obtain the transit time. Preference should be given to the closest event
	// in time to the current calculation time
	if (jdToday == transitToday2 && fabs(transit_time2) < fabs(transit_time1)) transit_time = transit_time2;
	double transit = jd + transit_time;

	// Make calculations for rise and set
	double rise = -1, set = -1;
	if (fabs(tmp) <= 1.0) {
		double ang_hor = fabs(acos(tmp));
		double rise_time1 = celestialHoursToEarthTime * normalizeRadians(ra - ang_hor - lst);
		double set_time1 = celestialHoursToEarthTime * normalizeRadians(ra + ang_hor - lst);
		double rise_time2 = celestialHoursToEarthTime * (normalizeRadians(ra - ang_hor - lst) - TWO_PI);
		double set_time2 = celestialHoursToEarthTime * (normalizeRadians(ra + ang_hor - lst) - TWO_PI);

		// Obtain the current events in time. Preference should be given to the closest event
		// in time to the current calculation time (so that iteration in other method will converge)
		double rise_time = rise_time1;
		double riseToday2 = floor(jd + rise_time2 - 0.5) + 0.5;
		if (jdToday == riseToday2 && fabs(rise_time2) < fabs(rise_time1)) rise_time = rise_time2;

		double set_time = set_time1;
		double setToday2 = floor(jd + set_time2 - 0.5) + 0.5;
		if (jdToday == setToday2 && fabs(set_time2) < fabs(set_time1)) set_time = set_time2;
		rise = jd + rise_time;
		set = jd + set_time;
	}


//	arr[2] = rise;
//	arr[3] = set;
//	arr[4] = transit;
//	arr[5] = transit_alt;


	////end moon postition calc


	LunarDeclination = dec * RAD_TO_DEG;
		LunarRightAsc = ra * RAD_TO_DEG;
		EcLongitude = moonlongitude;
		EcLatitude = moonlatitude;
		EcDistance = moondistance;
		LunarElapsedDays = days;
		LocalClockTime = lst;
		LunarAltAngle = alt * RAD_TO_DEG;
		LunarAzimuth = azi * RAD_TO_DEG;
		SiderealTime = lst;
		LunarHRA = angh;
		LunarParallax = parallax;
		LunarAngularRadius = moonangularRadius;
		LunarAdjustedAngularRadious = tmp;


	return FRotator(LunarAltAngle, LunarAzimuth, 0);
}


//TIME IS WRONG!
FRotator ATimeManager::CalculateMoonAnglefINAL(float Latitude2, float Longitude2, float TimeZone, bool bIsDaylightSavingTime, int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds)
{

};