// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuilderBlock.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	None = 0,
	Floor,
	Corner,
	Wall,
	Stairs
};

UCLASS()
class TPSPLUTOOL_API ABuilderBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilderBlock();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* mMesh;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//class UBoxComponent* mBoxCollider;

	float rotAngle = 0.0;
	float rotated = false;
	void RotateRight();
	void RotateLeft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		EBlockType mBlockType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int32 blockIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool mAllowRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool mMeetsPrerequisites = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool mSnapping = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	FVector mSnappedPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", Meta = (ExposeOnSpawn = true), ReplicatedUsing = OnRep_PlaceBlock)
	bool mPlaced;

	UFUNCTION()
	void OnRep_PlaceBlock();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	FVector mNewBlockPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	FVector mPlayerForward;

	UFUNCTION(BlueprintCallable)
	bool CheckForPrerequisites(TMap<EBlockType, int32> prerequisites, TArray<ABuilderBlock*> blocksInScene);

	//UFUNCTION()
	//void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	//		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION(BlueprintNativeEvent)
	//void Overlaping();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float AngleBetweenVectors(FVector a, FVector b);

	float unsnappedZ = 0;

};
