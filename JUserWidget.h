// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UJUserWidget : public UUserWidget
{
	GENERATED_BODY()	

public:
	virtual bool Initialize() override;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif

private:

	// bind widget array properties.
	void BindArrayWidgets();

	void GetBindArrayWidgets(TArray<FArrayProperty*>& OutList) const;
	FString GetArrayWidgetName(const FString& OriginName, int32 Index) const;
};
