#pragma once
namespace DRType
{
	 /** 登录 */
	 static  FString Login = TEXT("Login");
	 /** 户型构建 */
	 static FString HouseBuild = TEXT("HouseBuild");
	 /** 方案设计 */
	 static FString SolutionDesign = TEXT("SolutionDesign");
	  /** 方案渲染 */
	 static FString SolutionRender = TEXT("SolutionRender");
	  /** 方案提交 */
	 static FString SolutionSubmit = TEXT("SolutionSubmit");
};
namespace EventID
{
	/** 登进 */
	 static FString LoginIn = TEXT("LoginIn");
	 /** 登出 */
	 static FString LoginOut = TEXT("LoginOut");
	 /** 户型进入 */
	 static FString HouseEnter = TEXT("HouseEnter");
	 /** 户型保存 */
	 static FString HouseSave = TEXT("HouseSave");
	 /** 户型退出 */
	 static FString HouseExit = TEXT("HouseExit");
	 /** 方案创建 */
	 static FString CreateSolution = TEXT("CreateSolution");
	 /** 鼠标状态 */
	 static FString MouseState = TEXT("MouseState");

	 /** 方案设计进入 */
	 static FString SolutionDesignEnter = TEXT("SolutionDesignEnter");
	 /** 方案设计保存 */
	 static FString SolutionDesignSave = TEXT("SolutionDesignSave");
	 /** 方案设计退出 */
	 static FString SolutionDesignExit = TEXT("SolutionDesignExit");

	 /** 进入云渲染3D界面 */
	 static FString Render3DEnter = TEXT("Render3DEnter");
	 /** 保存视角 */
	 static FString Render3DSaveView = TEXT("Render3DSaveView");
	 /** 3D界面发起渲染任务 */
	 static FString Render3DStartTask = TEXT("Render3DStartTask");
	 /** 退出本次云渲染界面 */
	 static FString Render3DExit = TEXT("Render3DExit");

	 /** 进入云渲染2D界面 */
	 static FString Render2DEnter = TEXT("Render2DEnter");
	 /** 退出云渲染2D界面 */
	 static FString Render2DExit = TEXT("Render2DExit");
	 /** 2D界面发起渲染任务 */
	 static FString Render2DStartTask = TEXT("Render2DStartTask");
	 /** 删除视角 */
	 static FString DeleteView = TEXT("DeleteView");
	 /** 删除单张图片 */
	 static FString DeleteSinglePic = TEXT("DeleteSinglePic");


	 /** 提交方案 */
	 static FString SubmitSolution = TEXT("SubmitSolution");
	 /** 方案提交界面进入 */
	 static FString SolutionSubmitEnter = TEXT("SolutionSubmitEnter");
	 /** 方案提交界面离开 */
	 static FString SolutionSubmitExit = TEXT("SolutionSubmitExit");
};

namespace CrashID
{
	static  FString LoginCrash= TEXT("LoginCrash");//1
	static  FString HouseBuildCrash = TEXT("HouseBuildCrash");//2
	static  FString SolutionDesignCrash = TEXT("SolutionDesignCrash");//3
	static  FString Render2DCrash = TEXT("Render2DCrash");//4
	static  FString Render3DCrash = TEXT("Render3DCrash");//5
	static  FString SolutionSubmitCrash = TEXT("SolutionSubmitCrash");//6
	static  FString HouseLoadingCrash = TEXT("HouseLoadingCrash");//7
	static  FString SolutionLoadingCrash = TEXT("SolutionLoadingCrash");//8
}