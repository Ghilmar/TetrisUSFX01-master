// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.h"
#include <functional>
#include "Piece.generated.h"

UCLASS()
class TETRISUSFX01_API APiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APiece();

private:


	//APiece();
	////The Inventory of this Actor	SINGLETON 
	//UPROPERTY()
	//	static APiece* Instance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		class USceneComponent* SceneComponent;

	void DrawDebugLines();
	void TestRotate();
	void MoveLeft();
	void MoveRight();
	bool MoveDown(bool PlaySound = true);

	void Dismiss();
	bool CheckWillCollision(std::function<FVector(FVector OldLocation)> ChangeBeforeCheck);

	//mover despacio hacia abajo
	bool MoveDownSlow( bool PlaySound = true );


	UPROPERTY(EditAnywhere)
	TArray<class UMaterial*> Colors;

private:

	TArray<ABlock*> Blocks;

	 
	
public:
	void mensaje();
	void SpawnBlocks();
	int ObtenerNumBloques() const;
	////The instance of this Class 
	//UPROPERTY()
	//	APiece* getInstance();
};
