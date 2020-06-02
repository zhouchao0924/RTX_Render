#pragma once
namespace DRType
{
	 /** ��¼ */
	 static  FString Login = TEXT("Login");
	 /** ���͹��� */
	 static FString HouseBuild = TEXT("HouseBuild");
	 /** ������� */
	 static FString SolutionDesign = TEXT("SolutionDesign");
	  /** ������Ⱦ */
	 static FString SolutionRender = TEXT("SolutionRender");
	  /** �����ύ */
	 static FString SolutionSubmit = TEXT("SolutionSubmit");
};
namespace EventID
{
	/** �ǽ� */
	 static FString LoginIn = TEXT("LoginIn");
	 /** �ǳ� */
	 static FString LoginOut = TEXT("LoginOut");
	 /** ���ͽ��� */
	 static FString HouseEnter = TEXT("HouseEnter");
	 /** ���ͱ��� */
	 static FString HouseSave = TEXT("HouseSave");
	 /** �����˳� */
	 static FString HouseExit = TEXT("HouseExit");
	 /** �������� */
	 static FString CreateSolution = TEXT("CreateSolution");
	 /** ���״̬ */
	 static FString MouseState = TEXT("MouseState");

	 /** ������ƽ��� */
	 static FString SolutionDesignEnter = TEXT("SolutionDesignEnter");
	 /** ������Ʊ��� */
	 static FString SolutionDesignSave = TEXT("SolutionDesignSave");
	 /** ��������˳� */
	 static FString SolutionDesignExit = TEXT("SolutionDesignExit");

	 /** ��������Ⱦ3D���� */
	 static FString Render3DEnter = TEXT("Render3DEnter");
	 /** �����ӽ� */
	 static FString Render3DSaveView = TEXT("Render3DSaveView");
	 /** 3D���淢����Ⱦ���� */
	 static FString Render3DStartTask = TEXT("Render3DStartTask");
	 /** �˳���������Ⱦ���� */
	 static FString Render3DExit = TEXT("Render3DExit");

	 /** ��������Ⱦ2D���� */
	 static FString Render2DEnter = TEXT("Render2DEnter");
	 /** �˳�����Ⱦ2D���� */
	 static FString Render2DExit = TEXT("Render2DExit");
	 /** 2D���淢����Ⱦ���� */
	 static FString Render2DStartTask = TEXT("Render2DStartTask");
	 /** ɾ���ӽ� */
	 static FString DeleteView = TEXT("DeleteView");
	 /** ɾ������ͼƬ */
	 static FString DeleteSinglePic = TEXT("DeleteSinglePic");


	 /** �ύ���� */
	 static FString SubmitSolution = TEXT("SubmitSolution");
	 /** �����ύ������� */
	 static FString SolutionSubmitEnter = TEXT("SolutionSubmitEnter");
	 /** �����ύ�����뿪 */
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