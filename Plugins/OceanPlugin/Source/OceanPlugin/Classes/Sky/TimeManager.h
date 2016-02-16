/*
* =================================================
* FileName: TimeManager.h
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

#pragma once

#include "OceanPluginPrivatePCH.h"
#include "Sky/TimeDate.h"
//#include "TimeManager.generated.h"


// An actor based calendar system for tracking date and time
// UCLASS(BlueprintType)
// class ATimeManager : public AActor
// 	{
// 	GENERATED_UCLASS_BODY()
// 
// public:
// 
// 	// The desired starting date for the calendar. 
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Calendar")
// 	FTimeDate StartTime;
// 
// 	// The current date and time (read only).
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	FTimeDate CurrentTime;
// 
// 	// The current UTC date and time (read only).
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	FTimeDate UTC_Time;
// 
// 	// The Longitude/Latitude location used to determine the sun's position.
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")
// 	FLocation LocalLocation = FLocation(0, 0);
// 
// 	// The number of milliseconds in a single second (default = 1000)
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 MillisecondsInSecond = 1000;
// 
// 	// The number of seconds in a single minute (default = 60)
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 SecondsInMinute = 60;
// 
// 	// The number of seconds in a single day (default = 86400)
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 SecondsInDay = 86400;
// 
// 	// The number of minutes in a single hour (default = 60)
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 MinutesInHour = 60;
// 
// 	// The number of minutes in a single day (default = 1440)
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 MinutesInDay = 1440;
// 
// 	// The number of hours in a single day (default = 24)
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 HoursInDay = 24;
// 
// 	// The number of months in a year.
// 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// 	int32 MonthsInYear = 12;
// 
// 	// The time-zone offset of the current location from UTC in the range of -12 to +12 hours.	
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Calendar")
// 	int32 HourOffset = 0;
// 
// 	// A time speed multiplier (1min in game = X minutes real-time).
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Calendar")
// 	float TimeScaleMultiplier = 1.0f;
// 
// 	// Determines whether Daylight Savings should be taken into account
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Calendar")
// 	bool bIsDaylightSavingsActive = false;
// 	
// 
// 
// 
// 
// 	// The current Declination angle
// // 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// // 	float Declination = 0.0f;
// // 
// // 	// The Azimuth value (Yaw)
// // 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// // 	float Azimuth = 0.0f;
// // 
// // 	// The Zenith angle (Pitch)
// // 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// // 	float Zenith = 0.0f;
// // 
// // 	// The current Hour Angle (Zenith (pitch) = 90 - HRA)
// // 	UPROPERTY(BlueprintReadOnly, Category = "Calendar")
// // 	float HRA = 0.0f;
// 
// 	 
// 	// -------------------
// 	// PUBLIC FUNCTIONS
// 	// -------------------
// 	
// 
// 
// 	/**
// 	* Name: GetDayOfYear
// 	* Description: Gets the elapsed number of days (plus fractional day) for the current year.
// 	*
// 	* @return: float - The number of days (plus fractional day) elapsed in the current year.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	float GetDayOfYear();
// 
// 	/**
// 	* Name: DaysInYear
// 	* Description: Gets the total number of days in a given year (takes leap years into account).
// 	*
// 	* @param: int32 year - The year value.
// 	* @return: int32 - The total number of days in the given year.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	int32 DaysInYear(int32 year);
// 	
// 	/**
// 	* Name: DaysInMonth
// 	* Description: The number of days in the current month (leap years are taken into account).
// 	*
// 	* @param: int32 month - The month value.
// 	* @param: int32 year - The year value.
// 	* @return: int32 - The number of days in the given month for the given year.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	int32 DaysInMonth(int32 year, int32 month);
// 	
// 	/**
// 	* Name: GetElapsedDayInMinutes
// 	* Description: Gets the accumulated number of minutes (plus fractional) for the current day.
// 	*
// 	* @return: float - The number of minutes (plus fractional minute - NOT seconds) elapsed in the given day.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	float GetElapsedDayInMinutes();
// 
// 	/**
// 	* Name: GetElapsedDayInMinutes
// 	* Description: Gets the accumulated number of minutes (plus fractional) for the specified day.
// 	*
// 	* @param: FTimeDate time - The time & date value.
// 	* @return: float - The number of minutes (plus fractional minute - NOT seconds) elapsed in the given day.
// 	*/
// 	//UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	//float GetElapsedDayInMinutes(FTimeDate time);
// 
// 
// 	/**
// 	* Name: SetTheCurrentTime
// 	* Description: Sets the CurrentTime variable to the given TimeDate value.
// 	*
// 	* @param: FTimeDate & time - The new time value.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	FTimeDate SetTheCurrentTime(FTimeDate time);
// 
// 
// 	/**
// 	* Name: GetDayPhase
// 	* Description: Gets the current day phase in a 0 to 1 range (fractional).
// 	*
// 	* @return: float - The day phase in a 0.0 to 1.0 range.
// 	*/
// 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calendar")
// 	float GetDayPhase();
// 
// 
// 	/**
// 	* Name: GetYearPhase
// 	* Description: Gets the current year phase in a 0 to 1 range (fractional).
// 	*
// 	* @return: float - The year phase in a 0.0 to 1.0 range.
// 	*/
// 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calendar")
// 	float GetYearPhase();
// 
// 	/**
// 	* Name: IsLeapYear
// 	* Description: Determines whether the specified year is a leap year.
// 	*
// 	* @param: int32 year - The year value to check
// 	* @return: bool - Will return true if it is a leap year, otherwise false.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	bool IsLeapYear(int32 year);
// 
// 
// 	/**
// 	* Name: IncrementTime
// 	* Description: Increments time based on the delta time + TimeScaleMultiplier
// 	*
// 	* @param: float deltaSeconds - The Tick (or accumulated ticks) delta time since the last update
// 	* @param: FTimeDate & time - The current time to be incremented.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Calendar")
// 	void IncrementTime(float deltaSeconds);
// 
// 	/**
// 	* @Name: GetCurrentSunRotation
// 	* @Description: Calculates the current sun angle and position for the current time.
// 	*
// 	* @return: FRotator - The sun rotation value for the current time.
// 	*/
// 	UFUNCTION(BlueprintCallable, Category = "Sun")
// 	FRotator GetCurrentSunRotation(FTimeDate time);
// 
// 
// private:
// 
// 	// The current day phase (0-1 scale). Use GetDayPhase() to obtain this value.
// 	float DayPhase = 0.0f;
// 
// 	// The current month phase (0-1 scale). Use GetMonthPhase() to obtain this value.
// 	float MonthPhase = 0.0f;
// 
// 	// The current year phase (0-1 scale). Use GetYearPhase() to obtain this value.
// 	float YearPhase = 0.0f;
// 
// 	// The time elapsed during the current day in Seconds
// 	double ElapsedTime = 0.0d;
// 
// 	// The days elapsed in the current year
// 	int32 ElapsedDays = 0;
// 
// 	// The previous CurrentTime value (the FTimeDate from the last IncrementTime() call)
// 	FTimeDate PreviousTime;
// 
// 	// The DateTime value used to access FDateTime functions. Set to CurrentTime before use.
// 	FDateTime BaseDateTime;
// 
// 	// time based offset calculated from Longitude (the exact offset, HourOffset rounds to 15deg meridian offsets)
// 	float LocalTimeOffset = 0.0f;
// 
// 	// Local solar time (UT +/- offset from UT, in degrees)
// 	float SolarTime = 0.0f;
// 
// 	// Local clock time = SolarTime - (EOT / 60) + LongitudeCorrection + DaylightSaving
// 	float LocalClockTime = 0.0f;
// 
// 	float LongitudeCorrection = 0.0f;
// 
// 	// Local Standard Time Meridian
// 	int32 LSTM = 0;
// 
// 	// Equation of Time
// 	float EoT = 0.0f;
// 
// 	/* --- Sun --- */
// 
// 
// 	// The current Declination angle
// 	float Declination = 0.0f;
// 
// 	// The Azimuth value (Yaw)
// 	float Azimuth = 0.0f;
// 
// 	// The Zenith angle (Pitch)
// 	float Zenith = 0.0f;
// 
// 	// The current Hour Angle (Zenith (pitch) = 90 - HRA)
// 	float HRA = 0.0f;
// 
// 
// 
// 	/* --- Utility Functions --- */
// 
// 	// Performs FMath::Sin() using degrees
// 	float SinD(float input);
// 
// 	// Performs FMath::Asin() using degrees
// 	float ASinD(float input);
// 
// 	// Performs FMath::Cos() using degrees
// 	float CosD(float input);
// 
// 	// Performs FMath::Acos() using degrees
// 	float ACosD(float input);
// 
// 	// Performs FMath::Tan() using degrees
// 	float TanD(float input);
// 
// 	// Performs FMath::Atan() using degrees
// 	float ATanD(float input);
// 
// 
// 
// 	/* --- Time Of Day --- */
// 
// 	/**
// 	 * Name: GetTimeOffsetFromLocation
// 	 * Description: Gets the time offset of a given location relative to GMT (return value will be in a -12.0f to 12.0f range)
// 	 *
// 	 * @param: FLocation location - The location to calculate the offset from
// 	 * @return: float - The time offset from UTC in hours (plus fractional offset - minutes must be calculated separately)
// 	 */
// 	float GetTimeOffsetFromLocation(FLocation location);
// 
// 	/**
// 	* Name: CalculateEoT
// 	* Description: Calculates the difference between mean solar time and true solar time on a given date.
// 	*              Corrects for the eccentricity of Earth's orbit and Axial tilt.
// 	*
// 	* @return: float - The calculated Equation of Time.
// 	*/
// 	float CalculateEoT(float eotBase);
// 
// 	void SetHourAngle(float timeCorrection);
// 
// 	float GetSolarZenith(float latitude);





	/* --- TODO --- */

	//	double GetJulianDate();

	// Gets the Local Solar Time for a given longitude.
	//float GetSolarTime(float longitude);
	//
	//float GetSolarTime(FLocation location);


	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calendar")
	//float GetDayPhase(FTimeDate time);

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calendar")
	//float GetMonthPhase(FTimeDate time);

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calendar")
	//float GetYearPhase(FTimeDate time);


	// TODO - Requires extra functions & rewriting to accommodate, FUTURE/NOT URGENT
	// Designates that the calendar should use custom Date & Time struct rather than
	// using the built in DateTime values. This is useful for worlds that have longer days,
	// months, and years.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DateTime")
	//bool UsingCustomCaledar;
//	};

