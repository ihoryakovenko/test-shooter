#include "BaseCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"
#include "Engine/Blueprint.h"
#include "Components/SphereComponent.h"

#include "Log.h"
#include "BaseWeapon.h"
#include "BaseEnemy.h"

ACBaseCharacter::ACBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	check(m_pSpringArm != nullptr);
	m_pSpringArm->SetupAttachment(RootComponent);

	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	check(m_pCamera != nullptr);
	m_pCamera->SetupAttachment(m_pSpringArm);
	m_pCamera->bUsePawnControlRotation = 1;

	m_pSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	check(m_pSkeletalMesh != nullptr);
	m_pSkeletalMesh->SetOnlyOwnerSee(true);
	m_pSkeletalMesh->SetupAttachment(m_pCamera);
	m_pSkeletalMesh->bCastDynamicShadow = false;
	m_pSkeletalMesh->CastShadow = false;
}

void ACBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(GEngine != nullptr);
}

void ACBaseCharacter::MoveForward(float fScaleValue)
{
	AxisMove(fScaleValue, EAxis::X);
}

void ACBaseCharacter::MoveRight(float fScaleValue)
{
	AxisMove(fScaleValue, EAxis::Y);
}

void ACBaseCharacter::Attack()
{
	Notify(GamplayAction::AttackAction);

	if (m_bCanAttack)
	{
		if (m_pCurrentWeapon == nullptr)
		{
			UE_LOG(LogProjectCore, Warning, TEXT("ACBaseCharacter::Attack - m_pCurrentWeapon is nullptr"));
			return;
		}

		m_pCurrentWeapon->Attack();
	}
}

void ACBaseCharacter::AxisMove(float fScaleValue, EAxis::Type InAxis)
{
	if (Controller == nullptr)
	{
		UE_LOG(LogProjectCore, Warning, TEXT("ACBaseCharacter::AxisMove - Controller is nullptr"));
		return;
	}

	if (FMath::IsNearlyZero(fScaleValue))
	{
		return;
	}

	const FRotator cRotation = Controller->GetControlRotation();
	const FRotator cYawRotation(0, cRotation.Yaw, 0);

	const FRotationMatrix cRotationMatrix(cYawRotation);
	const FVector cDirection = cRotationMatrix.GetUnitAxis(InAxis);
	AddMovementInput(cDirection, fScaleValue);
}

void ACBaseCharacter::SaveGame()
{
	Notify(GamplayAction::SaveAction);
}

void ACBaseCharacter::LoadGame()
{
	Notify(GamplayAction::LoadAction);
}

void ACBaseCharacter::SetupPlayerInputComponent(UInputComponent * pPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(pPlayerInputComponent);
	
	pPlayerInputComponent->BindAction("Save", IE_Pressed, this, &ACBaseCharacter::SaveGame);
	pPlayerInputComponent->BindAction("Load", IE_Pressed, this, &ACBaseCharacter::LoadGame);
	pPlayerInputComponent->BindAction("Restore", IE_Pressed, this, &ACBaseCharacter::RestoreEnemies);
	pPlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	pPlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	pPlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACBaseCharacter::Attack);
	pPlayerInputComponent->BindAxis("MoveForward", this, &ACBaseCharacter::MoveForward);
	pPlayerInputComponent->BindAxis("MoveRight", this, &ACBaseCharacter::MoveRight);
	pPlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	pPlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void ACBaseCharacter::AddWeapon(ACBaseWeapon * pNewWeapon)
{
	if (pNewWeapon == nullptr)
	{
		UE_LOG(LogProjectCore, Error, TEXT("ACBaseCharacter::AddWeapon - SpawnedWeapon is nullptr"));
		return;
	}
	
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);

	pNewWeapon->AttachToComponent(m_pSkeletalMesh, AttachmentTransformRules);

	const FRotator cRotationCharacter = GetActorRotation();
	pNewWeapon->SetActorRotation(cRotationCharacter);

	m_pCurrentWeapon = pNewWeapon;
}

void ACBaseCharacter::RestoreEnemies()
{
	Notify(GamplayAction::RestoreAction);
}

void ACBaseCharacter::Notify(GamplayAction Action)
{
	if (m_pObserver != 0)
	{
		m_pObserver->UpdateGameplay(Action);
	}
}
