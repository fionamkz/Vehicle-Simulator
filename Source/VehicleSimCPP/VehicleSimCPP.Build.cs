// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VehicleSimCPP : ModuleRules
{
    public VehicleSimCPP(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "ChaosVehicles", "PhysicsCore"
        });
    }
}

