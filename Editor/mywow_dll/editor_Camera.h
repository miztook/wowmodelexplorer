#pragma once

#include "editor_base.h"

MW_API void Camera_setPosition(ICamera* cam, vector3df pos);
MW_API void Camera_getPosition(ICamera* cam, vector3df* pos);
MW_API void Camera_setDir(ICamera* cam, vector3df dir);
MW_API void Camera_getDir(ICamera* cam, vector3df* dir);
MW_API void Camera_getUp(ICamera* camera, vector3df* up);
MW_API void Camera_setLookat(ICamera* camera, vector3df lookat);
MW_API void Camera_getLookat(ICamera* camera, vector3df* lookat);
MW_API void Camera_onKeyMove(ICamera* cam, float speed, ICamera::SKeyControl keyControl);
MW_API void Camera_recalculateAll(ICamera* cam);
MW_API void Camera_pitch_yaw_Maya(ICamera* cam, float pitchDegree, float yawDegree);
MW_API void Camera_move_offset_Maya(ICamera* cam, float xOffset, float yOffset);
MW_API void Camera_pitch_yaw_FPS(ICamera* cam, float pitchDegree, float yawDegree);
