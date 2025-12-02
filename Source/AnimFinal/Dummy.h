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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = Setup)
	virtual void RecordPosition(USkeletalMeshComponent* SkeletalMeshComp,FString SaveDirectory) const;
	UFUNCTION(BlueprintCallable, Category = Setup)
	virtual void LoadSkeletonPosition(UPoseableMeshComponent* SkeletalMeshComp, FString SaveDirectory) const;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
