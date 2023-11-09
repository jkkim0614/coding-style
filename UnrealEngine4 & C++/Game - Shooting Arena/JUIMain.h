#pragma once

#include "GunFighter.h"
#include "UI/Widget/Base/JUIGamePlayWidgetBase.h"
#include "JUIMain.generated.h"

UCLASS()
class UJUIMain : public UJUIGamePlayWidgetBase
{
	GENERATED_BODY()

public:
	UJUIMain(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnClickedSinglePlayButton();

	UFUNCTION()
	void OnClickedMultiPlayButton();

	void RequestMultiPlay();

private:
	UPROPERTY(EditAnywhere, Category=MainUI)
	FSoftObjectPath SingleMapPath;

private:
	UPROPERTY()
	class UButton* SinglePlayButton;

	UPROPERTY()
	class UButton* MultiPlayButton;
};
