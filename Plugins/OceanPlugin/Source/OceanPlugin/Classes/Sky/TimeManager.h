/*=================================================
* FileName: TimeManager.h
*
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/07/12
*
* Last Edited on: 2018/03/15
* Last Edited by: Felipe "Zoc" Silveira
*
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sky/TimeDate.h"
#include "TimeManager.generated.h"


//An actor based calendar system for tracking date + time, and Sun/Moon rotation/phase.
UCLASS(BlueprintType)
class ATimeManager : public AActor
    {
	GENERATED_UCLASS_BODY()

public:

	// Current Local Clock Time (LCT)
	UPROPERTY(BlueprintReadOnly, Category = "TimeManager")
	FTimeDate CurrentLocalTime;

	// The Latitude of the local location (-90 to +90 in degrees)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float Latitude = 0.0f;

	// The Longitude of the local location (-180 to +180 in degrees)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float Longitude = 0.0f;

	// The number of hours offset from UTC for the local location (value in the range of -12 to +12 hours from UTC)	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int32 OffsetUTC = 0;

	// The number of hours (0 or 1) to subtract for the current TimeDate for Daylight Savings Time (if enabled)
	UPROPERTY(BlueprintReadOnly, Category = "TimeManager")
	int32 OffsetDST = 0;

	// Determines whether Daylight Savings time should be enabled for the local location
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bAllowDaylightSavings = false;

	// Determines whether Daylight Savings is active for the current date
	UPROPERTY(BlueprintReadOnly, Category = "TimeManager")
	bool bDaylightSavingsActive = false;

	// The value to multiply the base game time by (1 second real time is multiplied to equal X seconds in game)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float TimeScaleMultiplier = 1.0f;

	// The current Local Solar Time (in minutes)
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float SolarTime = 0.0f;

	// The current Local Clock Time value (in minutes)
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float LocalClockTime = 0.0f;

	// The current Time Correction factor
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float TimeCorrection = 0.0f;

	// The value of the local Standard Time Meridian (15deg intervals)
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	int32 LSTM = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	int32 DayOfYear = 0;

	// THe current Equation of Time value
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float EoT = 0.0f;

	// The current Altitude Angle of the sun
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float SolarAltAngle = 0.0f;

	// The current Declination angle of the sun
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float SolarDeclination = 0.0f;

	// The Azimuth angle of the sun
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float SolarAzimuth = 0.0f;

	// The current Hour Angle of the sun
	UPROPERTY(BlueprintReadOnly, Category = "Sun Debug")
	float SolarHRA = 0.0f;

	// The current Sidereal Time value
	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float SiderealTime = 0.0f;

	// The current Altitude Angle of the moon
	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float LunarAltAngle = 0.0f;

	// The current Hour Angle of the moon
	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float LunarHRA = 0.0f;

	// The current Declination angle of the moon
	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float LunarDeclination = 0.0f;

	// The current Azimuth angle of the moon
	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float LunarAzimuth = 0.0f;

	// The current Right Ascension angle for the moon
	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float LunarRightAsc = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float LunarElapsedDays = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float EcLongitude = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float EcLatitude = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float EcDistance = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float PartL = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float PartM = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lunar Debug")
	float PartF = 0.0f;

	// -------------------
	// PUBLIC FUNCTIONS
	// -------------------

	/**
	* Name: InitializeCalendar
	* Description: Initializes the calendar with the provided TimeDate, and validates the range of all input values.
	*
	* @param: time (TimeDate) - The TimeDate value to calculate from.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	void InitializeCalendar(FTimeDate time);


	/**
	* Name: GetDayOfYear
	* Description: Gets the number of full days elapsed in the current year for the provided date.
	*
	* @param: time (TimeDate) - The TimeDate value to calculate from.
	* @return: int32 - The number of days elapsed in the current year.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	int32 GetDayOfYear(FTimeDate time);

	/**
	* Name: DaysInYear
	* Description: Gets the total number of days in a given year (takes leap years into account).
	*
	* @param: year (int32) - The year value.
	* @return: int32 - The total number of days in the given year.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	int32 GetDaysInYear(int32 year);

	/**
	* Name: DaysInMonth
	* Description: The number of days in the specified month (leap years are taken into account).
	*
	* @param: month (int32) - The month value.
	* @param: year (int32) - The year value.
	* @return: int32 - The number of days in the given month for the given year.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	int32 GetDaysInMonth(int32 year, int32 month);

	/**
	* Name: GetElapsedDayInMinutes
	* Description: Gets the accumulated number of minutes (plus fractional) for the current day.
	*
	* @return: float - The number of minutes (plus fractional minute - NOT seconds) elapsed in the given day.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	float GetElapsedDayInMinutes();


	/**
	* Name: SetCurrentLocalTime
	* Description: Sets the local time from minutes, and runs InitializeCalendar to validate and set variables.
	*
	* @param: time (float) - The number of minutes (+ frac minutes) to calculate from.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	void SetCurrentLocalTime(float time);



	/**
	* Name: GetDayPhase
	* Description: Gets the current day phase in a 0 to 1 range (fractional).
	*
	* @return: float - The day phase in a 0.0 to 1.0 range.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TimeManager")
	float GetDayPhase();


	/**
	* Name: GetYearPhase
	* Description: Gets the current year phase in a 0 to 1 range (fractional).
	*
	* @return: float - The year phase in a 0.0 to 1.0 range.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TimeManager")
	float GetYearPhase();

	/**
	* Name: IsLeapYear
	* Description: Determines whether the specified year is a leap year.
	*
	* @param: year (int32) - The year value to check
	* @return: bool - Will return true if it is a leap year, otherwise false.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	bool IsLeapYear(int32 year);


	/**
	* Name: IncrementTime
	* Description: Increments time based on the deltaSeconds * TimeScaleMultiplier
	*
	* @param: deltaSeconds (float) - The Tick (or accumulated ticks) delta time since the last update
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	void IncrementTime(float deltaSeconds);


	/**
	* @Name: CalculateSunAngle
	* @Description: Calculates the sun angle rotator from the current time.
	*
	* @return: FRotator - The sun rotation value for the current time.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	FRotator CalculateSunAngle();


	/**
	* @Name: CalculateMoonAngle
	* @Description: Calculates the moon angle rotator from the current time.
	*
	* @return: FRotator - The moon rotation value for the current time.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	FRotator CalculateMoonAngle();


	/**
	* @Name: CalculateMoonPhase
	* @Description: Calculates the moon phase for the current time and date.
	*
	* @return: FRotator - The moon rotation value for the current time.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeManager")
	float CalculateMoonPhase();


private:

	bool bIsCalendarInitialized = false;

	FDateTime InternalTime;

	// The UTC + DST TimeSpan difference vs current time
	FTimespan SpanUTC;

	// The Julian Day number for Jan 1, 2000 @ 12:00 UTC
	double JD2000 = 2451545.0;

	// The Julian Day number for Jan 1, 1900 @ 12:00 UTC
	double JD1900 = 2415020.0;

	double ElapsedJD1900 = 0.0;

	// Obliquity of the Ecliptic (as of 2000/01/01 - approximation, but fairly accurate)
	double EcObliquity = 23.4397;

	/* --- Utility Functions --- */

	// Float versions

	// Performs FMath::Sin(input) using degrees
	float SinD(float input);

	// Performs FMath::Asin(input) using degrees
	float ASinD(float input);

	// Performs FMath::Cos(input) using degrees
	float CosD(float input);

	// Performs FMath::Acos(input) using degrees
	float ACosD(float input);

	// Performs FMath::Tan(input) using degrees
	float TanD(float input);

	// Performs FMath::Atan(input) using degrees
	float ATanD(float input);

	// Performs FMath::Atan2(A, B) using degrees
	float ATan2D(float A, float B);

	// Double versions

	// Performs FMath::Sin(input) using degrees
	double SinD(double input);

	// Performs FMath::Asin(input) using degrees
	double ASinD(double input);

	// Performs FMath::Cos(input) using degrees
	double CosD(double input);

	// Performs FMath::Acos(input) using degrees
	double ACosD(double input);

	// Performs FMath::Tan(input) using degrees
	double TanD(double input);

	// Performs FMath::Atan(input) using degrees
	double ATanD(double input);

	// Performs FMath::Atan2(A, B) using degrees
	double ATan2D(double A, double B);

	FTimeDate ConvertToTimeDate(FDateTime dt);

	FDateTime ConvertToDateTime(FTimeDate td);

	FTimeDate ValidateTimeDate(FTimeDate time);

	// TODO - Requires extra functions & rewriting to accommodate, FUTURE/NOT URGENT
	// Designates that the calendar should use custom Date & Time struct rather than
	// using the built in DateTime values. This is useful for worlds that have longer days,
	// months, and years.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DateTime")
	//bool UsingCustomCaledar;

    };

