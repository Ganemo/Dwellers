// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DWELLERS_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:


	//Float as String With Precision
	static FORCEINLINE FString UFunctionLibrary::GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
	{
		//Round to integral if have something like 1.9999 within precision
		float Rounded = roundf(TheFloat);
		if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
		{
			TheFloat = Rounded;
		}
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
		NumberFormat.MaximumIntegralDigits = 10000;
		NumberFormat.MinimumFractionalDigits = Precision;
		NumberFormat.MaximumFractionalDigits = Precision;
		return FText::AsNumber(TheFloat, &NumberFormat).ToString();
	}

	static float UFunctionLibrary::GetSeededRandInRange(float min, float max)
	{
		return min + (FMath::SRand() * (max - min));
	}

	static bool UFunctionLibrary::GetSeededRandBool(float weight)
	{
		return FMath::SRand() < weight;
	}
	
};
