
#include "CharacterGame.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "JoyWayTest/Core/Components/AttackComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACharacterGame::ACharacterGame()
{
	PrimaryActorTick.bCanEverTick = true;

}

UCameraComponent* ACharacterGame::GetCameraComponent()
{
	return FindComponentByClass<UCameraComponent>();
}

UAttackComponent* ACharacterGame::GetAttackComponent()
{
	return FindComponentByClass<UAttackComponent>();
}

void ACharacterGame::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy) { GetMesh()->SetVisibility(false); }

	if (!HasAuthority()) { return; }

	for (auto i = 0; i < _maxBuffer; ++i) {_arrLastPositions.Add({});}

	GetWorldTimerManager().SetTimer(_timerHandle,this,&ACharacterGame::CorrectTransform,1.f,true);
}

void ACharacterGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_deltaTime = DeltaTime;

	if (!HasAuthority()) {return;}

	const auto capsuleComponent = GetCapsuleComponent();

	DrawDebugBox(GetWorld(), capsuleComponent->GetComponentLocation(),FVector(20.f,20.f,100.f),FColor::Green);

	SavingLastPosition(DeltaTime);
}

void ACharacterGame::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterGame::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterGame::OnJumpReleased);
	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ACharacterGame::OnFire);
	PlayerInputComponent->BindAxis("MoveForward",this,&ACharacterGame::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ACharacterGame::OnMoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ACharacterGame::OnTurn);
	PlayerInputComponent->BindAxis("LookUp",this,&ACharacterGame::OnLookUp);
}

void ACharacterGame::OnJumpPressed()
{
	Jump();
}

void ACharacterGame::OnJumpReleased()
{
	StopJumping();
}

void ACharacterGame::OnFire()
{
	AttackEvent.Broadcast();
}

void ACharacterGame::OnMoveForward(float Axis)
{
	AddMovementInput(GetActorForwardVector(),Axis);
}

void ACharacterGame::OnMoveRight(float Axis)
{
	AddMovementInput(GetActorRightVector(),Axis);
}

void ACharacterGame::OnTurn(float Axis)
{
	AddControllerYawInput(Axis);
}

void ACharacterGame::OnLookUp(float Axis)
{
	AddControllerPitchInput(Axis);
}

void ACharacterGame::CorrectTransform()
{
	Client_OnCorrectTransform(GetActorTransform());
}

void ACharacterGame::SavingLastPosition(float DeltaTime)
{
	_stepBuffer += DeltaTime;

	if (_stepBuffer<_saveStep) {return;}

	_stepBuffer = 0.f;

	for (auto i= _maxBuffer-1;i>=0;i--)
	{
		if (i-1>=0)
		{
			_arrLastPositions[i] = _arrLastPositions[i-1];
		}
		else
		{
			_arrLastPositions[0]= GetActorLocation();
		}
	}
}

void ACharacterGame::Client_OnCorrectTransform_Implementation(FTransform Transform)
{
	SetActorTransform(UKismetMathLibrary::TInterpTo(GetActorTransform(),Transform,_deltaTime,1.0f));
}

