#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BaseWeapon.generated.h"

class ACBaseProjectile;

class IShoot
{
public:
	virtual void Shoot() = 0;
};

UCLASS(Abstract, DisplayName = "BaseWeapon")
class PROJECTCORE_API ACBaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ACBaseWeapon();

	virtual void Attack() PURE_VIRTUAL(ACBaseRangeWeapon::Attack, );

protected:
	void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon", DisplayName = "WeaponMesh");
	UStaticMeshComponent * m_pWeaponMesh = nullptr;
};

UCLASS(DisplayName = "ProjectileWeapon")
class PROJECTCORE_API ACProjectileWeapon : public ACBaseWeapon, public IShoot
{
	GENERATED_BODY()

public:
	void Attack() override;

protected:
	void BeginPlay() override;

	void Shoot() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", DisplayName = "Projectile");
	TSubclassOf<ACBaseProjectile> m_Projectile;
};
