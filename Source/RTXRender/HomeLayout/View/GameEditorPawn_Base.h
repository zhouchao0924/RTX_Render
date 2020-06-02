 

#pragma once

#include "GameFramework/Pawn.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "GameEditorPawn_Base.generated.h"

UCLASS()
class RTXRENDER_API AGameEditorPawn_Base : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameEditorPawn_Base();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		int32 LocationAlignmentValue;//网络对齐值

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		int32 RotationAlignmentValue;//旋转对齐值

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		int32 ScaleAlignmentValue;//缩放对齐值

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		bool MouseLeftClick;//鼠标左键按下

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		bool MouseRightClick;//鼠标右键按下

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		float CameraSpeed;//相机速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp_Move, meta = (AllowPrivateAccess = "true"))
		float FrontMoveAxis;//前后移动偏移值

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp_Move, meta = (AllowPrivateAccess = "true"))
		float RightMoveAxis;//左右移动偏移值

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp_Move, meta = (AllowPrivateAccess = "true"))
		float UpAndDownAxis;//上下移动偏移值

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp_Move, meta = (AllowPrivateAccess = "true"))
		float UpAndDownSpeed;//上下移动速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp_Move, meta = (AllowPrivateAccess = "true"))
		bool IsMoving;//是否正在移动

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool IsMultipleChoice;//多选

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool IsMarquee;//多选键按下

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool CenterOfTheWorld;//坐标轴世界中心点模式

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool WorldMode;//坐标轴世界坐标模式

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool MouseEntersTheWindow;//鼠标是否进入了某个窗口

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool WireframeMode;//线框模式


	//GetActorClass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ActorCopy, meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> ActorClassList;//复制_Actor类列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ActorCopy, meta = (AllowPrivateAccess = "true"))
		TArray<FTransform>ActorListTrans;//复制出的Actor位置列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ActorCopy, meta = (AllowPrivateAccess = "true"))
		TArray<AActor*>ReplicateTheActors;//复制的Actor列表

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Display, meta = (AllowPrivateAccess = "true"))
		bool AuxiliaryLine;//线框模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Display, meta = (AllowPrivateAccess = "true"))
		bool HideEditorElement;//隐藏编辑器元素


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		FTransform RightMenuPopUpPositionDetection;//右键菜单弹出时的位置检测

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		AActor* Temp_SpawnActor;//生成出的模型

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool isAddModel;//正在添加模型

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool SetAlignment;//设置对齐

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool IsBoxSelection;//框选

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool isMeasuringTools;//测量工具

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool SpawnPoint;//在出生点播放

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool SurfaceCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
    	bool NoLoadLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
     	TArray<AActor*> EndGroundIgnoreActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	   bool RealTimeLightingPattern;//点亮模式

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	  AActor*  tempactor;




	//墙体绘制模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wall, meta = (AllowPrivateAccess = "true"))
    	bool WallDrawingMode; 
	
	//2D预览
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wall, meta = (AllowPrivateAccess = "true"))
	   bool TDPreview; 

	//2D预览
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TD, meta = (AllowPrivateAccess = "true"))
		bool Enabled2DMode;

	//3d原始位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TD, meta = (AllowPrivateAccess = "true"))
    	FTransform OriginalLocation; 

	//2d位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TD, meta = (AllowPrivateAccess = "true"))
	    FTransform TDLocation;

	  //植被模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vegetation, meta = (AllowPrivateAccess = "true"))
	bool VegetationMode; 

	//植被模式-开始绘制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vegetation, meta = (AllowPrivateAccess = "true"))
	bool VegetationResources_EnableDrag;
	
    //拾取切割目标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cutting, meta = (AllowPrivateAccess = "true"))
	bool PickTheCuttingTarget; 

	//设置还原材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cutting, meta = (AllowPrivateAccess = "true"))
	bool SettingOrRestoreMaterial; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	bool Move;

	//仅选择顶点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	bool SelectOnlyVertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> TempActorList;

	//不创建属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	bool DoNotCreateAttributes;


	//禁用选择
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	bool DisableSelection;

	//禁用视口控制
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	bool DisableViewportControl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool StopReal;

	//几何体模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
	  bool GeometryMode;

	UFUNCTION(BlueprintCallable)
	void OnEscapeDown();
	public:
#pragma region 全屋定制缓存变量
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
			int32 customizationCategoryId;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
			ARoomActor *HouseCustomizationArea;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
			TArray<FAreaInnerWall>CacheCustomizationInnerLines;

#pragma endregion
protected:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void OnLeftMouseButtonDown();

	void OnLeftMouseButtonUp();

	void OnRightMouseButtonDown();

	void OnRightMouseButtonUp();

	void OnSpaceBar();

protected:
	UPROPERTY()
	class UCommandMgr *CmdMgr;
};