#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TestGameModeBase.h"

#include "BaseEnemy.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UMaterialInstanceDynamic;

UCLASS(DisplayName = "BaseEnemy")
class PROJECTCORE_API ACBaseEnemy : public AActor, public IGamplaySubject
{
	GENERATED_BODY()

public:
	ACBaseEnemy();

	void Tick(float fDeltaTime) override;
	void Notify(GamplayAction Action) override;

protected:
	void BeginPlay() override;

	float TakeDamage(float fDamageAmount, FDamageEvent const & DamageEvent,
		AController * pEventInstigator, AActor * pDamageCauser) override;
public:
	UPROPERTY(EditAnywhere, Category = "Enemy", DisplayName = "CurrentHitpoints");
	float m_fCurrentHitpoints = 0.0f;

protected:
	UPROPERTY(EditAnywhere, Category = "Enemy", DisplayName = "EnemyMesh");
	UStaticMeshComponent * m_pEnemyMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy", DisplayName = "StartHitpoints");
	float m_fStartHitpoints = 100.0f;
};
