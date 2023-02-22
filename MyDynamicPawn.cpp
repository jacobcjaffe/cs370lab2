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

	// Attach our visible object to our root component

	OurVisibleComponent->SetupAttachment(RootComponent);

	// Create a camera 
	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	// Attach our camera to our root component. 

	// Offset and rotate the camera with respect to the root component.
	OurCamera->SetRelativeLocation(FVector(-500.0f, 0.0f, 350.0f));
	OurCamera->SetRelativeRotation(FRotator(-35.0f, 0.0f, 0.0f));

	// Initialize the CountDownText
	CountdownText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountdownNumber"));
    CountdownText->SetHorizontalAlignment(EHTA_Center);
    CountdownText->SetWorldSize(150.0f);
	CountdownText->SetupAttachment(RootComponent);  
	CountdownTime = 3;
	CountdownText->SetVisibility(false);

	// Listens to mouse hovering
	OnBeginCursorOver.AddDynamic(this, &AMyDynamicPawn::OnMouseOver);

	// Listens to mouse click
	OnClicked.AddDynamic(this, &AMyDynamicPawn::OnMouseClick);
}

// Called when the game starts or when spawned
void AMyDynamicPawn::BeginPlay()
{
/* 	// Starts Timer, updates the display and location
	UpdateTimerDisplay();
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AMyDynamicPawn::AdvanceTimer, 1.0f, true);
	CountdownText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); //Values are just an example
	CountdownText->SetRelativeRotation(FRotator(0, -180, 0)); //Values are just an example
 */


	CountdownText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); //Values are just an example
	CountdownText->SetRelativeRotation(FRotator(0, -180, 0)); //Values are just an example
	// Make cursor visible on screen
	if(GetWorld()) {
		APlayerController *myPlayerController = GetWorld()->GetFirstPlayerController();

		if(myPlayerController != NULL) {
			myPlayerController->bShowMouseCursor=true;
			myPlayerController->bEnableMouseOverEvents=true;
			myPlayerController->bEnableClickEvents=true;
		}
	}
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

	//change zoomRate over time
	zoomRate = FMath::Clamp(zoomRate + cZoom*DeltaTime, 0, 300);

	//if zoomRate = 0, display debug message
	if (zoomRate == 0 && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Magenta, FString::Printf(TEXT("Caution: Zoom Rate is 0. Unable to Move!")));
    }
	
	//clamp distance between 600 and 800,  bZooming ? zoom in : zoom out
	if(bZooming && distance > 600) {
		cameraLoc += direction*zoomRate*DeltaTime;
	}
	else if(!bZooming && distance < 900){
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
		FVector NewLocation = objectLoc + (CurrentVelocity * DeltaTime); //Note multiplication by DeltaTime
		OurVisibleComponent->SetRelativeLocation(NewLocation);

		// Making the text move with the visible component
		CountdownText->SetRelativeLocation(NewLocation + FVector(0.0f, 0.0f, 50.0f));
	}

	// Handle Camera rotataion
	OurCamera->SetRelativeRotation((1*direction).Rotation());

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
	PlayerInputComponent->BindAxis("changeZoom", this, &AMyDynamicPawn::changeZoom);
}

// Toggles bZooming, mapped to space bar in AMyDynamicPawn::SetupPlayerInputComponent()
void AMyDynamicPawn::ToggleZooming()
{
    bZooming = !bZooming;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Yellow, FString::Printf(TEXT("Reversing direction due to pressing spacebar!")));
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

// Clamps the Zoomrate between 0 and 300 cm/s
void AMyDynamicPawn::changeZoom(float AxisValue) 
{
	cZoom = FMath::Clamp(AxisValue, -1.0, 1.0f) * 100.0f;
}

// Called to update the text in CountdownText
void AMyDynamicPawn::UpdateTimerDisplay() {
	CountdownText->SetText(FText::FromString(FString::FromInt(FMath::Max(CountdownTime, 0))));
}

// Counts the time for CountdownText
void AMyDynamicPawn::AdvanceTimer()
{
    --CountdownTime;
    UpdateTimerDisplay();

	if (CountdownTime < 1) {
		CountdownHasFinished();
	}
    if (CountdownTime < 0)
    {
        //We're done counting down, so stop running the timer.
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
        
		//reverse zoom
		bZooming = !bZooming;

		CountdownText->SetVisibility(false);
    }
}

// Called when timer completes
void AMyDynamicPawn::CountdownHasFinished()
{
    //Change to a special readout
    CountdownText->SetText(FText::FromString("GO!"));
}

// Called when Mouse is over actor
void AMyDynamicPawn::OnMouseOver(AActor* touchedActor)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Magenta, FString::Printf(TEXT("Mouse over!!")));
    }
}


// called when mouse click
void AMyDynamicPawn::OnMouseClick(AActor* clickedActor, FKey click)
{
	if(GEngine) {
		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Orange, FString::Printf(TEXT("Mouse clicked!! Timer reset to:%i"), CountdownTime));
	}
	// Starts Timer, updates the display and location
	CountdownText->SetVisibility(true);
	UpdateTimerDisplay();
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AMyDynamicPawn::AdvanceTimer, 1.0f, true);
}
