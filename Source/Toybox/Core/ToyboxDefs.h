#pragma once


namespace Toybox
{

///  Keyboard map to each controller
enum KeyMap
{
    //  Character controller map
    KM_CHARACTER_FORWARD = 1 << 0,
    KM_CHARACTER_BACKWARD = 1 << 1,
    KM_CHARACTER_STRAFE_LEFT = 1 << 2,
    KM_CHARACTER_STRAFE_RIGHT = 1 << 3,
    KM_CHARACTER_TURN_LEFT = 1 << 4,
    KM_CHARACTER_TURN_RIGHT = 1 << 5,
    KM_CHARACTER_JUMP = 1 << 6,
    KM_CHARACTER_WALK = 1 << 7,

    //  Ship controller map
    KM_VEHICLE_THRUST_X_INC = 1 << 0,
    KM_VEHICLE_THRUST_X_DEC = 1 << 1,
    KM_VEHICLE_THRUST_X_MAX = 1 << 2,
    KM_VEHICLE_THRUST_X_MIN = 1 << 3,
    KM_VEHICLE_THRUST_X_STOP = 1 << 4,
    KM_VEHICLE_ROT_X_INC = 1 << 5,
    KM_VEHICLE_ROT_X_DEC = 1 << 6,
    KM_VEHICLE_ROT_X_MAX = 1 << 7,
    KM_VEHICLE_ROT_X_MIN = 1 << 8,
    KM_VEHICLE_ROT_X_STOP = 1 << 9,

    KM_VEHICLE_THRUST_Y_INC = 1 << 10,
    KM_VEHICLE_THRUST_Y_DEC = 1 << 11,
    KM_VEHICLE_THRUST_Y_MAX = 1 << 12,
    KM_VEHICLE_THRUST_Y_MIN = 1 << 13,
    KM_VEHICLE_THRUST_Y_STOP = 1 << 14,
    KM_VEHICLE_ROT_Y_INC = 1 << 15,
    KM_VEHICLE_ROT_Y_DEC = 1 << 16,
    KM_VEHICLE_ROT_Y_MAX = 1 << 17,
    KM_VEHICLE_ROT_Y_MIN = 1 << 18,
    KM_VEHICLE_ROT_Y_STOP = 1 << 19,

    KM_VEHICLE_THRUST_Z_INC = 1 << 20,
    KM_VEHICLE_THRUST_Z_DEC = 1 << 21,
    KM_VEHICLE_THRUST_Z_MAX = 1 << 22,
    KM_VEHICLE_THRUST_Z_MIN = 1 << 23,
    KM_VEHICLE_THRUST_Z_STOP = 1 << 24,
    KM_VEHICLE_ROT_Z_INC = 1 << 25,
    KM_VEHICLE_ROT_Z_DEC = 1 << 26,
    KM_VEHICLE_ROT_Z_MAX = 1 << 27,
    KM_VEHICLE_ROT_Z_MIN = 1 << 28,
    KM_VEHICLE_ROT_Z_STOP = 1 << 29,

	//  Camera controller map
	KM_CAMERA_MODE = 1 << 0,
	KM_CAMERA_RESET = 1 << 1,
	KM_CAMERA_MOVE = 1 << 2,
	KM_CAMERA_FORWARD = 1 << 3,
	KM_CAMERA_BACKWARD = 1 << 4,
	KM_CAMERA_PAN_LEFT = 1 << 5,
	KM_CAMERA_PAN_RIGHT = 1 << 6,
	KM_CAMERA_ROLL_LEFT = 1 << 7,
	KM_CAMERA_ROLL_RIGHT = 1 << 8
};


enum CameraCtrlType
{
	CAMERA_TYPE_VEHICLE = 2,
	CAMERA_TYPE_CHARACTER = 3
};

///  Camera controller modes
enum CameraCtrlMode
{
	CAMERA_MODE_FOLLOW = 1,
	CAMERA_MODE_FREE = 2
};


///  Character controller modes
enum CharCtrlMode
{
    CHAR_CONTROLLER_KINEMATIC = 1 << 0,
    CHAR_CONTROLLER_DYNAMIC = 1 << 1
};


///  Scene types
enum SceneType
{
	SCENE_SPACE = 1,
	SCENE_GROUND = 2
};


///  Collision layers
enum CollisionLayers
{
    CL_CHARACTERS = 1 << 0,
    CL_TERRAIN = 1 << 1,
    CL_STATIC = 1 << 2,
    CL_NOCOLLISION = 1 << 3
};

}
