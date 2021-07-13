#include "BaseEnemy.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

#include "Log.h"

ACBaseEnemy::ACBaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	SetCanBeDamaged(true);

	m_pEnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMeshComponent"));
	check(m_pEnemyMesh != nullptr);

	m_fCurrentHitpoints = m_fStartHitpoints;
}

float ACBaseEnemy::TakeDamage(float fDamageAmount, FDamageEvent const & DamageEvent,
	AController * pEventInstigator, AActor * pDamageCauser)
{
	if (CanBeDamaged())
	{
		m_fCurrentHitpoints = m_fCurrentHitpoints - fDamageAmount;
		if (m_fCurrentHitpoints <= 0)
		{
			Notify(GamplayAction::DeathAction);
			Destroy();
		}
	}

	return fDamageAmount;
}

void ACBaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	check(GEngine != nullptr);
}

void ACBaseEnemy::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);
}

void ACBaseEnemy::Notify(GamplayAction Action)
{
	if (m_pObserver != 0)
	{
		m_pObserver->UpdateGameplay(Action);
	}
}
