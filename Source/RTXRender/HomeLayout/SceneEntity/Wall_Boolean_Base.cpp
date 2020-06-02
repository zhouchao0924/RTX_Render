 


#include "Wall_Boolean_Base.h"
#include "EditorGameInstance.h"
#include "../../Building/DRFunLibrary.h"
#include"KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRDoorHoleAdapter.h"

AWall_Boolean_Base::AWall_Boolean_Base()
{

	PrimaryActorTick.bCanEverTick = true;
	//创建根组件
	B_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//设置根组件
	RootComponent = B_RootComponent;

	Left_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Left_Arrow"));
	Left_Arrow->SetupAttachment(RootComponent);
	Left_Arrow->SetRelativeRotation(RPYTOPRY(FRotator( 0, 0, -90)));//设置旋转 1=中间  2=右边 3=左边
	Left_Arrow->SetRelativeLocation(FVector(0, -100, 0));//设置位置
 
	Right_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Right_Arrow"));
	Right_Arrow->SetupAttachment(RootComponent);
	Right_Arrow->SetRelativeRotation(RPYTOPRY(FRotator(0, 0, 90)));//设置旋转
	Right_Arrow->SetRelativeLocation(FVector(0, 100, 0));//设置位置

	Lit_Cube_01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lit_Cube_01"));
 	Lit_Cube_02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lit_Cube_02"));
	Lit_Cube_03 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lit_Cube_03"));
	Lit_Cube_04 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lit_Cube_04"));


	Lit_Cube_01->SetupAttachment(RootComponent);
	Lit_Cube_02->SetupAttachment(RootComponent);
	Lit_Cube_03->SetupAttachment(RootComponent);
	Lit_Cube_04->SetupAttachment(RootComponent);


	Lit_Cube_01->SetRelativeRotation(RPYTOPRY(FRotator(0, 0, 0)));
	Lit_Cube_02->SetRelativeRotation(RPYTOPRY(FRotator(0, 0, 0)));
	Lit_Cube_03->SetRelativeRotation(RPYTOPRY(FRotator(0, 0, -180)));
	Lit_Cube_04->SetRelativeRotation(RPYTOPRY(FRotator(0, 0, -180)));


	Lit_Cube_01->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Lit_Cube_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Lit_Cube_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Lit_Cube_04->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LitUp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LitUp"));
	LitUp->SetupAttachment(RootComponent);

	Left = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Left"));
	Left->SetupAttachment(RootComponent);
	Right = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Right"));
	Right->SetupAttachment(RootComponent);
	Up = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Up"));
	Up->SetupAttachment(RootComponent);
	Under = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Under"));
	Under->SetupAttachment(RootComponent);

	Start = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Start"));
	Start->SetHiddenInGame(true);
	Start->SetupAttachment(RootComponent);

	End = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("End"));
	End->SetHiddenInGame(true);
	End->SetupAttachment(RootComponent);

	Cur_Up = CreateDefaultSubobject<USceneComponent>(TEXT("Cur_Up"));
	Cur_Up->SetupAttachment(RootComponent);

	Cur_Up->SetRelativeRotation(FRotator(90, 0 , 0));

	Cur_Under = CreateDefaultSubobject<USceneComponent>(TEXT("Cur_Under"));
	Cur_Under->SetupAttachment(RootComponent);

	Cur_Under->SetRelativeRotation(FRotator(-90, 0, 0));



	//static ConstructorHelpers::FObjectFinder<UStaticMesh>SizeBoxAsset(TEXT("/Game/GameEditor/EditorPrograms/MapWallSystem/StativMesh/SizeBox.SizeBox"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>SizeBox1Asset(TEXT("/Game/GameEditor/EditorPrograms/MapWallSystem/StativMesh/SizeBox_02.SizeBox_02"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
 

	//设置发光体模型
	//if (SizeBoxAsset.Succeeded())
	//{
	//	Lit_Cube_01->SetStaticMesh(SizeBoxAsset.Object);
	//	Lit_Cube_03->SetStaticMesh(SizeBoxAsset.Object);
	//}
	//if (SizeBox1Asset.Succeeded())
	//{
	//	Lit_Cube_02->SetStaticMesh(SizeBox1Asset.Object);
	//	Lit_Cube_04->SetStaticMesh(SizeBox1Asset.Object);
	//}
	//if (CubeAsset.Succeeded())
	//{
	//	Start->SetStaticMesh(CubeAsset.Object);
	//	End->SetStaticMesh(CubeAsset.Object);
	//}


}


void AWall_Boolean_Base::BeginPlay()
{
	Super::BeginPlay();
	
 
}


void AWall_Boolean_Base::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AWall_Boolean_Base::Hidden(bool isHidden)
{
	Lit_Cube_01->SetHiddenInGame(isHidden,false);
	Lit_Cube_02->SetHiddenInGame(isHidden, false);
    Lit_Cube_03->SetHiddenInGame(isHidden, false);
	Lit_Cube_04->SetHiddenInGame(isHidden, false);

	Up->SetHiddenInGame(!isHidden, false);
	Under->SetHiddenInGame(!isHidden, false);
	Left->SetHiddenInGame(!isHidden, false);
	Right->SetHiddenInGame(!isHidden, false);

}



void AWall_Boolean_Base::Construct()
{
	UDRProjData *ProjectData =  UDRProjData::GetProjectDataManager(this);
	UDRHoleAdapter *HoleAdapter = static_cast<UDRHoleAdapter*>(ProjectData->GetAdapter(ObjectID));

	float _Width = HoleAdapter->GetWidth() - 0.05f;
	float _Hight = HoleAdapter->GetHeight() + 0.05f;
	float _Thickness = HoleAdapter->GetThickness() + 0.1f;
	float _Error = 0.04f;
	
	TArray<FVector> Ver;
	TArray<int32> TL;
	TArray<FVector> Normal;
	TArray<FVector2D> UV0;
	UV0 =
	{
		FVector2D(0.f,0.f),
		FVector2D(0.f,1.f),
		FVector2D(1.f,1.f),
		FVector2D(1.f,0.f)
	};


	Ver =
	{
		FVector((-_Width / 2.f) + _Error,( _Thickness / 2.f),0.f),
		FVector((-_Width / 2.f) + _Error,( _Thickness / 2.f),_Hight),
		FVector((-_Width / 2.f) + _Error,(-_Thickness / 2.f),_Hight),
		FVector((-_Width / 2.f) + _Error,(-_Thickness / 2.f),0.f),
	};
	TL = {0,2,1,0,3,2};
	Normal =
	{
		FVector(1.f,0.f,0.f),
		FVector(1.f,0.f,0.f),
		FVector(1.f,0.f,0.f),
		FVector(1.f,0.f,0.f)
	};
	Left->CreateMeshSection(0,Ver,TL, Normal, UV0, NullVColors, NullTangents, true);
	
	Ver =
	{
		FVector(( _Width / 2.f) - _Error,( _Thickness / 2.f) ,0.f),
		FVector(( _Width / 2.f) - _Error,( _Thickness / 2.f) ,_Hight),
		FVector(( _Width / 2.f) - _Error,(-_Thickness / 2.f) ,_Hight),
		FVector(( _Width / 2.f) - _Error,(-_Thickness / 2.f) ,0.f),
	};
	TL = { 0,1,2,0,2,3 };
	Normal =
	{
		FVector(-1.f,0.f,0.f),
		FVector(-1.f,0.f,0.f),
		FVector(-1.f,0.f,0.f),
		FVector(-1.f,0.f,0.f)
	};
	Right->CreateMeshSection(0, Ver, TL, Normal, UV0, NullVColors, NullTangents, true);

	Ver =
	{
		FVector(_Width / 2.f,-_Thickness / 2.f,_Hight - _Error),
		FVector(_Width / 2.f,_Thickness / 2.f,_Hight - _Error),
		FVector(-_Width / 2.f,_Thickness / 2.f,_Hight - _Error),
		FVector(-_Width / 2.f,-_Thickness / 2.f,_Hight - _Error),
	};
	TL = { 0,1,2,0,2,3 };
	Normal =
	{
		FVector(0.f,0.f,-1.f),
		FVector(0.f,0.f,-1.f),
		FVector(0.f,0.f,-1.f),
		FVector(0.f,0.f,-1.f)
	};
	Up->CreateMeshSection(0, Ver, TL, Normal, UV0, NullVColors, NullTangents, true);

	Ver =
	{
		FVector(_Width / 2.f,-_Thickness / 2.f,_Error+0.15),
		FVector(_Width / 2.f,_Thickness / 2.f,_Error+0.15),
		FVector(-_Width / 2.f,_Thickness / 2.f,_Error+0.15),
		FVector(-_Width / 2.f,-_Thickness / 2.f,_Error+0.15),
	};
	TL = { 0,3,2,0,2,1 };
	Normal =
	{
		FVector(0.f,0.f,1.f),
		FVector(0.f,0.f,1.f),
		FVector(0.f,0.f,1.f),
		FVector(0.f,0.f,1.f)
	};
	Under->CreateMeshSection(0, Ver, TL, Normal, UV0, NullVColors, NullTangents, true);
	Ver =
	{
		FVector(_Width / 2.f,-_Thickness / 2.f,282.f),
		FVector(_Width / 2.f,_Thickness / 2.f,282.f),
		FVector(-_Width / 2.f,_Thickness / 2.f,282.f),
		FVector(-_Width / 2.f,-_Thickness / 2.f,282.f),
	};
	LitUp->CreateMeshSection(0, Ver, TL, Normal, UV0, NullVColors, NullTangents, true);
	/*
	FVector Ver1= GetVertex(0, 0);
	FVector Ver2= GetVertex(0, 1);
	FVector Ver3= GetVertex(0, 2);
	FVector Ver4= GetVertex(0, 3);

	//设置 Start End 拖动点位置
	Start->SetRelativeLocation(FVector((Ver1 + Ver2).X / 2, RightWidth, WallHeight + WallLocation.Z-GetActorLocation().Z+1));
	End->SetRelativeLocation(FVector((Ver3 + Ver4).X / 2, LeftWidth*-1, WallHeight + WallLocation.Z -GetActorLocation().Z+1));

	LitUp->SetRelativeLocation(FVector(0,0, WallHeight + WallLocation.Z - GetActorLocation().Z));


	//Start End 拖动点缩放
	Start->SetWorldScale3D(FVector((Wall_Thick_Left+ Wall_Thick_Right)/100, (Wall_Thick_Left + Wall_Thick_Right) /100/ 2,0.01));
	End->SetWorldScale3D(FVector((Wall_Thick_Left + Wall_Thick_Right) / 100, (Wall_Thick_Left + Wall_Thick_Right) / 100 / 2, 0.01));

	//设置Arrow位置
	Left_Arrow->SetRelativeLocation(FVector(0, LeftWidth*-1,0));
	Right_Arrow->SetRelativeLocation(FVector(0, RightWidth, 0));


	Lit_Cube_01->SetWorldScale3D(FVector(Wall_Thick_Right/10+0.01, RightWidth/10, Height/10));
	Lit_Cube_02->SetWorldScale3D(FVector(Wall_Thick_Left / 10 + 0.01, RightWidth / 10, Height / 10));
	Lit_Cube_03->SetWorldScale3D(FVector(Wall_Thick_Left / 10 + 0.01, LeftWidth / 10, Height / 10));
	Lit_Cube_04->SetWorldScale3D(FVector(Wall_Thick_Right / 10 + 0.01, LeftWidth / 10, Height / 10));


	Cur_Up->SetRelativeLocation(FVector(0,0, Height));

	//创建内部面
	TArray<FVector> VerList;
	TArray<int32>TriangleList;
	TArray<FVector>Normal;//
	TArray<FVector2D>UV0;
	TArray<FProcMeshTangent>Tangents;//

	////////////////////////////////////////////////////////顶部区域//////////////////////////////////////////////////////
	//顶点
	VerList =
	{
		GetVertex(1,0),
		GetVertex(1,1),
		GetVertex(1,2),
		GetVertex(1,3)
	};

	TriangleList = { 2,1,0,0,3,2 };
	UV0 = VertexToUV(VerList, false, FVector2D(100, 100), false);//计算UV
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VerList, TriangleList, UV0, Normal, Tangents);//计算法线

	LitUp->CreateMeshSection(0, VerList, TriangleList, Normal, UV0, NullVColors, NullTangents, true);//创建

    ////////////////////////////////////////////////////////顶部区域//////////////////////////////////////////////////////



	//////////////////////////////////////////////////////Left/////////////////////////////////////////////////////
    //顶点
	VerList =
	{
	GetVertex(0,0) + FVector(0,0,Height),
	GetVertex(0,1) + FVector(0,0,Height),
	GetVertex(0,1),
	GetVertex(0,0)
	}; 
	//面
	TriangleList = { 2,1,0,0,3,2 }; 
	UV0 = VertexToUV(VerList, true,FVector2D(100,100), false);//计算UV
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VerList, TriangleList, UV0, Normal,Tangents);//计算法线
	Left->CreateMeshSection(0,VerList,TriangleList,Normal,UV0, NullVColors, NullTangents,true);//创建
	//////////////////////////////////////////////////////Left/////////////////////////////////////////////////////

	//清空
	VerList.Empty();
	TriangleList.Empty();
	Normal.Empty();
	UV0.Empty();
	Tangents.Empty();

	//////////////////////////////////////////////////////Right/////////////////////////////////////////////////////
	//顶点
	VerList =
	{
		GetVertex(0,2) + FVector(0,0,Height),
		GetVertex(0,3) + FVector(0,0,Height),
		GetVertex(0,3),
		GetVertex(0,2)
	};
	//面
	TriangleList = { 2,1,0,0,3,2 };
	UV0 = VertexToUV(VerList, true, FVector2D(100, 100), false);//计算UV
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VerList, TriangleList, UV0, Normal, Tangents);//计算法线
	Right->CreateMeshSection(0, VerList, TriangleList, Normal, UV0, NullVColors, NullTangents, true);//创建
	//////////////////////////////////////////////////////Right/////////////////////////////////////////////////////

	//清空
	VerList.Empty();
	TriangleList.Empty();
	Normal.Empty();
	UV0.Empty();
	Tangents.Empty();

	//////////////////////////////////////////////////////Up/////////////////////////////////////////////////////
	//顶点
	VerList =
	{
		FVector(GetVertex(0,3).X,GetVertex(0,3).Y,Height),
		FVector(GetVertex(0,2).X,GetVertex(0,2).Y,Height),
		FVector(GetVertex(0,1).X,GetVertex(0,1).Y,Height),
		FVector(GetVertex(0,0).X,GetVertex(0,0).Y,Height)
	};
	//面
	TriangleList = { 2,1,0,0,3,2 };
	UV0 = VertexToUV(VerList, false, FVector2D(100, 100), false);//计算UV
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VerList, TriangleList, UV0, Normal, Tangents);//计算法线
	Up->CreateMeshSection(0, VerList, TriangleList, Normal, UV0, NullVColors, NullTangents, true);//创建
    //////////////////////////////////////////////////////Up/////////////////////////////////////////////////////

	//清空
	VerList.Empty();
	TriangleList.Empty();
	Normal.Empty();
	UV0.Empty();
	Tangents.Empty();


	//////////////////////////////////////////////////////Under/////////////////////////////////////////////////////
	//顶点
	VerList =
	{
		FVector(GetVertex(0,0).X,GetVertex(0,0).Y,0.01f),
		FVector(GetVertex(0,1).X,GetVertex(0,1).Y,0.01f),
		FVector(GetVertex(0,2).X,GetVertex(0,2).Y,0.01f),
		FVector(GetVertex(0,3).X,GetVertex(0,3).Y,0.01f)
	};
	//面
	TriangleList = { 2,1,0,0,3,2 };
	UV0 = VertexToUV(VerList, false, FVector2D(100, 100), true);//计算UV
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VerList, TriangleList, UV0, Normal, Tangents);//计算法线
	Under->CreateMeshSection(0, VerList, TriangleList, Normal, UV0, NullVColors, NullTangents, true);//创建
    //////////////////////////////////////////////////////Under/////////////////////////////////////////////////////

	//set all cast shadow false  mod lijiuyi 2017.07.17
	Lit_Cube_01->SetCastShadow(false);
	Lit_Cube_02->SetCastShadow(false);
	Lit_Cube_03->SetCastShadow(false);
	Lit_Cube_04->SetCastShadow(false);
	LitUp->SetCastShadow(false);
	Left->SetCastShadow(false);
	Right->SetCastShadow(false);
	Up->SetCastShadow(false);
	Under->SetCastShadow(false);
	Start->SetCastShadow(false);
	End->SetCastShadow(false);
	*/

}

void AWall_Boolean_Base::SetHoleStruct(FDRHoleStruct const & Hole)
{
	HoleStruct = Hole;
	if (BuildingData)
	{
		BuildingData->SetBool(TEXT("bHorizonalFlip"), HoleStruct.bFlip);
		BuildingData->SetBool(TEXT("bVerticalFlip"), HoleStruct.bOpeningDoor);
	}
}


FOpeningSimpleAttr AWall_Boolean_Base::GetOpeningSimpleAttrBlueprint() const
{
	return GetOpeningSimpleAttr();
}

//获取顶点
FVector AWall_Boolean_Base::GetVertex(float VectorAddValue , int32 Index)
{
	switch (Index)
	{
	case 0:return FVector(Left_Arrow->RelativeLocation.X - Wall_Thick_Left, Left_Arrow->RelativeLocation.Y, Left_Arrow->RelativeLocation.Z+ VectorAddValue);
		break;
	case 1:return FVector(Left_Arrow->RelativeLocation.X + Wall_Thick_Right, Left_Arrow->RelativeLocation.Y, Left_Arrow->RelativeLocation.Z + VectorAddValue);
		break;
	case 2:return FVector(Right_Arrow->RelativeLocation.X + Wall_Thick_Right, Right_Arrow->RelativeLocation.Y, Right_Arrow->RelativeLocation.Z + VectorAddValue);
		break;
	case 3:return FVector(Right_Arrow->RelativeLocation.X - Wall_Thick_Left, Right_Arrow->RelativeLocation.Y, Right_Arrow->RelativeLocation.Z + VectorAddValue);

	default:return FVector(0, 0, 0);
		break;
	}
	return FVector(0,0,0);
}


//旋转转换
FRotator AWall_Boolean_Base::RPYTOPRY(FRotator Rotation)
{
	return FRotator(Rotation.Pitch, Rotation.Roll, Rotation.Yaw);
}


TArray<FVector2D>AWall_Boolean_Base::VertexToUV(TArray<FVector>InVector, bool YZ, FVector2D Scale, bool IsWorld)
{
	TArray<FVector2D>Null;
	TArray<FVector>sVector = InVector;
	if (IsWorld)
	{
		for (int i = 0; i <= InVector.Num() - 1; i++)
		{
			FRotator ro= GetActorRotation()- FRotator(0,0,90);
			sVector[i] = sVector[i] - ro.RotateVector(GetActorLocation());
		///	GetActorRotation().RotateVector(sVector[i]);


			//UKismetSystemLibrary::PrintString(this, FString(TEXT("a")), true, true, FLinearColor(0.0, 0.66, 1.0), 2.f);
		}
		InVector = sVector;
	}

	if (InVector.Num() == 0)
	{
		return Null;
	}
	else
	{
		TArray<FVector2D>A;
		for (int i = 0; i < InVector.Num(); i++)
		{
			if (YZ)
			{
				A.Add(FVector2D(InVector[i].X , InVector[i].Z)/ Scale );
			}
			else
			{
				A.Add(FVector2D(InVector[i].X, InVector[i].Y)/ Scale );
			}
		}
		return A;
	}
 

}

void AWall_Boolean_Base::SetBuildingData(UBuildingData * Data)
{
	if (Data)
	{
		BuildingData = Data;
	}
}

void AWall_Boolean_Base::UpdateBuildingData(UBuildingData * Data)
{

	if (Data == BuildingData)
	{
		float Local_GroundHeight = BuildingData->GetFloat(TEXT("ZPos"));
		FVector Local_Location = BuildingData->GetVector(TEXT("Location"));
		int32 _WallID = BuildingData->GetInt(TEXT("WallID"));
		UBuildingData * _WallData = BuildingData->GetBuildingSystem()->GetData(_WallID);
		float WallHeight = _WallData->GetFloat("Height");
		if (BooleanBaseHUDWidget)
		{
			BooleanBaseHUDWidget->WallID = _WallID;
		}
		float Local_Width = BuildingData->GetFloat(TEXT("Width"));
		float Local_Hight = BuildingData->GetFloat(TEXT("Height"));
		float Local_Thickness = BuildingData->GetFloat(TEXT("Thickness"));
		bool Local_HorizonalFlip = BuildingData->GetBool(TEXT("bHorizonalFlip"));
		LeftWidth = RightWidth = 0.5*Local_Width;

		FRotator Local_Rotation = FRotator::ZeroRotator;
		if (_WallData)
		{
			Local_Rotation = _WallData->GetVector(TEXT("Forward")).ToOrientationRotator();
		}

		bool Local_OpeningDoor = BuildingData->GetBool(TEXT("bVerticalFlip"));

		{
			float _Width = Local_Width - 0.05f;
			float _Hight = Local_Hight - 0.05f;
			float _Thickness = Local_Thickness + 0.1f;
			float _Error = 0.04f;

			TArray<FVector> Ver;
			TArray<FVector> Normal;
			TArray<FVector2D> UV0;
			UV0 =
			{
				FVector2D(0.f,0.f),
				FVector2D(0.f,1.f),
				FVector2D(1.f,1.f),
				FVector2D(1.f,0.f)
			};


			Ver =
			{
				FVector((-_Width / 2.f) + _Error,(_Thickness / 2.f),0.f),
				FVector((-_Width / 2.f) + _Error,(_Thickness / 2.f),_Hight),
				FVector((-_Width / 2.f) + _Error,(-_Thickness / 2.f),_Hight),
				FVector((-_Width / 2.f) + _Error,(-_Thickness / 2.f),0.f),
			};
			Normal =
			{
				FVector(1.f,0.f,0.f),
				FVector(1.f,0.f,0.f),
				FVector(1.f,0.f,0.f),
				FVector(1.f,0.f,0.f)
			};
			Left->UpdateMeshSection(0,Ver, Normal,UV0,NullVColors,NullTangents);
			Ver =
			{
				FVector((_Width / 2.f) - _Error,(_Thickness / 2.f) ,0.f),
				FVector((_Width / 2.f) - _Error,(_Thickness / 2.f) ,_Hight),
				FVector((_Width / 2.f) - _Error,(-_Thickness / 2.f) ,_Hight),
				FVector((_Width / 2.f) - _Error,(-_Thickness / 2.f) ,0.f),
			};
			Normal =
			{
				FVector(-1.f,0.f,0.f),
				FVector(-1.f,0.f,0.f),
				FVector(-1.f,0.f,0.f),
				FVector(-1.f,0.f,0.f)
			};
			Right->UpdateMeshSection(0, Ver, Normal, UV0, NullVColors, NullTangents);
			Ver =
			{
				FVector(_Width / 2.f,-_Thickness / 2.f,_Hight - _Error),
				FVector(_Width / 2.f,_Thickness / 2.f,_Hight - _Error),
				FVector(-_Width / 2.f,_Thickness / 2.f,_Hight - _Error),
				FVector(-_Width / 2.f,-_Thickness / 2.f,_Hight - _Error),
			};
			Normal =
			{
				FVector(0.f,0.f,-1.f),
				FVector(0.f,0.f,-1.f),
				FVector(0.f,0.f,-1.f),
				FVector(0.f,0.f,-1.f)
			};
			Up->UpdateMeshSection(0, Ver, Normal, UV0, NullVColors, NullTangents);
			Ver =
			{
				FVector(_Width / 2.f,-_Thickness / 2.f,_Error+0.15),
				FVector(_Width / 2.f,_Thickness / 2.f,_Error+0.15),
				FVector(-_Width / 2.f,_Thickness / 2.f,_Error+0.15),
				FVector(-_Width / 2.f,-_Thickness / 2.f,_Error+0.15),
			};
			Normal =
			{
				FVector(0.f,0.f,1.f),
				FVector(0.f,0.f,1.f),
				FVector(0.f,0.f,1.f),
				FVector(0.f,0.f,1.f)
			};
			//默认过门石UV是U为1，V为0.2，然按长宽设置比例	 ,默认洞的长宽是100cm * 20cm  ( _Width * _Thickness)
			TArray<FVector2D> UnderUV0;
			UnderUV0 =
			{
				FVector2D(0.f,0.f),
				FVector2D(0.f, 0.2 * _Thickness / 20),
				FVector2D(1.f * _Width / 100 ,0.2 * _Thickness / 20),
				FVector2D(1.f * _Width / 100  ,0.f)
			};

			Under->UpdateMeshSection(0, Ver, Normal, UnderUV0, NullVColors, NullTangents);
			Ver =
			{
				FVector(_Width / 2.f,-_Thickness / 2.f,WallHeight+2),
				FVector(_Width / 2.f,_Thickness / 2.f,WallHeight+2),
				FVector(-_Width / 2.f,_Thickness / 2.f,WallHeight+2),
				FVector(-_Width / 2.f,-_Thickness / 2.f,WallHeight+2),
			};
			LitUp->UpdateMeshSection(0, Ver, Normal, UV0, NullVColors, NullTangents);
		}
		this->SetActorLocationAndRotation(FVector(Local_Location.X, Local_Location.Y, Local_GroundHeight), Local_Rotation);
		BPUpdateBuildingData(Data);
	}
}






EDRModelType AWall_Boolean_Base::IsDoororBayWindow()
{
	return HoleStruct.ModelType;
}

void AWall_Boolean_Base::ShowUI(bool IsShow)
{
	if (BooleanBaseHUDWidget)
	{
		if (IsShow)
		{
			BooleanBaseHUDWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			BooleanBaseHUDWidget->bISet = false;
		}
		else
			BooleanBaseHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		BooleanBaseHUDWidget->SetColor(IsShow);
	}
}

void AWall_Boolean_Base::MoveComponent(bool IsSelected)
{
	if (BooleanBaseHUDWidget)
	{
		bIsMove = IsSelected;
		BooleanBaseHUDWidget->bIsMove = bIsMove;
	}
}

void AWall_Boolean_Base::Selected(const FVector2D MousePos, bool IsSelected)
{
	//ClearAllSelect(true);
	if (BooleanBaseHUDWidget)
	{
		bIsSelect = IsSelected;
		BooleanBaseHUDWidget->bIsSelect = IsSelected;
		if (IsSelected)
		{
			if (BuildingData != nullptr)
			{
				FVector Local_Location = BuildingData->GetVector(TEXT("Location"));
				BooleanBaseHUDWidget->Deltavector = FVector2D(Local_Location) - MousePos;
			}
		}
	}
}

void AWall_Boolean_Base::ClearAllSelect(bool bIsIgnoreSelf)
{
	//去掉其它Boolean的选中状态
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, AWall_Boolean_Base::StaticClass(), ActorList);
	for (AActor* Item : ActorList)
	{
		AWall_Boolean_Base* BooleanItem = Cast<AWall_Boolean_Base>(Item);
		if (BooleanItem)
		{
			//忽略自己的选中
			if (bIsIgnoreSelf)
			{
				if (BooleanItem == this)	continue;
			}
			BooleanItem->bIsSelect = false;
			if (BooleanItem->BooleanBaseHUDWidget)
			{
				BooleanItem->BooleanBaseHUDWidget->bIsSelect = false;
			}
		}
	}
}

void AWall_Boolean_Base::DeleteSelf()
{
	if (BooleanBaseHUDWidget)
	{
		BooleanBaseHUDWidget->RemoveFromParent();
		BooleanBaseHUDWidget = nullptr;
	}
}

void AWall_Boolean_Base::Init()
{
	if(ObjectID > 0)
	{
		UWorld *MyWorld = GetWorld();
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst)
		{
			UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
			if (WallSystem)
			{
				UBuildingData *HoleData = WallSystem->BuildingSystem->GetData(ObjectID);
				UClass *WidgetClass = WallSystem->BooleanUMG.Get();

				UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
				if (projectDataManager == nullptr) {
					return;
				}

				UDRHoleAdapter* holeAdapter = Cast<UDRHoleAdapter>(projectDataManager->GetAdapter(ObjectID));
				if (holeAdapter == nullptr) {
					return;
				}

				if (WidgetClass && WidgetClass->IsChildOf(UBooleanBase::StaticClass()))
				{
					BooleanBaseHUDWidget = Cast<UBooleanBase>(CreateWidget<UUserWidget>(GetWorld(), WallSystem->BooleanUMG));
					if (BooleanBaseHUDWidget)
					{
						BooleanBaseHUDWidget->AddToViewport(-200);
						BooleanBaseHUDWidget->HoleID = ObjectID;
						BooleanBaseHUDWidget->BooleanType = holeAdapter->GetHoleType();
						BooleanBaseHUDWidget->Width = holeAdapter->GetWidth();
						BooleanBaseHUDWidget->horizontalflip = holeAdapter->GetHorizontalFlip();
						BooleanBaseHUDWidget->Verticalflip = holeAdapter->GetVerticalFlip();
						BooleanBaseHUDWidget->HolestatusType = holeAdapter->GetHoleStatusType();
						if (WallSystem->BuildingSystem)
						{
							if (HoleData)
							{
								BooleanBaseHUDWidget->WallID = holeAdapter->GetDependWallId();
							}
						}
						if (UGameplayStatics::GetCurrentLevelName(MyWorld) != "2DMode")
						{
							BooleanBaseHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
						}
					}
					//此时设置选中，但是没有显示属性面板
					//Selected(true);
				}
			}
		}
	}
}

void AWall_Boolean_Base::ReadyMove(bool IsReadyMove)
{
	if (BooleanBaseHUDWidget)
	{
		bIsReadyMove = IsReadyMove;
	}
}

void AWall_Boolean_Base::UpDataWallID(int32 ID)
{
	if (BooleanBaseHUDWidget)
	{
		BooleanBaseHUDWidget->NewWallID = ID;
	}
}

void AWall_Boolean_Base::GetMoveCurWallID(int32 &ID)
{
	ID = -1;
	if (BooleanBaseHUDWidget)
	{
		if (BooleanBaseHUDWidget->NewWallID != INDEX_NONE)
		{
			ID = BooleanBaseHUDWidget->NewWallID;
		}
		else
		{
			ID = BooleanBaseHUDWidget->WallID;
		}
	}
}

void AWall_Boolean_Base::GetCurrentHoleWidth(float &Width)
{
	Width = 0;
	if (BooleanBaseHUDWidget)
	{
		Width = BooleanBaseHUDWidget->Width;
	}
}

int32 AWall_Boolean_Base::GetHoleID()
{
	return ObjectID;
}

void AWall_Boolean_Base::SetNewWallID(int32 ID)
{
	if (BooleanBaseHUDWidget)
	{
		if (ID != INDEX_NONE)
		{
			BooleanBaseHUDWidget->WallID = ID;
		}
	}
}


float AWall_Boolean_Base::GetMaxZPos()
{
	float MaxZPos = 0;
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
		if (HoleData)
		{
			int WallID = HoleData->GetInt("WallID");
			UBuildingData *WallData = BuildingData->GetBuildingSystem()->GetData(WallID);
			if (WallData)
			{
				float MaxHeight = WallData->GetFloat("Height");
				float HoleHeight = HoleData->GetFloat("Height");
				MaxZPos = MaxHeight - HoleHeight;
			}
		}
	}
	return MaxZPos;
}
float AWall_Boolean_Base::GetMaxWidth()
{
	float MaxWidth = 0;
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
		if (HoleData)
		{
			int WallID = HoleData->GetInt("WallID");
			TArray<FVector> WallNodes;
			FVector Start, End;
			float Zpos;
			BuildingData->GetBuildingSystem()->GetWallBorderLines(WallID, WallNodes, Zpos);
			UDRFunLibrary::CalculateBooleanMaxMinLoction(WallNodes, Start, End);
			MaxWidth = (End - Start).Size() - 10;
		}
	}
	return MaxWidth;
}
float AWall_Boolean_Base::GetMaxHeight()
{
	float MaxHight = 0;
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
		if (HoleData)
		{
			int WallID = HoleData->GetInt("WallID");
			UBuildingData *WallData = BuildingData->GetBuildingSystem()->GetData(WallID);
			if (WallData)
			{
				float MaxHeight = WallData->GetFloat("Height");
				float HoleGroundHeight = HoleData->GetFloat("ZPos");
				MaxHight = MaxHeight - HoleGroundHeight;
			}
		}
	}
	return MaxHight;
}

float AWall_Boolean_Base::GetZPos()
{
	UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
	float ZPos = 0.f;
	if (HoleData)
	{
		ZPos = HoleData->GetFloat("ZPos");
	}
	return ZPos;
}

float AWall_Boolean_Base::GetWidth()
{
	UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
	float Width = 0.f;
	if (HoleData)
	{
		Width = HoleData->GetFloat("Width");
	}
	return Width;
}

float AWall_Boolean_Base::GetHeight()
{
	UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
	float Height = 0.f;
	if (HoleData)
	{
		Height = HoleData->GetFloat("Height");
	}
	return Height;
}

void AWall_Boolean_Base::SetZPos(const float &z)
{
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
		if (HoleData)
		{
			int WallID = HoleData->GetInt("WallID");
			UBuildingData *WallData = BuildingData->GetBuildingSystem()->GetData(WallID);
			if (WallData)
			{
				float MaxHeight = WallData->GetFloat("Height");
				if ((z + HoleData->GetFloat("Height")) < MaxHeight)
				{
					HoleData->SetFloat("ZPos", z);
					GroundHeight = z;
				}
				else
				{
					GroundHeight = MaxHeight - BuildingData->GetFloat("Height");
					HoleData->SetFloat("ZPos", GroundHeight);
				}
			}
		}
	}
}

void AWall_Boolean_Base::SetWidth(const float &w)
{
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
		if (HoleData)
		{
			float ThickRight = 0;
			float ThickLeft = 0;
			IObject* CornerObj = nullptr;
			UBuildingData *PinCornerData=nullptr;
			FVector2D Pos1;
			FVector dir;
			int WallID = HoleData->GetInt("WallID");
			TArray<FVector> WallNodes;
			FVector Start, End;
			float Zpos;
			BuildingData->GetBuildingSystem()->GetWallBorderLines(WallID, WallNodes, Zpos);
			UDRFunLibrary::CalculateBooleanMaxMinLoction(WallNodes, Start, End);
			float MaxWidth = (End - Start).Size() - 10;
			UBuildingData *WallData = BuildingData->GetBuildingSystem()->GetData(WallID);
			int32 CornerID = HoleData->GetInt("CornerID");
			FVector2D RightTemp = (FVector2D(End) - FVector2D(Start)).GetRotated(90);
			RightTemp.Normalize();
			if (CornerID != INDEX_NONE)
			{
				PinCornerData = BuildingData->GetBuildingSystem()->GetData(CornerID);
				if (PinCornerData&&WallData)
				{
					Pos1 = FVector2D(PinCornerData->GetVector("Location"));
					dir = WallData->GetVector("Forward");
					ThickRight = WallData->GetFloat("ThickRight");
					ThickLeft = WallData->GetFloat("ThickLeft");
				}
				CornerObj = BuildingData->GetBuildingSystem()->GetObject(CornerID);
			}
			//FVector Pos2 = PinCornerData->GetVector("Location");
			if (w < MaxWidth)
			{
				//HoleData->SetFloat("Width", w);
				Pos1 = FVector2D(FMath::ClosestPointOnSegment(FVector(Pos1,0), Start + dir* (w / 2 + 5), End - dir* (w / 2 + 5)));
				HoleData->SetFloat("Width", w);
				//BuildingData->GetBuildingSystem()->ForceUpdateSuit();
				Length = w;
				if (ThickRight != ThickLeft)
				{
					Pos1 += RightTemp * (ThickRight - ThickLeft) / 2;
				}
				FVector2D Loc = PinCornerData->GetVector2D("Location");
				BuildingData->GetBuildingSystem()->Move(CornerID, Pos1 - Loc);
				//BuildingData->GetBuildingSystem()->ForceUpdateSuit();
			}
			else
			{
				HoleData->SetFloat("Width", MaxWidth);
				//BuildingData->GetBuildingSystem()->ForceUpdateSuit();
				Pos1 = FVector2D(FMath::ClosestPointOnSegment(FVector(Pos1,0), Start + dir * (MaxWidth / 2 + 5), End - dir * (MaxWidth / 2 + 5)));
				Length = MaxWidth;
				if (ThickRight != ThickLeft)
				{
					Pos1 += RightTemp * (ThickRight - ThickLeft) / 2;
				}
				FVector2D Loc = PinCornerData->GetVector2D("Location");
				BuildingData->GetBuildingSystem()->Move(CornerID, Pos1 - Loc);
				//HoleData->SetVector2D("Location", Loc);
			}
			if (BooleanBaseHUDWidget)
			{
				BooleanBaseHUDWidget->Width= Length;
			}
		}
	}
}

void AWall_Boolean_Base::SetHeight(const float &h)
{
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(ObjectID);
		if (HoleData)
		{
			int WallID = HoleData->GetInt("WallID");
			UBuildingData *WallData = BuildingData->GetBuildingSystem()->GetData(WallID);
			if (WallData)
			{
				float MaxHeight = WallData->GetFloat("Height");
				if ((h + HoleData->GetFloat("ZPos")) < MaxHeight)
				{
					HoleData->SetFloat("Height", h);
					Height = h;
				}
				else
				{
					HoleData->SetFloat("Height", MaxHeight - HoleData->GetFloat("ZPos"));
					Height = MaxHeight - HoleData->GetFloat("ZPos");
				}
			}
		}
	}
}

void AWall_Boolean_Base::SetVerticalflip(bool filp)
{
	//BuildingData->SetBool("bVerticalFlip", filp);
	UDRProjData *ProjectData = UDRProjData::GetProjectDataManager(this);
	if (ProjectData != nullptr)
	{
		UDRHoleAdapter *HoleAdapter = Cast<UDRHoleAdapter>(ProjectData->GetAdapter(ObjectID));
		if (HoleAdapter != nullptr)
		{
			HoleAdapter->SetVerticalFlip(filp);
			VerticalFlip = filp;
			if (BooleanBaseHUDWidget)
			{
				BooleanBaseHUDWidget->Verticalflip = filp;
			}
		}
	}		
}

void AWall_Boolean_Base::Sethorizontalflip(bool filp)
{
	UDRProjData *ProjectData = UDRProjData::GetProjectDataManager(this);
	if (ProjectData != nullptr)
	{
		UDRHoleAdapter *HoleAdapter = Cast<UDRHoleAdapter>(ProjectData->GetAdapter(ObjectID));
		if (HoleAdapter != nullptr)
		{
			HoleAdapter->SetHorizontalFlip(filp);
			HorizontalFlip = filp;
			if (BooleanBaseHUDWidget)
			{
				BooleanBaseHUDWidget->horizontalflip = filp;
			}
		}
	}
}

bool AWall_Boolean_Base::GetVerticalflip()
{
	bool bOpeningDoor = false;
	UDRProjData *ProjectData = UDRProjData::GetProjectDataManager(this);
	if (ProjectData != nullptr)
	{
		UDRHoleAdapter *HoleAdapter = Cast<UDRHoleAdapter>(ProjectData->GetAdapter(ObjectID));
		if (HoleAdapter != nullptr)
		{
			bOpeningDoor = HoleAdapter->GetVerticalFlip();
		}
	}
	return bOpeningDoor;
}

bool AWall_Boolean_Base::Gethorizontalflip()
{
	bool bhorizontalflip = false;
	UDRProjData *ProjectData = UDRProjData::GetProjectDataManager(this);
	if (ProjectData != nullptr)
	{
		UDRHoleAdapter *HoleAdapter = Cast<UDRHoleAdapter>(ProjectData->GetAdapter(ObjectID));
		if (HoleAdapter != nullptr)
		{
			bhorizontalflip = HoleAdapter->GetHorizontalFlip();
		}
	}
	return bhorizontalflip;
}

void AWall_Boolean_Base::UpdataUMGflip()
{
	if (BooleanBaseHUDWidget)
	{
		if (BuildingData != nullptr)
		{
			BooleanBaseHUDWidget->horizontalflip = Gethorizontalflip();
			BooleanBaseHUDWidget->Verticalflip = GetVerticalflip();
		}
	}
}

int AWall_Boolean_Base::GetDependsWallId()
{
	if (BuildingData&&BuildingData->GetBuildingSystem())
	{
		UBuildingData *HoleData = BuildingData->GetBuildingSystem()->GetData(HoleStruct.HoleID);
		if (HoleData)
		{
			return HoleData->GetInt("WallID");
		}

	}
	return 0;
}

float AWall_Boolean_Base::GetBayWindowLength()
{
	if (BuildingData != nullptr)
	{
		EModelType HoleType = static_cast<EModelType>(BuildingData->GetInt("Type"));
		if ((HoleType == EModelType::EDRBayWindow) && BooleanBaseHUDWidget)
		{
			return BooleanBaseHUDWidget->length;
		}
	}
	return 0;
}

void AWall_Boolean_Base::SetBayWindowLength(const float &Lm)
{
	if (BuildingData != nullptr)
	{
		EModelType HoleType = static_cast<EModelType>(BuildingData->GetInt("Type"));
		if ((HoleType == EModelType::EDRBayWindow) && BooleanBaseHUDWidget)
		{
			BooleanBaseHUDWidget->length = Lm;
		}
	}
}

void AWall_Boolean_Base::AutoSetWindowForward()
{
	if (BuildingData != nullptr)
	{
		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
		if (projectDataManager == nullptr) {
			return;
		}
		UDRHoleAdapter *HoleAdater = Cast<UDRHoleAdapter>(projectDataManager->GetAdapter(ObjectID));
		if (HoleAdater)
		{
			EModelType Type = HoleAdater->GetHoleType();
			if ((Type == EModelType::EDRWindow || Type == EModelType::EDRBayWindow) && BooleanBaseHUDWidget)
			{
				if (BuildingData&&BuildingData->GetBuildingSystem())
				{
					int WallID = BuildingData->GetInt("WallID");
					UBuildingData *WallData = BuildingData->GetBuildingSystem()->GetData(WallID);
					if (WallData)
					{
						int RoomRightID = WallData->GetInt("AreaRight");
						int RoomLeftID = WallData->GetInt("AreaLeft");
						if (RoomRightID != -1 && RoomLeftID == -1)
						{
							SetVerticalflip(true);
						}
						else
						{
							//if (Type == EModelType::EDRBayWindow)
							//{
								SetVerticalflip(false);
							//}
						}
						UpdateHoleModel();
					}
				}
			}
		}
	}
}

void AWall_Boolean_Base::IsNeedAdaptation()
{
	float Curwidth = GetWidth();
	float MaxWidth = GetMaxWidth();
	if (Curwidth> MaxWidth)
	{
		SetWidth(MaxWidth);
	}
}

bool AWall_Boolean_Base::IsOriginalHole()
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return false;
	}
	UDRHoleAdapter *HoleAdater = Cast<UDRHoleAdapter>(projectDataManager->GetAdapter(ObjectID));
	if (HoleAdater)
	{
		if (HoleAdater->GetHoleStatusType() == EHolestatusType::EDRNormalHole)
			return true;
	}
	return false;
}

void AWall_Boolean_Base::SetHoleStatusType(const EHolestatusType CurrentHolestatusType)
{
	if (BooleanBaseHUDWidget)
	{
		BooleanBaseHUDWidget->HolestatusType = CurrentHolestatusType;
	}
}