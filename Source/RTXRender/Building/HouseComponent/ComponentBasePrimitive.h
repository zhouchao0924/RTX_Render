#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Building/DRStruct.h"
#include "ComponentBasePrimitive.generated.h"


UENUM()
enum class EComponenetState :uint8
{
	_Add,
	_Default,
	_Select
};


USTRUCT()
struct FPrimitive2D
{
	GENERATED_BODY()
public:
	FPrimitive2D() {}
	FPrimitive2D(FVector2D LT, FVector2D RT, FVector2D RB, FVector2D LB)
		:LeftTopPos(LT)
		,RightTopPos(RT)
		,RightBottomPos(RB)
		,LeftBottomPos(LB)
	{

	}
	UPROPERTY()
		FVector2D LeftTopPos;//左上
	UPROPERTY()
		FVector2D RightTopPos;//右上
	UPROPERTY()
		FVector2D RightBottomPos;//右下
	UPROPERTY()
		FVector2D LeftBottomPos;//左下
};
UCLASS()
class UComponentBasePrimitive:public UObject
{
	GENERATED_BODY()
public:
	UComponentBasePrimitive();
	virtual void Draw(FPaintContext& InContext) const {};
	virtual void PrimitiveVertexDataGenerate(UWorld* MyWorld, const FVector2D& mousePos) {};
	void DrawBox(FPaintContext& InContext, const FPrimitive2D& inPrimitive,
	     const FLinearColor& FrameColor, const FLinearColor& Tint, float Angle = 0.0f)const;
	void DrawAuxLine(FPaintContext& InContext)const;
	//void GetOtherPos(const FVector2D& posLeftTop, const FVector2D& posRightBottom, FVector2D& posRightTop, FVector2D&posLeftBottom) const;
	FPrimitive2D WorldLocationToScreen(UWorld* MyWorld,const FVector2D& loc, int32 length, int32 width) const;

	void DrawCircle(FPaintContext& InContext, const FVector2D& centerPos, int32 r, const FLinearColor& color,float Anglef=1)const;
	void DrawSelectStatePrimitive(FPaintContext& InContext,const FPrimitive2D& PrimitivePos) const;

	//数据保存\还原
	void UpdataComponenetInfo(const FComponenetInfo& Info);

	FPrimitive2D GetPrimitive2DConst(const FPrimitive2D& inPrimitive, float Angle = 0.0f)const;
	void SetBrush(FSlateBrush B);
	void SetBrushLineColor(const FLinearColor& color);
	void SetComponentDRActorID(int32 ID);
	int32 GetComponentDRActorID() const;
	void SetComponentRoomID(int32 ID);
	int32 GetComponentRoomID() const;
	bool GetHasWall() const;
	void AddWallInfo(int32 id);
	void DestroyWall();
	//void RemoveOtherWall(int32 id);
	void RemoveAll();
	void RenewSetWall(int32 ID0,int32 ID1=INDEX_NONE);
	TArray<int32> GetAllWallIdsInfo() const;
	void SetWallIdsInfo(const TArray<int32>&WallIds);
	void SetCenterPos(FVector2D c);
	FVector2D GetCenterPos() const;
	void SetWidth(float w);
	void SetLength(float l);
	void SetAngleVal(float a);
	void SetTPoylgon(const TArray<FVector2D>& datas);
	TArray<FVector2D> GetTPoylgon() const;
	void SetIsWidth(bool b);
	void SetComponenetType(EComponenetType type);
	EComponenetType GetComponenetType() const;
	bool GetIsWidth() const;
	float GetAngleVal() const;
	float GetWidth() const;
	float GetLength() const;
	void SetHeight(float H);
	float GetHeight() const;
	FVector2D GetLoc() const;
    FComponenetInfo GetComponenetInfo() const;
	void SetMaterialInfo(int32 MaterialChannel, const FString& ResID,int32 modelID,const int32& RoomClassID, const int32& CraftID);
	void SetMaterialUVInfo(FVector Scale, FVector Offset, float Angle);
	TMap<int32, FString> GetMaterialInfo() const;
	void SetModelID(int32 id);
	int32 GetModelID() const; 
	void SetComponentUUID(FString uuid);
	FString GetComponentUUID();
	int32 GetRoomClassID() const;
	int32 GetCraftID() const;
	FVector GetScale() const;
	void SetScale(FVector scale);
	FVector GetOffset() const;
	void SetOffset(FVector offset);
	float GetAngle() const;
	void SetAngle(float angle);
	void SetWorld(UWorld* World);
	void SetDrawActive(const bool& b);
	FString GetSewerName() const;
	void SetSewerName(FString Name);
	virtual void SetFrameColor(const FLinearColor Color) {};
public:
	UPROPERTY()
	FPrimitive2D FramePrimitive;//框
	//UPROPERTY()
	//FPrimitive2D HitPrimitive;//碰撞框
	UPROPERTY()
	mutable FSlateBrush Brush;
	UPROPERTY()
	EComponenetState ComponenetState;
    
	UPROPERTY()
	float ViewAngleVal;
	UPROPERTY()
	mutable FVector2D size;
	UPROPERTY()
	FVector2D PosAtWall;
	UPROPERTY()
	FVector2D NodeForWard;
	UPROPERTY()
	FVector2D rightBottomLoc;
	UPROPERTY()
		TArray<float> TempX;
	UPROPERTY()
		TArray<float> TempY;
	UPROPERTY()
	float ObjectSnapTol;
	UPROPERTY()
	bool bDrawActive;
	UPROPERTY()
	UWorld *MyWorld;
protected:
	UPROPERTY()
	FComponenetInfo ComponenetInfo;
	UPROPERTY()
	FVector2D CenterPos;
	//UPROPERTY()
	//float m_iWidth;
	//UPROPERTY()
	//float m_iLegth;
	//UPROPERTY()
	//float m_fAngle;

};


