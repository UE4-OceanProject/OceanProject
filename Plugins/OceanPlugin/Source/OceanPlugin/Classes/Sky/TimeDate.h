/*=================================================
* FileName: TimeDate.h
*
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/07/29
*
* Last Edited on: 2019/12/27
* Last Edited by: iliags
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
	GENERATED_BODY()

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

	// Fully initialized constructor
	FTimeDate(int32 InYear = 1900, int32 InMonth = 1, int32 InDay = 1, int32 InHour = 0, int32 InMinute = 0, int32 InSecond = 0, int32 InMillisecond = 0)
	{
		Year = InYear;
		Month = InMonth;
		Day = InDay;
		Hour = InHour;
		Minute = InMinute;
		Second = InSecond;
		Millisecond = InMillisecond;
	}
};
