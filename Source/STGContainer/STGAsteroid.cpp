// 2019 Justin Walsh, Marcus Brown, and UnrealPHX

#include "STGAsteroid.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASTGAsteroid::ASTGAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	RootComponent = SphereComponent;
}

// Called when the game starts or when spawned
void ASTGAsteroid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTGAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASTGAsteroid::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (Role == ROLE_SimulatedProxy)
	{
		if (MovementComponent)
		{
			MovementComponent->Velocity = NewVelocity;
		}
	}
}