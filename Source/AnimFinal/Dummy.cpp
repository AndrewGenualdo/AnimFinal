// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/CapsuleComponent.h"
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

		FVector BoneLocation = BoneTransform.GetLocation();

		FRotator BoneRotation = BoneTransform.Rotator();

		FString BoneDataLine = FString::Printf(TEXT("%s %.4f %.4f %.4f %.4f %.4f %.4f"),*BoneName.ToString(),BoneLocation.X,BoneLocation.Y,BoneLocation.Z,BoneRotation.Roll,BoneRotation.Pitch,BoneRotation.Yaw);

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

void ADummy::LoadSkeletonPosition(UPoseableMeshComponent* SkeletalMeshComp, FString File) const
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*File))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail"));
		return;
	}

	FString FileData;
	if (!FFileHelper::LoadFileToString(FileData, *File))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail"));
		return;
	}

	TArray <FString> Lines;
	FVector RootLoc;
	FileData.ParseIntoArray(Lines, TEXT("\n"),true);

	for (const FString& Line : Lines)
	{
		UE_LOG(LogTemp, Log, TEXT("Read Line: %s"), *Line);
		TArray<FString> Data;
		Line.ParseIntoArray(Data, TEXT(" "));

		if (Data.Num() < 7)
		{
			return;
		}

		FName BoneName(*Data[0]);
		int32 boneIndex = PoseableMesh->GetBoneIndex(BoneName);

		if (boneIndex == INDEX_NONE)
		{
			return;
		}

		FTransform BoneTransform = PoseableMesh->GetBoneTransform(boneIndex);

		float X = FCString::Atof(*Data[1]);
		float Y = FCString::Atof(*Data[2]);
		float Z = FCString::Atof(*Data[3]);
		if (BoneName == TEXT("root"))
		{
			RootLoc = FVector(X, Y, Z);
		}
		BoneTransform.SetLocation(FVector(X-RootLoc.X, Y-RootLoc.Y, Z-RootLoc.Z - 90));
		
		FRotator rotation;

		rotation.Roll = FCString::Atof(*Data[4]);
		rotation.Pitch = FCString::Atof(*Data[5]);
		rotation.Yaw = FCString::Atof(*Data[6]);
		BoneTransform.SetRotation(rotation.Quaternion());

		PoseableMesh->SetBoneTransformByName(BoneName,BoneTransform,EBoneSpaces::ComponentSpace);


	}
	
	
}



// Called to bind functionality to input
void ADummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

