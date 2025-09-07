#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "WheeledVehiclePawn.h"
#include "ProceduralMeshComponent.h"
#include "VehicleBase.generated.h"

UCLASS()
class VEHICLESIMCPP_API AVehicleBase : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    AVehicleBase();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    UProceduralMeshComponent* ProceduralMesh;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Brake(float Value);

    // Input action functions for debugging
    void OnWPressed();
    void OnSPressed();
    void OnAPressed();
    void OnDPressed();
    void OnSpacePressed();

private:
    void CreateBoxMesh();
    void CreateCarBody();
    void CreateWindows();
    void CreateWheels();
};
