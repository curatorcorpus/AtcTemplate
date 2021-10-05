// Fill out your copyright notice in the Description page of Project Settings.

#include "VIMRUDPComponent.h"

#include <chrono>

#include "Engine.h"
#include "VoxelRenderSubComponent.h"


// Sets default values for this component's properties
UVIMRUDPComponent::UVIMRUDPComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UVIMRUDPComponent::BeginPlay()
{
  Super::BeginPlay();
  SetComponentTickEnabled(false);

  if(!InstanceConfigPath.IsEmpty()){
    UE_LOG(VoxLog, Log, TEXT("Using instance path from DefaultGame.ini: %s"), *InstanceConfigPath);
    VIMRInstanceConfig = InstanceConfigPath;
  }else
  {
    UE_LOG(VoxLog, Log, TEXT("Using instance path from VIMRUDP object: %s"), *VIMRInstanceConfig);
  }

  UE_LOG(VoxLog, Log, TEXT("VIMR ComponentID: %s"), *VIMRComponentID);
  
  if (VIMRLogFolder.IsEmpty())
  {
    VIMRLogFolder = FPaths::ProjectDir();
    UE_LOG(VoxLog, Log, TEXT("VIMR log path empty, logging to project dir: %s"), *VIMRLogFolder);
  }else
  {
    UE_LOG(VoxLog, Log, TEXT("VIMR logging to: %s"), *VIMRLogFolder);
  }

  if (VIMRInstanceConfig.IsEmpty())
  {
    UE_LOG(VoxLog, Log, TEXT("VIMR config path empty, attempting to load defaults from working dir"));
  }else
  {
    UE_LOG(VoxLog, Log, TEXT("VIMR config file path: %s"), *VIMRInstanceConfig);
  }
  
  if(!DisableForDebug){
    vox_merge = new VIMR::VoxMerge();
    if (vox_merge->init_safe(
      TCHAR_TO_ANSI(*VIMRComponentID), "VIMR_ROOT_07", LogLvl::Fatal,
      VIMRInstanceConfig.IsEmpty() ? nullptr : TCHAR_TO_ANSI(*VIMRInstanceConfig),
      VIMRLogFolder.IsEmpty() ? nullptr : TCHAR_TO_ANSI(*VIMRLogFolder))) {
        vox_merge->set_vox_sink([this](VIMR::VoxelMessage* g){ this->CopyVoxelData(g->octree);  });
    } else
    {
      delete vox_merge;
      vox_merge = nullptr;
      UE_LOG(VoxLog, Error, TEXT("VIMR init failed, check config files and VIMR logs"));
    }
  }
  SetComponentTickEnabled(true);
}

void UVIMRUDPComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
  if(vox_merge != nullptr)
    delete vox_merge;
}

// Called every frame
void UVIMRUDPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UVIMRUDPComponent::SendPose(FRotator r, FVector t, int devidx)
{
  if(vox_merge != nullptr)
  {
    FQuat q = r.Quaternion();
    auto t_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // Scale by 1/100 to convert centimeters to meters.
    VIMR::Pose p(devidx,t.X / 100.0, t.Y / 100.0, t.Z / 100.0, q.W, q.X, q.Y, q.Z);
    return vox_merge->send_pose(p, devidx);
  }
  return false;
}

