#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS(DisplayName = "BaseCProjectile")
class PROJECTCORE_API ACBaseProjectile : public AActor
{
	GENERATED_BODY()

public:	
	ACBaseProjectile();

	void Tick(float DeltaTime) override;

	void FireInDirection(FVector ShootDirection);

	UFUNCTION()
	void OnHit(UPrimitiveComponent * pHitComponent, AActor * pOtherActor,
		UPrimitiveComponent * pOtherComponent, FVector NormalImpulse, const FHitResult & cHit);

protected:
	void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "Collision");
	USphereComponent * m_pCollision = nullptr;

	UPROPERTY(EditAnywhere, Category = "Movement", DisplayName = "ProjectileMovement");
	UProjectileMovementComponent * m_pProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "ProjectileMesh");
	UStaticMeshComponent * m_pProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup", DisplayName = "ProjectileDamage");
	float m_ProjectileDamage = 20.0f;
};
