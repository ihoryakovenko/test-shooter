#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/SaveGame.h"

#include "TestGameModeBase.generated.h"

class ACBaseCharacter;
class ACBaseEnemy;
class FThreadSafeBool;

USTRUCT(DisplayName = "EnemySaveData")
struct FCEnemySaveData
{
	GENERATED_BODY()

	UPROPERTY();
	FVector m_Location;

	UPROPERTY();
	FRotator m_Rotation;

	UPROPERTY();
	float m_fCurrentHitpoints = 0;

	UPROPERTY();
	UClass * m_pClass = nullptr;
};

USTRUCT(DisplayName = "PlayerSaveData")
struct FCPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY();
	FVector m_PlayerLocation;

	UPROPERTY();
	int m_nMissionNumber = 0;
};

UCLASS(DisplayName = "TestGameModeBase")
class PROJECTCORE_API UCBaseSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY();
	FCPlayerSaveData m_PlayerSaveData;

	UPROPERTY();
	TArray<FCEnemySaveData> m_EnemySaveData;
};

enum GamplayAction
{
	AttackAction,
	DeathAction,
	SaveAction,
	LoadAction,
	RestoreAction
};

class IGameplayObserver
{
public:
	virtual void UpdateGameplay(GamplayAction Action) = 0;
};

class IGamplaySubject
{
public:
	void AttachObserver(IGameplayObserver * pObserver);
	virtual void Notify(GamplayAction Action) = 0;

protected:
	IGameplayObserver * m_pObserver = nullptr;
};

UCLASS(DisplayName = "TestGameModeBase")
class PROJECTCORE_API ACTestGameModeBase : public AGameModeBase, public IGameplayObserver
{
	GENERATED_BODY()

public:
	ACTestGameModeBase();

	void Tick(float fDeltaTime) override;
	void UpdateGameplay(GamplayAction Action) override;

protected:
	void StartPlay() override;
	void EndPlay(const EEndPlayReason::Type cEndPlayReason) override;

private:
	void MissionOne();
	void MissionTwo();
	void MissionThree();

	void SaveGame();
	void LoadGame();

	void RestoreEnemies();
	bool SpawnEnemy(const FCEnemySaveData & cEnemySpawnData);

protected:
	UPROPERTY(EditAnywhere, Category = "Gameplay", DisplayName = "AttackLimit");
	float m_fAttackLimit = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay", DisplayName = "TraceDistance");
	float m_fTraceDistance = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Gameplay", DisplayName = "AttackAngleLimit");
	float m_fAttackAngleLimit = 15.0f;

private:
	UWorld * m_pWorld = nullptr;
	ACBaseCharacter * m_pPlayer = nullptr;

	TArray<AActor *> m_Enemies;
	int m_nMissionNumber = 1;

	FThreadSafeBool m_bOnThreadRun = false;
	FCriticalSection m_Mutex;

	FString m_sMainSaveName = TEXT("Save1");
	int32 m_nUserIndex = 0;
	TArray<FCEnemySaveData> m_EnemiesSpawnData;
};
