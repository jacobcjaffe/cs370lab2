// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/TextRenderComponent.h"
#include "MyDynamicPawn.generated.h"


UCLASS()
class PLAYINGWITHDELTATIME_API AMyDynamicPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyDynamicPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// boolean variable to determine growth
	UPROPERTY(EditAnywhere, Category = "bGrowing")
	bool bGrowing;

	// boolean variable to determine camera zoom
	UPROPERTY(EditAnywhere, Category = "bZooming")
	bool bZooming;

	// zoom rate
	UPROPERTY(EditAnywhere, Category = "zoomRate")
	float zoomRate = 40;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// static mesh component
	UPROPERTY(EditAnywhere, Category = "MyComponents")
	UStaticMeshComponent* OurVisibleComponent;

	// camera component
	UPROPERTY(EditAnywhere, Category = "MyCamera")
	UCameraComponent* OurCamera; 

	// toggle zoom
	void ToggleZooming();

	// move x axis
	void Move_XAxis(float AxisValue);

	// move y axis
	void Move_YAxis(float AxisValue);

	// changes zoomRate
	void changeZoom(float AxisValue);
	float cZoom = 0;

	// vector to update camera relative position
	FVector CurrentVelocity;

	//Set up Countdown Timer
	UPROPERTY(EditAnywhere)
	int32 CountdownTime = 3;

	// updates the text in CountdownText
	void UpdateTimerDisplay();	
	UTextRenderComponent* CountdownText;

	// calls to keep track of time
	void AdvanceTimer();

	// changes CountdownText when timer has finished
	void CountdownHasFinished();


	FTimerHandle CountdownTimerHandle;

	// callback function when mouse is over actor
	UFUNCTION()
	void OnMouseOver(AActor* touchedActor);

	// callback function for mouse click on object
	UFUNCTION()
	void OnMouseClick(AActor* clickedActor, FKey click);
};
