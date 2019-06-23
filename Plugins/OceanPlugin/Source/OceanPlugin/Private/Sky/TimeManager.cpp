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


FRotator ATimeManager::CalculateSunMoonAngle(float Latitude2, float Longitude2, float TimeZone, bool bIsDaylightSavingTime, int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds)
{
	if (Year == 0 || Month == 0 || Day == 0)
	{
		return FRotator(0, 0, 0);
	}
	//double LatitudeHard = 29.8907;
	//double LongitudeHard = -97.9115;
	InitSunMoonCalculator(Year, Month, Day, Hours, Minutes, Seconds, Longitude * DEG_TO_RAD, Latitude * DEG_TO_RAD, TimeZone);
	calcSunAndMoon();
	LunarAngularRadius = 0;// moon->angularRadius;
	LunarDeclination = moon->declination * RAD_TO_DEG;
	EcDistance = moon->distance * AU;
	EcLatitude = 0;// moon->eclipticLatitude;
	EcLongitude = 0;// moon->eclipticLongitude;
	LunarRightAsc = moon->rightAscension * RAD_TO_DEG;
	LunarAltAngle = moon->elevation * RAD_TO_DEG;
	LunarAzimuth = moon->azimuth * RAD_TO_DEG;

	LunarHRA = 0;


	return FRotator(moon->azimuth * RAD_TO_DEG, moon->elevation * RAD_TO_DEG, moonIll);
};
FEphemeris ATimeManager::CalculateMoonAngle(float Latitude2, float Longitude2, float TimeZone, bool bIsDaylightSavingTime, 
	int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds,
	float &moonIll2, float &moonAngle, float &moonPhaseAngle, float &moonPhaseShadowAngle, float &moonBL2, float &moonPar2, float &SiderealTime2)
{
	if (Year == 0 || Month == 0 || Day == 0)
	{
		return FEphemeris();
	}
	InitSunMoonCalculator(Year, Month, Day, Hours, Minutes, Seconds, Longitude * DEG_TO_RAD, Latitude * DEG_TO_RAD, TimeZone);
	calcSunAndMoon();
	//return TArray<double> {lp, bp, p, bl, par, lst};
	TArray<double> Test = getMoonDiskOrientationAngles();


	moonIll2= (1 - FMath::Cos(moonAge / LUNAR_CYCLE_DAYS * 2 * PI)) / 2;
	moonP = Test[2];
	moonBL2 = Test[3];
	moonPar2 = Test[4];
	SiderealTime2 = Test[5];
	moonPhaseAngle = ((FMath::Acos(-FMath::Cos(moonAge	/ LUNAR_CYCLE_DAYS * 2 * PI))));
	moonAngle = (-(moonP - moonPar2));
	moonPhaseShadowAngle = (-(moonBL2 - moonPar2));

	return *moon;
};
FEphemeris ATimeManager::CalculateSunAngle(float Latitude2, float Longitude2, float TimeZone, bool bIsDaylightSavingTime, int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds)
{
	InitSunMoonCalculator(Year, Month, Day, Hours, Minutes, Seconds, Longitude * DEG_TO_RAD, Latitude * DEG_TO_RAD, TimeZone);
	calcSunAndMoon();

	return *sun;
};

/**
	 * Main constructor for Sun/Moon calculations. Time should be given in
	 * Universal Time (UT), observer angles in radians.
	 * @param year The year.
	 * @param month The month.
	 * @param day The day.
	 * @param h The hour.
	 * @param m Minute.
	 * @param s Second.
	 * @param obsLon Longitude for the observer.
	 * @param obsLat Latitude for the observer.
	 * @throws Exception If the date does not exists.
	 */
void ATimeManager::InitSunMoonCalculator(int year, int month, int day, int h, int m, int s,	double obsLonIn, double obsLatIn, float TimeZone) {


	///fixed below comment! now UTC corrected. leaving here for now
	///////////this is NOT UTC CORRECTED!!! RIGHT NOW WE ARE MANUALLY CONVERTING FROM LOCAL TO UTC TIME!!!!////////

	//bool julian = false;
	//if (year < 1582 || (year == 1582 && month <= 10) || (year == 1582 && month == 10 && day < 15)) julian = true;
	//int D = day;
	//int M = month;
	//int Y = year;
	//if (M < 3)
	//{
	//	Y--;
	//	M += 12;
	//}
	//int A = Y / 100;
	//int B = julian ? 0 : 2 - A + A / 4;

	//double dayFraction = (h + (m + (s / 60.0)) / 60.0) / 24.0;

	//double jd = dayFraction + (int)(365.25 * (Y + 4716)) + (int)(30.6001 * (M + 1)) + D + B - 1524.5;

	//if (jd < 2299160.0 && jd >= 2299150.0)
	//{
	//	return;
	//}


	this->obsLon = obsLonIn;
	this->obsLat = obsLatIn;

	//FDateTime CalcTime;
	////Here we adjust for timezone to convert to UTC time
	////YYYY - mm - ddTHH:MM:SS(.ssss)(Z | +th:tm | -th : tm)

	//FString Iso8601Time =
	//	FString::FromInt(year) + " " + FString::FromInt(month) + " " + FString::FromInt(day) +
	//	"T" + FString::FromInt(h) + " " + FString::FromInt(m) + " " + FString::FromInt(s) +
	//	FString::FromInt(TimeZone) + " 00";
	//
	//FDateTime::ParseIso8601(*Iso8601Time, CalcTime);

	//FString Test = CalcTime.ToString();
	FDateTime CalcTime(year, month, day, h, m, s, 0);

	// adjust for the timezone (bringing the DateTime into UTC)
	int TzHour = TimeZone;
	int TzMinute = 0;
	int32 TzOffsetMinutes = (TzHour < 0) ? TzHour * 60 - TzMinute : TzHour * 60 + TzMinute;
	CalcTime -= FTimespan::FromMinutes(TzOffsetMinutes);


	double jd = CalcTime.GetJulianDay() + CalcTime.GetTimeOfDay().GetTotalHours() / 24.0;

	TTminusUT = 0;
	if (CalcTime.GetYear() > -600 && CalcTime.GetYear() < 2200) {
		double x = CalcTime.GetYear() + (CalcTime.GetMonth() - 1 + CalcTime.GetDay() / 30.0) / 12.0;
		double x2 = x * x, x3 = x2 * x, x4 = x3 * x;
		if (CalcTime.GetYear() < 1600) {
			TTminusUT = 10535.328003326353 - 9.995238627481024 * x + 0.003067307630020489 * x2 - 7.76340698361363E-6 * x3 + 3.1331045394223196E-9 * x4 +
				8.225530854405553E-12 * x2 * x3 - 7.486164715632051E-15 * x4 * x2 + 1.9362461549678834E-18 * x4 * x3 - 8.489224937827653E-23 * x4 * x4;
		}
		else {
			TTminusUT = -1027175.3477559977 + 2523.256625418965 * x - 1.885686849058459 * x2 + 5.869246227888417E-5 * x3 + 3.3379295816475025E-7 * x4 +
				1.7758961671447929E-10 * x2 * x3 - 2.7889902806153024E-13 * x2 * x4 + 1.0224295822336825E-16 * x3 * x4 - 1.2528102370680435E-20 * x4 * x4;
		}
	}

	//double jd = toJulianDay(year, month, day, h, m, s);
	setUTDate(jd);
}

double  ATimeManager::toJulianDay(int year, int month, int day, int h, int m, int s) {
	// The conversion formulas are from Meeus, chapter 7.
	bool julian = false; // Use Gregorian calendar
	if (year < 1582 || (year == 1582 && month <= 10) || (year == 1582 && month == 10 && day < 15)) julian = true;
	int D = day;
	int M = month;
	int Y = year;
	if (M < 3) {
		Y--;
		M += 12;
	}
	int A = Y / 100;
	int B = julian ? 0 : 2 - A + A / 4;

	double dayFraction = (h + (m + (s / 60.0)) / 60.0) / 24.0;
	double jd = dayFraction + (int)(365.25 * (Y + 4716)) + (int)(30.6001 * (M + 1)) + D + B - 1524.5;

	if (jd < 2299160.0 && jd >= 2299150.0)
		//need to make this better somehow
		return 0.0;

	return jd;
}

/**
 * Sets the rise/set times to return. Default is for the local horizon.
 * @param t The Twilight.
 */
void  ATimeManager::setTwilight(TWILIGHT tIn) {
	this->twilight = tIn;
}

void  ATimeManager::setUTDate(double jd) {
	this->jd_UT = jd;
	this->t = (jd + TTminusUT / SECONDS_PER_DAY - J2000) / JULIAN_DAYS_PER_CENTURY;
}

/** Calculates everything for the Sun and the moon-> */
void  ATimeManager::calcSunAndMoon() {
	double jd = this->jd_UT;

	// First the Sun
	sun = doCalc(getSun(), false);

	int niter = 3; // Number of iterations to get accurate rise/set/transit times
	sun->rise = obtainAccurateRiseSetTransit(sun->rise, EVENT::RISE, niter, true);
	sun->set = obtainAccurateRiseSetTransit(sun->set, EVENT::SET, niter, true);
	sun->transit = obtainAccurateRiseSetTransit(sun->transit, EVENT::TRANSIT, niter, true);
	if (sun->transit == -1) {
		sun->transitElevation = 0;
	}
	else {
		// Update Sun's maximum elevation
		setUTDate(sun->transit);
		sun->transitElevation = doCalc(getSun(), false)->transitElevation;
	}

	// Now Moon
	setUTDate(jd);

	moon = doCalc(getMoon(), false);
	//moon = doCalc(getMoon(), false);
	double ma = moonAge;

	niter = 5; // Number of iterations to get accurate rise/set/transit times
	moon->rise = obtainAccurateRiseSetTransit(moon->rise, EVENT::RISE, niter, false);
	moon->set = obtainAccurateRiseSetTransit(moon->set, EVENT::SET, niter, false);
	moon->transit = obtainAccurateRiseSetTransit(moon->transit, EVENT::TRANSIT, niter, false);
	if (moon->transit == -1) {
		moon->transitElevation = 0;
	}
	else {
		// Update Moon's maximum elevation
		setUTDate(moon->transit);
		getSun();
		moon->transitElevation = doCalc(getMoon(), false)->transitElevation;
	}
	setUTDate(jd);
	moonAge = ma;

	// Compute illumination phase percentage for the Moon (do not use for other bodies!)
	double dlon = moon->rightAscension - sun->rightAscension;
	double elong = FMath::Acos(FMath::Sin(sun->declination) * FMath::Sin(moon->declination) +
		FMath::Cos(sun->declination) * FMath::Cos(moon->declination) * FMath::Cos(dlon));
	moon->illuminationPhase = 100 * (1.0 - FMath::Cos(elong)) * 0.5;
}



TArray<double>  ATimeManager::getSun() {
	double L = 0.0, R = 0.0;
	double t2 = t * 0.01;
	//DOUBLE CHECK THIS!!
	for (int i = 0; i < sizeof(sun_elements) / sizeof(sun_elements[0]); i++) {
	//for (int i = 0; i < sun_elements.length; i++) {
		double v = sun_elements[i][2] + sun_elements[i][3] * t2;
		double u = normalizeRadians(v);
		L = L + sun_elements[i][0] * FMath::Sin(u);
		R = R + sun_elements[i][1] * FMath::Cos(u);
	}

	double lon = normalizeRadians(4.9353929 + normalizeRadians(62833.196168 * t2) + L / 10000000.0) * RAD_TO_DEG;
	double sdistance = 1.0001026 + R / 10000000.0;

	// Now subtract aberration. Note light-time is not corrected, negligible for Sun
	lon += -.00569;

	double slongitude = lon; // apparent longitude (error<0.001 deg)
	double slatitude = 0; // Sun's ecliptic latitude is always negligible

	return TArray<double> {slongitude, slatitude, sdistance, FMath::Atan(696000 / (AU * sdistance))};
}

TArray<double> ATimeManager::getMoon() {
	// MOON PARAMETERS (Formulae from "Calendrical Calculations")
	double phase = normalizeRadians((297.8502042 + 445267.1115168 * t - 0.00163 * t * t + t * t * t / 538841 - t * t * t * t / 65194000) * DEG_TO_RAD);

	// Anomalistic phase
	double anomaly = (134.9634114 + 477198.8676313 * t + .008997 * t * t + t * t * t / 69699 - t * t * t * t / 14712000);
	anomaly = anomaly * DEG_TO_RAD;

	// Degrees from ascending node
	double node = (93.2720993 + 483202.0175273 * t - 0.0034029 * t * t - t * t * t / 3526000 + t * t * t * t / 863310000);
	node = node * DEG_TO_RAD;

	double E = 1.0 - (.002495 + 7.52E-06 * (t + 1.0)) * (t + 1.0);

	// Solar anomaly
	double sanomaly = (357.5291 + 35999.0503 * t - .0001559 * t * t - 4.8E-07 * t * t * t) * DEG_TO_RAD;

	// Now longitude, with the three main correcting terms of evection,
	// variation, and equation of year, plus other terms (error<0.01 deg)
	// P. Duffet's MOON program taken as reference
	double l = (218.31664563 + 481267.8811958 * t - .00146639 * t * t + t * t * t / 540135.03 - t * t * t * t / 65193770.4);
	l += 6.28875 * FMath::Sin(anomaly) + 1.274018 * FMath::Sin(2 * phase - anomaly) + .658309 * FMath::Sin(2 * phase);
	l += 0.213616 * FMath::Sin(2 * anomaly) - E * .185596 * FMath::Sin(sanomaly) - 0.114336 * FMath::Sin(2 * node);
	l += .058793 * FMath::Sin(2 * phase - 2 * anomaly) + .057212 * E * FMath::Sin(2 * phase - anomaly - sanomaly) + .05332 * FMath::Sin(2 * phase + anomaly);
	l += .045874 * E * FMath::Sin(2 * phase - sanomaly) + .041024 * E * FMath::Sin(anomaly - sanomaly) - .034718 * FMath::Sin(phase) - E * .030465 * FMath::Sin(sanomaly + anomaly);
	l += .015326 * FMath::Sin(2 * (phase - node)) - .012528 * FMath::Sin(2 * node + anomaly) - .01098 * FMath::Sin(2 * node - anomaly) + .010674 * FMath::Sin(4 * phase - anomaly);
	l += .010034 * FMath::Sin(3 * anomaly) + .008548 * FMath::Sin(4 * phase - 2 * anomaly);
	l += -E * .00791 * FMath::Sin(sanomaly - anomaly + 2 * phase) - E * .006783 * FMath::Sin(2 * phase + sanomaly) + .005162 * FMath::Sin(anomaly - phase) + E * .005 * FMath::Sin(sanomaly + phase);
	l += .003862 * FMath::Sin(4 * phase) + E * .004049 * FMath::Sin(anomaly - sanomaly + 2 * phase) + .003996 * FMath::Sin(2 * (anomaly + phase)) + .003665 * FMath::Sin(2 * phase - 3 * anomaly);
	l += E * 2.695E-3 * FMath::Sin(2 * anomaly - sanomaly) + 2.602E-3 * FMath::Sin(anomaly - 2 * (node + phase));
	l += E * 2.396E-3 * FMath::Sin(2 * (phase - anomaly) - sanomaly) - 2.349E-3 * FMath::Sin(anomaly + phase);
	l += E * E * 2.249E-3 * FMath::Sin(2 * (phase - sanomaly)) - E * 2.125E-3 * FMath::Sin(2 * anomaly + sanomaly);
	l += -E * E * 2.079E-3 * FMath::Sin(2 * sanomaly) + E * E * 2.059E-3 * FMath::Sin(2 * (phase - sanomaly) - anomaly);
	l += -1.773E-3 * FMath::Sin(anomaly + 2 * (phase - node)) - 1.595E-3 * FMath::Sin(2 * (node + phase));
	l += E * 1.22E-3 * FMath::Sin(4 * phase - sanomaly - anomaly) - 1.11E-3 * FMath::Sin(2 * (anomaly + node));
	double longitude = l;

	// Get accurate Moon age
	double Psin = LUNAR_CYCLE_DAYS; //XXX Using constant
	moonAge = normalizeRadians(longitude * DEG_TO_RAD - sun->eclipticLongitude) * Psin / TWO_PI;

	// Now Moon parallax
	double parallax = .950724 + .051818 * FMath::Cos(anomaly) + .009531 * FMath::Cos(2 * phase - anomaly);
	parallax += .007843 * FMath::Cos(2 * phase) + .002824 * FMath::Cos(2 * anomaly);
	parallax += 0.000857 * FMath::Cos(2 * phase + anomaly) + E * .000533 * FMath::Cos(2 * phase - sanomaly);
	parallax += E * .000401 * FMath::Cos(2 * phase - anomaly - sanomaly) + E * .00032 * FMath::Cos(anomaly - sanomaly) - .000271 * FMath::Cos(phase);
	parallax += -E * .000264 * FMath::Cos(sanomaly + anomaly) - .000198 * FMath::Cos(2 * node - anomaly);
	parallax += 1.73E-4 * FMath::Cos(3 * anomaly) + 1.67E-4 * FMath::Cos(4 * phase - anomaly);

	// So Moon distance in Earth radii is, more or less,
	double distance = 1.0 / FMath::Sin(parallax * DEG_TO_RAD);

	// Ecliptic latitude with nodal phase (error<0.01 deg)
	l = 5.128189 * FMath::Sin(node) + 0.280606 * FMath::Sin(node + anomaly) + 0.277693 * FMath::Sin(anomaly - node);
	l += .173238 * FMath::Sin(2 * phase - node) + .055413 * FMath::Sin(2 * phase + node - anomaly);
	l += .046272 * FMath::Sin(2 * phase - node - anomaly) + .032573 * FMath::Sin(2 * phase + node);
	l += .017198 * FMath::Sin(2 * anomaly + node) + .009267 * FMath::Sin(2 * phase + anomaly - node);
	l += .008823 * FMath::Sin(2 * anomaly - node) + E * .008247 * FMath::Sin(2 * phase - sanomaly - node) + .004323 * FMath::Sin(2 * (phase - anomaly) - node);
	l += .0042 * FMath::Sin(2 * phase + node + anomaly) + E * .003372 * FMath::Sin(node - sanomaly - 2 * phase);
	l += E * 2.472E-3 * FMath::Sin(2 * phase + node - sanomaly - anomaly);
	l += E * 2.222E-3 * FMath::Sin(2 * phase + node - sanomaly);
	l += E * 2.072E-3 * FMath::Sin(2 * phase - node - sanomaly - anomaly);
	double latitude = l;

	return TArray<double> {longitude, latitude, distance * EARTH_RADIUS / AU, FMath::Atan(1737.4 / (distance * EARTH_RADIUS))};
}

FEphemeris* ATimeManager::doCalc(TArray<double> pos, bool geocentric) {
	// Correct for nutation in longitude and obliquity
	double M1 = (124.90 - 1934.134 * t + 0.002063 * t * t) * DEG_TO_RAD;
	double M2 = (201.11 + 72001.5377 * t + 0.00057 * t * t) * DEG_TO_RAD;
	double dLon = -.0047785 * FMath::Sin(M1) - .0003667 * FMath::Sin(M2);
	double dLat = .002558 * FMath::Cos(M1) - .00015339 * FMath::Cos(M2);
	pos[0] += dLon;
	pos[1] += dLat;

	// Ecliptic to equatorial coordinates
	double t2 = this->t / 100.0;
	double tmp = t2 * (27.87 + t2 * (5.79 + t2 * 2.45));
	tmp = t2 * (-249.67 + t2 * (-39.05 + t2 * (7.12 + tmp)));
	tmp = t2 * (-1.55 + t2 * (1999.25 + t2 * (-51.38 + tmp)));
	tmp = (t2 * (-4680.93 + tmp)) / 3600.0;
	double angle = (23.4392911111111 + tmp) * DEG_TO_RAD; // mean obliquity

	pos[0] *= DEG_TO_RAD;
	pos[1] *= DEG_TO_RAD;
	double cl = FMath::Cos(pos[1]);
	double x = pos[2] * FMath::Cos(pos[0]) * cl;
	double y = pos[2] * FMath::Sin(pos[0]) * cl;
	double z = pos[2] * FMath::Sin(pos[1]);
	tmp = y * FMath::Cos(angle) - z * FMath::Sin(angle);
	z = y * FMath::Sin(angle) + z * FMath::Cos(angle);
	y = tmp;

	if (geocentric) return new FEphemeris(0, 0, -1, -1, -1, -1, normalizeRadians(FMath::Atan2(y, x)),
		FMath::Atan2(z / FMath::Sqrt(x * x + y * y), 1.0), FMath::Sqrt(x * x + y * y + z * z), pos[0], pos[1], pos[3]);

	// Obtain local apparent sidereal time
	double jd0 = FMath::FloorToDouble(jd_UT - 0.5) + 0.5;
	double T0 = (jd0 - J2000) / JULIAN_DAYS_PER_CENTURY;
	double secs = (jd_UT - jd0) * SECONDS_PER_DAY;
	double gmst = (((((-6.2e-6 * T0) + 9.3104e-2) * T0) + 8640184.812866) * T0) + 24110.54841;
	double msday = 1.0 + (((((-1.86e-5 * T0) + 0.186208) * T0) + 8640184.812866) / (SECONDS_PER_DAY * JULIAN_DAYS_PER_CENTURY));
	gmst = (gmst + msday * secs) * (15.0 / 3600.0) * DEG_TO_RAD;
	double lst = gmst + obsLon;

	// Obtain topocentric rectangular coordinates
	double radiusAU = EARTH_RADIUS / AU;
	double correction[] = {radiusAU * FMath::Cos(obsLat) * FMath::Cos(lst), radiusAU * FMath::Cos(obsLat) * FMath::Sin(lst),	radiusAU * FMath::Sin(obsLat)};

	double xtopo = x - correction[0];
	double ytopo = y - correction[1];
	double ztopo = z - correction[2];

	// Obtain topocentric equatorial coordinates
	double ra = 0.0;
	double dec = PI_OVER_TWO;
	if (ztopo < 0.0) dec = -dec;
	if (ytopo != 0.0 || xtopo != 0.0) {
		ra = FMath::Atan2(ytopo, xtopo);
		dec = FMath::Atan2(ztopo / FMath::Sqrt(xtopo * xtopo + ytopo * ytopo), 1.0);
	}
	double dist = FMath::Sqrt(xtopo * xtopo + ytopo * ytopo + ztopo * ztopo);

	// Hour angle
	double angh = lst - ra;

	// Obtain azimuth and geometric alt
	double sinlat = FMath::Sin(obsLat);
	double coslat = FMath::Cos(obsLat);
	double sindec = FMath::Sin(dec), cosdec = FMath::Cos(dec);
	double h = sinlat * sindec + coslat * cosdec * FMath::Cos(angh);
	double alt = FMath::Asin(h);
	double azy = FMath::Sin(angh);
	double azx = FMath::Cos(angh) * sinlat - sindec * coslat / cosdec;
	double azi = PI + FMath::Atan2(azy, azx); // 0 = north

	// Get apparent elevation
	if (alt > -3 * DEG_TO_RAD) {
		double r = 0.016667 * DEG_TO_RAD * FMath::Abs(FMath::Tan(PI_OVER_TWO - (alt * RAD_TO_DEG + 7.31 / (alt * RAD_TO_DEG + 4.4)) * DEG_TO_RAD));
		double refr = r * (0.28 * 1010 / (10 + 273.0)); // Assuming pressure of 1010 mb and T = 10 C
		alt = FMath::Min(alt + refr, PI_OVER_TWO); // This is not accurate, but acceptable

	}

	switch (twilight) {
	case HORIZON_34arcmin:
		// Rise, set, transit times, taking into account Sun/Moon angular radius (pos[3]).
		// The 34' factor is the standard refraction at horizon.
		// Removing angular radius will do calculations for the center of the disk instead
		// of the upper limb.
		tmp = -(34.0 / 60.0) * DEG_TO_RAD - pos[3];
		break;
	case TWILIGHT_CIVIL:
		tmp = -6 * DEG_TO_RAD;
		break;
	case TWILIGHT_NAUTICAL:
		tmp = -12 * DEG_TO_RAD;
		break;
	case TWILIGHT_ASTRONOMICAL:
		tmp = -18 * DEG_TO_RAD;
		break;
	}

	// Compute cosine of hour angle
	tmp = (FMath::Sin(tmp) - FMath::Sin(obsLat) * FMath::Sin(dec)) / (FMath::Cos(obsLat) * FMath::Cos(dec));
	/** Length of a sidereal day in days according to IERS Conventions. */
	double siderealDayLength = 1.00273781191135448;
	double celestialHoursToEarthTime = 1.0 / (siderealDayLength * TWO_PI);

	// Make calculations for the meridian
	double transit_time1 = celestialHoursToEarthTime * normalizeRadians(ra - lst);
	double transit_time2 = celestialHoursToEarthTime * (normalizeRadians(ra - lst) - TWO_PI);
	double transit_alt = FMath::Asin(FMath::Sin(dec) * FMath::Sin(obsLat) + FMath::Cos(dec) * FMath::Cos(obsLat));
	if (transit_alt > -3 * DEG_TO_RAD) {
		double r = 0.016667 * DEG_TO_RAD * FMath::Abs(FMath::Tan(PI_OVER_TWO - (transit_alt * RAD_TO_DEG + 7.31 / (transit_alt * RAD_TO_DEG + 4.4)) * DEG_TO_RAD));
		double refr = r * (0.28 * 1010 / (10 + 273.0)); // Assuming pressure of 1010 mb and T = 10 C
		transit_alt = FMath::Min(transit_alt + refr, PI_OVER_TWO); // This is not accurate, but acceptable
	}

	// Obtain the current event in time
	double transit_time = transit_time1;
	double jdToday = FMath::FloorToFloat(jd_UT - 0.5) + 0.5;
	double transitToday2 = FMath::FloorToFloat(jd_UT + transit_time2 - 0.5) + 0.5;
	// Obtain the transit time. Preference should be given to the closest event
	// in time to the current calculation time
	if (jdToday == transitToday2 && FMath::Abs(transit_time2) < FMath::Abs(transit_time1)) transit_time = transit_time2;
	double transit = jd_UT + transit_time;

	// Make calculations for rise and set
	double rise = -1, set = -1;
	if (FMath::Abs(tmp) <= 1.0) {
		double ang_hor = FMath::Abs(FMath::Acos(tmp));
		double rise_time1 = celestialHoursToEarthTime * normalizeRadians(ra - ang_hor - lst);
		double set_time1 = celestialHoursToEarthTime * normalizeRadians(ra + ang_hor - lst);
		double rise_time2 = celestialHoursToEarthTime * (normalizeRadians(ra - ang_hor - lst) - TWO_PI);
		double set_time2 = celestialHoursToEarthTime * (normalizeRadians(ra + ang_hor - lst) - TWO_PI);

		// Obtain the current events in time. Preference should be given to the closest event
		// in time to the current calculation time (so that iteration in other method will converge)
		double rise_time = rise_time1;
		double riseToday2 = FMath::FloorToFloat(jd_UT + rise_time2 - 0.5) + 0.5;
		if (jdToday == riseToday2 && FMath::Abs(rise_time2) < FMath::Abs(rise_time1)) rise_time = rise_time2;

		double set_time = set_time1;
		double setToday2 = FMath::FloorToFloat(jd_UT + set_time2 - 0.5) + 0.5;
		if (jdToday == setToday2 && FMath::Abs(set_time2) < FMath::Abs(set_time1)) set_time = set_time2;
		rise = jd_UT + rise_time;
		set = jd_UT + set_time;
	}

	FEphemeris* out = new FEphemeris(azi, alt, rise, set, transit, transit_alt,
		normalizeRadians(ra), dec, dist, pos[0], pos[1], pos[3]);
	return out;
}

/**
 * Transforms a Julian day (rise/set/transit fields) to a common date.
 * @param jd The Julian day.
 * @return A set of integers: year, month, day, hour, minute, second.
 * @throws Exception If the input date does not exists.
 */
TArray<int32>  ATimeManager::getDate(double jd) {
	if (jd < 2299160.0 && jd >= 2299150.0)
		//throw new Exception("invalid julian day " + jd + ". This date does not exist.");
		return TArray<int32>{0};
	// The conversion formulas are from Meeus,
	// Chapter 7.
	double Z = FMath::FloorToFloat(jd + 0.5);
	double F = jd + 0.5 - Z;
	double A = Z;
	if (Z >= 2299161.0) {
		int a = (int)((Z - 1867216.25) / 36524.25);
		A += 1 + a - a / 4;
	}
	double B = A + 1524;
	int C = (int)((B - 122.1) / 365.25);
	int D = (int)(C * 365.25);
	int E = (int)((B - D) / 30.6001);

	double exactDay = F + B - D - (int)(30.6001 * E);
	int day = (int)exactDay;
	int month = (E < 14) ? E - 1 : E - 13;
	int year = C - 4715;
	if (month > 2) year--;
	double h = ((exactDay - day) * SECONDS_PER_DAY) / 3600.0;

	int hour = (int)h;
	double m = (h - hour) * 60.0;
	int minute = (int)m;
	int second = (int)((m - minute) * 60.0);

	return TArray<int32> {year, month, day, hour, minute, second};
}

/**
 * Returns a date as a string.
 * @param jd The Julian day.
 * @return The String.
 * @throws Exception If the date does not exists.
 */
FString  ATimeManager::getDateAsString(double jd) {
	if (jd == -1) return "NO RISE/SET/TRANSIT FOR THIS OBSERVER/DATE";

	//DOUBLE CHECK THIS
	//int date[] = SunMoonCalculator.getDate(jd);
	TArray<int32> date = getDate(jd);
	FString zyr = "", zmo = "", zh = "", zm = "", zs = "";
	if (date[1] < 10) zyr = "0";
	if (date[2] < 10) zmo = "0";
	if (date[3] < 10) zh = "0";
	if (date[4] < 10) zm = "0";
	if (date[5] < 10) zs = "0";
	return date[0] + "/" + zyr + FString::FromInt(date[1]) + "/" + zmo + FString::FromInt(date[2]) + " " + zh + FString::FromInt(date[3]) + ":" + zm + FString::FromInt(date[4]) + ":" + zs + FString::FromInt(date[5]) + " UT";
}

/**
 * Reduce an angle in radians to the range (0 - 2 Pi).
 * @param r Value in radians.
 * @return The reduced radians value.
 */
double  ATimeManager::normalizeRadians(double r)
{
	if (r < 0 && r >= -TWO_PI) return r + TWO_PI;
	if (r >= TWO_PI && r < 2 * TWO_PI) return r - TWO_PI;
	if (r >= 0 && r < TWO_PI) return r;

	r -= TWO_PI * FMath::FloorToFloat(r / TWO_PI);
	if (r < 0.) r += TWO_PI;

	return r;
}

double  ATimeManager::obtainAccurateRiseSetTransit(double riseSetJD, EVENT index, int niter, bool sunIn) {
	double step = -1;
	for (int i = 0; i < niter; i++) {
		if (riseSetJD == -1) return riseSetJD; // -1 means no rise/set from that location
		setUTDate(riseSetJD);
		FEphemeris* out;
		if (sunIn) {
			out = doCalc(getSun(), false);
		}
		else {
			getSun();
			out = doCalc(getMoon(), false);
		}

		double val = out->rise;
		if (index == EVENT::SET) val = out->set;
		if (index == EVENT::TRANSIT) val = out->transit;
		step = FMath::Abs(riseSetJD - val);
		riseSetJD = val;
	}
	if (step > 1.0 / SECONDS_PER_DAY) return -1; // did not converge => without rise/set/transit in this date
	return riseSetJD;
}

//You may want to draw a figure with the appearance of the Moon disk as seen from an observer on Earth, considering the illumination percentage,
//the axis position angle, the pole position angle, or even the longitude of the central meridian to show coordinates on the disk.
//The following algorithm uses some methods from the previous program to compute the necessary angles.
//First two values are the optical librations lp and bp(lunar coordinates of the center of the disk, or in other words,
//longitude of central meridian and position angle of pole), then the position angle of axis p,
//the bright limb angle bl(angle respect north direction from where the sunlight is coming), and the paralactic angle par.
//In case your chart will be oriented respect celestial north, par can be neglected, but in case you prefer to show the lunar disk as seen by the observer, 
//the apparent inclination of the figure will be(p - par) instead of p(and, of course, -par should be added also to the apparent bright limb angle).
//The illumination fraction can be directly computed from the moon age, knowing that the cycle is 29.5306 days and the lunar disk will be fully illuminated at cycle / 2.
//
//Of course you may need to see some charts in Internet to fully understand the angles and to check you apply the rotations correctly, 
//since charting requires some care.Computations are based on the method by Eckhardt, simplified to neglect the physical librations of the moon->
//Accuracy is better than 0.5 degrees.Some code lines could be removed by integrating the method in the previous code, as well as the initial calls to computations.
TArray<double> ATimeManager::getMoonDiskOrientationAngles()
{
	sun = doCalc(getSun(), false);
	TArray<double> moonPos = getMoon();
	moon = doCalc(moonPos, false);
	double moonLon = moonPos[0], moonLat = moonPos[1],
		moonRA = moon->rightAscension, moonDEC = moon->declination;
	double sunRA = sun->rightAscension, sunDEC = sun->declination;

	// Moon's argument of latitude
	double F = (93.2720993 + 483202.0175273 * t - 0.0034029 * t * t - t * t * t / 3526000.0 + t * t * t * t / 863310000.0) * DEG_TO_RAD;
	// Moon's inclination
	double I = 1.54242 * DEG_TO_RAD;
	// Moon's mean ascending node longitude
	double omega = (125.0445550 - 1934.1361849 * t + 0.0020762 * t * t + t * t * t / 467410.0 - t * t * t * t / 18999000.0) * DEG_TO_RAD;
	// Obliquity of ecliptic (approx, better formulae up)
	double eps = 23.43929 * DEG_TO_RAD;

	// Obtain optical librations lp and bp
	double W = moonLon - omega;
	double sinA = FMath::Sin(W) * FMath::Cos(moonLat) * FMath::Cos(I) - FMath::Sin(moonLat) * FMath::Sin(I);
	double cosA = FMath::Cos(W) * FMath::Cos(moonLat);
	double A = FMath::Atan2(sinA, cosA);
	double lp = normalizeRadians(A - F);
	double sinbp = -FMath::Sin(W) * FMath::Cos(moonLat) * FMath::Sin(I) - FMath::Sin(moonLat) * FMath::Cos(I);
	double bp = FMath::Asin(sinbp);

	// Obtain position angle of axis p
	double x = FMath::Sin(I) * FMath::Sin(omega);
	double y = FMath::Sin(I) * FMath::Cos(omega) * FMath::Cos(eps) - FMath::Cos(I) * FMath::Sin(eps);
	double w = FMath::Atan2(x, y);
	double sinp = FMath::Sqrt(x*x + y * y) * FMath::Cos(moonRA - w) / FMath::Cos(bp);
	double p = FMath::Asin(sinp);

	// Compute bright limb angle bl
	double bl = (PI + FMath::Atan2(FMath::Cos(sunDEC) * FMath::Sin(moonRA - sunRA), FMath::Cos(sunDEC) *
		FMath::Sin(moonDEC) * FMath::Cos(moonRA - sunRA) - FMath::Sin(sunDEC) * FMath::Cos(moonDEC)));

	// Paralactic angle par (first obtain local apparent sidereal time)
	double jd0 = FMath::FloorToDouble(jd_UT - 0.5) + 0.5;
	double T0 = (jd0 - J2000) / JULIAN_DAYS_PER_CENTURY;
	double secs = (jd_UT - jd0) * SECONDS_PER_DAY;
	double gmst = (((((-6.2e-6 * T0) + 9.3104e-2) * T0) + 8640184.812866) * T0) + 24110.54841;
	double msday = 1.0 + (((((-1.86e-5 * T0) + 0.186208) * T0) + 8640184.812866) / (SECONDS_PER_DAY * JULIAN_DAYS_PER_CENTURY));
	gmst = (gmst + msday * secs) * (15.0 / 3600.0) * DEG_TO_RAD;
	double lst = gmst + obsLon;

	y = FMath::Sin(lst - moonRA);
	x = FMath::Tan(obsLat) * FMath::Cos(moonDEC) - FMath::Sin(moonDEC) * FMath::Cos(lst - moonRA);
	double par = 0.0;
	if (x != 0.0)
	{
		par = FMath::Atan2(y, x);
	}
	else {
		par = (y / FMath::Abs(y)) * PI_OVER_TWO;
	}
	//return TArray<double> {lp, bp, p, bl, par, lst};
	return TArray<double> {lp * RAD_TO_DEG, bp * RAD_TO_DEG, p * RAD_TO_DEG, bl * RAD_TO_DEG, par * RAD_TO_DEG, lst};
}


/**
 * Main test program.
 * @param args Not used
 */
void  ATimeManager::test() {
//	System.out.println("SunMoonCalculator test run");
//
//	try {
//		int year = 2018, month = 7, day = 28, h = 16 - 2, m = 28, s = 52; // in UT !!!
//		double obsLon = -3.7 * DEG_TO_RAD, obsLat = 40.417 * DEG_TO_RAD; // lon is negative to the west
//		SunMoonCalculator smc = new SunMoonCalculator(year, month, day, h, m, s, obsLon, obsLat);
//
//		smc.calcSunAndMoon();
//
//		String degSymbol = "\u00b0";
//		System.out.println("Sun");
//		System.out.println(" Az:       " + (float)(smc.sun->azimuth * RAD_TO_DEG) + degSymbol);
//		System.out.println(" El:       " + (float)(smc.sun->elevation * RAD_TO_DEG) + degSymbol);
//		System.out.println(" Dist:     " + (float)(smc.sun->distance) + " AU");
//		System.out.println(" RA:       " + (float)(smc.sun->rightAscension * RAD_TO_DEG) + degSymbol);
//		System.out.println(" DEC:      " + (float)(smc.sun->declination * RAD_TO_DEG) + degSymbol);
//		System.out.println(" Ill:      " + (float)(smc.sun->illuminationPhase) + "%");
//		System.out.println(" ang.R:    " + (float)(smc.sun->angularRadius * RAD_TO_DEG) + degSymbol);
//		System.out.println(" Rise:     " + SunMoonCalculator.getDateAsString(smc.sun->rise));
//		System.out.println(" Set:      " + SunMoonCalculator.getDateAsString(smc.sun->set));
//		System.out.println(" Transit:  " + SunMoonCalculator.getDateAsString(smc.sun->transit) + " (elev. " + (float)(smc.sun->transitElevation * RAD_TO_DEG) + degSymbol + ")");
//		/*
//		System.out.println(" Az=+angR: "+SunMoonCalculator.getDateAsString(smc.getAzimuthTime(true, PI+smc.sun->angularRadius)));
//		System.out.println(" Max Elev: "+SunMoonCalculator.getDateAsString(smc.getCulminationTime(true, false)));
//		System.out.println(" Az=0:     "+SunMoonCalculator.getDateAsString(smc.getAzimuthTime(true, 0)));
//		System.out.println(" Min Elev: "+SunMoonCalculator.getDateAsString(smc.getCulminationTime(true, true)));
//		*/
//		System.out.println("Moon");
//		System.out.println(" Az:       " + (float)(smc.moon->azimuth * RAD_TO_DEG) + degSymbol);
//		System.out.println(" El:       " + (float)(smc.moon->elevation * RAD_TO_DEG) + degSymbol);
//		System.out.println(" Dist:     " + (float)(smc.moon->distance * AU) + " km");
//		System.out.println(" RA:       " + (float)(smc.moon->rightAscension * RAD_TO_DEG) + degSymbol);
//		System.out.println(" DEC:      " + (float)(smc.moon->declination * RAD_TO_DEG) + degSymbol);
//		System.out.println(" Ill:      " + (float)(smc.moon->illuminationPhase) + "%");
//		System.out.println(" ang.R:    " + (float)(smc.moon->angularRadius * RAD_TO_DEG) + degSymbol);
//		System.out.println(" Age:      " + (float)(smc.moonAge) + " days");
//		System.out.println(" Rise:     " + SunMoonCalculator.getDateAsString(smc.moon->rise));
//		System.out.println(" Set:      " + SunMoonCalculator.getDateAsString(smc.moon->set));
//		System.out.println(" Transit:  " + SunMoonCalculator.getDateAsString(smc.moon->transit) + " (elev. " + (float)(smc.moon->transitElevation * RAD_TO_DEG) + degSymbol + ")");
//		/*
//		System.out.println(" Az=+angR: "+SunMoonCalculator.getDateAsString(smc.getAzimuthTime(false, PI+smc.moon->angularRadius)));
//		System.out.println(" Max Elev: "+SunMoonCalculator.getDateAsString(smc.getCulminationTime(false, false)));
//		System.out.println(" Az=0:     "+SunMoonCalculator.getDateAsString(smc.getAzimuthTime(false, 0)));
//		System.out.println(" Min Elev: "+SunMoonCalculator.getDateAsString(smc.getCulminationTime(false, true)));
//		*/
//
//		smc.setTwilight(TWILIGHT.TWILIGHT_ASTRONOMICAL);
//		smc.calcSunAndMoon();
//
//		System.out.println("");
//		System.out.println("Astronomical twilights:");
//		System.out.println("Sun");
//		System.out.println(" Rise:     " + SunMoonCalculator.getDateAsString(smc.sun->rise));
//		System.out.println(" Set:      " + SunMoonCalculator.getDateAsString(smc.sun->set));
//		System.out.println("Moon");
//		System.out.println(" Rise:     " + SunMoonCalculator.getDateAsString(smc.moon->rise));
//		System.out.println(" Set:      " + SunMoonCalculator.getDateAsString(smc.moon->set));
//
//		/*
//		System.out.println("");
//		System.out.println("Closest Moon phases:");
//		for (int i=0; i<MOONPHASE.values().length; i++) {
//			MOONPHASE mp = MOONPHASE.values()[i];
//			System.out.println(" "+mp.phaseName+"  "+SunMoonCalculator.getDateAsString(smc.getMoonPhaseTime(mp)));
//		}
//
//		double equinox[] = smc.getEquinoxes();
//		double solstices[] = smc.getSolstices();
//		System.out.println("");
//		System.out.println("Equinoxes and solstices:");
//		System.out.println(" Spring equinox:    "+SunMoonCalculator.getDateAsString(equinox[0]));
//		System.out.println(" Autumn equinox:    "+SunMoonCalculator.getDateAsString(equinox[1]));
//		System.out.println(" Summer solstice:   "+SunMoonCalculator.getDateAsString(solstices[0]));
//		System.out.println(" Winter solstice:   "+SunMoonCalculator.getDateAsString(solstices[1]));
//		*/
//
//		// Expected accuracy over 1800 - 2200:
//		// - Sun: 0.001 deg in RA/DEC, 0.003 deg or 10 arcsec in Az/El. 
//		//        <1s in rise/set/transit times. 1 min in Equinoxes/Solstices
//		// - Mon: 0.03 deg or better. 
//		//        10s or better in rise/set/transit times. 2 minutes in lunar phases.
//		//        In most cases the actual accuracy in the Moon will be better, but it is not guaranteed.
//	}
//	catch (Exception exc) {
//		exc.printStackTrace();
//	}
//}
}