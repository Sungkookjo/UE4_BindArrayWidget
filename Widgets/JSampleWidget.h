// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/JUserWidget.h"
#include "JSampleWidget.generated.h"

class UButton;

/**
 * Sample class
 *  for BindArrayWidget
 */
UCLASS()
class TESTPROJECT_API UJSampleWidget : public UJUserWidget
{
	GENERATED_BODY()	

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SampleButtonClicked();

private:

	UPROPERTY()
	TArray<UButton*> SampleArray;
};
