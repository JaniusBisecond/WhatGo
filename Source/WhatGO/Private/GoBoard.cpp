// Fill out your copyright notice in the Description page of Project Settings.


#include "GoBoard.h"
#include "GoGameMode.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGoBoard::AGoBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoBoard::BeginPlay()
{
	Super::BeginPlay();

    XUint = BoardXLenght / 18;
    YUint = BoardYLenght / 18;

    GameMode = Cast<AGoGameMode>(GetWorld()->GetAuthGameMode());
	checkf(GameMode,TEXT("Can't get GameMode"));
	GameMode->SetBoard(this);

    //testPreset();
}

// Called every frame
void AGoBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    ScanStones(DeltaTime);
    StoneFollowMouse();
}

void AGoBoard::StoneFollowMouse()
{
    if (FollowedStone)
    {
        if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
        {
            FVector MouseLocation, MouseDirection;
            PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
			MouseDirection.Z = 0;
            FVector NewLocation = GetFloatingStoneLocation() + MouseDirection * FollowDegree;
            FollowedStone->SetActorLocation(NewLocation);
        }
    }
}

void AGoBoard::InitializeBoard()
{
    for (int i = 0; i < 19; ++i)
    {
        for (int j = 0; j < 19; ++j)
        {
            Board[i][j] = EStoneColor::Empty;
        }
    }
}

void AGoBoard::PrintBoard()
{
    for (int i = 0; i < 19; ++i)
    {
        FString Row;
        for (int j = 0; j < 19; ++j)
        {
            Row += FString::Printf(TEXT("%d "), Board[i][j]);
        }
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Row);
    }
}

bool AGoBoard::PlaceStone(int X, int Y, EStoneColor Color)
{
    if (X < 0 || X >= 19 || Y < 0 || Y >= 19 || Board[X][Y] != EStoneColor::Empty)
    {
        return false;
    }
    if (IsForbiddenPoint(X, Y, Color))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Forbidden Point!"));
        return false;
    }
    Board[X][Y] = Color;
	SpawnStone(X, Y, Color);
    CaptureStones(X, Y, Color);
    return true;
}


void AGoBoard::CaptureStones(int X, int Y, EStoneColor Color)
{
    EStoneColor Opponent;
    if (Color == EStoneColor::Black)
    {
	    Opponent = EStoneColor::White;
    }
    else
    {
		Opponent = EStoneColor::Black;
    }

    TArray<FIntPoint> directions = { FIntPoint(-1, 0), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(0, 1) };
    for (const auto& dir : directions)
    {
        int Nx = X + dir.X;
        int Ny = Y + dir.Y;
        if (Nx >= 0 && Nx < 19 && Ny >= 0 && Ny < 19 && Board[Nx][Ny] == Opponent)
        {
            if (CountLiberties(Nx, Ny, Opponent) == 0)
            {
                if (GameMode->GetPlayerColor() == Opponent)
                {
                    RemoveStones(Nx, Ny, Opponent);
                }
                else
                {
                    int RemoveNum = RemoveStones(Nx, Ny, Opponent);
				    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Remove %d stones"), RemoveNum));
                }
            }
        }
    }
}

int AGoBoard::RemoveStones(int X, int Y, EStoneColor Color)
{
    TArray<FIntPoint> stack = { FIntPoint(X, Y) };
    while (!stack.IsEmpty())
    {
        FIntPoint point = stack.Pop();
        int Px = point.X;
        int Py = point.Y;
        if (Board[Px][Py] == Color)
        {
            Board[Px][Py] = EStoneColor::Empty;
			StoneMap.Find(point)->Get()->Destroy();
            StoneMap.Remove(point);
            RemovedStones++;
            TArray<FIntPoint> directions = { FIntPoint(-1, 0), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(0, 1) };
            for (const auto& dir : directions)
            {
                int Nx = Px + dir.X;
                int Ny = Py + dir.Y;
                if (Nx >= 0 && Nx < 19 && Ny >= 0 && Ny < 19 && Board[Nx][Ny] == Color)
                {
                    stack.Add(FIntPoint(Nx, Ny));
                }
            }
        }
    }
    return RemovedStones;
}

int AGoBoard::CountLiberties(int X, int Y, EStoneColor Color)
{
    TSet<FIntPoint> liberties;
    TSet<FIntPoint> visited;
    TArray<FIntPoint> queue = { FIntPoint(X, Y) };

    while (queue.Num() > 0)
    {
        FIntPoint point = queue[0];
        queue.RemoveAt(0);
        int Px = point.X;
        int Py = point.Y;

        if (Board[Px][Py] == Color)
        {
            TArray<FIntPoint> directions = { FIntPoint(-1, 0), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(0, 1) };

            for (const auto& dir : directions)
            {
                int Nx = Px + dir.X;
                int Ny = Py + dir.Y;

                if (Nx >= 0 && Nx < 19 && Ny >= 0 && Ny < 19) 
                {
                    if (Board[Nx][Ny] == EStoneColor::Empty && !liberties.Contains(FIntPoint(Nx, Ny)))
                    {
                        liberties.Add(FIntPoint(Nx, Ny)); // 发现空位，加入 liberties
                    }
                    else if (Board[Nx][Ny] == Color && !visited.Contains(FIntPoint(Nx, Ny)))
                    {
                        visited.Add(FIntPoint(Nx, Ny)); // 访问过的点加入 visited
                        queue.Add(FIntPoint(Nx, Ny)); // 将同色的点加入队列
                    }
                }
            }
        }
    }
    return liberties.Num(); // 返回气数
}

void AGoBoard::ComputerMove(EStoneColor Color)
{
	while (ComputerMoveCount > 0)
	{
		int X = FMath::RandRange(0, 18);
		int Y = FMath::RandRange(0, 18);
		if (Board[X][Y] == EStoneColor::Empty)
		{
			PlaceStone(X, Y, Color);
			ComputerMoveCount--;
            return;
		}
	}

    int BestX = -1, BestY = -1;
    int BestValue = -TNumericLimits<int>::Max();
    for (int i = 0; i < 19; ++i)
    {
        for (int j = 0; j < 19; ++j)
        {
            if (Board[i][j] == EStoneColor::Empty)
            {
                Board[i][j] = Color;
                int Liberties = CountLiberties(i, j, Color);
                if (Liberties > BestValue)
                {
                    BestX = i;
                    BestY = j;
                    BestValue = Liberties;
                }
                Board[i][j] = EStoneColor::Empty;
            }
        }
    }
    if (BestX != -1 && BestY != -1)
    {
        PlaceStone(BestX, BestY, Color);
    }
}

bool AGoBoard::IsForbiddenPoint(int X, int Y, EStoneColor Color)
{
	EStoneColor Opponent = Color == EStoneColor::Black ? EStoneColor::White : EStoneColor::Black;

    TArray<FIntPoint> directions = { FIntPoint(-1, 0), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(0, 1) };
	// 周围是否有空位
    for (const auto& dir : directions)
    {
        int Nx = X + dir.X;
        int Ny = Y + dir.Y;
        if (Nx >= 0 && Nx < 19 && Ny >= 0 && Ny < 19)
        {
			if (Board[Nx][Ny] == EStoneColor::Empty)
			{
				return false;
			}
        }
    }
    // 是否是打劫点
    Board[X][Y] = Color;
    bool HasCaptured = false;
    for (const auto& dir : directions)
    {
        int Nx = X + dir.X;
        int Ny = Y + dir.Y;
        if (Nx >= 0 && Nx < 19 && Ny >= 0 && Ny < 19 && Board[Nx][Ny] == Opponent)
        {
            if (CountLiberties(Nx, Ny, Opponent) == 0)
            {
                HasCaptured = true;
				Board[X][Y] = EStoneColor::Empty;
                return false;
            }
        }
    }
    // 禁着点
	Board[X][Y] = Color;
    if (CountLiberties(X, Y, Color) == 0)
    {
		Board[X][Y] = EStoneColor::Empty;
		return true;
    }
	Board[X][Y] = EStoneColor::Empty;
    return false;
}

void AGoBoard::MappingLocationToBoardAndPlaceStone(FVector Location, EStoneColor Color)
{
    //PrintBoard();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hit Location: %s"), *Location.ToString()));
    int XAxis = FMath::RoundToInt(Location.X / XUint);
    int YAxis = FMath::RoundToInt(Location.Y / YUint);
    RemovedStones = 0;
    if (PlaceStone(XAxis, YAxis, Color))
    {
		if (RemovedStones > 0)
		{
            if (Color == EStoneColor::Black)
            {
                GenerateFloatingStone(EStoneColor::White);
            }
            else
            {
                GenerateFloatingStone(EStoneColor::Black);
            }
		}

        /*CPU place stone*/
        if (Color == EStoneColor::Black)
        {
            ComputerMove(EStoneColor::White);
        }
        else
        {
	        ComputerMove(EStoneColor::Black);
        }
    }
}

void AGoBoard::FallStone(EStoneColor Color)
{
	if (FollowedStone)
	{
        UStaticMeshComponent* StaticMeshComponent = FollowedStone->FindComponentByClass<UStaticMeshComponent>();
        if (StaticMeshComponent)
        {
            StaticMeshComponent->SetEnableGravity(true);
			FollowedStone = nullptr;
            RemovedStones--;
        }
	}
    if (RemovedStones <= 0)
    {
        GameMode->SetFallStone(false);
    }
    else
    {
		GenerateFloatingStone(Color);
    }
}

void AGoBoard::SpawnStone(int X, int Y, EStoneColor Color)
{
    FVector BoardLocation = GetActorLocation();
	FVector StoneLocation = FVector(X * XUint + XOffset, Y * XUint + YOffset, BoardLocation.Z + StoneSetHeight);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("StoneLocation: %s"), *StoneLocation.ToString()));
	FRotator Rotation = FRotator(0, 0, 0);
	FActorSpawnParameters SpawnInfo;
	AActor* Stone = nullptr;
	if (Color == EStoneColor::Black)
	{
		Stone = GetWorld()->SpawnActor<AActor>(BlackStoneClass, StoneLocation, Rotation, SpawnInfo);
	}
	else if (Color == EStoneColor::White)
	{
        Stone = GetWorld()->SpawnActor<AActor>(WhiteStoneClass, StoneLocation, Rotation, SpawnInfo);
	}
    if (Stone)
    {
        StoneMap.Add(FIntPoint(X, Y), Stone);
		AllSpawnedStones.Add(Stone);
    }
}

// 在棋盖上方生成悬浮棋子
void AGoBoard::GenerateFloatingStone(EStoneColor Color)
{
	FVector StoneLocation = GetFloatingStoneLocation();
    FActorSpawnParameters SpawnInfo;
	AActor* Stone = nullptr;
    if (Color == EStoneColor::Black)
    {
        Stone = GetWorld()->SpawnActor<AActor>(BlackStoneClass, StoneLocation, FRotator(0,0,0), SpawnInfo);
    }
    else if (Color == EStoneColor::White)
    {
        Stone = GetWorld()->SpawnActor<AActor>(WhiteStoneClass, StoneLocation, FRotator(0,0,0), SpawnInfo);
    }
    if (Stone)
    {
		AllSpawnedStones.Add(Stone);
        //获取生成的 Actor 的根组件并禁用重力
        UStaticMeshComponent* StaticMeshComponent = Stone->FindComponentByClass<UStaticMeshComponent>();
        if (StaticMeshComponent)
        {
            StaticMeshComponent->SetEnableGravity(false);
        }
		FollowedStone = Stone;
        GameMode->SetFallStone(true);
    }
}

FVector AGoBoard::GetFloatingStoneLocation()
{
    FVector LidLocation = GameMode->GetLid()->GetActorLocation();
    return FVector(LidLocation.X, LidLocation.Y, LidLocation.Z + StoneFallHeight);
}

void AGoBoard::ScanStones(float DeltaTime)
{
    ScanTimer += DeltaTime;
    if (ScanTimer > ScanInterval)
    {
        ScanTimer = 0;
    }
    else
    {
		return;
    }

    StoneMap.Empty();
	FVector BoardLocation = GetActorLocation();
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    ActorsToIgnore.Add(GameMode->GetDesk());
    for (int X = 0; X < 19; ++X)
    {
        for (int Y = 0; Y < 19; ++Y)
        {
            Board[X][Y] = EStoneColor::Empty;
            FHitResult OutHit;
            FVector Start = FVector(X * XUint + XOffset, Y * XUint + YOffset, BoardLocation.Z);/* 球体起点 */;
            FVector End = FVector(X * XUint + XOffset, Y * XUint + YOffset, BoardLocation.Z + StoneSetHeight);/* 球体终点 */;
            float Radius = 1.0f/* 球体半径 */;
            bool bTraceComplex = false;
            bool bHit = UKismetSystemLibrary::SphereTraceSingle(
                GetWorld(),
                Start,
                End,
                Radius,
                UEngineTypes::ConvertToTraceType(ECC_Visibility),
                bTraceComplex,
                ActorsToIgnore,
                EDrawDebugTrace::None,
                OutHit,
                true
            );
			if (bHit)
			{
                if (OutHit.GetActor()->GetClass() == WhiteStoneClass)
                {
                    Board[X][Y] = EStoneColor::White;
					StoneMap.Add(FIntPoint(X, Y), OutHit.GetActor());
                }
                else if (OutHit.GetActor()->GetClass() == BlackStoneClass)
                {
                    Board[X][Y] = EStoneColor::Black;
                    StoneMap.Add(FIntPoint(X, Y), OutHit.GetActor());
                }
			}
        }
    }
}

void AGoBoard::Hammer()
{
	for (auto Stone : StoneMap)
	{
		UStaticMeshComponent* StoneMesh =  Stone.Value->FindComponentByClass<UStaticMeshComponent>();
		if (StoneMesh)
		{
            FVector RandomImpulse = FVector(FMath::RandRange(-2.0f, 2.0f),
                FMath::RandRange(-2.0f, 2.0f),
                FMath::RandRange(0.0f, 20.0f));
			StoneMesh->AddImpulse(RandomImpulse);
		}
	}
}

void AGoBoard::Reset()
{
	for (auto Stone : AllSpawnedStones)
	{
		if (Stone)
		{
			Stone->Destroy();
		}
	}
	AllSpawnedStones.Empty();
	StoneMap.Empty();
	RemovedStones = 0;
	FollowedStone = nullptr;
	ComputerMoveCount = 5;
	GameMode->SetFallStone(false);
	InitializeBoard();
}


void AGoBoard::testPreset()
{
    /*
    *   bb
    *  bwwb
    *   b
    */
	PlaceStone(1, 2, EStoneColor::White);
	PlaceStone(1, 3, EStoneColor::White);

	PlaceStone(1, 1, EStoneColor::Black);
	PlaceStone(1, 4, EStoneColor::Black);
	PlaceStone(2, 2, EStoneColor::Black);
	PlaceStone(0, 2, EStoneColor::Black);
	PlaceStone(2, 3, EStoneColor::Black);

    /*
    *   ww
    *  wb w
    *   ww
    */
    PlaceStone(1, 7, EStoneColor::Black);

    PlaceStone(1, 6, EStoneColor::White);
    PlaceStone(1, 9, EStoneColor::White);
    PlaceStone(2, 7, EStoneColor::White);
    PlaceStone(0, 7, EStoneColor::White);
    PlaceStone(2, 8, EStoneColor::White);
    PlaceStone(0, 8, EStoneColor::White);


	/*
    *   |w
	*   | w
    *   |w
    */
    PlaceStone(5, 0, EStoneColor::White);
    PlaceStone(6, 1, EStoneColor::White);
    PlaceStone(7, 0, EStoneColor::White);

    /*
    *  bw
    * bw w
    *  bw
    */

    PlaceStone(9, 8, EStoneColor::Black);
    PlaceStone(8, 9, EStoneColor::Black);
    PlaceStone(10, 9, EStoneColor::Black);

    PlaceStone(8, 10, EStoneColor::White);
    PlaceStone(9, 11, EStoneColor::White);
    PlaceStone(10, 10, EStoneColor::White);
    PlaceStone(9, 9, EStoneColor::White);
}
