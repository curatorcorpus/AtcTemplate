// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelVideoComponent.h"

DEFINE_LOG_CATEGORY(VoxVidLog);

UVoxelVideoComponent::UVoxelVideoComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;
}

void UVoxelVideoComponent::BeginPlay()
{
  Super::BeginPlay();

  // VIMR Settings
  VIMRShowInvisible = false;
  VIMRShowSpecial = false;
  VIMRComponentID = "VoxelVideo";

  if (GConfig->GetString(TEXT("Voxels"), TEXT("VoxelVideoPath"), voxelvideo_path, GGameIni))
  {
	UE_LOG(VoxLog, Log, TEXT("Using voxel video path from DefaultGame.ini %s"), *voxelvideo_path);
  }

  //voxelvideosPath = FPaths::ProjectContentDir() + FString("VoxelVideos/");
}

void UVoxelVideoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  if (!cmdStack.empty()) {
	  cmdStack.top()();
	  cmdStack.pop();
  }
}

void UVoxelVideoComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  if(VoxelVideoReader != nullptr) 
	delete VoxelVideoReader;
  VoxelVideoReader = nullptr;
  Super::EndPlay(EndPlayReason);
}

// Custom Methods

void UVoxelVideoComponent::LoadVoxelVideo(FString file)
{
  if (VoxelVideoReader != nullptr)
  {
	  VoxelVideoReader->close();
	  for (auto i : AudioStreams) {
		  i.second->Stop();
		  i.second->clear();
	  }
	  AudioStreams.clear();
  }

  FString file_path = voxelvideo_path + file;

  VoxelVideoReader = new VIMR::VoxVidPlayer(on_frame, on_loop, on_end);
  bool isopen = VoxelVideoReader->open(TCHAR_TO_ANSI(*file_path));
  if (!isopen)
  {
	UE_LOG(VoxVidLog, Error, TEXT("Failed to open the Voxel Video: %s"), *file_path);
  }
  VoxelVideoReader->set_loop(false);
  UE_LOG(VoxVidLog, Log, TEXT("Loaded file %s"), *file_path);

  VIMR::AudioStream tmp_astrm;
  //while (VoxelVideoReader->get_audiostream(tmp_astrm)) - should be in a loop
  VoxelVideoReader->get_audiostream(tmp_astrm);

  URuntimeAudioSource* newSource = NewObject<URuntimeAudioSource>(this);
  FString wav_path = voxelvideo_path + FString(tmp_astrm.file_name);
  FString wav_label = FString(tmp_astrm.voxel_label);

  newSource->RegisterComponent();
  newSource->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
  newSource->LoadWav(wav_path);
  AudioStreams[tmp_astrm.voxel_label] = newSource;

  UE_LOG(VoxLog, Log, TEXT("Loaded wav: %s"), *wav_path);
  UE_LOG(VoxLog, Log, TEXT("Loaded wav: %s"), *wav_label);
}

TArray<FString> UVoxelVideoComponent::GetAllRecordings()
{
  TArray<FString> files;
  files.Empty();

  FString recordingPath = FPaths::ProjectContentDir() + "VoxelVideos/";
  FString voxelvideo_ext = "vx4";

  if (FPaths::DirectoryExists(*recordingPath))
  {
	IFileManager::Get().FindFiles(files, *recordingPath, *voxelvideo_ext);
	for (int i = 0; i < files.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("These files Exists: %s"), *files[i]);
	}
	UE_LOG(LogTemp, Log, TEXT("This Path Exists: %s"), *recordingPath)
  }
  else UE_LOG(LogTemp, Log, TEXT("This path doesn't exist"));

  return files;
}

void UVoxelVideoComponent::SetAudioLocation(FVector Location)
{
	for (auto as : AudioStreams)
	{
		as.second->GetAudioComponent()->SetWorldLocation(Location);
	}
}

// MULTIMEDIA CONTROL FUNCTIONS

void UVoxelVideoComponent::_pause()
{
	VoxelVideoReader->pause();
	for (auto i : AudioStreams) {
		i.second->Pause();
	}
}

void UVoxelVideoComponent::_play()
{
	if (!VoxelVideoReader->start())
	{
		UE_LOG(VoxVidLog, Log, TEXT("Player failed to start!"));
	}

	for (auto i : AudioStreams) {
		i.second->Start();
	}
}

void UVoxelVideoComponent::_stop()
{
	//_restart();
	_pause();
}

/*
void UVoxelVideoComponent::_restart()
{
	VoxelVideoReader->restart();
	for (auto i : AudioStreams) {
		i.second->Stop();
		i.second->Start();
	}
}*/
