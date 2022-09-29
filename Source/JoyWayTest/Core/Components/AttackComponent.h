#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"


class ACharacterGame;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JOYWAYTEST_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackComponent();

	UFUNCTION(Server, Reliable)
		void Server_CalculateHit(ACharacterGame* GameCharacter, FVector StartHit, FVector EndHit, uint8 Ping);

	UFUNCTION(Server, Reliable)
		void Server_OnCalculateHit(const TArray<AActor*>& CharacterGames,FVector StartHit, FVector EndHit, uint8 Ping);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY()
		class ACharacterGame* _owner{};

	void OnAttack();

};
