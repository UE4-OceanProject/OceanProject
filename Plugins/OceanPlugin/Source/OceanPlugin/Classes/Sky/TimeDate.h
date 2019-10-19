/*=================================================
* FileName: TimeDate.h
*
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/07/29
*
* Last Edited on: 2019/10/19
* Last Edited by: KamikazeXeX
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

	// Default constructor
	FTimeDate()
	{
		Year = Month = Day = Hour = Minute = Second = Millisecond = 0;
	}

	// Date values only constructor
	FTimeDate(int32 InYear, int32 InMonth, int32 InDay)
	{
		Year = InYear;
		Month = InMonth;
		Day = InDay;
		Hour = Minute = Second = Millisecond = 0;
	}

	// Fully initialized constructor
	FTimeDate(int32 InYear, int32 InMonth, int32 InDay, int32 InHour, int32 InMinute, int32 InSecond, int32 InMillisecond)
	{
		Year = InYear;
		Month = InMonth;
		Day = InDay;
		Hour = InHour;
		Minute = InMinute;
		Second = InSecond;
		Millisecond = InMillisecond;
	}

	// The following functions are only used in code.
	//bool IsEqual(const FTimeDate& Other) const
	//{
	//	return (Year == Other.Year) && (Month == Other.Month) && (Day == Other.Day) &&
	//		(Hour == Other.Hour) && (Minute == Other.Minute) && (Second == Other.Second);
	//}

	//bool operator==(const FTimeDate& Other) const
	//{
	//	return IsEqual(Other);
	//}

	//bool operator!=(const FTimeDate& Other) const
	//{
	//	return !IsEqual(Other);
	//}

	//bool operator>(const FTimeDate& Other) const
	//{
	//	return (Year > Other.Year) || (Month > Other.Month) || (Day > Other.Day) ||
	//		(Hour > Other.Hour) || (Minute > Other.Minute) || (Second > Other.Second);
	//}

	//bool operator>=(const FTimeDate& Other) const
	//{
	//	return IsEqual(Other) || (Year > Other.Year) || (Month > Other.Month) || (Day > Other.Day) ||
	//		(Hour > Other.Hour) || (Minute > Other.Minute) || (Second > Other.Second);
	//}

	//bool operator<(const FTimeDate& Other) const
	//{
	//	return (Year < Other.Year) || (Month < Other.Month) || (Day < Other.Day) ||
	//		(Hour < Other.Hour) || (Minute < Other.Minute) || (Second < Other.Second);
	//}

	//bool operator<=(const FTimeDate& Other) const
	//{
	//	return IsEqual(Other) || (Year < Other.Year) || (Month < Other.Month) || (Day < Other.Day) ||
	//		(Hour < Other.Hour) || (Minute < Other.Minute) || (Second < Other.Second);
	//}
};
