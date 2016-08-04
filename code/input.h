#pragma once

enum Controllers
{
	INPUT_CONTROLLER_1 = 0,
	INPUT_CONTROLLER_2 = 1,
	INPUT_CONTROLLER_3 = 2,
	INPUT_CONTROLLER_4 = 3,
	
	INPUT_MAX_CONTROLLERS = 4,		
};
 
enum MouseButton
{
	INPUT_MOUSE_BUTTON_UNKNOWN = 0,
	INPUT_MOUSE_BUTTON_LEFT = 1,
	INPUT_MOUSE_BUTTON_MIDDLE = 2,
	INPUT_MOUSE_BUTTON_RIGHT = 3,
	INPUT_MOUSE_WHEEL_UP = 4,
	INPUT_MOUSE_WHEEL_DOWN = 5,
	
	INPUT_NUM_MOUSEBUTTONS = 32
};


struct Keyboard;
struct Mouse;

typedef struct Controller
{
	i16 stickX;
	i16 stickY;
	u8 aButton;
	u8 invalid;
	u8 bButton;
	u8 xButton;
	u8 yButton;	
	u8 back;
	u8 guide;
	u8 start;
	u8 leftStick;
	u8 rightStick;	
	u8 leftShoulder;
	u8 rightShoulder;
	u8 up;
	u8 down;
	u8 left;
	u8 right;
} Controller;

typedef struct Input
{
	/* Keyboard */
	u8* keys;
	u8* keysDown;
	u8* keysUp;
	int numKeys;
	/* Mouse */
	u8 mouseButtons[INPUT_NUM_MOUSEBUTTONS];
	u8 mouseButtonsDown[INPUT_NUM_MOUSEBUTTONS];
	u8 mouseButtonsUp[INPUT_NUM_MOUSEBUTTONS];		
	i32 mouseRelativeX;
	i32 mouseRelativeY;
	i32 mouseX;
	i32 mouseY;
	i32 mouseScrollX;
	i32 mouseScrollY;
	/* Controller */
	Controller controllers[INPUT_MAX_CONTROLLERS];	
} Input;

Input CreateInput(u8* keys, int numKeys)
{
	Input input;
	input.keys = keys;
	input.numKeys = numKeys;
	numKeys *= sizeof(u8);
	input.keysDown = (u8*) malloc(numKeys);	
	input.keysUp = (u8*) malloc(numKeys);
	for (int i = 0; i < INPUT_NUM_MOUSEBUTTONS; i++) {
		input.mouseButtons[i] = 0;
	}
	for (int i = 0; i < INPUT_NUM_MOUSEBUTTONS; i++) {
		input.mouseButtonsDown[i] = 0;
	}
	for (int i = 0; i < INPUT_NUM_MOUSEBUTTONS; i++) {
		input.mouseButtonsUp[i] = 0;
	}
	input.mouseRelativeX = 0;
	input.mouseRelativeY = 0;
	input.mouseX = 0;
	input.mouseY = 0;
	input.mouseScrollX = 0;
	input.mouseScrollY = 0;
	
	
	return input;
}

void DestroyInput(Input* input)
{
	free(input->keysUp);
	free(input->keysDown);	
}

void InputInitKeyStates(Input* input, u8* keys)
{
	input->keys = keys;
}

void InputSetRelativeMouseMotion(Input* input, i32 x, i32 y)
{
	input->mouseRelativeX = x;
	input->mouseRelativeY = y;
}

void InputSetMousePosition(Input* input, i32 x, i32 y)
{
	input->mouseX = x;
	input->mouseY = y;
}

void InputAddMouseScroll(Input* input, i32 x, i32 y)
{
	input->mouseScrollX += x;	
	input->mouseScrollY += y;
}

void InputResetMouseScroll(Input* input)
{
	input->mouseScrollX = 0;	
	input->mouseScrollY = 0;
}