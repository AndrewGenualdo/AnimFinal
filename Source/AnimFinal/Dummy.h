// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Dummy.generated.h"


UCLASS()
class ANIMFINAL_API ADummy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADummy();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPoseableMeshComponent* PoseableMesh;
	FString IdlePositions;
	TArray<FTransform> InitialPositions;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPoseableMeshComponent* TargetMesh;
	float duration = 0;
	float ElapseTime = 0;
	int32 animationIndex = 0;
	TArray<FString> AnimationFiles;
	TArray<float> Durations;
	TArray<FVector> Locations;
	TArray<float> Speeds;
	bool isLooping;
	float loopDuration;
	float currentLoopTime;
	FVector EndingSpot;
	float CurrentSpeed;
	bool finalTransition;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = Setup)
	virtual void RecordPosition(USkeletalMeshComponent* SkeletalMeshComp,FString SaveDirectory) const;
	UFUNCTION(BlueprintCallable, Category = Setup)
	virtual void InitSkeleton(FString SaveDirectory);
	virtual void LoadSkeletonPosition(UPoseableMeshComponent* SkeletalMeshComp, FString SaveDirectory);
	virtual bool IsNewTarget(UPoseableMeshComponent* SkelMeshComp) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetNewPose(FString NextPose,float duration,FVector NewLocation, float speed);
	UFUNCTION(BlueprintCallable, Category = Movement)
	virtual void PlayAnimation();
	UFUNCTION(BlueprintCallable, Category = Movement)
	virtual void SetAnimationFile(TArray<FString> Files, TArray<float> durations, TArray<FVector> MovementDirection, TArray<float> speed, bool loop, float lDuration);
	// Called to bind functionality to input
	 FString  CheckFile(FString File) const;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
