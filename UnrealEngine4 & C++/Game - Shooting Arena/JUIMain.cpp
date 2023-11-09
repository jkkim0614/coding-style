#include "UI/Widget/GamePlay/JUIMain.h"
#include "Button.h"

UJUIMain::UJUIMain(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UJUIMain::NativeConstruct()
{
	Super::NativeConstruct();

	SinglePlayButton->OnClicked.AddUniqueDynamic(this, &UJUIMain::OnClickedSinglePlayButton);
	MultiPlayButton->OnClicked.AddUniqueDynamic(this, &UJUIMain::OnClickedMultiPlayButton);
}

void UJUIMain::NativeDestruct()
{
	Super::NativeDestruct();

	SinglePlayButton->OnClicked.RemoveDynamic(this, &UJUIMain::OnClickedSinglePlayButton);
	MultiPlayButton->OnClicked.RemoveDynamic(this, &UJUIMain::OnClickedMultiPlayButton);
}

void UJUIMain::RequestMultiPlay()
{
}

void UJUIMain::OnClickedSinglePlayButton()
{
	FString RemovedDotSingleMapName = UJGameLibrary::GetAssetPathWithoutDot(SingleMapPath.GetAssetPathString());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*RemovedDotSingleMapName));
}

void UJUIMain::OnClickedMultiPlayButton()
{
	RequestMultiPlay();
}
