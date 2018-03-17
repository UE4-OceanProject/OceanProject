/*=================================================
* FileName: TimeData.h
*
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/07/29
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
#include "TimeDate.generated.h"


USTRUCT(BlueprintType)
struct FTimeDate
    {
	GENERATED_USTRUCT_BODY()

		// Default constructor
	FTimeDate() { FTimeDate(0, 0, 0, 0, 0, 0, 0); }

	// Copy Constructor
	//FTimeDate(FTimeDate& time) { FTimeDate(time.Year, time.Month, time.Day, time.Hour, time.Minute, time.Second, time.Millisecond); }

	// Date values only constructor
	FTimeDate(int32 year, int32 month, int32 day) { FDateTime(year, month, day, 0, 0, 0, 0); }

	// Fully initialized constructor
	FTimeDate(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond)
	    {
		Year = year; Month = month; Day = day; Hour = hour; Minute = minute; Second = second; Millisecond = millisecond;
	    }

	// The millisecond value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Millisecond;

	// The second value for this time and date.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int32 Second;

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


