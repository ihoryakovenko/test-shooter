#include "BaseProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Log.h"

ACBaseProjectile::ACBaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	InitialLifeSpan = 3.0f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	check(RootComponent != nullptr);

	m_pCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(RootComponent != nullptr);
	m_pCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	m_pCollision->OnComponentHit.AddDynamic(this, &ACBaseProjectile::OnHit);
	m_pCollision->InitSphereRadius(15.0f);
	RootComponent = m_pCollision;

	m_pProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	check(m_pProjectileMovement != nullptr);
	m_pProjectileMovement->SetUpdatedComponent(m_pCollision);
	m_pProjectileMovement->InitialSpeed = 3000.0f;
	m_pProjectileMovement->MaxSpeed = 3000.0f;
	m_pProjectileMovement->bRotationFollowsVelocity = true;
	m_pProjectileMovement->bShouldBounce = true;
	m_pProjectileMovement->Bounciness = 0.3f;
	m_pProjectileMovement->ProjectileGravityScale = 0.0f;

	m_pProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	check(m_pProjectileMesh != nullptr);
	m_pProjectileMesh->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	m_pProjectileMesh->SetupAttachment(RootComponent);

	m_pProjectileMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
}

void ACBaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	check(GEngine != nullptr);
}

void ACBaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACBaseProjectile::FireInDirection(FVector ShootDirection)
{
	m_pProjectileMovement->Velocity = ShootDirection * m_pProjectileMovement->InitialSpeed;
}

void ACBaseProjectile::OnHit(UPrimitiveComponent * pHitComponent, AActor * pOtherActor,
	UPrimitiveComponent * pOtherComponent, FVector NormalImpulse, const FHitResult & cHit)
{
	if (pOtherActor != nullptr)
	{
		m_pCollision->DestroyComponent();
		m_pProjectileMovement->DestroyComponent();
		m_pProjectileMesh->DestroyComponent();

		UGameplayStatics::ApplyPointDamage(pOtherActor, m_ProjectileDamage, GetVelocity().GetSafeNormal(),
			cHit, GetInstigatorController(), this, UDamageType::StaticClass());

		Destroy();
	}
	else
	{
		UE_LOG(LogProjectCore, Warning, TEXT("ACBaseProjectile::OnHit - pOtherActor is nullptr"));
	}
}
