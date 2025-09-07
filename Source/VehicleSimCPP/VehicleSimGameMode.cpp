#include "VehicleSimGameMode.h"
#include "Engine/World.h"
#include "VehicleBase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "GameFramework/InputSettings.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Components/LightComponent.h"

AVehicleSimGameMode::AVehicleSimGameMode()
{
    // Set the default pawn class to our vehicle
    DefaultPawnClass = AVehicleBase::StaticClass();
    
    // Note: Input axis mappings will be set up in BeginPlay() when InputSettings is fully initialized
    
    // Alternative method with class finder if needed
    // static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Script/VehicleSimCPP.VehicleBase"));
    // if (PlayerPawnClassFinder.Succeeded())
    // {
    //     DefaultPawnClass = PlayerPawnClassFinder.Class;
    // }
}

void AVehicleSimGameMode::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: BeginPlay() called"));
    
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Super::BeginPlay() completed"));

    // Setup input axis mappings first (when InputSettings is fully initialized)
    SetupInputAxisMappings();

    // Create race track environment
    // CreateRaceTrack();  // Commented out to prevent crashes

    // Setup racing atmosphere and lighting
    // CreateRacingAtmosphere();  // Commented out to prevent crashes

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: World is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: World is valid"));

    // Check player controller and pawn possession
    CheckPlayerPossession();

    // Set a timer to check possession status after a short delay (0.1 seconds for proper initialization)
    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, this, &AVehicleSimGameMode::CheckPlayerPossession, 0.1f, false);
}

void AVehicleSimGameMode::CheckPlayerPossession()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Checking player possession status"));

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: World is NULL in CheckPlayerPossession!"));
        return;
    }

    // Get the player controller
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: PlayerController is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: PlayerController found: %s"), *PlayerController->GetName());

    // Check what pawn is possessed
    APawn* PossessedPawn = PlayerController->GetPawn();
    if (!PossessedPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: No pawn is possessed by PlayerController!"));
        
        // Try to find and possess a VehicleBase
        AVehicleBase* Vehicle = nullptr;
        for (TActorIterator<AVehicleBase> ActorIterator(World); ActorIterator; ++ActorIterator)
        {
            Vehicle = *ActorIterator;
            break;
        }
        
        if (Vehicle)
        {
            UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Found VehicleBase, attempting to possess: %s"), *Vehicle->GetName());
            PlayerController->Possess(Vehicle);
            
            // Verify possession
            if (PlayerController->GetPawn() == Vehicle)
            {
                UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Successfully possessed VehicleBase!"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: Failed to possess VehicleBase!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: No VehicleBase found in world!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: PlayerController possesses pawn: %s (Class: %s)"), 
               *PossessedPawn->GetName(), *PossessedPawn->GetClass()->GetName());
        
        // Check if it's a VehicleBase
        AVehicleBase* VehicleBase = Cast<AVehicleBase>(PossessedPawn);
        if (VehicleBase)
        {
            UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Possessed pawn is a VehicleBase - input should work!"));
            
            // Check if input component is set up
            UInputComponent* InputComp = VehicleBase->InputComponent;
            if (InputComp)
            {
                UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: VehicleBase has InputComponent"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: VehicleBase has no InputComponent!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Possessed pawn is NOT a VehicleBase (it's %s)"), *PossessedPawn->GetClass()->GetName());
        }
    }
}

void AVehicleSimGameMode::SetupInputAxisMappings()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Setting up Input Axis Mappings"));

    UInputSettings* InputSettings = UInputSettings::GetInputSettings();
    if (!InputSettings)
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleSimGameMode: Failed to get InputSettings!"));
        return;
    }

    // Check if mappings already exist to avoid duplicates
    bool bHasMoveForward = false;
    bool bHasMoveRight = false;
    
    for (const FInputAxisKeyMapping& Mapping : InputSettings->GetAxisMappings())
    {
        if (Mapping.AxisName == "MoveForward")
        {
            bHasMoveForward = true;
            UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Found existing MoveForward mapping: %s = %f"), 
                   *Mapping.Key.ToString(), Mapping.Scale);
        }
        if (Mapping.AxisName == "MoveRight")
        {
            bHasMoveRight = true;
            UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Found existing MoveRight mapping: %s = %f"), 
                   *Mapping.Key.ToString(), Mapping.Scale);
        }
    }

    // Only create mappings if they don't exist
    if (!bHasMoveForward)
    {
        UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Creating MoveForward axis mappings"));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.0f));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::S, -1.0f));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Up, 1.0f));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Down, -1.0f));
    }

    if (!bHasMoveRight)
    {
        UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Creating MoveRight axis mappings"));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::A, -1.0f));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::D, 1.0f));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::Left, -1.0f));
        InputSettings->AddAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::Right, 1.0f));
    }

    // Always create Turn and Brake mappings (alternative names)
    InputSettings->AddAxisMapping(FInputAxisKeyMapping("Turn", EKeys::A, -1.0f));
    InputSettings->AddAxisMapping(FInputAxisKeyMapping("Turn", EKeys::D, 1.0f));
    InputSettings->AddAxisMapping(FInputAxisKeyMapping("Turn", EKeys::Left, -1.0f));
    InputSettings->AddAxisMapping(FInputAxisKeyMapping("Turn", EKeys::Right, 1.0f));

    InputSettings->AddAxisMapping(FInputAxisKeyMapping("Brake", EKeys::SpaceBar, 1.0f));

    // Save and force rebuild keymaps
    InputSettings->SaveKeyMappings();
    InputSettings->ForceRebuildKeymaps();
    
    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Input Axis Mappings setup complete"));
    
    // Verify mappings were created
    int32 AxisMappingCount = InputSettings->GetAxisMappings().Num();
    UE_LOG(LogTemp, Warning, TEXT("VehicleSimGameMode: Total axis mappings in InputSettings: %d"), AxisMappingCount);
}






