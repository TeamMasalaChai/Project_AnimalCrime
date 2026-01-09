// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ACCustomWidget.h"
#include "ACBoundItemWidget.generated.h"

 /**
   * @brief 바운드 아이템 위젯 (무전기, 밀수품 등)
   * @details 평소에는 투명, 아이템 획득 시 이미지 표시
   */
UCLASS()
class ANIMALCRIME_API UACBoundItemWidget : public UACCustomWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    /**
     * @brief 아이템 등록 (이미지 표시)
     * @param ItemImage - 표시할 아이템 이미지
     */
    UFUNCTION(BlueprintCallable, Category = "BoundItem")
    void SetBoundItem(UTexture2D* ItemImage);

    /**
     * @brief 아이템 등록 해제 (투명 처리)
     */
    UFUNCTION(BlueprintCallable, Category = "BoundItem")
    void ClearBoundItem();

protected:
    // 아이템 이미지
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> ItemImage;

    // 아이템이 등록되어 있는지 여부
    UPROPERTY(BlueprintReadOnly, Category = "BoundItem")
    bool bHasItem = false;
};
