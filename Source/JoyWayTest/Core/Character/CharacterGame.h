#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterGame.generated.h"

DECLARE_EVENT(AGameCharacter,FAttack);

UCLASS()
class JOYWAYTEST_API ACharacterGame : public ACharacter
{
	GENERATED_BODY()

public:
	
	ACharacterGame();

	FAttack AttackEvent;

	class UCameraComponent* GetCameraComponent();
	class UAttackComponent* GetAttackComponent();

	TArray<FVector>& GetLastPositions() { return _arrLastPositions; }

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	FTimerHandle _timerHandle{};

	TArray<FVector> _arrLastPositions{};

	int8 _maxBuffer = 10;

	float _saveStep = 0.1f;
	float _stepBuffer = 0.f;
	float _deltaTime = 0.f;
	float _baseTurnRate = 45.f;
	float _baseLookUpRate = 45.f;

	void OnJumpPressed();
	void OnJumpReleased();
	void OnFire();
	void OnMoveForward(float Axis);
	void OnMoveRight(float Axis);
	void OnTurn(float Axis);
	void OnLookUp(float Axis);

	void CorrectTransform();

	void SavingLastPosition(float DeltaTime);

	UFUNCTION(Client,Unreliable)
		void Client_OnCorrectTransform(FTransform Transform);
};
