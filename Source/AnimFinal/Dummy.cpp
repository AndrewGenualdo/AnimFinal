// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
ADummy::ADummy()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));
	PoseableMesh->SetupAttachment(CapsuleComp);

	PoseableMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	PoseableMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));


	TargetMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("TargetMesh"));
	TargetMesh->SetupAttachment(CapsuleComp);
	TargetMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	TargetMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	ElapseTime = 0;
	duration = 0;
}

// Called when the game starts or when spawned
void ADummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!InitialPositions.IsEmpty() && ElapseTime < duration)
	{
			ElapseTime += DeltaTime;
			currentLoopTime += DeltaTime;
			float Alpha = FMath::Clamp(ElapseTime / duration, 0.0f, 1.0f);
			FVector TargetRootLoc = TargetMesh->GetBoneLocation(TEXT("root"), EBoneSpaces::ComponentSpace);
			for (int32 boneIndex = 0; boneIndex < PoseableMesh->GetNumBones(); ++boneIndex)
			{
				FName BoneName = PoseableMesh->GetBoneName(boneIndex);
				FName TargetBoneName = TargetMesh->GetBoneName(boneIndex);
				FTransform NewTransform;
				NewTransform.SetLocation(FMath::Lerp(InitialPositions[boneIndex].GetLocation(), TargetMesh->GetBoneLocation(BoneName,EBoneSpaces::ComponentSpace) - TargetRootLoc, Alpha));
				NewTransform.SetRotation(FQuat::Slerp(InitialPositions[boneIndex].GetRotation(), TargetMesh->GetBoneRotationByName(BoneName,EBoneSpaces::ComponentSpace).Quaternion(), Alpha));
				NewTransform.SetScale3D(FMath::Lerp(InitialPositions[boneIndex].GetScale3D(), TargetMesh->GetBoneScaleByName(BoneName, EBoneSpaces::ComponentSpace), Alpha));
				UE_LOG(LogTemp, Warning, TEXT("Target Rotation %f %f %f"),NewTransform.GetLocation().X, NewTransform.GetLocation().Y, NewTransform.GetLocation().Z);
				PoseableMesh->SetBoneTransformByName(BoneName, NewTransform, EBoneSpaces::ComponentSpace);
			}
			GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;

	
			if (EndingSpot.Z > 0)
			{
				GetCharacterMovement()->JumpZVelocity = EndingSpot.Z;
				if (EndingSpot.X > 0 || EndingSpot.Y > 0)
				{
					LaunchCharacter(EndingSpot,true,true);
				}
				else
				{
					Jump();
				}

			}
			else
			{
				AddMovementInput(EndingSpot);
			}
			if (Alpha == 1.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("AnimationDone"));
				animationIndex++;
				if (!finalTransition)
				PlayAnimation();
			}
	}

}

void ADummy::SetNewPose(FString NextPose,float newDuration,FVector EndLocation,float speed)
{
	LoadSkeletonPosition(TargetMesh, NextPose);
	duration = newDuration;
	ElapseTime = 0;
	InitialPositions.Empty();
	EndingSpot = EndLocation;
	CurrentSpeed = speed;
	for (int32 boneIndex = 0; boneIndex <= PoseableMesh->GetNumBones(); ++boneIndex)
	{
		FTransform BoneT;
		FName BoneName = PoseableMesh->GetBoneName(boneIndex);
		BoneT.SetLocation(PoseableMesh->GetBoneLocation(BoneName,EBoneSpaces::ComponentSpace));
		BoneT.SetRotation(PoseableMesh->GetBoneQuaternion(BoneName, EBoneSpaces::ComponentSpace));
		BoneT.SetScale3D(PoseableMesh->GetBoneScaleByName(BoneName, EBoneSpaces::ComponentSpace));
		InitialPositions.Add(BoneT);
	}
}

void ADummy::PlayAnimation()
{
	if (AnimationFiles.IsEmpty() || Durations.IsEmpty() || Locations.IsEmpty() || Speeds.IsEmpty())
		return;

	if (animationIndex < AnimationFiles.Num())
	{
		SetNewPose(AnimationFiles[animationIndex], Durations[animationIndex],Locations[animationIndex],Speeds[animationIndex]);
	}
	else
	{
		animationIndex = 0;
		if (isLooping && loopDuration > currentLoopTime)
		{
			SetNewPose(AnimationFiles[animationIndex], Durations[animationIndex],Locations[animationIndex],Speeds[animationIndex]);
		}
		else
		{
			currentLoopTime = 0;
			finalTransition = true;
			SetNewPose(IdlePositions, 0.25, FVector(0, 0, 0), 0);
		}
	}
}

void ADummy::SetAnimationFile(TArray<FString> Files, TArray<float> durations, const TArray<FVector> MovementVector, TArray<float> Speed, bool loop, float lDuration)
{
	if (Files.Num() != durations.Num() || Files.Num() != MovementVector.Num() || Files.Num() != Speed.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FAIL"));
		return;
	}

	AnimationFiles.Empty();
	Durations.Empty();
	Locations.Empty();
	Speeds.Empty();
	finalTransition = false;
	for (int32 i = 0; i < Files.Num(); ++i)
	{
		AnimationFiles.Add(Files[i]);
		Durations.Add(durations[i]);
		Locations.Add(MovementVector[i]);
		Speeds.Add(Speed[i]);
	}

	isLooping = loop;
	loopDuration = lDuration;
}



//https://www.youtube.com/watch?v=YcTa_wMjCzY (For reading and writing files in UE5)
void ADummy::RecordPosition(USkeletalMeshComponent* SkeletalMeshComp,FString SaveDirectory) const
{
	if (!SkeletalMeshComp || !SkeletalMeshComp->GetSkinnedAsset()) return;

	const FReferenceSkeleton& RefSkeleton = SkeletalMeshComp->GetSkinnedAsset()->GetRefSkeleton();
	int32 NumBones = RefSkeleton.GetNum();
	FString Display;

	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
		FName BoneName = RefSkeleton.GetBoneName(BoneIndex);

		FTransform BoneTransform = SkeletalMeshComp->GetBoneTransform(BoneIndex);

		FVector BoneLocation = SkeletalMeshComp->GetBoneLocation(BoneName, EBoneSpaces::ComponentSpace);

		FQuat BoneRotation = SkeletalMeshComp->GetBoneQuaternion(BoneName, EBoneSpaces::ComponentSpace);

		FVector BoneScale = BoneTransform.GetScale3D();

		FString BoneDataLine = FString::Printf(TEXT("%s %.4f %.4f %.4f %.4f %.4f %.4f %.4f % .4f % .4f % .4f"),*BoneName.ToString(),BoneLocation.X,BoneLocation.Y,BoneLocation.Z,BoneRotation.X, BoneRotation.Y,BoneRotation.Z,BoneRotation.W,BoneScale.X,BoneScale.Y,BoneScale.Z);

		Display += BoneDataLine + TEXT("\n");

	}

	//Save to text file
	FString AbsoluteFilePath = SaveDirectory;

	if (!FFileHelper::SaveStringToFile(Display, *AbsoluteFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Success"));
}

void ADummy::InitSkeleton(FString SaveDirectory)
{
	LoadSkeletonPosition(PoseableMesh, SaveDirectory);
	IdlePositions = SaveDirectory;
}

void ADummy::LoadSkeletonPosition(UPoseableMeshComponent* SkeletalMeshComp, FString File)
{
	FString FileData = CheckFile(File);

	TArray <FString> Lines;
	FileData.ParseIntoArray(Lines, TEXT("\n"),true);


	const FTransform CompToWorld = SkeletalMeshComp->GetComponentTransform();
	const FTransform WorldToComp = CompToWorld.Inverse();
	for (const FString& Line : Lines)
	{
		UE_LOG(LogTemp, Log, TEXT("Read Line: %s"), *Line);
		TArray<FString> Data;
		Line.ParseIntoArray(Data, TEXT(" "));

		if (Data.Num() < 11)
		{
			continue;
		}

		FName BoneName(*Data[0]);
		int32 boneIndex = SkeletalMeshComp->GetBoneIndex(BoneName);

		if (boneIndex == INDEX_NONE)
		{
			continue;
		}

		FTransform BoneTransform = SkeletalMeshComp->GetBoneTransform(boneIndex);

		float X = FCString::Atof(*Data[1]);
		float Y = FCString::Atof(*Data[2]);
		float Z = FCString::Atof(*Data[3]);
		BoneTransform.SetLocation(FVector(X, Y, Z));
		
		FQuat rotation;

		rotation.X = FCString::Atof(*Data[4]);
		rotation.Y = FCString::Atof(*Data[5]);
		rotation.Z = FCString::Atof(*Data[6]);
		rotation.W = FCString::Atof(*Data[7]);
		rotation.Normalize();
		BoneTransform.SetRotation(rotation);

		float ScaleX = FCString::Atof(*Data[8]);
		float ScaleY = FCString::Atof(*Data[9]);
		float ScaleZ = FCString::Atof(*Data[10]);
		BoneTransform.SetScale3D(FVector(ScaleX, ScaleY, ScaleZ));

		//FTransform TrueTransform = BoneTransform * WorldToComp;

		SkeletalMeshComp->SetBoneTransformByName(BoneName,BoneTransform,EBoneSpaces::ComponentSpace);


	}
	
	
}

bool ADummy::IsNewTarget(UPoseableMeshComponent* SkelMeshComp) const
{
	int32 NumBones = PoseableMesh->GetNumBones();

	for (int32 boneIndex = 0; boneIndex <= NumBones; ++boneIndex)
	{
		if (!PoseableMesh->GetBoneTransform(boneIndex).Equals(SkelMeshComp->GetBoneTransform(boneIndex)))
		{
			return true;
		}
	}
	return false;
}



 FString ADummy::CheckFile(FString File) const
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*File))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail"));
		return FString();
	}

	FString FileData;
	if (!FFileHelper::LoadFileToString(FileData, *File))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail"));
		return FString();
	}
	return FileData;
}

// Called to bind functionality to input
void ADummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

