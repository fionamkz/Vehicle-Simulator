#include "VehicleBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AVehicleBase::AVehicleBase()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Constructor called - adding procedural mesh"));
    
    PrimaryActorTick.bCanEverTick = true;

    // Create procedural mesh component
    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    ProceduralMesh->SetupAttachment(RootComponent);

    // Create a simple box mesh (200x100x80 units)
    CreateBoxMesh();

    // Enhanced racing camera setup
    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 600.0f;          // Further back for racing view
    SpringArm->SocketOffset = FVector(0.0f, 0.0f, 100.0f); // Higher camera position
    SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f)); // Slight downward angle
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bInheritPitch = true;
    SpringArm->bInheritYaw = true;
    SpringArm->bInheritRoll = false;              // Don't roll with vehicle
    SpringArm->bDoCollisionTest = true;           // Collision detection

    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->SetFieldOfView(90.0f);                // Wider FOV for racing feel
    
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Constructor finished - procedural mesh created"));
}

void AVehicleBase::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: BeginPlay() called"));
    
    Super::BeginPlay();
    
    // Force possession by finding the first player controller
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PC = World->GetFirstPlayerController();
        if (PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Found PlayerController, attempting to possess vehicle"));
            PC->Possess(this);
            UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Possession attempt completed"));
            
            // Check if possession was successful
            if (PC->GetPawn() == this)
            {
                UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Successfully possessed by PlayerController!"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("VehicleBase: Possession failed - PC->GetPawn() != this"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("VehicleBase: No PlayerController found in world!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleBase: World is null!"));
    }
    
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: BeginPlay() completed"));
}

void AVehicleBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: SetupPlayerInputComponent() called"));

    if (!PlayerInputComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("VehicleBase: PlayerInputComponent is NULL!"));
        return;
    }

    // Bind WASD controls using standard Unreal input axis names
    PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleBase::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleBase::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AVehicleBase::MoveRight); // Alternative for Turn
    
    // Also bind individual keys for debugging (using correct syntax)
    PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AVehicleBase::OnWPressed);
    PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &AVehicleBase::OnSPressed);
    PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &AVehicleBase::OnAPressed);
    PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &AVehicleBase::OnDPressed);
    
    // Arrow keys as backup
    PlayerInputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AVehicleBase::OnWPressed);
    PlayerInputComponent->BindKey(EKeys::Down, IE_Pressed, this, &AVehicleBase::OnSPressed);
    PlayerInputComponent->BindKey(EKeys::Left, IE_Pressed, this, &AVehicleBase::OnAPressed);
    PlayerInputComponent->BindKey(EKeys::Right, IE_Pressed, this, &AVehicleBase::OnDPressed);
    
    // Bind brake control
    PlayerInputComponent->BindAxis("Brake", this, &AVehicleBase::Brake);
    PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AVehicleBase::OnSpacePressed);

    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Input bindings completed successfully"));
}

void AVehicleBase::MoveForward(float Value)
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: MoveForward called with value: %f"), Value);
    
    if (FMath::Abs(Value) > 0.1f) // Only move if significant input
    {
        // Get current forward vector
        FVector ForwardVector = GetActorForwardVector();
        
        // Calculate movement velocity (simple approach: 500 units/sec max speed)
        float MovementSpeed = 500.0f;
        FVector MovementVelocity = ForwardVector * Value * MovementSpeed;
        
        // Apply movement to actor location
        FVector DeltaTime = GetWorld()->GetDeltaSeconds() * MovementVelocity;
        AddActorWorldOffset(DeltaTime, true); // true = sweep for collision
        
        UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Moving forward with velocity: %s"), *MovementVelocity.ToString());
    }
}

void AVehicleBase::MoveRight(float Value)
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: MoveRight called with value: %f"), Value);
    
    if (FMath::Abs(Value) > 0.1f) // Only move if significant input
    {
        // For turning/steering: rotate the actor around Z-axis
        float TurnSpeed = 90.0f; // degrees per second
        float DeltaSeconds = GetWorld()->GetDeltaSeconds();
        FRotator DeltaRotation(0.0f, Value * TurnSpeed * DeltaSeconds, 0.0f);
        
        AddActorLocalRotation(DeltaRotation);
        
        UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Turning with rotation: %s"), *DeltaRotation.ToString());
    }
}

void AVehicleBase::Brake(float Value)
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Brake called with value: %f"), Value);
    
    if (FMath::Abs(Value) > 0.1f) // Only brake if significant input
    {
        // Simple braking: just log for now since we're using direct transform movement
        // In a real vehicle system, this would affect the vehicle's momentum/physics
        UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Braking applied with force: %f"), Value);
    }
}

void AVehicleBase::OnWPressed()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: W key pressed!"));
}

void AVehicleBase::OnSPressed()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: S key pressed!"));
}

void AVehicleBase::OnAPressed()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: A key pressed!"));
}

void AVehicleBase::OnDPressed()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: D key pressed!"));
}

void AVehicleBase::OnSpacePressed()
{
    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Space key pressed!"));
}

void AVehicleBase::CreateBoxMesh()
{
    if (!ProceduralMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("ProceduralMesh component is null!"));
        return;
    }

    // Create detailed car body with multiple sections
    CreateCarBody();
    CreateWindows();
    CreateWheels();

    UE_LOG(LogTemp, Warning, TEXT("VehicleBase: Detailed car mesh created"));
}

void AVehicleBase::CreateCarBody()
{
    // Car body dimensions - more realistic proportions
    float Length = 240.0f;
    float Width = 100.0f;
    float HoodHeight = 25.0f;      // Lower hood
    float CabinHeight = 45.0f;     // Raised cabin
    float RoofHeight = 35.0f;      // Roof above cabin
    
    float HalfLength = Length * 0.5f;
    float HalfWidth = Width * 0.5f;
    
    // Create vertices for a more car-like shape
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FLinearColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;
    
    // Bottom vertices (ground level)
    Vertices.Add(FVector(-HalfLength, -HalfWidth, 0));        // 0: Bottom front left
    Vertices.Add(FVector(HalfLength, -HalfWidth, 0));         // 1: Bottom front right
    Vertices.Add(FVector(HalfLength, HalfWidth, 0));          // 2: Bottom back right
    Vertices.Add(FVector(-HalfLength, HalfWidth, 0));         // 3: Bottom back left
    
    // Hood level vertices (lower front section)
    float HoodEnd = -40.0f;  // Hood extends from front to this point
    Vertices.Add(FVector(-HalfLength + 10, -HalfWidth + 5, HoodHeight));     // 4: Hood front left
    Vertices.Add(FVector(-HalfLength + 10, HalfWidth - 5, HoodHeight));      // 5: Hood front right
    Vertices.Add(FVector(HoodEnd, -HalfWidth + 5, HoodHeight));              // 6: Hood back left
    Vertices.Add(FVector(HoodEnd, HalfWidth - 5, HoodHeight));               // 7: Hood back right
    
    // Cabin level vertices (main body - higher)
    float CabinStart = -50.0f;  // Cabin starts here
    float CabinEnd = 60.0f;     // Cabin ends here
    Vertices.Add(FVector(CabinStart, -HalfWidth, CabinHeight));              // 8: Cabin front left
    Vertices.Add(FVector(CabinStart, HalfWidth, CabinHeight));               // 9: Cabin front right
    Vertices.Add(FVector(CabinEnd, -HalfWidth, CabinHeight));                // 10: Cabin back left
    Vertices.Add(FVector(CabinEnd, HalfWidth, CabinHeight));                 // 11: Cabin back right
    
    // Trunk level vertices (sloped back section)
    float TrunkHeight = 30.0f;   // Lower than cabin for sloped look
    Vertices.Add(FVector(CabinEnd + 10, -HalfWidth + 8, TrunkHeight));       // 12: Trunk front left
    Vertices.Add(FVector(CabinEnd + 10, HalfWidth - 8, TrunkHeight));        // 13: Trunk front right
    Vertices.Add(FVector(HalfLength - 10, -HalfWidth + 15, TrunkHeight));    // 14: Trunk back left
    Vertices.Add(FVector(HalfLength - 10, HalfWidth - 15, TrunkHeight));     // 15: Trunk back right
    
    // Roof vertices (cabin roof - highest point)
    float RoofStart = -30.0f;
    float RoofEnd = 40.0f;
    float RoofWidth = 70.0f;
    float RoofWidthHalf = RoofWidth * 0.5f;
    Vertices.Add(FVector(RoofStart, -RoofWidthHalf, CabinHeight + RoofHeight));   // 16: Roof front left
    Vertices.Add(FVector(RoofStart, RoofWidthHalf, CabinHeight + RoofHeight));    // 17: Roof front right
    Vertices.Add(FVector(RoofEnd, -RoofWidthHalf, CabinHeight + RoofHeight));     // 18: Roof back left
    Vertices.Add(FVector(RoofEnd, RoofWidthHalf, CabinHeight + RoofHeight));      // 19: Roof back right
    
    // Create triangles for realistic car shape
    // Bottom face
    Triangles.Append({0, 2, 1, 0, 3, 2});
    
    // Hood section (front of car)
    Triangles.Append({0, 1, 4, 1, 5, 4});      // Bottom to hood front
    Triangles.Append({0, 4, 6, 0, 6, 3});      // Hood left side
    Triangles.Append({1, 2, 7, 1, 7, 5});      // Hood right side
    Triangles.Append({2, 3, 6, 2, 6, 7});      // Hood back connection
    Triangles.Append({4, 7, 6, 4, 5, 7});      // Hood top surface
    
    // Hood to cabin transition
    Triangles.Append({6, 7, 8, 7, 9, 8});      // Hood back to cabin front
    
    // Cabin section (main body)
    Triangles.Append({8, 9, 10, 9, 11, 10});   // Cabin bottom
    Triangles.Append({8, 10, 16, 10, 18, 16}); // Cabin left side
    Triangles.Append({9, 17, 11, 11, 17, 19}); // Cabin right side
    
    // Cabin to trunk transition
    Triangles.Append({10, 11, 12, 11, 13, 12}); // Cabin back to trunk front
    
    // Trunk section (sloped back)
    Triangles.Append({12, 13, 14, 13, 15, 14}); // Trunk bottom
    Triangles.Append({12, 14, 1, 14, 2, 1});    // Trunk to car back left
    Triangles.Append({13, 2, 15, 15, 2, 1});    // Trunk to car back right
    Triangles.Append({14, 15, 1, 15, 2, 1});    // Trunk back surface
    
    // Roof section
    Triangles.Append({16, 19, 17, 16, 18, 19}); // Roof surface
    
    // Windshield (cabin front to roof)
    Triangles.Append({8, 9, 16, 9, 17, 16});    // Front windshield
    
    // Rear window (roof to trunk)
    Triangles.Append({18, 19, 12, 19, 13, 12}); // Rear window
    
    // Generate normals, UVs, colors, and tangents
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        Normals.Add(FVector(0, 0, 1));
        UVs.Add(FVector2D(0.0f, 0.0f));
        VertexColors.Add(FLinearColor::Red); // Red car body
        Tangents.Add(FProcMeshTangent(1, 0, 0));
    }
    
    // Create car body mesh section
    ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
}

void AVehicleBase::CreateWindows()
{
    TArray<FVector> WindowVertices;
    TArray<int32> WindowTriangles;
    TArray<FVector> WindowNormals;
    TArray<FVector2D> WindowUVs;
    TArray<FLinearColor> WindowColors;
    TArray<FProcMeshTangent> WindowTangents;
    
    float Height = 40.0f;
    float RoofHeight = 35.0f;
    
    // Front windshield
    WindowVertices.Add(FVector(-80, -25, Height + 5));         // Front windshield corners
    WindowVertices.Add(FVector(15, -25, Height + RoofHeight - 5));
    WindowVertices.Add(FVector(15, 25, Height + RoofHeight - 5));
    WindowVertices.Add(FVector(-80, 25, Height + 5));
    
    // Rear window
    WindowVertices.Add(FVector(20, -25, Height + RoofHeight - 5));  // Rear window corners
    WindowVertices.Add(FVector(60, -25, Height + 5));
    WindowVertices.Add(FVector(60, 25, Height + 5));
    WindowVertices.Add(FVector(20, 25, Height + RoofHeight - 5));
    
    // Window triangles
    WindowTriangles.Append({0, 1, 2, 0, 2, 3});  // Front windshield
    WindowTriangles.Append({4, 6, 5, 4, 7, 6});  // Rear window
    
    // Generate window properties
    for (int32 i = 0; i < WindowVertices.Num(); i++)
    {
        WindowNormals.Add(FVector(0, 0, 1));
        WindowUVs.Add(FVector2D(0.0f, 0.0f));
        WindowColors.Add(FLinearColor(0.2f, 0.2f, 0.8f, 0.7f)); // Semi-transparent blue windows
        WindowTangents.Add(FProcMeshTangent(1, 0, 0));
    }
    
    // Create windows mesh section
    ProceduralMesh->CreateMeshSection_LinearColor(1, WindowVertices, WindowTriangles, WindowNormals, WindowUVs, WindowColors, WindowTangents, false);
}

void AVehicleBase::CreateWheels()
{
    // Wheel positions relative to car center
    TArray<FVector> WheelPositions = {
        FVector(80, -60, 15),   // Front right
        FVector(80, 60, 15),    // Front left
        FVector(-80, -60, 15),  // Rear right
        FVector(-80, 60, 15)    // Rear left
    };
    
    float WheelRadius = 25.0f;
    float WheelWidth = 15.0f;
    int32 WheelSegments = 8;
    
    for (int32 WheelIndex = 0; WheelIndex < WheelPositions.Num(); WheelIndex++)
    {
        TArray<FVector> WheelVertices;
        TArray<int32> WheelTriangles;
        TArray<FVector> WheelNormals;
        TArray<FVector2D> WheelUVs;
        TArray<FLinearColor> WheelColors;
        TArray<FProcMeshTangent> WheelTangents;
        
        FVector WheelCenter = WheelPositions[WheelIndex];
        
        // Create cylinder vertices for wheel
        for (int32 i = 0; i <= WheelSegments; i++)
        {
            float Angle = (2.0f * PI * i) / WheelSegments;
            float CosAngle = FMath::Cos(Angle);
            float SinAngle = FMath::Sin(Angle);
            
            // Outer rim vertices
            WheelVertices.Add(WheelCenter + FVector(0, CosAngle * WheelRadius, SinAngle * WheelRadius - WheelWidth * 0.5f));
            WheelVertices.Add(WheelCenter + FVector(0, CosAngle * WheelRadius, SinAngle * WheelRadius + WheelWidth * 0.5f));
        }
        
        // Create wheel triangles
        for (int32 i = 0; i < WheelSegments; i++)
        {
            int32 Current = i * 2;
            int32 Next = ((i + 1) % (WheelSegments + 1)) * 2;
            
            // Side faces
            WheelTriangles.Append({Current, Next, Current + 1});
            WheelTriangles.Append({Current + 1, Next, Next + 1});
        }
        
        // Generate wheel properties
        for (int32 i = 0; i < WheelVertices.Num(); i++)
        {
            WheelNormals.Add(FVector(0, 0, 1));
            WheelUVs.Add(FVector2D(0.0f, 0.0f));
            WheelColors.Add(FLinearColor::Black); // Black wheels
            WheelTangents.Add(FProcMeshTangent(1, 0, 0));
        }
        
        // Create wheel mesh section (sections 2-5 for the 4 wheels)
        ProceduralMesh->CreateMeshSection_LinearColor(2 + WheelIndex, WheelVertices, WheelTriangles, WheelNormals, WheelUVs, WheelColors, WheelTangents, true);
    }
}
