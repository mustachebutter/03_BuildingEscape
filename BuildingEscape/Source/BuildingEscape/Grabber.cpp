// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "BuildingEscape.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
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
	FindPhysicsHandleComponent(); 
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {
		//Move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

void UGrabber::Grab()
{	
	// LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent(); //gets the mesh in our case
	auto ActorHit = HitResult.GetActor();

	// If we hit something then attach a physics handle
		//TODO attach physics handle
	if (ActorHit)
	{
		PhysicsHandle->GrabComponent(
			ComponentToGrab, 
			NAME_None, //no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true //allow rotation
		);
	}
}

void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();

}


void UGrabber::FindPhysicsHandleComponent() 
{
	/// Look for attached Physics Handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s physics handle is missing "), *GetOwner()->GetName());

	}
	

}

void UGrabber::SetupInputComponent()
{
	/// Look for attached Input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
	//Bind the input
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s input component is missing "), *GetOwner()->GetName());
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	///Set up query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Line trace(Ray-casting) out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}

FVector UGrabber::GetReachLineStart()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	///Calculate LineTraceEnd
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector()  * Reach;

}