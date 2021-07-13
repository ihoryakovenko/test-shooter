#include "TestGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "Math/Vector.h"
#include "Async/Async.h"
#include "HAL/ThreadSafeBool.h"

#include "Log.h"
#include "BaseCharacter.h"
#include "BaseWeapon.h"
#include "BaseEnemy.h"

void IGamplaySubject::AttachObserver(IGameplayObserver * pObserver)
{
	m_pObserver = pObserver;
}

ACTestGameModeBase::ACTestGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ACTestGameModeBase::StartPlay()
{
	Super::StartPlay();
	check(GEngine != nullptr);

	m_pWorld = GetWorld();
	check(m_pWorld != nullptr);

	APawn * pPlayerPawn = m_pWorld->GetFirstPlayerController()->GetPawn();
	check(pPlayerPawn != nullptr);
	m_pPlayer = Cast<ACBaseCharacter>(pPlayerPawn);
	m_pPlayer->AttachObserver(this);

	UGameplayStatics::GetAllActorsOfClass(m_pWorld, ACBaseEnemy::StaticClass(), m_Enemies);

	for (auto pActor : m_Enemies)
	{
		ACBaseEnemy * pEnemy = Cast<ACBaseEnemy>(pActor);
		FCEnemySaveData SpawnData{ pEnemy->GetActorLocation(), pEnemy->GetActorRotation(),
			pEnemy->m_fCurrentHitpoints, pEnemy->GetClass() };

		m_EnemiesSpawnData.Emplace(SpawnData);

		pEnemy->AttachObserver(this);
	}

	m_bOnThreadRun = true;
	Async(EAsyncExecution::Thread,
	[&]()
	{
		while (m_bOnThreadRun)
		{
			{
				FScopeLock Lock(&m_Mutex);
				const int32 cNum = m_Enemies.Num();
				if (cNum >= 2)
				{
					m_Enemies.Sort(
					[](const AActor & A, const AActor & B)
					{
						const ACBaseEnemy & cA = static_cast<const ACBaseEnemy &>(A);
						const ACBaseEnemy & cB = static_cast<const ACBaseEnemy &>(B);
						return cA.m_fCurrentHitpoints > cB.m_fCurrentHitpoints;
					});
				}
			}

			FPlatformProcess::Yield();
		}
	});

	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, "Move - WASD, Shoot - LMB");
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, "Load - 9, Save - 5, Restore enemies - R");
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, "Objectives");
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red,
		FString::Printf(TEXT("1) Minimum distance to target - %f"), m_fAttackLimit));
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, "2) Enemy is in sight");
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red,
		FString::Printf(TEXT("3) Attack angle limit - %f"), m_fAttackAngleLimit));
}

void ACTestGameModeBase::EndPlay(const EEndPlayReason::Type cEndPlayReason)
{
	m_bOnThreadRun = false;
}

bool ACTestGameModeBase::SpawnEnemy(const FCEnemySaveData & cEnemySpawnData)
{
	bool bResult = false;

	do
	{
		UWorld * pWorld = GetWorld();
		if (pWorld == nullptr)
		{
			UE_LOG(LogProjectCore, Error, TEXT("ACTestGameModeBase::SpawnEnemy - pWorld is nullptr"));
			break;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ACBaseEnemy * pEnemy = pWorld->SpawnActor<ACBaseEnemy>(cEnemySpawnData.m_pClass,
			cEnemySpawnData.m_Location, cEnemySpawnData.m_Rotation, SpawnParams);

		if (pEnemy == nullptr)
		{
			UE_LOG(LogProjectCore, Error, TEXT("ACTestGameModeBase::SpawnEnemy - pEnemy is nullptr"));
			break;
		}

		pEnemy->m_fCurrentHitpoints = cEnemySpawnData.m_fCurrentHitpoints;
		pEnemy->AttachObserver(this);

		m_Enemies.Push(pEnemy);

		bResult = true;
	}
	while (false);

	return bResult;
}

void ACTestGameModeBase::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);
}

void ACTestGameModeBase::UpdateGameplay(GamplayAction Action)
{
	switch (Action)
	{
		case AttackAction:

			if (m_nMissionNumber == 1)
			{
				MissionOne();
			}
			else if (m_nMissionNumber == 2)
			{
				MissionTwo();
			}
			else if (m_nMissionNumber == 3)
			{
				MissionThree();
			}
			break;

		case DeathAction:

			{
				FScopeLock Lock(&m_Mutex);
				m_Enemies.Pop();
			}

			++m_nMissionNumber;
			if (m_nMissionNumber > 3)
			{
				m_nMissionNumber = 1;
			}

			break;

		case SaveAction:

			SaveGame();
			break;

		case LoadAction:

			LoadGame();
			break;

		case RestoreAction:

			RestoreEnemies();
			break;

		default:
			break;
	}
}

void ACTestGameModeBase::SaveGame()
{
	UCBaseSaveGame * pSave = Cast<UCBaseSaveGame>(UGameplayStatics::CreateSaveGameObject(
		UCBaseSaveGame::StaticClass()));

	pSave->m_PlayerSaveData.m_PlayerLocation = m_pPlayer->GetActorLocation();
	pSave->m_PlayerSaveData.m_nMissionNumber = m_nMissionNumber;

	{
		FScopeLock Lock(&m_Mutex);

		for (auto pActor : m_Enemies)
		{
			ACBaseEnemy * pEnemy = Cast<ACBaseEnemy>(pActor);
			FCEnemySaveData SpawnData{pEnemy->GetActorLocation(), pEnemy->GetActorRotation(),
				 pEnemy->m_fCurrentHitpoints, pEnemy->GetClass() };

			pSave->m_EnemySaveData.Emplace(SpawnData);
		}
	}

	if (UGameplayStatics::SaveGameToSlot(pSave, m_sMainSaveName, m_nUserIndex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Game Saved");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Save Failed");
	}
}

void ACTestGameModeBase::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(m_sMainSaveName, m_nUserIndex))
	{
		UCBaseSaveGame * pSave = Cast<UCBaseSaveGame>(UGameplayStatics::LoadGameFromSlot(
			m_sMainSaveName, m_nUserIndex));

		if (pSave != nullptr)
		{
			m_pPlayer->SetActorLocation(pSave->m_PlayerSaveData.m_PlayerLocation);
			m_nMissionNumber = pSave->m_PlayerSaveData.m_nMissionNumber;

			{
				FScopeLock Lock(&m_Mutex);

				for (auto * pEnemy : m_Enemies)
				{
					pEnemy->Destroy();
				}

				m_Enemies.Empty();

				for (const auto & cSaveData : pSave->m_EnemySaveData)
				{
					if (!SpawnEnemy(cSaveData))
					{
						UE_LOG(LogProjectCore, Warning, TEXT("ACTestGameModeBase::LoadGame spawn emnemy false"));
					}
				}
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Game Loaded");
		}
		else
		{
			UE_LOG(LogProjectCore, Error, TEXT("ACTestGameModeBase::SpawnEnemy - pSave is nullptr"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Save slot does not exist");
	}
}

void ACTestGameModeBase::MissionOne()
{
	int nSuitableEnemies = 0;

	{
		FScopeLock Lock(&m_Mutex);

		for (auto pEnemy : m_Enemies)
		{
			const FVector cPlayerLocation = m_pPlayer->GetActorLocation();
			const FVector cActivatedEnemyLocation = pEnemy->GetActorLocation();
			const float cDistance = FVector::Dist(cPlayerLocation, cActivatedEnemyLocation);

			if (cDistance <= m_fAttackLimit)
			{
				pEnemy->SetCanBeDamaged(true);
				++nSuitableEnemies;
			}
			else
			{
				pEnemy->SetCanBeDamaged(false);
			}
		}
	}

	if (nSuitableEnemies >= 1)
	{
		m_pPlayer->m_bCanAttack = true;
	}
	else
	{
		m_pPlayer->m_bCanAttack = false;
	}
}

void ACTestGameModeBase::MissionTwo()
{
	FVector StartTracePoint;
	FRotator Rotator;
	FHitResult HitResult;

	m_pPlayer->m_pCurrentWeapon->GetActorEyesViewPoint(StartTracePoint, Rotator);

	const FVector cEndTracePoint = StartTracePoint + (Rotator.Vector() * m_fTraceDistance);

	FCollisionQueryParams TraceParams;
	UWorld * pWorld = GetWorld();
	const bool cbTraceResult = pWorld->LineTraceSingleByChannel(HitResult, StartTracePoint,
		cEndTracePoint, ECC_Visibility, TraceParams);

	if (cbTraceResult)
	{
		AActor * pActor = HitResult.GetActor();
		if (pActor->IsA(ACBaseEnemy::StaticClass()))
		{
			pActor->SetCanBeDamaged(true);
			m_pPlayer->m_bCanAttack = true;
			return;
		}
	}

	m_pPlayer->m_bCanAttack = false;
}

void ACTestGameModeBase::MissionThree()
{
	int nSuitableEnemies = 0;

	{
		FScopeLock Lock(&m_Mutex);

		for (auto pEnemy : m_Enemies)
		{
			const FRotator cPlayerForwardRotation = m_pPlayer->GetViewRotation();
			const FRotationMatrix cPlayerRotationMatrix(cPlayerForwardRotation);
			const FVector cPlayerYRotation = cPlayerRotationMatrix.GetScaledAxis(EAxis::Y);
			const FVector cPlayerZRotation = cPlayerRotationMatrix.GetScaledAxis(EAxis::Z);
			const FVector cDistance = pEnemy->GetActorLocation() - m_pPlayer->GetActorLocation();

			// Calculate angles
			const float cfX = FVector::DotProduct(cDistance, cPlayerForwardRotation.Vector());
			const float cfY = FVector::DotProduct(cDistance, cPlayerYRotation);
			const float cfZ = FVector::DotProduct(cDistance, cPlayerZRotation);
			// Convert to degrees
			const float cfAngleToEnemyY = FMath::RadiansToDegrees(atan2(cfX, cfY)) - 90.0f;
			const float cfAngleToEnemyZ = FMath::RadiansToDegrees(atan2(cfX, cfZ)) - 90.0f;

			if ((FMath::Abs(cfAngleToEnemyY) <= m_fAttackAngleLimit) &&
				(FMath::Abs(cfAngleToEnemyZ) <= m_fAttackAngleLimit))
			{
				pEnemy->SetCanBeDamaged(true);
				++nSuitableEnemies;
			}
			else
			{
				pEnemy->SetCanBeDamaged(false);
			}
		}
	}

	if (nSuitableEnemies >= 1)
	{
		m_pPlayer->m_bCanAttack = true;
	}
	else
	{
		m_pPlayer->m_bCanAttack = false;
	}
}

void ACTestGameModeBase::RestoreEnemies()
{
	FScopeLock Lock(&m_Mutex);

	for (auto pEnemy : m_Enemies)
	{
		pEnemy->Destroy();
	}

	m_Enemies.Empty();

	for (const auto & cSaveData : m_EnemiesSpawnData)
	{
		if (!SpawnEnemy(cSaveData))
		{
			UE_LOG(LogProjectCore, Warning, TEXT("ACTestGameModeBase::RestoreEnemies spawn emnemy false"));
		}
	}
}
