#include "VehicleBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"

AVehicleBase::AVehicleBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // C�mara
    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.f;
    SpringArm->bUsePawnControlRotation = true;

    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    // Movimiento
    UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
    if (Movement)
    {
        Movement->EngineSetup.MaxRPM = 5700.f;
        Movement->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
        Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.f, 400.f);
        Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.f, 500.f);
        Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.f, 400.f);
        Movement->TransmissionSetup.bUseAutomaticGears = true;
    }
}

void AVehicleBase::BeginPlay()
{
    Super::BeginPlay();
}

void AVehicleBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleBase::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleBase::MoveRight);
}

void AVehicleBase::MoveForward(float Value)
{
    GetVehicleMovementComponent()->SetThrottleInput(Value);
}

void AVehicleBase::MoveRight(float Value)
{
    GetVehicleMovementComponent()->SetSteeringInput(Value);
}
