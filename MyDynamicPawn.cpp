// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDynamicPawn.h"

// Sets default values
AMyDynamicPawn::AMyDynamicPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a dummy root component we can attach things to.

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create a visible object (note it is of type UStaticMeshComponent)

	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	// Create text for CountdownText
	//CountdownText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountdownNumber"));

	// Attach our visible object to our root component

	OurVisibleComponent->SetupAttachment(RootComponent);

	// Create a camera 
	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	// Attach our camera to our root component. 

	// Offset and rotate the camera with respect to the root component.
	OurCamera->SetRelativeLocation(FVector(-500.0f, 0.0f, 350.0f));
	OurCamera->SetRelativeRotation(FRotator(-35.0f, 0.0f, 0.0f));

	// Initialize the CountDownText
 /* CountdownText->SetHorizontalAlignment(EHTA_Center);
    CountdownText->SetWorldSize(150.0f);
	CountdownText->SetupAttachment(RootComponent);  
	CountdownTime = 3; */

}

// Called when the game starts or when spawned
void AMyDynamicPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyDynamicPawn::Tick(float DeltaTime)
{

	//Get Current scale of the actor in X
    float CurrentScale = OurVisibleComponent->GetComponentScale().X;
    if (bGrowing)
    {
    	// Grow to double size over the course of one second
        CurrentScale += DeltaTime;
    }
    else
    {
    	// Shrink half as fast as we grow
        CurrentScale -= (DeltaTime * 0.5f);
    }
    // Make sure we never drop below our starting size, or increase past triple size.
    CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 3.0f);
    // Update the scale
    OurVisibleComponent->SetWorldScale3D(FVector(CurrentScale));


	//Get relative location of camera in cameraLoc, static mesh relative location in objectLoc
	FTransform camRelTransform = OurCamera->GetRelativeTransform();
	FVector cameraLoc = camRelTransform.GetLocation();
	FTransform objectRelTransform = OurVisibleComponent->GetRelativeTransform();
	FVector objectLoc = objectRelTransform.GetLocation();

	//get vector FROM camera TO static mesh
	FVector direction = (objectLoc - cameraLoc).GetSafeNormal();
	
	//get distance from camera to static mesh
	float distance = (objectLoc- cameraLoc).Size();
	
	//clamp distance between 600 and 800,  bZooming ? zoom in : zoom out
	if(bZooming && distance > 600) {
		cameraLoc += direction*zoomRate*DeltaTime;
	}
	else if(!bZooming && distance < 800){
		cameraLoc -= direction*zoomRate*DeltaTime;
	}
	OurCamera->SetRelativeLocation(cameraLoc);

	//on screen debug 
	if(GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("BGrowing:%s , CurrentScale:%f, bZooming:%s, zoomRate:%f,"
		 "distance:%f "), bGrowing ? TEXT("True") : TEXT("False"), CurrentScale, bZooming ? TEXT("True") : TEXT("False"), zoomRate, distance));
	}

	// Handle movement
	if(!CurrentVelocity.IsZero()) {
		FVector NewLocation = objectLoc - (CurrentVelocity * DeltaTime); //Note multiplication by DeltaTime
		OurVisibleComponent->SetRelativeLocation(NewLocation);
	}

	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyDynamicPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//binding controls: space to zoom, WASD
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMyDynamicPawn::ToggleZooming);
	PlayerInputComponent->BindAxis("MoveX", this, &AMyDynamicPawn::Move_XAxis);
	PlayerInputComponent->BindAxis("MoveY", this, &AMyDynamicPawn::Move_YAxis);
}

// Toggles bZooming, mapped to space bar in AMyDynamicPawn::SetupPlayerInputComponent()
void AMyDynamicPawn::ToggleZooming()
{
    bZooming = !bZooming;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(TEXT("Reversing direction due to pressing spacebar!")));
    }
}

// Clamps X axis movement to -100 to 100
void AMyDynamicPawn::Move_XAxis(float AxisValue)
{
    // Move at 100 units per second forward or backward
    CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

//Clamps Y axis movement to -100 to 100
void AMyDynamicPawn::Move_YAxis(float AxisValue) 
{
	// Move at 100 units per second left or right
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

// updates the timer display
/* void ADynamicPawn::UpdateTimerDisplay()
{
    CountdownText->SetText(FString::FromInt(FMath::Max(CountdownTime, 0)));
} */