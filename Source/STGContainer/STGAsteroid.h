// 2019 Justin Walsh, Marcus Brown, and UnrealPHX

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STGAsteroid.generated.h"

UCLASS()
class STGCONTAINER_API ASTGAsteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTGAsteroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when a velocity change was replicated
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UProjectileMovementComponent* MovementComponent;
};
