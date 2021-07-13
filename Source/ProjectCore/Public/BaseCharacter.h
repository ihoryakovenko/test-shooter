#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "TestGameModeBase.h"

#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class ACBaseWeapon;
class ACProjectileWeapon;
class USphereComponent;

UCLASS(DisplayName = "BaseCharacter")
class PROJECTCORE_API ACBaseCharacter : public ACharacter, public IGamplaySubject
{
	GENERATED_BODY()

public:
	ACBaseCharacter();

	void SetupPlayerInputComponent(class UInputComponent * pPlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddWeapon(ACBaseWeapon * pNewWeapon);

public:
	UPROPERTY(EditAnywhere, Category = "Weapon", DisplayName = "");
	bool m_bCanAttack = true;

	ACBaseWeapon * m_pCurrentWeapon;

protected:
	void BeginPlay() override;
	void SaveGame();
	void LoadGame();
	void RestoreEnemies();

	void MoveForward(float fScaleValue);
	void MoveRight(float fScaleValue);
	void Attack();
	void Notify(GamplayAction Action) override;

private:
	void AxisMove(float fScaleValue, EAxis::Type InAxis);

protected:
	UPROPERTY(EditAnywhere, Category = "Camera", DisplayName = "SpringArm");
	USpringArmComponent * m_pSpringArm = nullptr;

	UPROPERTY(EditAnywhere, Category = "Camera", DisplayName = "Camera");
	UCameraComponent * m_pCamera = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", DisplayName = "SkeletalMesh");
	USkeletalMeshComponent * m_pSkeletalMesh = nullptr;
};
