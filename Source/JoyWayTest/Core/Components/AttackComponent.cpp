
#include "AttackComponent.h"

#include "AttackComponent.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerState.h"
#include "JoyWayTest/Core/Character/CharacterGame.h"
#include "Kismet/KismetMathLibrary.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	_owner = Cast<ACharacterGame>(GetOwner());

	if (!_owner) {return;}

	_owner->AttackEvent.AddUObject(this,&UAttackComponent::OnAttack);
}

void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttackComponent::OnAttack()
{
	FHitResult hitResult{};

	const auto cameraComponent = _owner->GetCameraComponent();
	const auto cameraLocation=cameraComponent->GetComponentLocation();
	const auto cameraForwardVector=cameraComponent->GetForwardVector()*10000.f;
	const auto world = GetWorld();

	FCollisionQueryParams collisionQueryParams{};
	collisionQueryParams.AddIgnoredActor(_owner);

	DrawDebugLine(world, cameraLocation, cameraLocation + cameraForwardVector, FColor::Green);
	world->LineTraceSingleByChannel(hitResult, cameraLocation, cameraLocation+ cameraForwardVector,ECollisionChannel::ECC_Camera, collisionQueryParams);

	const auto hitCharacterGame=Cast<ACharacterGame>(hitResult.Actor);

	if (hitCharacterGame)
	{
		Server_CalculateHit(hitCharacterGame, cameraLocation, cameraLocation + cameraForwardVector, _owner->GetPlayerState()->GetPing());
	}
}


void UAttackComponent::Server_CalculateHit_Implementation(ACharacterGame* GameCharacter, FVector StartHit, FVector EndHit, uint8 Ping)
{
	TArray<AActor*> arrIgnoreGameCharacter{};

	arrIgnoreGameCharacter.Add(_owner);

	GameCharacter->GetAttackComponent()->Server_OnCalculateHit(arrIgnoreGameCharacter,StartHit, EndHit, Ping);
}

void UAttackComponent::Server_OnCalculateHit_Implementation(const TArray<AActor*>& CharacterGames,FVector StartHit, FVector EndHit, uint8 Ping)
{
	_owner->SetReplicates(false);

	auto world = GetWorld();

	auto arrLastPositions=_owner->GetLastPositions();
	auto a = UKismetMathLibrary::FFloor(Ping*3 / 100);

	if (a > arrLastPositions.Num()-1) { a = arrLastPositions.Num()-1; }

	auto remainderDivision = Ping * 3 % 100;
	auto alphaProjectedLocation = UKismetMathLibrary::NormalizeToRange(remainderDivision, 0, 99);
	auto projectedLocation = UKismetMathLibrary::VEase(arrLastPositions[a], arrLastPositions[a + 1], alphaProjectedLocation,EEasingFunc::Linear);
	auto initialPosition =_owner->GetActorTransform();

	FHitResult hitResult{};
	FCollisionQueryParams collisionQueryParams{};

	collisionQueryParams.AddIgnoredActors(CharacterGames);

	DrawDebugBox(world, projectedLocation, FVector(20.f, 20.f, 100.f), FColor::Blue,false, 5.f);

	_owner->SetActorLocation(projectedLocation);

	DrawDebugLine(world, StartHit, EndHit,FColor::Blue, false, 5.f);
	world->LineTraceSingleByChannel(hitResult, StartHit, EndHit,ECollisionChannel::ECC_Camera, collisionQueryParams);

	if (hitResult.Actor==_owner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Hit"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("NotHit"));
	}

	_owner->SetActorTransform(initialPosition );
	_owner->SetReplicates(true);

}



