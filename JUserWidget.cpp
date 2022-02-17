// Fill out your copyright notice in the Description page of Project Settings.


#include "JUserWidget.h"
#include "Blueprint/WidgetTree.h"
#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif // WITH_EDITOR

#define LOCTEXT_NAMESPACE "JUserWidget"
DEFINE_LOG_CATEGORY_STATIC(LogJUserWidget, Verbose, All);

bool UJUserWidget::Initialize()
{
	if (Super::Initialize())
	{
		if (!IsDesignTime())
		{
			BindArrayWidgets();
		}

		return true;
	}

	return false;
}

void UJUserWidget::BindArrayWidgets()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UJUserWidget_BindArrayWidgets);

	// collect target array properties
	TArray<FArrayProperty*> TargetArrayWidgetProps;
	GetBindArrayWidgets(TargetArrayWidgetProps);

	// collect source widgets
	TMap<FString, UWidget*> SourceWidgets;
	WidgetTree->ForEachWidget([&](UWidget* Widget) {
		for (const auto* TargetArrProp : TargetArrayWidgetProps)
		{
			const FString WidgetName = Widget->GetName();
			if (WidgetName.StartsWith(TargetArrProp->GetName()))
			{
				UE_LOG(LogJUserWidget, Log, TEXT("[BindArrayWidgets] source widget added %s. "), *Widget->GetName());
				SourceWidgets.Add(WidgetName, Widget);
			}
		}
		});

	// bind array props
	for (const auto* TargetArrProp : TargetArrayWidgetProps)
	{
		FScriptArrayHelper ArrayHelper(TargetArrProp, TargetArrProp->ContainerPtrToValuePtr<void>(this));
		FObjectProperty* ObjInnerProp = CastField<FObjectProperty>(TargetArrProp->Inner);

		UE_LOG(LogJUserWidget, Log, TEXT("[BindArrayWidgets] try binding array widgets. %s"), *TargetArrProp->GetName());

		for (int32 Index = 0; Index < 99; ++Index)
		{
			const FString WidgetName = GetArrayWidgetName(*TargetArrProp->GetName(), Index);

			UWidget* Widget = nullptr;
			if (UWidget** WidgetContainer = SourceWidgets.Find(WidgetName))
			{
				Widget = *WidgetContainer;
			}

			if (Widget == nullptr)
			{
				break;
			}

			if (ArrayHelper.Num() <= Index)
			{
				ArrayHelper.AddValue();
			}
			void* ArrayElementValuePtr = ArrayHelper.GetRawPtr(Index);
			ObjInnerProp->SetObjectPropertyValue(ArrayElementValuePtr, Widget);
		}
	}
}

void UJUserWidget::GetBindArrayWidgets(TArray<FArrayProperty*>& OutList) const
{
	for (TFieldIterator<FArrayProperty> PropIt(GetClass(), EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
	{
		FArrayProperty* ArrayProp = *PropIt;
		FObjectProperty* ObjInnerProp = CastField<FObjectProperty>(ArrayProp->Inner);

		if (ObjInnerProp != nullptr && ObjInnerProp->PropertyClass->IsChildOf(UWidget::StaticClass()))
		{
			UE_LOG(LogJUserWidget, Log, TEXT("[BindArrayWidgets] target array prop added %s. "), *ArrayProp->GetName());
			OutList.Add(ArrayProp);
		}
	}
}

FString UJUserWidget::GetArrayWidgetName(const FString& OriginName, int32 Index) const
{
	return FString::Printf(TEXT("%s_%02d"), *OriginName, Index);
}

#if WITH_EDITOR
void UJUserWidget::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	TArray<FArrayProperty*> TargetArrayWidgetProps;
	GetBindArrayWidgets(TargetArrayWidgetProps);

	UWidgetBlueprintGeneratedClass* BGClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass());
	if (BGClass != nullptr)
	{
		BGClass = GetWidgetTreeOwningClass();
	}

	UWidgetTree* SourceWidgetTree = nullptr;
	if (BGClass != nullptr)
	{
		SourceWidgetTree = BGClass->GetWidgetTreeArchetype();
	}

	const FText txtUserWidgetName = FText::FromString(*GetName());

	if (SourceWidgetTree != nullptr)
	{
		// loop array props
		for (const auto* TargetArrProp : TargetArrayWidgetProps)
		{
			int32 LastValidIndex = INDEX_NONE;
			for (int32 Index = 0; Index < 99; ++Index)
			{
				const FText txtTargetArrPropName = FText::FromString(*TargetArrProp->GetName());
				const FString SourceWidgetName = GetArrayWidgetName(*TargetArrProp->GetName(), Index);

				// find source widget from widget tree
				if (auto* SourceWidget = SourceWidgetTree->FindWidget(*SourceWidgetName))
				{
					FObjectProperty* ObjInnerProp = CastField<FObjectProperty>(TargetArrProp->Inner);

					if (ObjInnerProp == nullptr)
					{
						CompileLog.Error(FText::Format(LOCTEXT("BindArrayWidget", "[BindArrayWidget][{0}]{1} has null ObjInnerProp.")
							, txtUserWidgetName
							, txtTargetArrPropName
						));
						continue;
					}

					// check widget type
					if (SourceWidget->GetClass()->IsChildOf(ObjInnerProp->PropertyClass) == false)
					{
						CompileLog.Error(FText::Format(LOCTEXT("BindArrayWidget", "[BindArrayWidget][{0}] {1}!={2} miss match widget type.")
							, txtUserWidgetName
							, txtTargetArrPropName
							, FText::FromString(*SourceWidgetName)
						));
						continue;
					}

					// check missing index
					if (Index - 1 > LastValidIndex)
					{
						CompileLog.Error(FText::Format(LOCTEXT("BindArrayWidget", "[BindArrayWidget][{0}] missing index.({1})")
							, txtUserWidgetName
							,FText::FromString(*GetArrayWidgetName(*TargetArrProp->GetName(), Index-1))
						));
					}

					LastValidIndex = Index;
				}
			}
		}
	}
	else
	{
		CompileLog.Error(FText::Format(LOCTEXT("BindArrayWidget", "[BindArrayWidget][{0}] failed find widgettree."), txtUserWidgetName));
	}
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE