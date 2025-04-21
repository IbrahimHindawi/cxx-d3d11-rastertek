#pragma once

#include <directxmath.h>

#include "core.h"

struct Camera
{
    f32 posx; f32 posy; f32 posz;
    f32 rotx; f32 roty; f32 rotz;
    DirectX::XMMATRIX view;
};

void cameraSetPosition(Camera *camera, f32 posx, f32 posy, f32 posz);
void cameraSetRotation(Camera *camera, f32 rotx, f32 roty, f32 rotz);
DirectX::XMFLOAT3 cameraGetPosition(Camera *camera);
DirectX::XMFLOAT3 cameraGetRotation(Camera *camera);
void cameraRender(Camera *camera);
void cameraGetViewMatrix(Camera *camera, DirectX::XMMATRIX *view);
