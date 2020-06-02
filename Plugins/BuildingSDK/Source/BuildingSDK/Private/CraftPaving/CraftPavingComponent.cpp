// Copyright? 2017 ihomefnt All Rights Reserved.

#include "CraftPaving/CraftPavingComponent.h"
#include "DesktopPlatformModule.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

UCraftPavingComponent::UCraftPavingComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FBuildingSDKModule &sdkModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
	mPatternEditor = QueryInterface<IPatternEditor>(sdkModule.GetSDK(), "PatternEditor");

	if (mPatternEditor == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Can not get the instance of IPatternEditor."));
		return;
	}

	mPatternObject = mPatternEditor->CreatePattern();
}

void UCraftPavingComponent::UpdatePolygon()
{
	ClearAllMeshSections();
	UpdateShapePolygon();
	UpdateIntervalPolygon();
}

bool UCraftPavingComponent::AddShape(const FShapeOptions& Options, int32& ShapeId)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	ShapeId = mPatternObject->AddShape(ConvertOptions(Options));
	return true;
}

bool UCraftPavingComponent::SetShapeOptions(int32 ShapeId, const FShapeOptions& Options)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	std::shared_ptr<ShapeOptions> options(ConvertOptions(Options));
	if (!options) {
		return false;
	}
	return mPatternObject->SetShapeOptions(ShapeId, options);
}

FShapeOptions UCraftPavingComponent::GetShapeOptions(int32 ShapeId)
{
	FShapeOptions resultOptions;
	resultOptions.Type = EShapeTypes::ST_UnKown;

	if (mPatternObject == nullptr) {
		return resultOptions;
	}

	std::shared_ptr<IShapeObject> shapeObject(mPatternObject->GetShape(ShapeId));
	if (!shapeObject) {
		return resultOptions;
	}

	std::shared_ptr<ShapeOptions> shapeOptions(shapeObject->GetShapeOptions());
	if (!shapeOptions) {
		return resultOptions;
	}

	resultOptions = ConvertOptions(shapeOptions);

	return resultOptions;
}

bool UCraftPavingComponent::TestHitShape(const FVector& StartPoint, const FVector& EndPoint, const TArray<AActor*>& IgnoreActors, int32& ShapeId, int32& SectionId)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	FHitResult hitResult;
	FCollisionQueryParams queryParams(FCollisionQueryParams::DefaultQueryParam);
	queryParams.bReturnFaceIndex = true;
	queryParams.AddIgnoredActors(IgnoreActors);

	if (!GetWorld()->LineTraceSingleByChannel(hitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility, queryParams)) {
		return false;
	}

	if (hitResult.GetComponent() != this) {
		return false;
	}

	FVector checkPoint(hitResult.ImpactPoint);
	
	for (int32 sectionIndex = 1; sectionIndex < GetNumSections(); sectionIndex++)
	{
		TArray<FVector> shapePolygon;
		
		FProcMeshSection* meshSection(GetProcMeshSection(sectionIndex));
		if (meshSection == nullptr) {
			continue;
		}

		shapePolygon.SetNumUninitialized(meshSection->ProcVertexBuffer.Num());
		for (int32 vertexIndex = 0; vertexIndex < meshSection->ProcVertexBuffer.Num(); vertexIndex++)
		{
			shapePolygon[vertexIndex] = meshSection->ProcVertexBuffer[vertexIndex].Position;
		}
		
		if (!IsPointInsidePolygon(shapePolygon, checkPoint)) {
			continue;
		}

		SectionId = sectionIndex;

		if (sectionIndex == 0) {
			ShapeId = 0;
			return false;
		}

		ShapeId = mShapeIdMap[sectionIndex];
		return true;
	}

	return false;
}

void UCraftPavingComponent::RemoveShape(int32 ShapeId)
{
	if (mPatternObject == nullptr) {
		return;
	}

	mPatternObject->RemoveShape(ShapeId);
	
	const uint32* sectionId(mShapeIdMap.FindKey(ShapeId));
	mShapeIdMap.Remove(*sectionId);
}

void UCraftPavingComponent::SetIntervalSize(float IntervalSize)
{
	if (mPatternObject == nullptr) {
		return;
	}

	mPatternObject->GetRawObject()->SetPropertyFloat("IntervalSize", IntervalSize);
}

void UCraftPavingComponent::GetShapeOffsetExpression(int32 ShapeId, bool bIsOffsetX, FString& Expression)
{
	if (mPatternObject == nullptr) {
		return;
	}

	std::shared_ptr<IShapeObject> shapeObject(mPatternObject->GetShape(ShapeId));
	if (!shapeObject) {
		return;
	}

	Expression = ANSI_TO_TCHAR(shapeObject->GetOffsetExpression(bIsOffsetX));
}

bool UCraftPavingComponent::SetShapeOffsetExpression(int32 ShapeId, const FString& Expression, bool bIsOffsetX)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	return mPatternObject->SetShapeOffsetExpression(ShapeId, TCHAR_TO_ANSI(*Expression), bIsOffsetX);
}

void UCraftPavingComponent::GetJointExpression(bool bIsHorizontal, FString& Expression)
{
	if (mPatternObject == nullptr) {
		return;
	}

	Expression = ANSI_TO_TCHAR(mPatternObject->GetJointExpression(bIsHorizontal));
}

bool UCraftPavingComponent::SetJointExpression(bool bIsHorizontal, const FString& Expression)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	return mPatternObject->SetJointExpression(TCHAR_TO_ANSI(*Expression), bIsHorizontal);
}

bool UCraftPavingComponent::CheckAliasIsValid(const FString& Alias)
{
	if (mPatternObject == nullptr) {
		return true;
	}

	return mPatternObject->CheckAliasIsValid(TCHAR_TO_ANSI(*Alias));
}

bool UCraftPavingComponent::GetAttributeAlias(int32 ShapeId, EAttributeTypes Type, FString& Alias)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	std::shared_ptr<IShapeObject> shapeObject(mPatternObject->GetShape(ShapeId));
	if (!shapeObject) {
		return false;
	}

	Alias = FString(shapeObject->GetAliasByAttribute(static_cast<EAttributeType>(Type)));

	return true;
}

void UCraftPavingComponent::SetShapeAttributeAlias(int32 ShapeId, EAttributeTypes Type, const FString& Alias)
{
	if (mPatternObject == nullptr) {
		return;
	}

	std::shared_ptr<IShapeObject> shapeObject(mPatternObject->GetShape(ShapeId));
	if (!shapeObject) {
		return;
	}

	shapeObject->SetAttributeAlias(static_cast<EAttributeType>(Type), TCHAR_TO_ANSI(*Alias));
}

void UCraftPavingComponent::RemoveShapeAttributeAlias(int32 ShapeId, EAttributeTypes Type)
{
	if (mPatternObject == nullptr) {
		return;
	}

	std::shared_ptr<IShapeObject> shapeObject(mPatternObject->GetShape(ShapeId));
	if (!shapeObject) {
		return;
	}

	shapeObject->RemoveAttributeAlias(static_cast<EAttributeType>(Type));
}

void UCraftPavingComponent::SetShapeAngle(int32 ShapeId, float NewAngle)
{
	if (mPatternObject == nullptr) {
		return;
	}

	mPatternObject->SetShapeAngle(ShapeId, NewAngle);
}

bool UCraftPavingComponent::SavePattern(const FString& Path, const FString& FileName)
{
	if (mPatternObject == nullptr || mPatternEditor == nullptr) {
		return false;
	}

	FString fullPath(FPaths::ConvertRelativePathToFull(Path));
	FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*fullPath);

	return mPatternObject->SavePattern(TCHAR_TO_ANSI(*fullPath), TCHAR_TO_ANSI(*FileName));
}

bool UCraftPavingComponent::LoadPattern(const FString& FilePath)
{
	ClearAllMeshSections();
	mShapeIdMap.Empty();

	if (mPatternEditor == nullptr || FilePath.IsEmpty() || !FPaths::FileExists(FilePath)) {
		return false;
	}

	FBuildingSDKModule &sdkModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));

	if (mPatternObject != nullptr && mPatternObject->GetRawObject() != nullptr) {
		IObject* objectHandle(mPatternObject->GetRawObject());
		ObjectID objectId(objectHandle->GetID());
		sdkModule.GetSDK()->GetResourceMgr()->DestroyResource(objectId, true);
	}

	IObject* object = sdkModule.GetSDK()->GetResourceMgr()->LoadResource(TCHAR_TO_ANSI(*(FPaths::ConvertRelativePathToFull(FilePath))));


	if (object == nullptr) {
		return false;
	}

	mPatternObject = mPatternEditor->GetPatternObject(object);

	if (mPatternObject == nullptr) {
		return false;
	}

	return true;
}

void UCraftPavingComponent::ReleasePattern()
{
	if (mPatternObject == nullptr) {
		return;
	}

	ClearAllMeshSections();
	mShapeIdMap.Empty();

	FBuildingSDKModule &sdkModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
	sdkModule.GetSDK()->GetResourceMgr()->DestroyResource(mPatternObject->GetRawObject()->GetID(), true);
	mPatternObject = nullptr;
}

FString UCraftPavingComponent::GetResId()
{
	if (mPatternObject == nullptr) {
		return TEXT("");
	}

	return FString(ANSI_TO_TCHAR(mPatternObject->GetResId()));
}

int32 UCraftPavingComponent::GetModifyVersion()
{
	if (mPatternObject == nullptr) {
		return -1;
	}

	return mPatternObject->GetModifyVersion();
}

int32 UCraftPavingComponent::GetLocalVersion()
{
	if (mPatternObject == nullptr) {
		return -1;
	}

	return mPatternObject->GetLocalVersion();
}

void UCraftPavingComponent::AddLocalVersion(int32 Number)
{
	if (mPatternObject == nullptr) {
		return;
	}

	mPatternObject->AddLocalVersion(Number);
}

void UCraftPavingComponent::ResetModifyFlag()
{
	if (mPatternObject == nullptr) {
		return;
	}

	mPatternObject->ResetModifyFlag();
}

void UCraftPavingComponent::GetPavingJoint(FVector& HorizontalJoint, FVector& VerticalJoint)
{
	if (mPatternObject == nullptr) {
		return;
	}

	kPoint horizontalJoint(mPatternObject->GetRawObject()->GetPropertyVector2D("HorizontalJoint"));
	kPoint verticalJoint(mPatternObject->GetRawObject()->GetPropertyVector2D("VerticalJoint"));

	HorizontalJoint.X = horizontalJoint.X;
	HorizontalJoint.Y = horizontalJoint.Y;
	VerticalJoint.X = verticalJoint.X;
	VerticalJoint.Y = verticalJoint.Y;
}

void UCraftPavingComponent::SetPavingJoint(const FVector& HorizontalJoint, const FVector& VerticalJoint)
{
	if (mPatternObject == nullptr) {
		return;
	}

	mPatternObject->GetRawObject()->SetPropertyVector2D("HorizontalJoint", kPoint(HorizontalJoint.X, HorizontalJoint.Y));
	mPatternObject->GetRawObject()->SetPropertyVector2D("VerticalJoint", kPoint(VerticalJoint.X, VerticalJoint.Y));
}

TArray<int32> UCraftPavingComponent::GetAllShapeIds()
{
	TArray<int32> ShapeIds;
	for(auto& Elem :mShapeIdMap)
	{
		ShapeIds.Add(Elem.Value);
	}
	return ShapeIds;
}

int32 UCraftPavingComponent::GetSectionIdByShapeId(int32 ShapeId)
{
	int32 sectionId;
	if (mShapeIdMap.FindKey(ShapeId) != nullptr)
	{
		sectionId = *(mShapeIdMap.FindKey(ShapeId));
	}
	return sectionId;
}

bool UCraftPavingComponent::GetShapePreviewColor(int32 ShapeId, FColor & Color)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	kColor previewColor;
	bool bSuccess = mPatternObject->GetShapePreviewColor(ShapeId, previewColor);
	Color.R = previewColor.R;
	Color.B = previewColor.B;
	Color.G = previewColor.G;
	Color.A = previewColor.A;
	return bSuccess;
}

bool UCraftPavingComponent::SetShapePreviewColor(int32 ShapeId, const FColor & Color)
{
	if (mPatternObject == nullptr) {
		return false;
	}

	kColor previewColor(Color.R, Color.G, Color.B, Color.A);
	return mPatternObject->SetShapePreviewColor(ShapeId, previewColor);
}

void UCraftPavingComponent::UpdateShapePolygon()
{
	if (mPatternObject == nullptr) {
		return;
	}
	std::shared_ptr<TArrayTemplate<FMeshSectionData>> meshSections(nullptr);
	std::shared_ptr<TArrayTemplate<SHAPE_ID>> meshShapeIds(nullptr);
	mPatternObject->GetAllShapeMeshData(meshSections, meshShapeIds);

	if (!meshSections || !meshShapeIds || meshSections->Num() != meshShapeIds->Num()) {
		return;
	}

	mShapeIdMap.Empty();

	for (int32 sectionIndex = 0; sectionIndex < meshSections->Num(); sectionIndex++)
	{
		FMeshSectionData& sectionData = (*meshSections)[sectionIndex];

		TArray<FVector> vertices;
		for (int32 verticesIndex = 0; verticesIndex < sectionData.Vertices.Num(); verticesIndex++)
		{
			vertices.Add(FVector(sectionData.Vertices[verticesIndex].X, sectionData.Vertices[verticesIndex].Y, sectionData.Vertices[verticesIndex].Z));
		}

		TArray<int32> triangles;
		for (int32 triangleIndex = 0; triangleIndex < sectionData.Triangles.Num(); triangleIndex++)
		{
			triangles.Add(sectionData.Triangles[triangleIndex]);
		}

		TArray<FVector> normals;
		for (int32 normalsIndex = 0; normalsIndex < sectionData.Normals.Num(); normalsIndex++)
		{
			FVector LocalNormal(sectionData.Normals[normalsIndex].X, sectionData.Normals[normalsIndex].Y, sectionData.Normals[normalsIndex].Z);
			normals.Add(LocalNormal);
		}

		TArray<FVector2D> uv0;
		for (int32 uv0Index = 0; uv0Index < sectionData.UV0.Num(); uv0Index++)
		{
			FVector2D LocalUV0(sectionData.UV0[uv0Index].X, sectionData.UV0[uv0Index].Y);
			uv0.Add(LocalUV0);
		}

		TArray<FProcMeshTangent> tangents;
		for (int32 tangentsIndex = 0; tangentsIndex < sectionData.Tangents.Num(); tangentsIndex++)
		{
			FProcMeshTangent LocalTangent(sectionData.Tangents[tangentsIndex].X, sectionData.Tangents[tangentsIndex].Y, sectionData.Tangents[tangentsIndex].Z);
			tangents.Add(LocalTangent);
		}

		mShapeIdMap.Add(sectionIndex + 1, (*meshShapeIds)[sectionIndex]);

		CreateMeshSection_LinearColor(sectionIndex + 1, vertices, triangles, normals, uv0, TArray<FLinearColor>(), tangents, true);
		ContainsPhysicsTriMeshData(true);
	}
}

void UCraftPavingComponent::UpdateIntervalPolygon()
{
	if (mPatternObject == nullptr) {
		return;
	}

	std::shared_ptr<FMeshSectionData> sectionData;
	mPatternObject->GetIntervalMeshData(sectionData);
	if (!sectionData) {
		return;
	}

	TArray<FVector> vertices;
	for (int32 verticesIndex = 0; verticesIndex < sectionData->Vertices.Num(); verticesIndex++)
	{
		vertices.Add(FVector(sectionData->Vertices[verticesIndex].X, sectionData->Vertices[verticesIndex].Y, sectionData->Vertices[verticesIndex].Z));
	}

	TArray<int32> triangles;
	for (int32 triangleIndex = 0; triangleIndex < sectionData->Triangles.Num(); triangleIndex++)
	{
		triangles.Add(sectionData->Triangles[triangleIndex]);
	}

	TArray<FVector> normals;
	for (int32 normalsIndex = 0; normalsIndex < sectionData->Normals.Num(); normalsIndex++)
	{
		FVector LocalNormal(sectionData->Normals[normalsIndex].X, sectionData->Normals[normalsIndex].Y, sectionData->Normals[normalsIndex].Z);
		normals.Add(LocalNormal);
	}

	TArray<FVector2D> uv0;
	for (int32 uv0Index = 0; uv0Index < sectionData->UV0.Num(); uv0Index++)
	{
		FVector2D LocalUV0(sectionData->UV0[uv0Index].X, sectionData->UV0[uv0Index].Y);
		uv0.Add(LocalUV0);
	}

	TArray<FProcMeshTangent> tangents;
	for (int32 tangentsIndex = 0; tangentsIndex < sectionData->Tangents.Num(); tangentsIndex++)
	{
		FProcMeshTangent LocalTangent(sectionData->Tangents[tangentsIndex].X, sectionData->Tangents[tangentsIndex].Y, sectionData->Tangents[tangentsIndex].Z);
		tangents.Add(LocalTangent);
	}

	CreateMeshSection_LinearColor(0, vertices, triangles, normals, uv0, TArray<FLinearColor>(), tangents, true);
	ContainsPhysicsTriMeshData(true);
}

FShapeOptions UCraftPavingComponent::ConvertOptions(std::shared_ptr<ShapeOptions> Options)
{
	FShapeOptions resultOptions;
	resultOptions.Type = EShapeTypes::ST_UnKown;

	if (!Options) {
		return resultOptions;
	}

	resultOptions.Type = static_cast<EShapeTypes>(Options->ShapeType);
	resultOptions.Transform.SetLocation(FVector(Options->ShapeTransform.Location.X, Options->ShapeTransform.Location.Y, Options->ShapeTransform.Location.Z));

	FRotator rotation;
	rotation.Pitch = Options->ShapeTransform.Rotation.Pitch;
	rotation.Yaw = Options->ShapeTransform.Rotation.Yaw;
	rotation.Roll = Options->ShapeTransform.Rotation.Roll;
	resultOptions.Transform.SetRotation(rotation.Quaternion());

	resultOptions.Transform.SetScale3D(FVector(Options->ShapeTransform.Scale.X, Options->ShapeTransform.Scale.Y, Options->ShapeTransform.Scale.Z));

	resultOptions.Length = Options->Length;
	resultOptions.Width = Options->Width;
	resultOptions.Radius = Options->Radius;
	resultOptions.Angle = Options->Angle;
	resultOptions.CornerRadius = Options->CornerRadius;
	resultOptions.Segments = Options->Segments;

	return resultOptions;
}

std::shared_ptr<ShapeOptions> UCraftPavingComponent::ConvertOptions(const FShapeOptions& Options)
{
	FVector shapeLocation(Options.Transform.GetLocation());
	FRotator shapeRotation(Options.Transform.GetRotation());
	FVector shapeScale(Options.Transform.GetScale3D());

	kXform shapeTransform;
	
	shapeTransform.Location = kVector3D(shapeLocation.X, shapeLocation.Y, shapeLocation.Z);
	shapeTransform.Scale = kVector3D(shapeScale.X, shapeScale.Y, shapeScale.Z);
	
	shapeTransform.Rotation.Pitch = shapeRotation.Pitch;
	shapeTransform.Rotation.Yaw = shapeRotation.Yaw;
	shapeTransform.Rotation.Roll = shapeRotation.Roll;

	std::shared_ptr<ShapeOptions> resultOptions(std::make_shared<ShapeOptions>());
	resultOptions->ShapeType = static_cast<EShapeType>(Options.Type);
	resultOptions->ShapeTransform = shapeTransform;
	resultOptions->Length = Options.Length;
	resultOptions->Radius = Options.Radius;
	resultOptions->Width = Options.Width;
	resultOptions->Angle = Options.Angle;
	resultOptions->CornerRadius = Options.CornerRadius;
	resultOptions->Segments = Options.Segments;

	return resultOptions;
}

bool UCraftPavingComponent::IsPointInsidePolygon(const TArray<FVector>& Polygon, const FVector& CheckPoint)
{
	int32 crossCount = 0;
	for (int32 pointIndex = 0; pointIndex < Polygon.Num(); pointIndex++)
	{
		FVector startPoint(Polygon[pointIndex]);
		FVector endPoint(Polygon[(pointIndex + 1) % Polygon.Num()]);

		if (startPoint.Y == endPoint.Y) {
			continue;
		}

		if (CheckPoint.Y < FMath::Min(startPoint.Y, endPoint.Y)) {
			continue;
		}

		if (CheckPoint.Y > FMath::Max(startPoint.Y, endPoint.Y)) {
			continue;
		}

		float checkPointX = (CheckPoint.Y - startPoint.Y) * (endPoint.X - startPoint.X) / (endPoint.Y - startPoint.Y) + startPoint.X;

		if (checkPointX > CheckPoint.X) {
			crossCount++;
		}
	}

	return (crossCount % 2) == 1;
}
