// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "RuntimeAudioSource.h"
#include "VoxelSourceBaseComponent.h"

#include <functional>
#include <stack>

#include "VIMR/octree.hpp"
#include "VIMR/player.hpp"

#include "VoxelVideoComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(VoxVidLog, All, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedPlayback);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), config=DefaultVIMR)
class VOXELS_API UVoxelVideoComponent : public UVoxelSourceBaseComponent
{
	GENERATED_BODY()
	
public:

	// BLUEPRINT CALLABLE FUNCTIONS

	UFUNCTION(BlueprintCallable, Category = "PlaybackControl")
		void Pause() { cmdStack.push((PlaybackControlFnPtr)std::bind(&UVoxelVideoComponent::_pause, this)); }
	UFUNCTION(BlueprintCallable, Category = "PlaybackControl")
		void Play() { cmdStack.push((PlaybackControlFnPtr)std::bind(&UVoxelVideoComponent::_play, this)); }
	UFUNCTION(BlueprintCallable, Category = "PlaybackControl")
		void Stop() { cmdStack.push((PlaybackControlFnPtr)std::bind(&UVoxelVideoComponent::_stop, this)); }

	//UFUNCTION(BlueprintCallable, Category = "PlaybackControl")
		//void Restart() { cmdStack.push((PlaybackControlFnPtr)std::bind(&UVoxelVideoComponent::_restart, this)); }

	UFUNCTION(BlueprintCallable, Category = "FileManagement")
		TArray<FString> GetAllRecordings();

	UFUNCTION(BlueprintCallable, Category = "FileManagement")
		void LoadVoxelVideo(FString filepath);

	UFUNCTION(BlueprintCallable)
		void SetAudioLocation(FVector Location);

	UPROPERTY(BlueprintAssignable, Category="EventDispatchers")
	  FOnFinishedPlayback OnFinishedPlayback;

	/**
	*	Constructs a Voxel Video component.
	*/
	UVoxelVideoComponent();

	// UE4 METHODS

	/**
	*	Main update method which is called every frame automatically by Unreal Engine.
	*
	*	@param DeltaTime
	*	@param TickType
	*	@param ThisTickFunction
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	bool playback_loop = false;
	bool playback_end = false;
	bool playback_update = false;

	int NumAudioStreams = 0;
	
	FString voxelvideo_path;

	typedef std::function<void(void)> PlaybackControlFnPtr;

	std::function<void(VIMR::Octree*)> on_frame = [this](Octree* voxels) 
	{ 
	  this->CopyVoxelData(*voxels);
	  playback_update = true;
	  
	};
	std::function<void()> on_loop = [this]() 
	{ 
	  playback_loop = true; 
	};
	std::function<void()> on_end = [this]() 
	{ 
	  playback_end = true; 
	  ResetFrameBuffer();
	  OnFinishedPlayback.Broadcast();
	  UE_LOG(VoxVidLog, Log, TEXT("END PLAY: %s"), playback_end ? TEXT("true") : TEXT("false"));
	};

	std::stack<PlaybackControlFnPtr> cmdStack;
	std::map<std::string, URuntimeAudioSource*> AudioStreams;

	// DON'T instantate these things here. UBT can't handle it. Do it in BeginPlay
	VIMR::VoxVidPlayer* VoxelVideoReader;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void _pause();
	void _play();
	void _stop();
	//void _restart();
};
