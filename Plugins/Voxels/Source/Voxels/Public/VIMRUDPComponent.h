// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelSourceBaseComponent.h"
#include "VIMR/voxmerge_safe.hpp"
#include "VIMRUDPComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VOXELS_API UVIMRUDPComponent : public UVoxelSourceBaseComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UVIMRUDPComponent();
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  UFUNCTION(BlueprintCallable, Category = "CameraRegistration")
  bool SendPose(FRotator r, FVector t, int devidx);
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool VIMREnablePoseEndpoint = false;
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool DisableForDebug = false;

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  VIMR::VoxMerge *vox_merge = nullptr;
};
