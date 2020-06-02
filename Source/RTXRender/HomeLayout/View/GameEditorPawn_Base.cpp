 


#include "GameEditorPawn_Base.h"
#include "HomeLayout/HouseArchitect/AreaSystem.h"
#include "HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "HomeLayout/HouseArchitect/SkirtingSystem.h"
#include "HomeLayout/HouseArchitect/ModelSystem.h"
#include "EditorGameInstance.h"
#include "HomeLayout/Paper/ProjectMgr.h"
#include "../Commands/CommandMgr.h"

// Sets default values
AGameEditorPawn_Base::AGameEditorPawn_Base()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameEditorPawn_Base::BeginPlay()
{
	Super::BeginPlay();

	UWorld *MyWorld = GetWorld();
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());

	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (!GameInst->AreaSystem)
	{
		GameInst->AreaSystem = MyWorld->SpawnActor<AAreaSystem>(SpawmParams);
	}
	if (!GameInst->ModelSystem)
	{
		GameInst->ModelSystem = MyWorld->SpawnActor<AModelSystem>(SpawmParams);
	}

	UProjectPaper *NewPaper = GameInst->ProjectMgr->CreateNewPaper();
	GameInst->ProjectMgr->SwitchPaper(NewPaper);

	CmdMgr = UCommandMgr::InitInstance(this);
}

static void InitializeCommandInputBindings()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("OnLeftMouseButton", EKeys::LeftMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("OnRightMouseButton", EKeys::RightMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("OnEscape", EKeys::Escape));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("OnSpaceBar", EKeys::SpaceBar));
	}
}

// Called to bind functionality to input
void AGameEditorPawn_Base::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	InitializeCommandInputBindings();
	InputComponent->BindAction(TEXT("OnLeftMouseButton"), IE_Pressed, this, &AGameEditorPawn_Base::OnLeftMouseButtonDown);
	InputComponent->BindAction(TEXT("OnLeftMouseButton"), IE_Released, this, &AGameEditorPawn_Base::OnLeftMouseButtonUp);

	InputComponent->BindAction(TEXT("OnRightMouseButton"), IE_Pressed, this, &AGameEditorPawn_Base::OnRightMouseButtonDown);
	InputComponent->BindAction(TEXT("OnRightMouseButton"), IE_Released, this, &AGameEditorPawn_Base::OnRightMouseButtonUp);

	InputComponent->BindAction(TEXT("OnEscape"), IE_Pressed, this, &AGameEditorPawn_Base::OnEscapeDown);

	InputComponent->BindAction(TEXT("OnSpaceBar"), IE_Pressed, this, &AGameEditorPawn_Base::OnSpaceBar);
}

// Called every frame
void AGameEditorPawn_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UBaseCmd *CurrentCmd = CmdMgr->GetCurrentCmd();
	if (CurrentCmd)
	{
		CurrentCmd->CommandTick(DeltaTime);
	}
}

void AGameEditorPawn_Base::OnLeftMouseButtonDown()
{
	UBaseCmd *CurrentCmd = CmdMgr->GetCurrentCmd();
	if (CurrentCmd)
	{
		CurrentCmd->OnLeftMouseButtonDown();
	}
}

void AGameEditorPawn_Base::OnLeftMouseButtonUp()
{
	UBaseCmd *CurrentCmd = CmdMgr->GetCurrentCmd();
	if (CurrentCmd)
	{
		CurrentCmd->OnLeftMouseButtonUp();
	}
}

void AGameEditorPawn_Base::OnRightMouseButtonDown()
{
	UBaseCmd *CurrentCmd = CmdMgr->GetCurrentCmd();
	if (CurrentCmd)
	{
		CurrentCmd->OnRightMouseButtonDown();
	}
}

void AGameEditorPawn_Base::OnRightMouseButtonUp()
{
	UBaseCmd *CurrentCmd = CmdMgr->GetCurrentCmd();
	if (CurrentCmd)
	{
		CurrentCmd->OnRightMouseButtonUp();
	}
}

void AGameEditorPawn_Base::OnEscapeDown()
{
	UBaseCmd *CurrentCmd = CmdMgr->GetCurrentCmd();
	if (CurrentCmd)
	{
		CurrentCmd->OnEscapeDown();
	}
}

void AGameEditorPawn_Base::OnSpaceBar()
{
	int i = 0;
}

