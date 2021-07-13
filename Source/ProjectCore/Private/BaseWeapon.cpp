#include "BaseWeapon.h"

#include "Log.h"
#include "BaseProjectile.h"
#include "BaseEnemy.h"

ACBaseWeapon::ACBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	m_pWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	check(m_pWeaponMesh != nullptr);
	m_pWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	check(GEngine != nullptr);
}

void ACProjectileWeapon::Attack()
{
	Shoot();
}

void ACProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	check(GEngine != nullptr);
}

void ACProjectileWeapon::Shoot()
{
	if (m_Projectile != nullptr)
	{
		UWorld * pWorld = GetWorld();
		if (pWorld != nullptr)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			const FVector MuzzleOffset(0.0f, 0.0f, 0.0f);
			const FVector cMuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
			const FRotator cMuzzleRotation = CameraRotation;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			ACBaseProjectile * pProjectile = pWorld->SpawnActor<ACBaseProjectile>(m_Projectile,
				cMuzzleLocation, cMuzzleRotation, SpawnParams);

			if (pProjectile != nullptr)
			{
				const FVector cLaunchDirection = cMuzzleRotation.Vector();
				pProjectile->FireInDirection(cLaunchDirection);
			}
			else
			{
				UE_LOG(LogProjectCore, Warning, TEXT("ACProjectileWeapon::Shoot - pProjectile is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogProjectCore, Warning, TEXT("ACProjectileWeapon::Shoot - pWorld is nullptr"));
		}
	}
}
