// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "BuildingEscape.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Engine/World.h"
#include "Public/DrawDebugHelpers.h"
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty"));

	/// Look for attached Physics Handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		//Physics handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s physics handle is missing "), *GetOwner()->GetName());
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	///Calculate LineTraceEnd
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector()  * Reach;
	
	///Draw a red trace in the world to visualize 
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f
	);

	///Set up query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Line trace(Ray-casting) out to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	// See what we hit
	AActor *ActorHit = Hit.GetActor();
	if (ActorHit) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Things that hit %s"), *(ActorHit->GetName()));
	}
}

