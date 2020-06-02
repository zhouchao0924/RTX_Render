
#include "BuildingSceneComponent.h"
#include "BuildingSDKComponent.h"
#include "BuildingSDKSystem.h"
#include "ResourceMeshComponent.h"
#include "ResourceActor.h"

UBuildingSceneComponent::UBuildingSceneComponent(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

bool UBuildingSceneComponent::LoadScene(const FString &Filename)
{
	FString Path = FPaths::GetPath(Filename);
	std::string SearchPath = TCHAR_TO_ANSI(*Path);

	FArchive* const FileAr = IFileManager::Get().CreateFileReader(*Filename);
	if (FileAr)
	{
		UResourceMgrComponent *ResMgr = GetResourceMgr();
		TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(FileAr);

		TSharedPtr<FJsonObject> RootObj;
		if (FJsonSerializer::Deserialize(Reader, RootObj))
		{
			IBuildingResourceMgr *ResMgrSDK = GetSDKResourceMgr();
			if (ResMgrSDK)
			{
				ResMgrSDK->AddSearchPath(SearchPath.c_str());
			}

			const TArray< TSharedPtr<FJsonValue>>& Nodes = RootObj->GetArrayField("Nodes");
			for (int32 i = 0; i < Nodes.Num(); ++i)
			{
				TSharedPtr<FJsonValue> JsonValue = Nodes[i];
				TSharedPtr<FJsonObject> JsonObj = JsonValue->AsObject();
				
				FString ResID = JsonObj->GetStringField("ResourceID");
				
				FVector Position, Scale;
				TSharedPtr<FJsonObject> PosObj = JsonObj->GetObjectField("Position");
				Position.X = PosObj->GetNumberField("x");
				Position.Y = PosObj->GetNumberField("y");
				Position.Z = PosObj->GetNumberField("z");

				TSharedPtr<FJsonObject> ScaleObj = JsonObj->GetObjectField("Scale");
				Scale.X = ScaleObj->GetNumberField("x");
				Scale.Y = ScaleObj->GetNumberField("y");
				Scale.Z = ScaleObj->GetNumberField("z");

				FRotator Rotation;
				TSharedPtr<FJsonObject> RotObj = JsonObj->GetObjectField("Rotation");
				Rotation.Pitch = RotObj->GetNumberField("pitch");
				Rotation.Yaw = RotObj->GetNumberField("yaw");
				Rotation.Roll = RotObj->GetNumberField("roll");
			
				IObject *pObj = ResMgr->GetResource(ResID, false);
				if (pObj)
				{
					SpawnMXActor(pObj, FTransform(Rotation, Position, Scale));
				}
			}

			if (ResMgrSDK)
			{
				ResMgrSDK->RemoveSearchPath(SearchPath.c_str());
			}
		}

		FileAr->Close();		
		return true;
	}
	
	return false;
}

UResourceMgrComponent *UBuildingSceneComponent::GetResourceMgr()
{
	ABuildingSDKSystem *SDKSystem = Cast<ABuildingSDKSystem>(GetOwner());
	if (SDKSystem)
	{
		return SDKSystem->ResourceMgr;
	}
	return nullptr;
}

AActor *UBuildingSceneComponent::SpawnMXActor(IObject *pObj, const FTransform &WorldTM)
{
	AResourceActor *pActor = (AResourceActor *)GetWorld()->SpawnActor(AResourceActor::StaticClass(), &WorldTM);
	if (pActor)
	{
		pActor->SetResource(nullptr, pObj->GetID());
	}
	return pActor;
}



