// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/ACAssetManager.h"
#include "Item/ACItemData.h"
#include "AnimalCrime.h"
#include "AssetRegistry/AssetRegistryModule.h"

UACAssetManager& UACAssetManager::Get()
{
	UACAssetManager* Manager = Cast<UACAssetManager>(GEngine->AssetManager);

	if (Manager)
	{
		return *Manager;
	}
	else
	{
		UE_LOG(LogHG, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini! Expected UACAssetManager."));
		return *NewObject<UACAssetManager>(); // Fallback (should never happen)
	}
}

void UACAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

void UACAssetManager::LoadItemDataInternal()
{
	if (bItemDataLoaded)
	{
		return; // 이미 로드됨
	}

	CachedItemData.Empty();

#if WITH_EDITOR
	// ===== 에디터: AssetRegistry 사용 (자동 검색) =====
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	FARFilter Filter;
	Filter.ClassPaths.Add(UACItemData::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(FName(TEXT("/Game/Project/Item")));
	Filter.bRecursivePaths = true;

	AssetRegistry.GetAssets(Filter, AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		UACItemData* ItemData = Cast<UACItemData>(AssetData.GetAsset());
		if (ItemData)
		{
			CachedItemData.Add(ItemData);
		}
	}
#else
	// ===== 패키징: Primary Asset 방식으로 로드 =====
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> ItemAssetIds;
	Manager.GetPrimaryAssetIdList(FPrimaryAssetType("ItemData"), ItemAssetIds);

	if (ItemAssetIds.Num() == 0)
	{
		UE_LOG(LogHG, Error, TEXT("[PACKAGED] No Primary Assets found! Check Project Settings → Asset Manager"));
		bItemDataLoaded = true;
		return;
	}

	// 개별적으로 동기 로드
	int32 SuccessCount = 0;
	int32 FailCount = 0;

	for (const FPrimaryAssetId& AssetId : ItemAssetIds)
	{
		// Primary Asset ID에서 실제 경로 얻기
		FSoftObjectPath AssetPath = Manager.GetPrimaryAssetPath(AssetId);

		if (!AssetPath.IsValid())
		{
			UE_LOG(LogHG, Error, TEXT("  - Invalid path for: %s"), *AssetId.ToString());
			FailCount++;
			continue;
		}

		// 경로로 동기 로드 (확실한 방법)
		UACItemData* ItemData = Cast<UACItemData>(AssetPath.TryLoad());

		if (ItemData)
		{
			CachedItemData.Add(ItemData);
			SuccessCount++;
		}
		else
		{
			UE_LOG(LogHG, Error, TEXT("  - Failed to load: %s (path: %s)"), *AssetId.ToString(), *AssetPath.ToString());
			FailCount++;
		}
	}
#endif
	bItemDataLoaded = true;
}

TArray<UACItemData*> UACAssetManager::GetAllItemData()
{
	// Lazy Loading: 첫 호출 시에만 로드
	LoadItemDataInternal();

	TArray<UACItemData*> Result;

	for (UACItemData* ItemData : CachedItemData)
	{
		if (ItemData)
		{
			Result.Add(ItemData);
		}
	}

	return Result;
}
