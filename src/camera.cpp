#include "camera.h"

void cameraSetPosition(Camera *camera, f32 posx, f32 posy, f32 posz)
{
    camera->posx = posx;
    camera->posy = posy;
    camera->posz = posz;
    return;
}

void cameraSetRotation(Camera *camera, f32 rotx, f32 roty, f32 rotz)
{
    camera->rotx = rotx;
    camera->roty = roty;
    camera->rotz = rotz;
    return;
}

DirectX::XMFLOAT3 cameraGetPosition(Camera *camera)
{
    return DirectX::XMFLOAT3(camera->posx, camera->posy, camera->posz);
}

DirectX::XMFLOAT3 cameraGetRotation(Camera *camera)
{
    return DirectX::XMFLOAT3(camera->rotx, camera->roty, camera->rotz);
}

void cameraRender(Camera *camera)
{

    DirectX::XMFLOAT3 up, position, lookAt;
    DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
    DirectX::XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = DirectX::XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = camera->posx;
	position.y = camera->posy;
	position.z = camera->posz;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = camera->rotx * 0.0174532925f;
	yaw   = camera->roty * 0.0174532925f;
	roll  = camera->rotz * 0.0174532925f;
	// pitch = 0.f;
	// yaw   = 0.f;
	// roll  = 0.f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	camera->view = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void cameraGetViewMatrix(Camera *camera, DirectX::XMMATRIX *view)
{
    *view = camera->view;
    return;
}
