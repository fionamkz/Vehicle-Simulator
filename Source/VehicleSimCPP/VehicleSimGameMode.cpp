#include "VehicleSimGameMode.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "VehicleBase.h"

AVehicleSimGameMode::AVehicleSimGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Script/VehicleSimCPP.VehicleBase"));
    if (PlayerPawnClassFinder.Succeeded())
    {
        DefaultPawnClass = PlayerPawnClassFinder.Class;
    }
}

void AVehicleSimGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Crear un piso plano básico
    if (GetWorld())
    {
        // Coordenadas donde aparece piso
        FVector Location = FVector(0.0f, 0.0f, 0.0f);
        FRotator Rotation = FRotator::ZeroRotator;

        AStaticMeshActor* Floor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);

        if (Floor)
        {
            static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));

            if (PlaneMesh.Succeeded())
            {
                Floor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh.Object);

                Floor->SetActorScale3D(FVector(10.0f, 10.0f, 1.0f));

                Floor->SetMobility(EComponentMobility::Static);
                Floor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);

                Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
            }
        }
    }
}
