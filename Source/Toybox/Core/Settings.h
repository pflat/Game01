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

    //  Spaceship controller map
    KM_VEHICLE_SPEED_INC = 1 << 0,
    KM_VEHICLE_SPEED_DEC = 1 << 1,
    KM_VEHICLE_PITCH_UP = 1 << 2,
    KM_VEHICLE_PITCH_DOWN = 1 << 3,
    KM_VEHICLE_YAW_LEFT = 1 << 4,
    KM_VEHICLE_YAW_RIGHT = 1 << 5,
    KM_VEHICLE_ROLL_LEFT = 1 << 6,
    KM_VEHICLE_ROLL_RIGHT = 1 << 7,
    KM_VEHICLE_SPEED_MAX = 1 << 8,
    KM_VEHICLE_SPEED_STOP = 1 << 9,

	//  Camera controller map
	KM_CAMERA_MODE = 1 << 0,
	KM_CAMERA_RESET = 1 << 1,
	KM_CAMERA_FORWARD = 1 << 2,
	KM_CAMERA_BACKWARD = 1 << 3,
	KM_CAMERA_PAN_LEFT = 1 << 4,
	KM_CAMERA_PAN_RIGHT = 1 << 5,
	KM_CAMERA_ROLL_LEFT = 1 << 6,
	KM_CAMERA_ROLL_RIGHT = 1 << 7
};


///  Mouse buttons map
enum MouseMap
{
	MM_BUTTON_LEFT = 1 << 0,
	MM_BUTTON_MIDDLE = 1 << 1,
	MM_BUTTON_RIGHT = 1 << 2
};


enum CameraCtrlType
{
	//    CAMERA_MODE_FREE = 1,
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


//  Settings - DEBUG -


}
