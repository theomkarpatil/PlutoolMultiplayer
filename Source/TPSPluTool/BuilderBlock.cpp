// Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("",off)

#include "BuilderBlock.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values
ABuilderBlock::ABuilderBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	mMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = mMesh;

	//mBoxCollider = CreateDefaultSubobject<UBoxComponent>("Box Collider");

	//mMesh->OnComponentBeginOverlap.AddDynamic(this, &ABuilderBlock::OverlapBegin);
}

void ABuilderBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuilderBlock, mPlaced);
}

// Called when the game starts or when spawned
void ABuilderBlock::BeginPlay()
{
	Super::BeginPlay();

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("RotateRight", IE_Pressed, this, &ABuilderBlock::RotateRight);
		InputComponent->BindAction("RotateLeft", IE_Pressed, this, &ABuilderBlock::RotateLeft);
		InputComponent->BindAction("PlaceBlock", IE_Pressed, this, &ABuilderBlock::OnRep_PlaceBlock);
		//InputComponent->BindAction("PickBlock", IE_Pressed, this, );

		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void ABuilderBlock::RotateRight()
{
	rotAngle = -90.0;
	rotated = true;
}

void ABuilderBlock::RotateLeft()
{
	rotAngle = 90.0;
	rotated = true;
}

void ABuilderBlock::OnRep_PlaceBlock()
{
	if (mMeetsPrerequisites && HasAuthority())
		mPlaced = true;
}


bool ABuilderBlock::CheckForPrerequisites(TMap<EBlockType, int32> prerequisites, TArray<ABuilderBlock*> blocksInScene)
{
	if (!HasAuthority())
		return false;

	// the required number of objects
	int required = prerequisites.Num();

	if (required == 0)
		return true;

	bool requirementMet = false;

	int blocksFound;
	for (auto& itr : prerequisites)
	{
		blocksFound = 0;
		for (int i = 0; i < blocksInScene.Num(); ++i)
		{
			if (!blocksInScene[i])
				break;

			if (itr.Key == blocksInScene[i]->mBlockType)
			{
				blocksFound++;
			}

			if (blocksFound == itr.Value)
				break;
		}

		if (blocksFound < itr.Value)
		{
			requirementMet = false;
			break;
		}

		requirementMet = true;
	}

	mMeetsPrerequisites = requirementMet;

	return mMeetsPrerequisites;
}

//void ABuilderBlock::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	Overlaping();
//}
//
//void ABuilderBlock::Overlaping_Implementation()
//{
//
//}

// Called every frame
void ABuilderBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!mPlaced && HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("BlockName: %s"), *GetFName().ToString());
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(301, 10.0f, FColor::Yellow, *GetFName().ToString());

		FHitResult hit;

		float angle = AngleBetweenVectors(FVector(0.0, 0.0, rotAngle), mPlayerForward);

		//UE_LOG(LogTemp, Warning, TEXT(mPlayerForward.ToString()));
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, mPlayerForward.ToString());*/


		if (mSnapping)
		{
			// while snapping restrict X & Y movement but keep +Z movement to exit snapping
			if (mNewBlockPosition.Z > unsnappedZ)
			{
				FVector newPos = FVector(GetActorLocation().X, GetActorLocation().Y, mNewBlockPosition.Z);
				SetActorLocation(newPos, false, &hit, ETeleportType::TeleportPhysics);
			}
			else
			{
				FVector newPos = FVector(GetActorLocation().X, GetActorLocation().Y, unsnappedZ);
				SetActorLocation(newPos, false, &hit, ETeleportType::TeleportPhysics);
			}
		}
		else
		{
			unsnappedZ = mNewBlockPosition.Z;
			SetActorLocation(mNewBlockPosition, false, &hit, ETeleportType::TeleportPhysics);
		}

		if (mAllowRotation)
		{
			if (rotated)
			{
				const FVector rot = FVector(0, 0, rotAngle);
				FQuat quat = FQuat::MakeFromEuler(rot);

				AddActorWorldRotation(quat, false, &hit, ETeleportType::TeleportPhysics);
				rotated = false;
			}
		}

		rotAngle = 0.0;
	}
}

float ABuilderBlock::AngleBetweenVectors(FVector a, FVector b)
{
	return UKismetMathLibrary::RadiansToDegrees(UKismetMathLibrary::Acos(FVector::DotProduct(a, b)));
}


