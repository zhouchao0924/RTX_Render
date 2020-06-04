

#include "BuildingComponent.h"
#include "ISuite.h"
#include "IProperty.h"
#include "DRGameMode.h"
#include "IMeshObject.h"
#include "ResourceMgr.h"
#include "SurfaceFile.h"
#include "EditorGameInstance.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Building/BuildingConfig.h"
#include "BuildingSDK.h"

UBuildingComponent::UBuildingComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	HitSurfaceID  = INVALID_OBJID;
}

void UBuildingComponent::SetData(UBuildingData *InData)
{
	ClearAllMeshSections();
	Data = InData;
	if (Data)
	{
		float *pVertices = nullptr;
		float *pNors = nullptr;
		float *pUVs = nullptr;
		float *pTans = nullptr;
		float *pLightmapUVs = nullptr;
		int	   nVtxs = 0, nIndices = 0;
		int	   *pIndices = nullptr;

		IObject *RawObj = Data->GetRawObj();
		if(!RawObj)		return;
		//if (RawObj->GetMeshCount() == 0)	return;
		IMeshObject *MeshObj = RawObj ? RawObj->GetMeshObject(0) : nullptr;
		if (MeshObj)
		{
			int SectionCount = MeshObj->GetSectionCount();
			for (int32 iSection = 0; iSection < SectionCount; ++iSection)
			{
				if (MeshObj->GetSectionMesh(iSection, pVertices, pNors, pTans, pUVs, pLightmapUVs, nVtxs, pIndices, nIndices))
				{
					TArray<FVector> Vertices, Normals;
					TArray<int32> Triangles;
					TArray<FVector2D> UVs;
					TArray<FColor> VertexColors;
					TArray<FProcMeshTangent> Tangents;

					UVs.SetNum(nVtxs);
					Vertices.SetNum(nVtxs);
					Normals.SetNum(nVtxs);
					Tangents.SetNum(nVtxs);
					VertexColors.SetNum(nVtxs);
					Triangles.SetNum(nIndices);

					FMemory::Memcpy(Vertices.GetData(), pVertices, nVtxs * 12);
					FMemory::Memcpy(Normals.GetData(), pNors, nVtxs * 12);

					for (int32 iVert = 0; iVert < Tangents.Num(); ++iVert)
					{
						VertexColors[iVert] = FColor::White;
						Tangents[iVert].TangentX = *((FVector *)(pTans + 3 * iVert));
					}

					FMemory::Memcpy(UVs.GetData(), pUVs, nVtxs * 8);
					FMemory::Memcpy(Triangles.GetData(), pIndices, nIndices * 4);

					CreateMeshSection(iSection, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

					ISuite *Suite = Data->GetSuite();
					if (Suite)
					{
						ObjectID SurfaceID = Suite->GetSurface(Data->GetID(), iSection);
						UpdateSurface(iSection, SurfaceID);
					}
				}
			}

			SetCollisionProfileName(TEXT("Architect"));
			UWorld *MyWorld = GetWorld();
			if (MyWorld)
			{
				UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
				if (GameInst)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					if (Type == EObjectType::ESolidWall)
					{
						UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
						if (WallSystem)
						{
							TArray<FVector>WallNodes;
							float HeightPos;
							float ThickLeft = Data->GetFloat("ThickLeft");
							float ThickRight = Data->GetFloat("ThickRight");
							WallSystem->GetWallBorderPos(Data->GetID(), WallNodes, HeightPos);
							++HeightPos;
							FVector RightDir = Data->GetVector("Right");
							FVector Temp = (WallNodes[5] + WallNodes[4]) / 2;
							FVector RightPos = Temp + RightDir * 10;
							Temp = (WallNodes[2] + WallNodes[1]) / 2;
							FVector LeftPos = Temp - RightDir * 10;
							float Angle=WallSystem->GetWallRotate(FVector2D(WallNodes[0]), FVector2D(WallNodes[3]));
							FRotator RulerRotator(90, Angle, 180);
							if ((90.0f < Angle && Angle < 180) || (-180 <= Angle && Angle < -90))
							{
								RulerRotator = FRotator(90, Angle - 180, 180);
							}
							if (WallNodes.Num() == 6)
							{
								if (WallPlane == NULL)
								{
									UClass *WallPlaneActor = WallSystem->WallPlaneActor.Get();
									if (WallPlaneActor&&WallPlaneActor->IsChildOf(AWallPlaneBase::StaticClass()))
									{
										WallPlane = (AWallPlaneBase*)MyWorld->SpawnActor(WallPlaneActor, &FTransform::Identity);
										WallPlane->InitPlane(WallNodes, HeightPos, ThickLeft, ThickRight);
										WallSystem->WallPlaneMap.Add(InData->GetID(), WallPlane);
									}
								}
								else
								{
									WallPlane->Plane->ClearAllMeshSections();
									WallPlane->InitPlane(WallNodes, HeightPos, ThickLeft, ThickRight);
								}
								WallPlane->RightLabelWidget->SetWorldLocationAndRotation(FVector(RightPos.X, RightPos.Y, HeightPos),RulerRotator);
								WallPlane->LeftLabelWidget->SetWorldLocationAndRotation(FVector(LeftPos.X, LeftPos.Y, HeightPos), RulerRotator);
								WallPlane->SpinBoxPos = FVector((LeftPos.X + RightPos.X) / 2, (LeftPos.Y + RightPos.Y) / 2, HeightPos);//
								//WallPlane->InputLabelWidget->SetWorldLocationAndRotation(FVector(LeftPos.X, LeftPos.Y, HeightPos), RulerRotator);//
								if (WallPlane->InputRulerWidget)
								if (!(WallPlane->InputRulerWidget->bUseDisLengthFlag)) //如果为true则以用户输入值为准
								{
									float length = (WallNodes[5] - WallNodes[4]).Size();
									float TempAngle;
									if ((90.0f < Angle && Angle < 180) || (-180 <= Angle && Angle < -90))
									{
										TempAngle = Angle - 180;
									}
									else
									{
										TempAngle = Angle;
									}
									if (WallPlane->RightLabelWidget->GetUserWidgetObject())
									{
										Cast<URulerLabelWidget>(WallPlane->RightLabelWidget->GetUserWidgetObject())->SetLabelTextData(length);
										//Cast<URulerLabelWidget>(WallPlane->RightLabelWidget->GetUserWidgetObject())->SetRenderAngle(TempAngle);
									}
									if (WallPlane->InputRulerWidget)
										WallPlane->InputRulerWidget->SetLabelTextLengthData(length);//
										//if (WallPlane->InputLabelWidget->GetUserWidgetObject())
										//Cast<UUserInputRulerWidget>(WallPlane->InputLabelWidget->GetUserWidgetObject())->SetLabelTextLengthData(length);
									length = (WallNodes[2] - WallNodes[1]).Size();
									if (WallPlane->LeftLabelWidget->GetUserWidgetObject())
									{
										Cast<URulerLabelWidget>(WallPlane->LeftLabelWidget->GetUserWidgetObject())->SetLabelTextData(length);
										//Cast<URulerLabelWidget>(WallPlane->LeftLabelWidget->GetUserWidgetObject())->SetRenderAngle(TempAngle);
									}
								}
							}
						}
					}
				}
			}
		}
		if (OnNotifyCreateMesh.IsBound())
		{
			OnNotifyCreateMesh.Broadcast();
		}
	}
}

void UBuildingComponent::UpdateSurface(int32 SectionIndex, int32 SurfaceID)
{
	UMaterialInterface* UE4Mat = GetMaterialInterfaceBySurfaceID(SurfaceID);
	UMaterialInstanceDynamic *MaterialInst = CreateDynamicMaterialInstance(SectionIndex, UE4Mat);
	FVector2D Temp;
	Temp.X = GetMatDefaultFloatParam(UE4Mat, TEXT("法线U"));
	Temp.Y = GetMatDefaultFloatParam(UE4Mat, TEXT("法线V"));
	SetMaterial(SectionIndex, MaterialInst);
	AutoSetMatParams(SectionIndex, SurfaceID, UE4Mat);

	if (OnNotifyUpdateSurface.IsBound())
	{
		OnNotifyUpdateSurface.Broadcast(SectionIndex, SurfaceID, ComponetType);
	}
	/*
	ISuite *Suite = Data ? Data->GetSuite() : nullptr;
	if (Suite)
	{
		UMaterialInterface *UE4Material = nullptr;
		FString MaterialPath = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
		int32 MaterialType = Suite->GetProperty(SurfaceID, "MaterialType").IntValue();
		if (MaterialPath.Len() > 0)
		{
			switch (MaterialType)
			{
				case EUE4Material:
				{
					if(MaterialPath.Len() > 0)
					{ 
						UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
					}
					break;
				}
				case ERefMaterial:
				{
					UResource *Resource = UResourceMgr::GetResourceMgr()->FindRes(MaterialPath, true);
					if(Resource)
					{
						Resource->ForceLoad();
					}
					USurfaceFile *Surface = Cast<USurfaceFile>(Resource);
					if (Surface)
					{
						UE4Material = Surface->GetUE4Material();
					}
					UModelFile *ModelFile = Cast<UModelFile>(Resource);
					if (ModelFile)
					{
						UE4Material = ModelFile->GetUE4Material(0);
					}
				}
				default: break;
			}
		}
		else
		{
			FBuildingConfig *Config = Data? Data->GetConfig() : nullptr;
			if (Config)
			{
				UE4Material = Config->WallMaterial;
			}
			else
			{
				UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *DEFAULT_WALL_MATERIAL));
			}
		}

		IObject *pSurface = Suite->GetObject(SurfaceID);

		UMaterialInstanceDynamic *MaterialInst = CreateDynamicMaterialInstance(SectionIndex, UE4Material);
		if (MaterialInst && pSurface)
		{
			IValue *pValue = pSurface->GetValueArray();
			if (pValue)
			{
				int32 numFields = pValue->GetNumFields();
				for (int32 i = 0; i < numFields; ++i)
				{
					IValue &ParamValue = pValue->GetField(i);
					const char *ParamName = pValue->GetFieldName(i);					
					EVarType Type = ParamValue.GetType();
					switch (Type)
					{
						case kV_Float:
						{
							MaterialInst->SetScalarParameterValue(ANSI_TO_TCHAR(ParamName), ParamValue.FloatValue());
							break;
						}
						case kV_Vec4D:
						{
							kVector4D Value2 = ParamValue.Vec4Value();
							MaterialInst->SetVectorParameterValue(ANSI_TO_TCHAR(ParamName), FLinearColor(FORCE_TYPE(FVector4,Value2)));
							break;
						}
						default:break;
					}
				}
			}
		}
	}
	*/
}


int32 UBuildingComponent::HitTestSurface(const FVector &RayStart, const FVector &RayDir) const
{
	ObjectID SurfaceID = INVALID_OBJID;
	IObject *pObj = Data->GetRawObj();
	if (pObj && pObj->HitTest(FORCE_TYPE(kVector3D, RayStart), FORCE_TYPE(kVector3D, RayDir), SurfaceID))
	{
		return SurfaceID;
	}
	return SurfaceID;
}

int32 UBuildingComponent::HitTestSubModelIndex(const FVector &RayStart, const FVector &RayDir)
{
	int SubModelIndex = INVALID_OBJID;
	if(Data)
	{
		IObject *RawObj = Data->GetRawObj();
		IMeshObject *MeshObj = RawObj ? RawObj->GetMeshObject(0) : nullptr;
		if (MeshObj && MeshObj->HitTest(FORCE_TYPE(kVector3D, RayStart), FORCE_TYPE(kVector3D, RayDir), &SubModelIndex, nullptr, nullptr))
		{
			//return SubModelIndex;
		}
	}
	return SubModelIndex;
}

int32 UBuildingComponent::GetSurfaceIDBySectionIndex(const int32& SectionIndex)
{
	if (Data)
	{
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		return (int32)Suite->GetSurface(Data->GetID(), SectionIndex);
	 }
	else
	{
		return -1;
	}
}

void UBuildingComponent::SetSaveUVData(const FVector2D& UV, const FVector2D& UVOffset, const float& Angle)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		if (Suite && BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{	
				kPoint kUV = ToBuildingPosition(UV);
				IValue& UVValue = VF->Create(&kUV, true);
				kPoint kUVOffset = ToBuildingPosition(UVOffset);
				IValue& UVOffsetValue = VF->Create(&kUVOffset, true);
				IValue& AngleValue = VF->Create(Angle);
				Suite->SetProperty(Data->GetID(), "UV", &UVValue);
				Suite->SetProperty(Data->GetID(), "UVOffset", &UVOffsetValue);
				Suite->SetProperty(Data->GetID(), "Angle", &AngleValue);
				//UpdateSurface(SubModelIndex, SurfaceID);
			}
		}
	}
}

void UBuildingComponent::SetSurfaceFloatParam(const int32& SurfaceID, FString& ParamName, const float& Value)
{
	 if(Data)
	 {
		 UBuildingSystem* BS = Data->GetBuildingSystem();
		 if ( BS)
		 {
			 IValueFactory* VF = BS->GetValueFactory();
			 if(VF)
			 {
				 IValue& NewValue = VF->Create(Value);
				 SetSurfaceParam(SurfaceID, ParamName, &NewValue);
			 }	
		 }
	 }
}

float UBuildingComponent::GetSurfaceFloatParam(const int32& SurfaceID, FString& ParamName)
{
	if (Data)
	{
		ISuite* Suite = Data->GetSuite();
		IObject *pSurface = Suite->GetObject(SurfaceID);
		if (pSurface)
		{
			IValue& Value = pSurface->GetPropertyValue(TCHAR_TO_ANSI(*ParamName));
			return Value.FloatValue();
		}		
	}
	
	return 0.0;
	
}

/*
void UBuildingComponent::SetMat2DParam(FString& ParamName, const FVector2D& Value)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		if (BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				kPoint kValue = ToBuildingPosition(Value);
				IValue& NewValue = VF->Create(&kValue, true);
				SetSurfaceParam(HitSurfaceID, ParamName, NewValue);
			}
		}
	}
}

void UBuildingComponent::SetMat3DParam(FString& ParamName, const FVector& Value)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		if (BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				kVector3D kValue = ToBuildingVector(Value);
				IValue& NewValue = VF->Create(&kValue, true);
				SetSurfaceParam(HitSurfaceID, ParamName, NewValue);
			}
		}
	}
}*/

void UBuildingComponent::SetMat4DParam(FString& ParamName, const FVector4& Value)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		if (BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				kVector4D kValue = ToBuildingVector4D(Value);
				IValue& NewValue = VF->Create(&kValue, true);
				SetSurfaceParam(HitSurfaceID, ParamName, &NewValue);
			}
		}
	}
}


void UBuildingComponent::SetSurfaceParam(const int32& SurfaceID, const FString& ParamName, IValue* Value)
{
	if(Data && Data->GetSuite())
	{	
		ISuite* Suite = Data->GetSuite();
		IObject *pSurface = Suite->GetObject(SurfaceID);
		if(pSurface)
		{
			pSurface->SetPropertyValue(TCHAR_TO_ANSI(*ParamName), Value);
		}
	}

}

void UBuildingComponent::SetSurfaceParams(const int32& SurfaceID, const FVector2D& UVScale, const FVector2D& UVOffset, const float& Angle, const FVector2D& DefaultUV)
{
	 if(Data && (SurfaceID != INVALID_OBJID))
	 {
		UBuildingSystem* BS = Data->GetBuildingSystem();
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		if (Suite && BS && VF)
		{
			FString UText = TEXT("密度U");
			FString VText = TEXT("密度V");
			FString NUText = TEXT("法线U");
			FString NVText = TEXT("法线V");
			FString OffsetXText = TEXT("位移X");
			FString OffsetYText = TEXT("位移Y");
			FString AnlgeText = TEXT("贴图角度");
			FString NAnlgeText = TEXT("法线角度");
			FString DefaultXText = TEXT("UVDefaultX");
			FString DefaultYText = TEXT("UVDefaultY");
			SetSurfaceFloatParam(SurfaceID, UText, UVScale.X / 100.0 );
			SetSurfaceFloatParam(SurfaceID, VText, UVScale.Y / 100.0);
			SetSurfaceFloatParam(SurfaceID, NUText, UVScale.X / 100.0);
			SetSurfaceFloatParam(SurfaceID, NVText, UVScale.Y / 100.0);
			SetSurfaceFloatParam(SurfaceID, OffsetXText, UVOffset.X);
			SetSurfaceFloatParam(SurfaceID, OffsetYText, UVOffset.Y);
			SetSurfaceFloatParam(SurfaceID, AnlgeText, Angle);
			SetSurfaceFloatParam(SurfaceID, NAnlgeText, Angle);
			//SetSurfaceFloatParam(SurfaceID, DefaultXText, UVDefault.X);	   //用于存储记录
			//SetSurfaceFloatParam(SurfaceID, DefaultYText, UVDefault.Y);	   //用于存储记录

			//IValue& Value = VF->Create();
			//kPoint KUVScale = ToBuildingPosition(UVScale);
			//IValue& ScaleV = VF->Create(&KUVScale, true);
			//Value.AddField("UVScale", ScaleV);
			//kPoint KUVOffset = ToBuildingPosition(UVOffset);
			//IValue& OffsetV = VF->Create(&KUVOffset, true);
			//Value.AddField("UVOffset", OffsetV);
			//kPoint KDefaultUV = ToBuildingPosition(DefaultUV);
			//IValue& DefaultUVV = VF->Create(&KDefaultUV, true);
			//Value.AddField("UVDefault", DefaultUVV);
			//Value.AddField("Angle", VF->Create(Angle));

			//pSurface->SetValue("MatParam", &Value);
			}	
		}
	 }

void UBuildingComponent::GetSurfaceParams(const int32& SurfaceID, FVector2D& UVScale, FVector2D& UVOffset, float& Angle, FVector2D& DefaultUV)
{
	FString UText = TEXT("密度U");
	FString VText = TEXT("密度V");
	FString NUText = TEXT("贴图U");
	FString NVText = TEXT("贴图V");
	FString OffsetXText = TEXT("位移X");
	FString OffsetYText = TEXT("位移Y");
	FString AnlgeText = TEXT("贴图角度");
	FString NAnlgeText = TEXT("法线角度");
	FString DefaultXText = TEXT("UVDefaultX");
	FString DefaultYText = TEXT("UVDefaultY");
	UVOffset.X = GetSurfaceFloatParam(SurfaceID, OffsetXText);
	UVOffset.Y = GetSurfaceFloatParam(SurfaceID, OffsetYText);
	Angle = GetSurfaceFloatParam(SurfaceID, AnlgeText);
	FVector2D UV;
	UV.X = GetSurfaceFloatParam(SurfaceID, UText);
	UV.Y = GetSurfaceFloatParam(SurfaceID, VText);

	UVScale.X = UV.X * 100.0;
	UVScale.Y = UV.Y * 100.0;
	
	
}

void UBuildingComponent::GetMatDefaultParamValues(const int32& SectionIndex, FVector2D& UVOffset, float& Angle, FVector2D& DefaultUV, FVector2D& ScaleUV)
{
	if (Data && SectionIndex < GetNumSections())
	{
		UMaterialInterface* UE4Mat = GetMaterialInterfaceBySurfaceID(GetSurfaceIDBySectionIndex(SectionIndex));
		if (UE4Mat)
		{
			DefaultUV.X = GetMatDefaultFloatParam(UE4Mat, TEXT("法线U"));
			DefaultUV.Y = GetMatDefaultFloatParam(UE4Mat, TEXT("法线V"));
			UVOffset.X = GetMatDefaultFloatParam(UE4Mat, TEXT("位移X"));
			UVOffset.Y = GetMatDefaultFloatParam(UE4Mat, TEXT("位移Y"));
			Angle = GetMatDefaultFloatParam(UE4Mat, TEXT("贴图角度"));
			ScaleUV = FVector2D(100.0, 100.0);
		}
	}
}

void UBuildingComponent::SetSectionMaterial(FVector2D& MaterialUVScale, const int32 &SectionIndex, const int32& ModelID, FString& ResID, const int32& MaterialType /*= 1*/, const FVector2D& UVSacle /*= FVector2D(100.0, 100.0)*/, const FVector2D& UVOffset /*= FVector2D(0.0, 0.0)*/, const float& Angle /*= 0.0*/, const FVector2D& DefaultUV)
{
	if (SectionIndex < 0 || SectionIndex >= GetNumSections() || ModelID == 0 || ResID.IsEmpty()) {
		return;
	}

	UMaterialInterface* UE4Interface = GetMaterialInterfaceByResIDAndType(ResID, MaterialType);
	if (UE4Interface == nullptr) {
		return;
	}

	UMaterialInstanceDynamic* UE4MID =  CreateDynamicMaterialInstance(SectionIndex, UE4Interface);
	if (UE4MID == nullptr) {
		return;
	}

	UE4MID->GetScalarParameterValue(TEXT("密度U"), MaterialUVScale.X);
	UE4MID->GetScalarParameterValue(TEXT("密度V"), MaterialUVScale.Y);

	UE4MID->SetScalarParameterValue(TEXT("位移X"), UVOffset.X);
	UE4MID->SetScalarParameterValue(TEXT("位移Y"), UVOffset.Y);

	UE4MID->SetScalarParameterValue(TEXT("密度U"), UVSacle.X / 100.0f * MaterialUVScale.X);
	UE4MID->SetScalarParameterValue(TEXT("密度V"), UVSacle.Y / 100.0f * MaterialUVScale.Y);
	UE4MID->SetScalarParameterValue(TEXT("法线U"), UVSacle.X / 100.0f * MaterialUVScale.X);
	UE4MID->SetScalarParameterValue(TEXT("法线V"), UVSacle.Y / 100.0f * MaterialUVScale.Y);

	UE4MID->SetScalarParameterValue(TEXT("贴图角度"), Angle);
	UE4MID->SetScalarParameterValue(TEXT("法线角度"), Angle);

	
}

FVector2D UBuildingComponent::SetSectionMaterialUseDefaultParams(const int32 &SectionIndex, const int32& ModelID, FString& ResID)
{
	FVector2D DefaultUV;
	SetSectionMaterial(DefaultUV, SectionIndex, ModelID, ResID, 1, FVector2D(100.0, 100.0), FVector2D(0.0, 0.0), 0.0, FVector2D(1.0, 1.0));
	return DefaultUV;
}

void UBuildingComponent::SetSectionMaterialParam(const int32 &SectionIndex, FString& ParamName, const float& Value)
{
	if (Data)
	{
		UMaterialInterface* SectionMat = GetMaterial(SectionIndex);
		UMaterialInstanceDynamic* DynaMatInst = Cast<UMaterialInstanceDynamic>(SectionMat);
		if (DynaMatInst)
		{
			FName Temp = FName(*ParamName);
			DynaMatInst->SetScalarParameterValue(Temp, Value);
		}
	}
}

void UBuildingComponent::SetSectionMaterialParams(const int32 &SectionIndex, const FVector2D& UV /*= FVector2D(1.0, 1.0)*/, const FVector2D& UVOffset /*= FVector2D(0.0, 0.0)*/, const float& Angle /*= 0.0*/)
{	
	FString UText = TEXT("密度U");
	FString VText = TEXT("密度V");
	FString NUText = TEXT("法线U");
	FString NVText = TEXT("法线V");
	FString OffsetXText = TEXT("位移X");
	FString OffsetYText = TEXT("位移Y");
	FString AnlgeText = TEXT("贴图角度");
	FString NAnlgeText = TEXT("法线角度");
	SetSectionMaterialParam(SectionIndex, UText, UV.X);
	SetSectionMaterialParam(SectionIndex, VText, UV.Y);
	SetSectionMaterialParam(SectionIndex, NUText, UV.X);
	SetSectionMaterialParam(SectionIndex, NVText, UV.Y);
	SetSectionMaterialParam(SectionIndex, OffsetXText, UVOffset.X);
	SetSectionMaterialParam(SectionIndex, OffsetYText, UVOffset.Y);
	SetSectionMaterialParam(SectionIndex, AnlgeText, Angle);
	SetSectionMaterialParam(SectionIndex, NAnlgeText, Angle);
}

UMaterialInterface * UBuildingComponent::GetMaterialInterfaceBySurfaceID(const int32& SurfaceID)
{
	ISuite *Suite = Data ? Data->GetSuite() : nullptr;
	UMaterialInterface *UE4Material = nullptr;
	if (Suite)
	{
		FString MaterialPath = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
		int32 MaterialType = Suite->GetProperty(SurfaceID, "MaterialType").IntValue();
		UE4Material = GetMaterialInterfaceByResIDAndType(MaterialPath, MaterialType);
		//if (MaterialPath.Len() > 0)
		//{
		//	switch (MaterialType)
		//	{
		//	case EUE4Material:
		//	{
		//		if (MaterialPath.Len() > 0)
		//		{
		//			UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
		//		}
		//		break;
		//	}
		//	case ERefMaterial:
		//	{
		//		UResource *Resource = UResourceMgr::GetResourceMgr()->FindRes(MaterialPath, true);
		//		if (Resource)
		//		{
		//			Resource->ForceLoad();
		//		}
		//		USurfaceFile *Surface = Cast<USurfaceFile>(Resource);
		//		if (Surface)
		//		{
		//			UE4Material = Surface->GetUE4Material();
		//		}
		//		UModelFile *ModelFile = Cast<UModelFile>(Resource);
		//		if (ModelFile)
		//		{
		//			UE4Material = ModelFile->GetUE4Material(0);
		//		}
		//	}
		//	default: break;
		//	}
		//}
		//else
		//{
		//	FBuildingConfig *Config = Data ? Data->GetConfig() : nullptr;
		//	if (Config)
		//	{
		//		UE4Material = Config->WallMaterial;
		//	}
		//	else
		//	{
		//		//UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *DEFAULT_WALL_MATERIAL));
		//		UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/_Basic/Materials/Materials_SecondLevel/M_Coating_DRN.M_Coating_DRN'")));
		//	}
		//}
	}
	//UMaterialInstanceDynamic *MaterialInst = CreateDynamicMaterialInstance(SectionIndex, UE4Material);
	return UE4Material;
}

UMaterialInterface * UBuildingComponent::GetMaterialInterfaceByResIDAndType(const FString& ResID, const int32& MaterialType)
{
	UMaterialInterface *UE4Material = nullptr;
	if (ResID.Len() > 0)
	{
		switch (MaterialType)
		{
		case EUE4Material:
		{
			if (ResID.Len() > 0)
			{
				UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *ResID));
			}
			break;
		}
		case ERefMaterial:
		{
			UResource *Resource = UResourceMgr::GetResourceMgr()->FindRes(ResID, true);
			if (Resource)
			{
				Resource->ForceLoad();
			}
			USurfaceFile *Surface = Cast<USurfaceFile>(Resource);
			if (Surface)
			{
				UE4Material = Surface->GetUE4Material();
			}
			UModelFile *ModelFile = Cast<UModelFile>(Resource);
			if (ModelFile)
			{
				UE4Material = ModelFile->GetUE4Material(0);
			}
		}
		default: break;
		}
	}
	else
	{
		FBuildingConfig *Config = Data ? Data->GetConfig() : nullptr;
		if (Config)
		{
			UE4Material = Config->WallMaterial;
		}
		else
		{
			//UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *DEFAULT_WALL_MATERIAL));
			UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("MaterialInstanceConstant'/Game/_Basic/Materials/Materials_SecondLevel/M_Coating_DRN.M_Coating_DRN'")));
		}
	}
	return UE4Material;
}

void UBuildingComponent::AutoSetMatParams(const int32& SectionIndex, const int32& SurfaceID, UMaterialInterface *UE4Mat)
{
	ISuite *Suite = Data ? Data->GetSuite() : nullptr;
	if(!Suite)	return;
	IObject *pSurface = Suite->GetObject(SurfaceID);

	UMaterialInstanceDynamic* MaterialInst = Cast<UMaterialInstanceDynamic>(GetMaterial(SectionIndex));
	if (MaterialInst && pSurface)
	{
		IValue *pValue = pSurface->GetValueArray();
		if (pValue)
		{
			kArray<const char *> Keys;
			kArray<IValue *> Values;

			pValue->GetPairs(Keys, Values);
			for (int32 i = 0; i < Keys.size(); ++i)
			{
				IValue *ParamValue = Values[i];
				const char *ParamName = Keys[i];
				EVarType Type = ParamValue->GetType();
				switch (Type)
				{
				case kV_Float:
				{
					MaterialInst->SetScalarParameterValue(ANSI_TO_TCHAR(ParamName), ParamValue->FloatValue());
					break;
				}
				case kV_Vec4D:
				{
					kVector4D Value2 = ParamValue->Vec4Value();
					MaterialInst->SetVectorParameterValue(ANSI_TO_TCHAR(ParamName), FLinearColor(FORCE_TYPE(FVector4, Value2)));
					break;
				}
				default:break;
				}
			}
		}
	}
}

float UBuildingComponent::GetMatDefaultFloatParam(const UMaterialInterface *UE4Mat, const FString& ParamName)
{
	if(!UE4Mat)	return 0.0;
	float Value;
	const TCHAR* TParamName = *ParamName;
	FMaterialParameterInfo FPI(TParamName);
	UE4Mat->GetScalarParameterValue(FPI, Value);
	return Value;
}

TArray<FString>  UBuildingComponent::GetMaterialResId()
{
	TArray<FString> MaterialResId;
	if (Data)
	{
		ISuite *Suite = Data->GetSuite();
		int32 NumSection = GetNumSections();
		for (int i = 0; i < NumSection; ++i)
		{
			int32 SurfaceID = GetSurfaceIDBySectionIndex(i);
			FString MaterialResid = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
			MaterialResId.Add(MaterialResid);
		}
		
	}
	return MaterialResId;
}
