// Fill out your copyright notice in the Description page of Project Settings.


#include "JSampleWidget.h"
#include "Components/Button.h"

void UJSampleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (int32 i=0;i<SampleArray.Num();++i)
	{
		SampleArray[i]->OnClicked.AddDynamic(this, &ThisClass::SampleButtonClicked);
	}
}

void UJSampleWidget::SampleButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("Button Clicked"));
}
