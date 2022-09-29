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

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	FTimerHandle _timerHandle{};

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

	UFUNCTION(Client,Unreliable)
		void Client_OnCorrectTransform(FTransform Transform);
};
