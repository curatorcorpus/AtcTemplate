// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VoxelSourceInterface.h"

#include "VIMR/octree.hpp"

#include <condition_variable>

#include "Voxels.h"
#include "VoxelRenderComponent.h"
#include "VoxelSourceBaseComponent.generated.h"

using VIMR::Voxel;
using VIMR::Octree;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Config=DefaultVIMR)
class VOXELS_API UVoxelSourceBaseComponent : public USceneComponent, public IVoxelSourceInterface
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UVoxelSourceBaseComponent();

  void GetFramePointers(int &VoxelCount, uint8*& CoarsePositionData, uint8*& PositionData, uint8*& ColourData, uint8& Voxelmm) override;
  void CopyVoxelData(const VIMR::Octree& voxels);

  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  
  UPROPERTY(Config, BlueprintReadOnly)
    FString InstanceConfigPath = "";

  UPROPERTY(Config, BlueprintReadOnly)
    FString UsingRenderRmtCompId = "";

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool VIMRShowInvisible = false;
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool VIMRShowSpecial = true;
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool VIMRColorVoxels = true;
  UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString VIMRComponentID = "VoxMerge";
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString VIMRInstanceConfig = "../../../../instance.json";
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString VIMRLogFolder = "";
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    float VoxelSize_mm = 8.0;

protected:
  
  static const int BufferSize = 2;
  
  int framecount=0;

  bool inDisplay = false;
  bool inProgress = false;
  
  int buffIdx;
  int dispIdx;
  int modprintctr = 0;

  uint32_t MaxVoxels = TOTAL_VOXELS; //FIXME: Hard-coded stuff here
  uint8* CoarsePositionData[BufferSize];
  uint8* PositionData[BufferSize];
  uint8* ColourData[BufferSize];
  uint8  VoxelSizemm[BufferSize];
  uint32_t VoxelCount[BufferSize];

  FString GVimrIni = "";

  // Called when the game starts
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  void ResetFrameBuffer();
};
