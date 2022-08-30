#pragma once
namespace MAYA_CAM
{
	struct CAMERA
	{
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float qX = 0.0f;
		float qY = 0.0f;
		float qZ = 0.0f;
		float qW = 0.0f;
		float aspectRatio = 0.0f;
		float fov = 0.0f;
		float nearPlane = 0.0f;
		float farPlane = 0.0f;
		
	};
	struct CAMERA_TRANSFORM
	{
		float tX = 0.0f;
		float tY = 0.0f;
		float tZ = 0.0f;
		float qX = 0.0f;
		float qY = 0.0f;
		float qZ = 0.0f;
		float qW = 0.0f;
		
	};
	enum class TYPE
	{
		DEFAULT,
		CAMERA,
		CAMERA_TRANSFORM
	};
	struct HEADER_TYPE
	{
		TYPE type = TYPE::DEFAULT;
	};
}