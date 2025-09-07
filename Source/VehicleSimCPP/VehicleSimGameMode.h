#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VehicleSimGameMode.generated.h"

UCLASS()
class VEHICLESIMCPP_API AVehicleSimGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AVehicleSimGameMode();

protected:
    virtual void BeginPlay() override;
};
