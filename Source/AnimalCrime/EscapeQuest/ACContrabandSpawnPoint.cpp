
#include "EscapeQuest/ACContrabandSpawnPoint.h"
#include "Components/BillboardComponent.h"

AACContrabandSpawnPoint::AACContrabandSpawnPoint()
{
#if WITH_EDITORONLY_DATA
	Billboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	if (Billboard == nullptr)
	{
		return;
	}
	RootComponent = Billboard;
#endif
}
