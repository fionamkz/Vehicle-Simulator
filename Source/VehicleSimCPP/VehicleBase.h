#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "WheeledVehiclePawn.h"
#include "VehicleBase.generated.h"

UCLASS()
class VEHICLESIMCPP_API AVehicleBase : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    AVehicleBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
};
