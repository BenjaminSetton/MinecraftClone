#ifndef _KEY_CODES_H
#define _KEY_CODES_H

struct KeyCode
{
	enum : uint16_t
	{
		// WHITE-SPACE CHARACTERS
		SPACE				= 0x20,
		TAB					= 0x09,

		// NUMBERS
		ZERO				= 0x30,
		ONE					= 0x31,
		TWO					= 0x32,
		THREE				= 0x33,
		FOUR				= 0x34,
		FIVE				= 0x35,
		SIX					= 0x36,
		SEVEN				= 0x37,
		EIGHT				= 0x38,
		NINE				= 0x39,

		// LETTERS
		A					= 0x41,
		B					= 0x42,
		C					= 0x43,
		D					= 0x44,
		E					= 0x45,
		F					= 0x46,
		G					= 0x47,
		H					= 0x48,
		I					= 0x49,
		J					= 0x4A,
		K					= 0x4B,
		L					= 0x4C,
		M					= 0x4D,
		N					= 0x4E,
		O					= 0x4F,
		P					= 0x50,
		Q					= 0x51,
		R					= 0x52,
		S					= 0x53,
		T					= 0x54,
		U					= 0x55,
		V					= 0x56,
		W					= 0x57,
		X					= 0x58,
		Y					= 0x59,
		Z					= 0x5A,

		// OTHER KEYS
		UP					= 0x26,
		DOWN				= 0x28,
		LEFT				= 0x25,
		RIGHT				= 0x27,

		LSHIFT				= 0xA0,
		RSHIFT				= 0xA1,
		LCONTROL			= 0xA2,
		RCONTROL			= 0xA3
	};
};

struct MouseCode
{
	enum : uint16_t
	{
		CONTROL = MK_CONTROL,
		SHIFT = MK_SHIFT,
		LBUTTON = MK_LBUTTON,
		RBUTTON = MK_RBUTTON,
		MBUTTON = MK_MBUTTON,
	};
};

#endif