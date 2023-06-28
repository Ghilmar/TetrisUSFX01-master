// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
// Sets default values




ABoard:: ABoard()
{

	PrimaryActorTick.bCanEverTick = true;
}

void ABoard::BeginPlay()
{
	Super::BeginPlay();
	
    for (TActorIterator<APiece> it(GetWorld()); it; ++it)
    {
        if (it->GetFName() == TEXT("DissmissPieces"))
        {
            it->Dismiss();
            it->Destroy();
        }
    }
    FVector NextPieceLocation(0.0f, 100.0f, 170.0f);
    FRotator NextPieceRotation(0.0f, 0.0f, 0.0f);
    NextPiece = GetWorld()->SpawnActor<APiece>(NextPieceLocation, NextPieceRotation);

    FVector SubNextPieceLocation(0.0f, 90.0f, 140.0f);
    FRotator SubNextPieceRotation(0.0f, 0.0f, 0.0f);
    SubNextPiece = GetWorld()->SpawnActor<APiece>(SubNextPieceLocation, SubNextPieceRotation);
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bGameOver)
    {
        return;
    }

    switch (Status)
    {
    case PS_NOT_INITED:
        NewPiece();
        CoolLeft = CoolDown;
        Status = PS_MOVING;
        break;
    case PS_MOVING:
        CoolLeft -= DeltaTime;
        if (CoolLeft <= 0.0f)
        {
            MoveDown();
        }
        break;
    case PS_GOT_BOTTOM:
        CoolLeft -= DeltaTime;
        if (CoolLeft <= 0.0f)
        {
            if (CurrentPiece)
            {
                CurrentPiece->Dismiss();
                CurrentPiece->Destroy();
            }
            CurrentPiece = nullptr;
            NewPiece();
            CoolLeft = CoolDown;
            Status = PS_MOVING;
        }
        break;
    default:
        break;
    }
}

// Called to bind functionality to input
void ABoard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ABoard::Rotate);
    PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ABoard::MoveLeft);
    PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ABoard::MoveRight);
    PlayerInputComponent->BindAction("MoveDownToEnd", IE_Pressed, this, &ABoard::MoveDownToEnd);
    PlayerInputComponent->BindAction("NewPiece", IE_Pressed, this, &ABoard::NewPiece);
    //PlayerInputComponent->BindAction("CheckLine", IE_Pressed, this, &ABoard::CheckLine);

    // reinicia el juego 
    PlayerInputComponent->BindAction("ReiniciaJuego", IE_Pressed, this, &ABoard::ReiniciaJuego);
    // fin del juego
    //PlayerInputComponent->BindAction("Fin_Juego", IE_Pressed, this, &ABoard::Fin_Juego);

    // bajar rapidp la pieza
    PlayerInputComponent->BindAction("BajarRapido", IE_Pressed, this, &ABoard::BajarRapido);


}

void ABoard::Rotate()
{
    if (CurrentPiece && Status != PS_GOT_BOTTOM)
    {
        CurrentPiece->TestRotate();
    }
}

void ABoard::MoveLeft()
{
    if (CurrentPiece)
    {
        CurrentPiece->MoveLeft();
        if (Status == PS_GOT_BOTTOM)
        {
            MoveDownToEnd();
        }
    }
}

void ABoard::MoveRight()
{
    if (CurrentPiece)
    {
        CurrentPiece->MoveRight();
        if (Status == PS_GOT_BOTTOM)
        {
            MoveDownToEnd();
        }
    }
}

void ABoard::MoveDown()
{
    if (CurrentPiece)
    {
        if (!CurrentPiece->MoveDown())
        {
            Status = PS_GOT_BOTTOM;
        }
        CoolLeft = CoolDown;
    }
}

void ABoard::NewPiece()
{
    CheckLine();
    if (CurrentPiece)
    {
        CurrentPiece->Dismiss();
        CurrentPiece->Destroy();
    }
    CurrentPiece = NextPiece;
    CurrentPiece->SetActorLocation(FVector(0.0f, 5.0f, 195.0f));
    CurrentPiece->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

    NextPiece = SubNextPiece;
    NextPiece -> SetActorLocation(FVector(0.0f, 100.0f, 180.0f));
    NextPiece -> SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

    FVector NextPieceLocation(0.0f, 100.0f, 110.0f);
    FRotator NextPieceRotation(0.0f, 0.0f, 0.0f);
    SubNextPiece = GetWorld()->SpawnActor<APiece>(NextPieceLocation, NextPieceRotation);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Mostramos siguiente new piece"));
   
    bGameOver = CheckGameOver();
    if (bGameOver)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Over!!!!!!!!"));
        /*if (GameOverSoundCue)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), GameOverSoundCue, GetActorLocation(), GetActorRotation());
        }*/
    }

    int32 Contar_Bloques = CurrentPiece->ObtenerNumBloques();
    //UE_LOG(LogTemp, Warning, TEXT("Número de bloques de la pieza actual: %d"), Contar_Bloques);
    FString Message = FString::Printf(TEXT("Numero de bloques en la pieza actual: %d"), Contar_Bloques);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
}

int conta = 0;
void ABoard::CheckLine()
{
    int Count = 0; // Variable para contar las líneas eliminadas
    auto MoveDownFromLine = [this](int z) {
        FVector Location(0.0f, 0.0f, 5.0 * z + 100.0);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        FVector Extent(4.5f, 49.5f, 95.0 + 4.5 - 5.0 * z);
        CollisionShape.SetBox(Extent);
        DrawDebugBox(GetWorld(), Location, Extent, FColor::Purple, false, 1, 0, 1);
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        if (GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam))
        {
            for (auto&& Result : OutOverlaps)
            {
                FVector NewLocation = Result.GetActor()->GetActorLocation();
                NewLocation.Z -= 10.0f;
                Result.GetActor()->SetActorLocation(NewLocation);
            }
        }
    };

    int z = 0;
    while (z < 20)
    {
        
        FVector Location(0.0f, 0.0f, 10.0f * z + 5.0f);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        CollisionShape.SetBox(FVector(4.0f, 49.0f, 4.0f));
       // DrawDebugBox(GetWorld(), Location, FVector(4.5f, 49.5f, 4.5f), FColor::Purple, false, 1, 0, 1);
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam);
        if (!b || OutOverlaps.Num() < 10)
        {
            ++z;
            continue;
    
        }
        else // esta línea está llena, elimine la línea
        {
          
          
            UE_LOG(LogTemp, Warning, TEXT("Encuentra LÍNEA COMPLETA en z=%d"), z);
            
            for (auto&& Result : OutOverlaps)
            {
                Result.GetActor()->Destroy();
            }
          
            MoveDownFromLine(z);  
            Count++;
        }
        conta = Count; // Actualizar el valor de conta con el contador de líneas eliminadas
        FString Message = FString::Printf(TEXT("Una línea eliminada. conta: %d"), conta);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
    }
 
}


void ABoard::MoveDownToEnd()
{
    if (!CurrentPiece)
    {
        return;
    }

    while (CurrentPiece->MoveDown())
    {
    }

    /*if (MoveToEndSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), MoveToEndSoundCue, GetActorLocation(), GetActorRotation());
    }*/

    switch (Status)
    {
    case PS_MOVING:
        Status = PS_GOT_BOTTOM;
        CoolLeft = CoolDown;
        break;
    case PS_GOT_BOTTOM:
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Wrong status for MoveDownToEnd"));
        break;
    }
}

void ABoard::ReiniciaJuego()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Q precionado"));
    if (CurrentPiece)
    {
		CurrentPiece->Dismiss();
		CurrentPiece->Destroy();
	}
    if (NextPiece)
    {
		NextPiece->Dismiss();
		NextPiece->Destroy();
	}
	
}

void ABoard::BajarRapido()
{
   
    if (CurrentPiece)
    {
        CurrentPiece->MoveDownSlow();
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Bajando rapido con Num 2"));
    }
}

bool ABoard::CheckGameOver()
{
    if (!CurrentPiece)
    {
        UE_LOG(LogTemp, Warning, TEXT("NoPieces"));
        return true;
    }

    return CurrentPiece->CheckWillCollision([](FVector OldVector) { return OldVector; });
}
