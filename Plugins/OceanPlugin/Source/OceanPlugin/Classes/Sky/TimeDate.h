/*=================================================
* FileName: TimeData.h
*
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/07/29
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
#include "Runtime/Core/Public/Misc/DateTime.h"
#include "TimeDate.generated.h"


// USTRUCT(BlueprintType)
// struct FLocation
// 	{
// 	GENERATED_USTRUCT_BODY()
// 
// 	/**
// 	* @Name: FLocation
// 	* @Description: Constructs a new FLocation instance.
// 	*/
// 	FLocation() { FLocation(0.0f, 0.0f); }
// 
// 	/**
// 	* @Name: FLocation
// 	* @Description: Constructs a new FLocation instance.
// 	*
// 	* @param: float - Latitude value (-90 to +90)
// 	* @param: float - Longitude value (-180 to +180)
// 	*/
// 	FLocation(float latitude, float longitude) { Latitude = latitude; Longitude = longitude > 180.0f ? longitude - 360.0f : longitude; }
// 
// 	// The latitude coordinate for the local location (valid value range -90 to +90 degrees).
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")
// 	float Latitude;
// 
// 	// The longitude coordinate for the local location (valid value range -180 to +180 degrees).
// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")
// 	float Longitude;
// 
// 	// Direction for calculating the Longitude value (East [false] = 0 to 180, West [true] = 180 to 360)
// 	//UPROPERTY(BlueprintReadOnly, Category = "Location")
// 	//bool IsLongitudeWest = Longitude < 0;
// 
// 	// Direction for calculating the Latitude value (North [false] = 0 to +90, South [true] = 0 to -90)
// 	//UPROPERTY(BlueprintReadOnly, Category = "Location")
// 	//bool IsLatitudeSouth = Latitude < 0;
// 
// 	bool operator==(const FLocation& other) const
// 		{
// 		return (Latitude == other.Latitude) && (Longitude == other.Longitude);
// 		}
// 
// 	bool operator!=(const FLocation& other) const
// 		{
// 		return (Latitude != other.Latitude) || (Longitude != other.Longitude);
// 		}
// 
// 	};



USTRUCT(BlueprintType)
struct FTimeDate
	{
	GENERATED_USTRUCT_BODY()

	// Default constructor
	FTimeDate() { FTimeDate(0.0f, 0, 0, 1, 1, 2000); }

	// Copy Constructor
	FTimeDate(FTimeDate& time) { FTimeDate(time.Second, time.Minute, time.Hour, time.Day, time.Month, time.Year); }

	// Date values only constructor
	FTimeDate(int32 day, int32 month, int32 year) { FDateTime(0.0f, 0, 0, day, month, year); }

	// Fully initialized constructor
	FTimeDate(float second, int32 minute, int32 hour, int32 day, int32 month, int32 year)
		{ Second = second; Minute = minute; Hour = hour; Day = day; Month = month; Year = year; }

	// The second (plus fractional second) value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	float Second;

	// The minute value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Minute;

	// The hour value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Hour;

	// The day value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Day;

	// The month value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Month;

	// The year value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Year;


	// The following functions are only used in code.

// 
// 	bool IsEqual(const FTimeDate& other) const
// 		{
// 		return (Year == other.Year) && (Month == other.Month) && (Day == other.Day) &&
// 			(Hour == other.Hour) && (Minute == other.Minute) && (Second == other.Second);
// 		}
// 
// 	bool operator==(const FTimeDate& other) const
// 		{
// 		return IsEqual(other);
// 		}
// 
// 	bool operator!=(const FTimeDate& other) const
// 		{
// 		return !IsEqual(other);
// 		}
// 
// 	bool operator>(const FTimeDate& other) const
// 		{
// 		return (Year > other.Year) || (Month > other.Month) || (Day > other.Day) ||
// 			(Hour > other.Hour) || (Minute > other.Minute) || (Second > other.Second);
// 		}
// 
// 	bool operator>=(const FTimeDate& other) const
// 		{
// 		return IsEqual(other) || (Year > other.Year) || (Month > other.Month) || (Day > other.Day) ||
// 			(Hour > other.Hour) || (Minute > other.Minute) || (Second > other.Second);
// 		}
// 
// 	bool operator<(const FTimeDate& other) const
// 		{
// 		return (Year < other.Year) || (Month < other.Month) || (Day < other.Day) ||
// 			(Hour < other.Hour) || (Minute < other.Minute) || (Second < other.Second);
// 		}
// 
// 	bool operator<=(const FTimeDate& other) const
// 		{
// 		return IsEqual(other) || (Year < other.Year) || (Month < other.Month) || (Day < other.Day) ||
// 			(Hour < other.Hour) || (Minute < other.Minute) || (Second < other.Second);
// 		}
	};


