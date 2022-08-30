// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}
//

using namespace std;
MCallbackIdArray callbackIdArray;
M3dView g_m3dView;
MDagPath g_cameraDag;
ComLib g_comLib;


void sendCameraObject(MObject& cameraNode)
{
	MAYA_CAM::CAMERA camera;
	MStatus resCamera = MS::kSuccess;
	MFnCamera mfnCamera(cameraNode, &resCamera);

	if (!MFAIL(resCamera))
	{
		camera.aspectRatio = (float)mfnCamera.aspectRatio();
		camera.farPlane = (float)mfnCamera.farClippingPlane();
		camera.fov = (float)mfnCamera.horizontalFieldOfView() * 180.0f / 3.14159265359f;
		camera.nearPlane = (float)mfnCamera.nearClippingPlane();
		
		MFnTransform mfnTransform(mfnCamera.parent(0));

		MMatrix transformationM = mfnTransform.transformationMatrix();

		MTransformationMatrix worldMatrix = transformationM;

		MVector translation = worldMatrix.getTranslation(MSpace::kWorld);
		double x;
		double y;
		double z;
		double w;

		worldMatrix.getRotationQuaternion(x, y, z, w);
		camera.qX = (float)x;
		camera.qY = (float)y;
		camera.qZ = (float)z;
		camera.qW = (float)w;
		if (!mfnCamera.isOrtho())
		{
			camera.posX = (float)translation.x;
			camera.posY = (float)translation.y;
			camera.posZ = (float)translation.z;


		}
		else
		{
			cout << "ORTHOGONAL NOT SUPPORTED\n";

		}
		g_comLib.sendCamera(camera);
	}
}

void cameraViewChanged(const MString& str, MObject& node, void* clientData)
{

	if (node.apiType() == MFn::Type::kCamera)
	{
		MStatus resCamera = MS::kSuccess;
		MFnCamera mfnCamera(node, &resCamera);
		{
			if (!MFAIL(resCamera))
			{
				g_m3dView = M3dView::active3dView();
				g_m3dView.getCamera(g_cameraDag);

				sendCameraObject(node);

			}
			else
			{
				cout << "CAMERA DIRTY\n";
			}

		}
	}
}
void cameraChanged(const MString& str, void* clientData)
{
	MAYA_CAM::CAMERA_TRANSFORM camTransform;
	MStatus resCamera = MS::kSuccess;
	MFnCamera mfnCamera(g_cameraDag.node(), &resCamera);
	if (!MFAIL(resCamera))
	{
		MFnTransform mfnTransform(mfnCamera.parent(0), &resCamera);

		if (!MFAIL(resCamera))
		{
			MMatrix transformationM;
			g_m3dView.modelViewMatrix(transformationM);
			transformationM = transformationM.inverse();
			MTransformationMatrix worldMatrix = transformationM;

			MVector translation = worldMatrix.getTranslation(MSpace::kWorld);

			if (!mfnCamera.isOrtho())
			{

				double x;
				double y;
				double z;
				double w;
				worldMatrix.getRotationQuaternion(x, y, z, w);
				camTransform.qX = (float)x;
				camTransform.qY = (float)y;
				camTransform.qZ = (float)z;
				camTransform.qW = (float)w;
				camTransform.tX = (float)translation.x;
				camTransform.tY = (float)translation.y;
				camTransform.tZ = (float)translation.z;
			}
			else
			{
				cout << "ORTHOGONAL NOT SUPPORTED\n";
			

				
			}
			g_comLib.sendCameraTransform(camTransform);
		}
	}


}

EXPORT MStatus initializePlugin(MObject obj) {

	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "level editor", "1.0", "Any", &res);

	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
		return res;
	}
	g_comLib.init();
	// redirect cout to cerr, so that when we do cout goes to cerr
	// in the maya output window (not the scripting output!)
	std::cout.set_rdbuf(MStreamUtils::stdOutStream().rdbuf());
	std::cerr.set_rdbuf(MStreamUtils::stdErrorStream().rdbuf());
	cout << "Plugin loaded ===========================" << endl;
	// register callbacks here for
	// MDGMessage::addNodeAddedCallback(nodedAdded, "dependNode", NULL, &status);
	// MDGMessage::addNodeRemovedCallback(nodeRemoved, "dependNode", NULL, &status);
	// MTimerMessage::addTimerCallback(0.1, timerCallback, NULL, &status);

	
	callbackIdArray.append(MUiMessage::addCameraChangedCallback("modelPanel4", cameraViewChanged, NULL, &res));
	callbackIdArray.append(MUiMessage::add3dViewPreRenderMsgCallback("modelPanel4", cameraChanged, NULL, &res));

	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);

	cout << "Plugin unloaded =========================" << endl;

	MMessage::removeCallbacks(callbackIdArray);

	return MS::kSuccess;
}