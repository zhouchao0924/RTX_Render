#pragma once

struct FUVTextureMap
{
public:
	FVector2D OffSet;
	FVector2D MatSize;
	float MatRotation;
	FVector TextureBaseInScene;
	FVector2D TextureBase;
	FVector UDir;
	FVector VDir;

	FVector2D MapScene2TexturePos(const FVector& ScenePos)
	{
		FVector UDirNormal = UDir.GetSafeNormal();
		FVector VDirNormal = VDir.GetSafeNormal();
		FVector TextureBase2Scene = ScenePos - TextureBaseInScene;
		FVector2D TexturePos(TextureBase2Scene | UDirNormal, TextureBase2Scene | VDirNormal);
		TexturePos = TextureBase + FVector2D(TexturePos.X / MatSize.X, TexturePos.Y / MatSize.Y);
		FVector2D Texture2D = TexturePos.GetRotated(-MatRotation) - OffSet;

		return Texture2D;
	}
};

// section for runtime mesh component
struct FMeshSectionDef
{
public:
	TArray<FVector> Vertices;
	TArray<int32> Indices;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	bool bCollision;

	void BuildTriangleMesh()
	{
		int32 TriangleNum = Vertices.Num() / 3;
		int32 CurrentIndice = 0;
		for (int32 i = 0; i < TriangleNum; ++i)
		{
			int32 PntIdx = 3 * i;
			Indices.Add(PntIdx);
			Indices.Add(PntIdx + 1);
			Indices.Add(PntIdx + 2);

			auto V0 = Vertices[PntIdx];
			auto V1 = Vertices[PntIdx + 1];
			auto V2 = Vertices[PntIdx + 2];

			FVector V0V1 = V1 - V0;
			FVector V0V2 = V2 - V0;
			FVector FaceNormal = FVector::CrossProduct(V0V2, V0V1);
			FaceNormal.Normalize();
			Normals.Add(FaceNormal);
			Normals.Add(FaceNormal);
			Normals.Add(FaceNormal);
		}
	}

	FMeshSectionDef()
	{
		bCollision = true;
	}
};