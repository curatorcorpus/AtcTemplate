#include "VoxelSourceBaseComponent.h"
#include "Engine.h"
#include "VIMR/semanticvoxel.hpp"
#include "VoxelRenderSubComponent.h"
#include <chrono>

using VIMR::SemVox;

DEFINE_LOG_CATEGORY(VoxLog);

void UVoxelSourceBaseComponent::CopyVoxelData(const VIMR::Octree& voxels) {

  if (inProgress) {
    UE_LOG(VoxLog, Log, TEXT("Received more voxels but didn't finish copying the last frame. Ignoring this frame."));
    return;
  }

  inProgress = true;
  VoxelCount[buffIdx] = 0;
  VoxelSizemm[buffIdx] = voxels.get_vox_mm();

  int pos_idx = 0;
  auto * v = voxels.get_next_voxel();

  while(v!=nullptr){
    if (SemVox::get_flag(v, SemVox::Flag::INVISIBLE) != 0 && !VIMRShowInvisible) {
    }
    else if (SemVox::get_flag(v, SemVox::Flag::SPECIAL) != 0 && !VIMRShowSpecial) {
    }
    else{

      const int16_t pY = v->pos.y;
      const int16_t pX = v->pos.x;
      const int16_t pZ = v->pos.z;
      const int8 label = SemVox::get_label<int8_t>(v);

      if (VIMRColorVoxels && label < 30 + 27 && label > 30) memcpy(&ColourData[buffIdx][pos_idx], VIMR::palette[label - 31], 3);
      else SemVox::color_to(v, reinterpret_cast<char*>(&ColourData[buffIdx][pos_idx]));

      CoarsePositionData[buffIdx][pos_idx + 0] = (pZ >> 8) + 128;
      CoarsePositionData[buffIdx][pos_idx + 1] = (pY >> 8) + 128;
      CoarsePositionData[buffIdx][pos_idx + 2] = (pX >> 8) + 128;

      PositionData[buffIdx][pos_idx + 0] = pZ & 0xFF;
      PositionData[buffIdx][pos_idx + 1] = pY & 0xFF;
      PositionData[buffIdx][pos_idx + 2] = pX & 0xFF;

      pos_idx += VOXEL_TEXTURE_BPP;
      VoxelCount[buffIdx]++;

      if (VoxelCount[buffIdx] >= MaxVoxels) {
          UE_LOG(VoxLog, Log, TEXT("Too many voxels!"));
          break;
      }
    }
    v = voxels.get_next_voxel();
  }
  framecount++;
  if (modprintctr % 10 == 0) {
    const auto time_now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    UE_LOG(VoxLog, Log, TEXT("frame %i %llu ms. voxcount=%llu"), framecount, voxels.get_timestamp(),  voxels.vox_count());
  }
  modprintctr++;

  while (inDisplay)
    ;

  buffIdx = (buffIdx + 1) % BufferSize;
  dispIdx = (dispIdx + 1) % BufferSize;
  inProgress = false;
}

UVoxelSourceBaseComponent::UVoxelSourceBaseComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void UVoxelSourceBaseComponent::BeginPlay()
{
  Super::BeginPlay();
  for (int i = 0; i < BufferSize; i++) {
    CoarsePositionData[i] = new uint8[MaxVoxels * VOXEL_TEXTURE_BPP]();
    PositionData[i] = new uint8[MaxVoxels * VOXEL_TEXTURE_BPP]();
    ColourData[i] = new uint8[MaxVoxels * VOXEL_TEXTURE_BPP]();
    VoxelCount[i] = 0;
    VoxelSizemm[i] = 0;
  }
  buffIdx = 1;
  dispIdx = 0;
  framecount = 0;
  if(GConfig->GetString(TEXT("Voxels"), TEXT("InstancePath"), InstanceConfigPath, GGameIni))
  {
    UE_LOG(VoxLog, Log, TEXT("Loaded instance config file path from DefaultGame.ini %s"), *InstanceConfigPath);
  }
  GConfig->GetString(TEXT("Voxels"), TEXT("UsingRenderRmtCompId"), UsingRenderRmtCompId, GGameIni);
}

void UVoxelSourceBaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{   
  Super::EndPlay(EndPlayReason);
  
  for (int i = 0; i < BufferSize; i++) {
    delete CoarsePositionData[i];
    delete PositionData[i];
    delete ColourData[i];
    VoxelCount[i] = 0;
    VoxelSizemm[i] = 0;
  }
  inProgress = false;
}


void UVoxelSourceBaseComponent::GetFramePointers(int & voxelCount, uint8 *& coarsePositionData, uint8 *& positionData, uint8 *& colourData, uint8 & voxelmm)
{
  inDisplay = true;
  voxelmm = this->VoxelSizemm[dispIdx];
  voxelCount = this->VoxelCount[dispIdx];
  coarsePositionData = this->CoarsePositionData[dispIdx];
  positionData = this->PositionData[dispIdx];
  colourData = this->ColourData[dispIdx];
  inDisplay = false;
}

void UVoxelSourceBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVoxelSourceBaseComponent::ResetFrameBuffer()
{
  for (int i = 0; i < BufferSize; i++) 
  {
    delete CoarsePositionData[i];
    CoarsePositionData[i] = new uint8[MaxVoxels * VOXEL_TEXTURE_BPP]();
    delete PositionData[i];
    PositionData[i] = new uint8[MaxVoxels * VOXEL_TEXTURE_BPP]();
    delete ColourData[i];
    ColourData[i] = new uint8[MaxVoxels * VOXEL_TEXTURE_BPP]();
    VoxelCount[i] = 0;
    VoxelSizemm[i] = 0;
  }
  buffIdx = 1;
  dispIdx = 0;
  modprintctr = 0;
  inDisplay = false;
  inProgress = false;
}