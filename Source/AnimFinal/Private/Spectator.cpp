// Fill out your copyright notice in the Description page of Project Settings.


#include "Spectator.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASpectator::ASpectator()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpectator::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpectator::Test() const
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->SetShowMouseCursor(true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("This is an Example on-screen debug message."));
	}

	
}

// Called every frame
void ASpectator::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

