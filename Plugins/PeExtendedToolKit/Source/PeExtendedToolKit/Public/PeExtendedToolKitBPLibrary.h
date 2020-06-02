#pragma once

#include "Engine.h"

#include "FileManagerGeneric.h"
#include "CoreMisc.h"
#include "Engine.h"

#include "Components/AudioComponent.h"
#include "AudioDecompress.h"
#include "AudioDevice.h"
#include "ActiveSound.h"
#include "Audio.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"
#include "VorbisAudioInfo.h"

#include "SColorPicker.h"

#include "ImageUtils.h"
//#include "ImageWrapper.h"
#include "IImageWrapper.h"

#include "UserWidget.h"

#include "ProceduralMeshComponent.h"
#include "PeExtendedToolKitBPLibrary.generated.h"

//图像类型
UENUM(BlueprintType)
enum class EPEImageFormats : uint8
{
	JPG		UMETA(DisplayName = "JPG        "),
	PNG		UMETA(DisplayName = "PNG        "),
	BMP		UMETA(DisplayName = "BMP        "),
	ICO		UMETA(DisplayName = "ICO        "),
	EXR		UMETA(DisplayName = "EXR        "),
	ICNS		UMETA(DisplayName = "ICNS        ")
};

//UENUM(BlueprintType)
//enum class EDisplayMode: uint8
//{
//	SW_HIDE,				//隐藏窗口，活动状态给令一个窗口
//	SW_SHOWNORMAL,			//与SW_RESTORE相同
//	SW_SHOWMINIMIZED,		//最小化窗口，并将其激活
//	SW_SHOWMAXIMIZED,		//最大化窗口，并将其激活
//	SW_SHOWNOACTIVATE,		//用最近的大小和位置显示一个窗口，同时不改变活动窗口
//	SW_SHOW,				//用当前的大小和位置显示一个窗口，同时令其进入活动状态
//	SW_MINIMIZE,			//最小化窗口，活动状态给令一个窗口
//	SW_SHOWMINNOACTIVE,		//最小化一个窗口，同时不改变活动窗口
//	SW_SHOWNA,				//用当前的大小和位置显示一个窗口，不改变活动窗口
//	SW_RESTORE,				//用原来的大小和位置显示一个窗口，同时令其进入活动状态
//};

//打开文件后缀枚举类型
UENUM(BlueprintType)
enum class PeFileBrowserFilterType : uint8
{
	EXE_ALL,
	TXT_ALL,
	PNG_ALL,
	JPG_ALL,
	EPR_ALL,
	ZIP_ALL,
	RAR_ALL,
	PNG_JPG_ALL,
};

UENUM(BlueprintType)
enum class MessageBoxType : uint8
{
	Button_OK,
	Button_OK_Cancel,
	Button_Stop_Retry_Ignore,
	Button_Yes_No_Cancel,
	Button_Yes_No,
	Button_Retry_Cancel,
};

UENUM(BlueprintType)
enum class MessageBoxICOType : uint8
{
	None,
	ICO_Error,
	ICO_Doubt,
	ICO_Sigh,
	ICO_Info,
	ICO_User,
};
UENUM(BlueprintType)
enum class WindowSizeMode : uint8
{
		/* The windows size fixed and cannot be resized **/
		FixedSize,
		
		/** The window size is computed from its content and cannot be resized by users */
		Autosized,

		/** The window can be resized by users */
		UserSized,
};
 

USTRUCT(BlueprintType)
struct FBPSlateGradientStop
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FVector2D Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FLinearColor Color;

};


using namespace UF;

UCLASS()
class PEEXTENDEDTOOLKIT_API UPeExtendedToolKitBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AJUtil")
		static void  openFolder(FString foldername);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
		static void  openUrl(FString url);

	// Actor
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool GetActorTagKey(AActor* InActor, FName InKey, FName &OutValue);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool SetActorTagKey(AActor* InActor, FName InKey, FName InValue);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool AddActorTagKey(AActor* InActor, FName InKey, FName InValue);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool DeleteActorTagKey(AActor* InActor, FName InKey);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool ClearActorTagKey(AActor* InActor);
	// Class
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool GetClassTagKey(TSubclassOf<AActor> InClass, FName InKey, FName &OutValue);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool SetClassTagKey(TSubclassOf<AActor> InClass, FName InKey, FName InValue);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool AddClassTagKey(TSubclassOf<AActor> InClass, FName InKey, FName InValue);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool DeleteClassTagKey(TSubclassOf<AActor> InClass, FName InKey);
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ActorTag")
		static bool ClearClassTagKey(TSubclassOf<AActor> InClass);


	//获取根路径
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Path")
		static FString GetRootPath();

	//获取Engine路径
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Path")
		static FString GetEnginePath(bool Full);

	//获取游戏路径
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Path")
		static FString GetGamePath(bool Full);

	//获取游戏存档文件路径
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Path")
		static FString GetGameSavePath(bool Full);

	//获取截图默认保存的路径
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Path")
		static FString GetGameScreenShotPath(bool Full);

	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Path")
		static FString GetGameDataPath(bool Full);

	//归递查找文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File"	,meta = (DisplayName = "File Find Recursive"))
		static TArray<FString> File_Find(FString InPath, FString InFilter, bool InFiles, bool InDirectory);

	//搜索指定目录下的所有文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File",meta = (DisplayName = "File Find"))
		static TArray<FString> SearchFile(FString InPath, FString InFilter);

 


	//删除文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool File_Delete(FString InPath);

	//删除目录
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool Dir_Delete(FString InPath);

	//创建目录
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool Dir_Create(FString InPath);

	//判断文件是否已存在
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|File")
		static bool File_IsExists(FString InPath);

	//判断目录是否存在
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|File")
		static bool Dir_IsExists(FString InPath);

	//复制 注意!InTo为被复制的文件，InFrom是复制到的位置
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static int32 File_Copy(FString InFrom, FString InTo);

	//移动文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static int32 File_Move(FString InFrom, FString InTo);



 
	UFUNCTION(Category = "ExtendedContent|File", BlueprintCallable)
		static bool LoadAsset(FString Path, UObject* &Object );


	UFUNCTION(BlueprintPure, Category = "ExtendedContent")
		static FString GetObjectFullName(UObject* Object);


	UFUNCTION(Category = "ExtendedContent|File", BlueprintCallable)
     	static TArray<FString> GetAssetList(FString Path);

//	UFUNCTION(Category = "ExtendedContent|File", BlueprintCallable)
	//    static bool AddAssetPath(FName Path);

	//载入图像
	UFUNCTION(Category = "ExtendedContent|Texture", BlueprintCallable)
		static UTexture2D*  LoadTexture2D(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight);


	//获取保存像素点
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture", meta = (Keywords = "create image png jpg jpeg bmp bitmap ico icon exr icns"))
		static bool SavePixels(const FString& FullFilePath, int32 Width, int32 Height, const TArray<FLinearColor>& ImagePixels, FString& ErrorString);

	//获取图像指定位置像素点
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture", meta = (Keywords = "create image png jpg jpeg bmp bitmap ico icon exr icns"))
		static bool GetPixelFromT2D(UTexture2D* T2D, int32 X, int32 Y, FLinearColor& PixelColor);

	//获取图像像素点数组
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture", meta = (Keywords = "create image png jpg jpeg bmp bitmap ico icon exr icns"))
		static bool GetPixelsArrayFromT2D(UTexture2D* T2D, int32& TextureWidth, int32& TextureHeight, TArray<FLinearColor>& PixelArray);

	//载入图像(可选文件类型)
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture", meta = (Keywords = "image png jpg jpeg bmp bitmap ico icon exr icns"))
		static UTexture2D* LoadTexture2D_From_File(const FString& FullFilePath, EPEImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height);

	//载入图像像素
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture", meta = (Keywords = "image png jpg jpeg bmp bitmap ico icon exr icns"))
		static UTexture2D* LoadTexture2D_From_File_Pixels(const FString& FullFilePath, EPEImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, TArray<FLinearColor>& OutPixels);


	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture")
		static UTexture2D* ColorToTexture2D(TArray<FLinearColor> ColorArray, int32 Width, int32 Height, bool Transparent, bool Invert);


	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture")
		static UTexture2D* ByteToTexture2D(TArray<uint8> ByteArray, int32 Width, int32 Height);

	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture")
		static TArray<uint8>LoadTexture2DToByte(const FString& FullFilePath, int32& Width, int32& Height);

	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Texture")
		static FString ByteArrayToString(TArray<uint8>Byte);










	//保存文本文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool SaveStringTextToFile(FString SaveDirectory, FString FileName, FString SaveText, bool AllowOverWriting = false);

	//以数组的形式保存文本文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool SaveStringArrayToFile(FString SaveDirectory, FString FileName, TArray<FString> SaveText, bool AllowOverWriting = false);

	//以数组的形式载入文本文件(不支持中文)
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool LoadStringArrayFromFile(TArray<FString>& StringArray, int32& ArraySize, FString FullFilePath, bool ExcludeEmptyLines);

	//载入文本文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static bool LoadStringFromFile(FString& Contents, FString FullFilePath);


	/**
	*截图，并设置名字
	*@picName 图片名字。可以不设置
	*@bUnique 是否是唯一的名字  例如 picName 设置为 a的话，会截一张名为a.png的图片。
	*然后，再次执行截图，  如果bUnique是true，则新截出来的图片会叫作a0001.png ，
	*如果bUnique为fasle，则新图片会覆盖之前的
	*如果ShowHUD为fasle，则不会显示游戏的窗口
	*/
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static FString TakeScreenShoot(FString picName, bool bUnique = true, bool ShowHUD = true);

	//载入Asset文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static UObject* LoadObjectFromAssetPath(TSubclassOf<UObject> ObjectClass, TSubclassOf<UObject> InOuter, FName Path, bool AllowObjectReconciliation, bool& IsValid);

	//载入Asset类
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static UClass* LoadObjectFromAssetClassPath(TSubclassOf<UObject> BaseClass, FName Path, bool& IsValid);


	//载入.OGG声音文件
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File")
		static class USoundWave* LoadSoundWaveFromFile(const FString& FilePath);




	//执行EXE文件(只能执行EXE)
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Windows")
		static bool ExecutionFile(FString FilePath);

	//打开文件 备注: 注意斜杠的正反!
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Windows")
		static FString  OpenFile(FString InPath, bool IsOpenFile);


	//打开选择文件窗口
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Windows")
		static FString  OpenFileBrowserWindow(FString DefaultPath, PeFileBrowserFilterType Filter, FString HandleWindowName);

	//打开选择路径窗口
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Windows")
		static FString  OpenFolderBrowserWindow(FString Title,FString HandleWindowName);

	//提示窗口
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Windows")
		static int32  PromptBox(FString Title, FString content, int32 Mode);

	//MessageBox
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Windows", DisplayName = "MessageBox")
      	static int32 BPMessageBox(FString Title, FString Conten, MessageBoxType ButtonType, MessageBoxICOType ICO,FString HandleWindowName);


	//UE库-打开文件窗口
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File", meta = (Title = "OpenFile", TypeList = "DemoFile(*.txt)|*.txt|AllFile(*.*)|*.*|"))
		static TArray<FString> UOpenFileDialog(bool IsSavdDialog, FString Title, FString DefaultFile, FString TypeList, FString InitialPath, bool Multiple, int& OutFilterIndex);

	//UE库-选择路径
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|File", meta = (Title = "Select The Directory"))
		static FString UOpenDirectoryDialog(FString Title, FString DefaultPath);





	//自定义路径保存存档
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Game")
		static bool SaveGameToPath(USaveGame* SaveGameObject, const FString& SavedPath, const int32 UserIndex);

	//自定义路径加载存档
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Game")
		static USaveGame* LoadGameSaved(const FString& SavedPath, const int32 UserIndex);


	//查找向量
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FindVector", CompactNodeTitle = "Find"), Category = "ExtendedContent|Array")
		static int32 FindVector(TArray<FVector>InArray, FVector InVector, float ErrorTolerance = 0.001);

	//设置StaticMeshActor移动性
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|StaticMeshActor")
		static void SetStaticMeshMobility(AStaticMeshActor* StaticMeshActor, EComponentMobility::Type Mobility);

    //设置窗口缩放模式
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Game")
       static void SetGameWindowSizeMode(WindowSizeMode Mode);


    //获取窗口标题
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Game")
       static FText GetGameWindowTitle();

	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Game")
	  static bool IsEditorMode();

	//查找浮点型
	UFUNCTION(BlueprintPure,meta = (DisplayName = "FindFloat", CompactNodeTitle = "  FindFloat  "), Category = "ExtendedContent|Array")
     static int32 FindFloat(TArray<float>InArray, float InFloat, float ErrorTolerance);

	//创建窗口
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Game", meta = (ClientSize = "(X=400,Y=250)", Title = "Window", AutoCenter = "True", CreateTitleBar = "True", SupportsMaximize = "True", SupportsMinimize = "True", HasCloseButton = "True", InitialOpacity = "1", AdvancedDisplay = "6", DevelopmentOnly))
		static void WindowCreate(FText Title, bool AutoCenter, bool IsInitiallyMaximized, FVector2D Postion, FVector2D ClientSize, UWidget* Widget, bool CreateTitleBar, bool SupportsMaximize, bool SupportsMinimize, bool HasCloseButton, bool UseOSWindowBorder, bool ShouldPreserveAspectRatio, float InitialOpacity);



	//获取局部鼠标位置
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
		static FVector2D GetLocalMouseLocation();
	//直接获取鼠标位置
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
		static FVector2D GetMouseLocation();
	//设置鼠标位置
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Mouse")
		static void SetMouseLocation(FVector2D Location);

	//设置游戏窗口位置
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Game")
		static void SetGameWindowPosition(FVector2D Location);
	//获取游戏窗口位置
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Game")
		static FVector2D GetGameWindowPosition();
 
	//获取游戏窗口大小
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Game")
		static FVector2D GetGameWindowSize();



	//鼠标右键是否按下
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
		static bool IsMouseRightClick();

	//鼠标左键是否按下
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
		static bool IsMouseLeftClick();

	//鼠标中键是否按下
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
		static bool IsMouseMiddleClick();

	//Ctrl是否按下
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
	   static bool IsCtrlClick();

	//Alt是否按下
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Mouse")
	    static bool IsAltClick();


	//设置SoundLooping
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|Sound")
		static void SetSoundLooping(USoundWave* Sound, bool IsLooping);

	//获取SoundLooping
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|Sound")
		static bool GetSoundLooping(USoundWave* Sound);



	 UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "2", DevelopmentOnly), Category = "Utilities|String")
	static void PrintOut(UObject* WorldContextObject, const FString& InString = FString(TEXT("Hello")), bool bPrintToScreen = true, bool bPrintToLog = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);

	 UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DevelopmentOnly), Category = "ExtendedContent|Actor")
  	 static AActor* SpawnActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation, bool bNoCollisionFail);


	 //UMG绘制样线条
	 UFUNCTION(BlueprintCallable, Category = "ExtendedContent|UMG", meta = (AdvancedDisplay = "7", Thickness = "1", Color = "(R=1,G=1,B=1,A=1)"))
		 static void DrawSpline(FPaintContext Context, int32 Layer, FVector2D StartPoint, FVector2D StartDir, FVector2D EndPoint, FVector2D EndDir, float Thickness, FLinearColor Color);


	 //UMG获取Context最高层
	 UFUNCTION(BlueprintPure, Category = "ExtendedContent|UMG")
		 static int32 GetContextMaxLayer(FPaintContext Context);


	 /** 绘制渐变 */
	 UFUNCTION(BlueprintCallable, Category = "ExtendedContent|UMG")
		 static void DrawGradient(FPaintContext Context, int32 Layer, FVector2D Position, FVector2D Size, TArray<FBPSlateGradientStop>Gradient, bool IsHorizontal);


	 /** 绘制 Rotation box */
	 UFUNCTION(BlueprintCallable, Category = "ExtendedContent|UMG", meta = (AdvancedDisplay = "8", Tint = "(R=1,G=1,B=1,A=1)"))
		 static void DrawRotationBox(FPaintContext Context, int32 Layer, FVector2D Position, FVector2D Size, float Angle, FVector2D InRotationPoint, bool RelativeToWorld, USlateBrushAsset* Brush, FLinearColor Tint);

	 /** 绘制渐变样线条 */
	 UFUNCTION(BlueprintCallable, Category = "ExtendedContent|UMG", meta = (Thickness = "1"))
		 static void DrawGradientSpline(FPaintContext Context, int32 InLayer, const FVector2D StartPostion, const FVector2D StartDir, const FVector2D EndPostion, const FVector2D EndDir, TArray<FBPSlateGradientStop>Gradient, float Thickness);

	 /*
	 @执行文件
	 @details RunFile with .txt/log/exe...
	 @filePath
	 @Displaymode 显示方式 Defual SW_SHOW 5
	 SW_HIDE				= 0	隐藏窗口，活动状态给令一个窗口
	 SW_SHOWNORMAL		= 1 与SW_RESTORE相同
	 SW_SHOWMINIMIZED	= 2 最小化窗口，并将其激活
	 SW_SHOWMAXIMIZED	= 3 最大化窗口，并将其激活
	 SW_SHOWNOACTIVATE	= 4 用最近的大小和位置显示一个窗口，同时不改变活动窗口
	 SW_SHOW				= 5 用当前的大小和位置显示一个窗口，同时令其进入活动状态
	 SW_MINIMIZE			= 6	最小化窗口，活动状态给令一个窗口
	 SW_SHOWMINNOACTIVE	= 7	最小化一个窗口，同时不改变活动窗口
	 SW_SHOWNA			= 8	用当前的大小和位置显示一个窗口，不改变活动窗口
	 SW_RESTORE			= 9	用原来的大小和位置显示一个窗口，同时令其进入活动状态
	 @author DCF
	 */
	 //UFUNCTION(BlueprintCallable, Category = "AJUtil")
	 //static void RunFile(FString filePath, EDisplayMode windowMode);

	 /*
	 * 获取显示屏大小  kael_add
	 */
	 UFUNCTION(BlueprintCallable, Category = "ExtendedContent|UMG")
		static FVector2D GetDisplayScreenSize();
#if !PLATFORM_PS4
private:
	// Thanks to @keru for the base code for loading an Ogg into a USoundWave: 
	// https://forums.unrealengine.com/showthread.php?7936-Custom-Music-Player&p=97659&viewfull=1#post97659

	/**
	* Read .ogg header file and refresh USoundWave metadata. NOT SUPPORTED BY PS4
	* @param sw             wave to put metadata
	* @param rawFile        pointer to src file in memory
	* @return 0     if everything is ok
	*                 1 if couldn't read metadata.
	*/
	static int32 fillSoundWaveInfo(USoundWave* sw, TArray<uint8>* rawFile);
#endif 
};



