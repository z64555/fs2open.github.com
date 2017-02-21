/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 




#include "controlconfig/controlsconfig.h"
#include "debugconsole/console.h"
#include "freespace.h"
#include "gamehelp/contexthelp.h"
#include "gamesequence/gamesequence.h"
#include "gamesnd/gamesnd.h"
#include "globalincs/alphacolors.h"
#include "globalincs/undosys.h"
#include "graphics/font.h"
#include "hud/hudsquadmsg.h"
#include "io/joy.h"
#include "io/key.h"
#include "io/timer.h"
#include "missionui/missionscreencommon.h"
#include "network/multi_pmsg.h"
#include "network/multiutil.h"
#include "scripting/scripting.h"
#include "pilotfile/pilotfile.h"
#include "popup/popup.h"
#include "ui/ui.h"
#include "ui/uidefs.h"

#include <algorithm>

#ifndef NDEBUG
#include "hud/hud.h"
#endif

using namespace std;

#define NUM_SYSTEM_KEYS			14
#define NUM_BUTTONS				19
#define NUM_TABS				4

// coordinate indicies
#define CONTROL_X_COORD 0
#define CONTROL_Y_COORD 1
#define CONTROL_W_COORD 2
#define CONTROL_H_COORD 3

const char* Conflict_background_bitmap_fname[GR_NUM_RESOLUTIONS] = {
	"ControlConfig",		// GR_640
	"2_ControlConfig"		// GR_1024
};

const char* Conflict_background_bitmap_mask_fname[GR_NUM_RESOLUTIONS] = {
	"ControlConfig-m",		// GR_640
	"2_ControlConfig-m"		// GR_1024
};

// control list area
int Control_list_coords[GR_NUM_RESOLUTIONS][4] = {
	{
		32, 58, 198, 259	// GR_640
	},
	{
		32, 94, 904, 424	// GR_1024
	}
};

// width of the control name section of the list
int Control_list_ctrl_w[GR_NUM_RESOLUTIONS] = {
	350,	// GR_640
	600		// GR_1024
};

// x start position of the binding area section of the list
int Control_list_key_x[GR_NUM_RESOLUTIONS] = {
	397,	// GR_640
	712		// GR_1024
};

// width of the binding area section of the list
int Control_list_key_w[GR_NUM_RESOLUTIONS] = {
	198,	// GR_640
	230		// GR_1024
};

// display the "more..." text under the control list
int Control_more_coords[GR_NUM_RESOLUTIONS][2] = {
	{
		320, 326			// GR_640
	},
	{
		500, 542			// GR_1024
	}
};

// area to display "conflicts with..." text
int Conflict_wnd_coords[GR_NUM_RESOLUTIONS][4] = {
	{
		32, 313, 250, 32	// GR_640
	},
	{
		48, 508, 354, 46	// GR_1024
	}
};

// conflict warning anim coords
int Conflict_warning_coords[GR_NUM_RESOLUTIONS][2] = {
	{
		-1, 420			// GR_640
	},
	{
		-1, 669			// GR_1024
	}
};

// for flashing the conflict text
#define CONFLICT_FLASH_TIME	250
int Conflict_stamp = -1;
int Conflict_bright = 0;

#define LIST_BUTTONS_MAX	42
#define JOY_AXIS			0x80000

static int Num_cc_lines;
static struct {
	const char *label;
	int cc_index;  // index into Control_config of item
	int y;  // Y coordinate of line
	int kx, kw, jx, jw;  // x start and width of keyboard and joystick bound text
} Cc_lines[CCFG_MAX];

Undo_system Undo_controls;

Control_LUT Control_config_backup;

int Axis_map_to[] = { JOY_X_AXIS, JOY_Y_AXIS, JOY_RX_AXIS, -1, -1 };
int Axis_map_to_defaults[] = { JOY_X_AXIS, JOY_Y_AXIS, JOY_RX_AXIS, -1, -1 };

// all this stuff is localized/externalized
#define NUM_AXIS_TEXT			6
#define NUM_MOUSE_TEXT			5
#define NUM_MOUSE_AXIS_TEXT		2
#define NUM_INVERT_TEXT			2	
char *Joy_axis_action_text[NUM_JOY_AXIS_ACTIONS];
char *Joy_axis_text[NUM_AXIS_TEXT];
char *Mouse_button_text[NUM_MOUSE_TEXT];
char *Mouse_axis_text[NUM_MOUSE_AXIS_TEXT];
char *Invert_text[NUM_INVERT_TEXT];

ubyte System_keys[NUM_SYSTEM_KEYS] = {
	KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	KEY_F11, KEY_F12, KEY_PRINT_SCRN
};

int Control_check_count = 0;

static int Tab;  // which tab we are currently in
static int Binding_mode = 0;  // are we waiting for a key to bind it?
static int Bind_time = 0;
static int Search_mode = 0;  // are we waiting for a key to bind it?
static int Last_key = -1;
static int Selected_line = 0;  // line that is currently selected for binding
static int Selected_item = -1;  // -1 = none, 0 = key, 1 = button
static int Scroll_offset;
static int Axis_override = -1;
static int Background_bitmap;
static int Conflicts_tabs[NUM_TABS];
static UI_BUTTON List_buttons[LIST_BUTTONS_MAX];  // buttons for each line of text in list
static UI_WINDOW Ui_window;
static unsigned int Defaults_cycle_pos; // the controls preset that was last selected

int Control_config_overlay_id;

struct Conflict_Action {
	pair<int, short> other_id[MAX_BINDINGS];	// Array of Control_LUT indices that this IoAction has a conflict with, and the controller Id (CID_KEYBOARD etc)

	Conflict_Action() {
		clear();
	}

	/*!
	 * Clears this entry
	 */
	void clear() {
		fill(other_id, other_id + MAX_BINDINGS, pair<int, short>(-1, -1));
	}

	/*!
	 * @brief Checks if any conflicts exist for this IoAction 
	 *
	 * @param[in] cr_id Optional. If provided, check for conflicts with this controller id (CID_KEYBOARD etc)
	 */
	bool has_conflict(short cr_id = -1) {
		for (int i = 0; i < MAX_BINDINGS; ++i) {
			if (other_id[i].first != -1) {
				if ((cr_id == -1) || (cr_id == other_id[i].second)) {
					return true;
				}
			}
		}
		return false;
	}
};


static SCP_vector<Conflict_Action> Conflicts(CCFG_MAX);
int Conflicts_axes[NUM_JOY_AXIS_ACTIONS];

#define TARGET_TAB				0
#define SHIP_TAB				1
#define WEAPON_TAB				2
#define COMPUTER_TAB			3
#define SCROLL_UP_BUTTON		4
#define SCROLL_DOWN_BUTTON		5
#define ALT_TOGGLE				6
#define SHIFT_TOGGLE			7
#define INVERT_AXIS				8
#define CANCEL_BUTTON			9
#define UNDO_BUTTON				10
#define RESET_BUTTON			11
#define SEARCH_MODE				12
#define BIND_BUTTON				13
#define HELP_BUTTON				14
#define ACCEPT_BUTTON			15
#define CLEAR_OTHER_BUTTON		16
#define CLEAR_ALL_BUTTON		17
#define CLEAR_BUTTON			18

ui_button_info CC_Buttons[GR_NUM_RESOLUTIONS][NUM_BUTTONS] = {
	{ // GR_640
		ui_button_info("CCB_00",	32,	348,	17,	384,	0),	// target tab
		ui_button_info("CCB_01",	101,	348,	103,	384,	1),	// ship tab
		ui_button_info("CCB_02",	173,	352,	154,	384,	2),	// weapon tab
		ui_button_info("CCB_03",	242,	347,	244,	384,	3),	// computer/misc tab
		ui_button_info("CCB_04",	614,	73,	-1,	-1,	4),	// scroll up
		ui_button_info("CCB_05",	614,	296,	-1,	-1,	5),	// scroll down
		ui_button_info("CCB_06",	17,	452,	12,	440,	6),	// alt toggle
		ui_button_info("CCB_07",	56,	452,	50,	440,	7),	// shift toggle
		ui_button_info("CCB_09",	162,	452,	155,	440,	9),	// invert
		ui_button_info("CCB_10",	404,	1,		397,	45,	10),	// cancel
		ui_button_info("CCB_11",	582,	347,	586,	386,	11),	// undo
		ui_button_info("CCB_12",	576,	1,		578,	45,	12),	// default
		ui_button_info("CCB_13",	457,	4,		453,	45,	13),	// search
		ui_button_info("CCB_14",	516,	4,		519,	45,	14),	// bind
		ui_button_info("CCB_15",	540,	428,	500,	440,	15),	// help
		ui_button_info("CCB_16",	574,	432,	571,	412,	16),	// accept
		ui_button_info("CCB_18",	420,	346,	417,	386,	18),	// clear other 
		ui_button_info("CCB_19",	476,	346,	474,	386,	19),	// clear all
		ui_button_info("CCB_20",	524,	346,	529,	386,	20),	// clear button
	},
	{ // GR_1024
		ui_button_info("2_CCB_00",	51,	557,	27,	615,	0),	// target tab
		ui_button_info("2_CCB_01",	162,	557,	166,	615,	1),	// ship tab
		ui_button_info("2_CCB_02",	277,	563,	246,	615,	2),	// weapon tab
		ui_button_info("2_CCB_03",	388,	555,	391,	615,	3),	// computer/misc tab
		ui_button_info("2_CCB_04",	982,	117,	-1,	-1,	4),	// scroll up
		ui_button_info("2_CCB_05",	982,	474,	-1,	-1,	5),	// scroll down
		ui_button_info("2_CCB_06",	28,	723,	24,	704,	6),	// alt toggle
		ui_button_info("2_CCB_07",	89,	723,	80,	704,	7),	// shift toggle
		ui_button_info("2_CCB_09",	260,	723,	249,	704,	9),	// invert
		ui_button_info("2_CCB_10",	646,	2,		635,	71,	10),	// cancel
		ui_button_info("2_CCB_11",	932,	555,	938,	619,	11),	// undo
		ui_button_info("2_CCB_12",	921,	1,		923,	71,	12),	// default
		ui_button_info("2_CCB_13",	732,	6,		726,	71,	13),	// search
		ui_button_info("2_CCB_14",	825,	6,		831,	71,	14),	// bind
		ui_button_info("2_CCB_15",	864,	685,	800,	704,	15),	// help
		ui_button_info("2_CCB_16",	919,	692,	914,	660,	16),	// accept
		ui_button_info("2_CCB_18",	672,	553,	668,	619,	18),	// clear other 
		ui_button_info("2_CCB_19",	761,	553,	749,	619,	19),	// clear all
		ui_button_info("2_CCB_20",	838,	553,	846,	619,	20),	// clear button
	}
};

// strings
#define CC_NUM_TEXT		20
UI_XSTR CC_text[GR_NUM_RESOLUTIONS][CC_NUM_TEXT] = {
	{ // GR_640
		{ "Targeting",		1340,		17,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][TARGET_TAB].button },
		{ "Ship",			1341,		103,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][SHIP_TAB].button },
		{ "Weapons",		1065,		154,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][WEAPON_TAB].button },
		{ "Misc",			1411,		244,	384,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][COMPUTER_TAB].button },		
		{ "Alt",				1510,		12,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][ALT_TOGGLE].button },
		{ "Shift",			1511,		50,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][SHIFT_TOGGLE].button },
		{ "Invert",			1342,		155,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][INVERT_AXIS].button },
		{ "Cancel",			641,		397,	45,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][CANCEL_BUTTON].button },
		{ "Undo",			1343,		586,	386,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][UNDO_BUTTON].button },
		{ "Defaults",		1344,		568,	45,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][RESET_BUTTON].button },
		{ "Search",			1345,		453,	45,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][SEARCH_MODE].button },
		{ "Bind",			1346,		519,	45,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][BIND_BUTTON].button },
		{ "Help",			928,		500,	440,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][HELP_BUTTON].button },
		{ "Accept",			1035,		571,	412,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][ACCEPT_BUTTON].button },
		{ "Clear",			1347,		417,	386,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_OTHER_BUTTON].button },
		{ "Conflict",		1348,		406,	396,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_OTHER_BUTTON].button },
		{ "Clear",			1413,		474,	386,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_ALL_BUTTON].button },
		{ "All",				1349,		483,	396,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[0][CLEAR_ALL_BUTTON].button },
		{ "Clear",			1414,		529,	388,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][CLEAR_BUTTON].button },
		{ "Selected",		1350,		517,	396,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[0][CLEAR_BUTTON].button },
	},
	{ // GR_1024
		{ "Targeting",		1340,		47,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][TARGET_TAB].button },
		{ "Ship",			1341,		176,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][SHIP_TAB].button },
		{ "Weapons",		1065,		266,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][WEAPON_TAB].button },
		{ "Misc",			1411,		401,	615,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][COMPUTER_TAB].button },		
		{ "Alt",				1510,		29,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][ALT_TOGGLE].button },
		{ "Shift",			1511,		85,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][SHIFT_TOGGLE].button },
		{ "Invert",			1342,		254,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][INVERT_AXIS].button },
		{ "Cancel",			641,		655,	71,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][CANCEL_BUTTON].button },
		{ "Undo",			1343,		938,	619,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][UNDO_BUTTON].button },
		{ "Defaults",		1344,		923,	71,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][RESET_BUTTON].button },
		{ "Search",			1345,		746,	71,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][SEARCH_MODE].button },
		{ "Bind",			1346,		846,	71,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][BIND_BUTTON].button },
		{ "Help",			928,		800,	704,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][HELP_BUTTON].button },
		{ "Accept",			1035,		914,	660,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][ACCEPT_BUTTON].button },
		{ "Clear",			1347,		683,	619,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_OTHER_BUTTON].button },
		{ "Conflict",		1348,		666,	634,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_OTHER_BUTTON].button },
		{ "Clear",			1413,		759,	619,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_ALL_BUTTON].button },
		{ "All",				1349,		772,	634,	UI_XSTR_COLOR_GREEN, -1, &CC_Buttons[1][CLEAR_ALL_BUTTON].button },
		{ "Clear",			1414,		871,	619,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][CLEAR_BUTTON].button },
		{ "Selected",		1350,		852,	634,	UI_XSTR_COLOR_PINK, -1, &CC_Buttons[1][CLEAR_BUTTON].button },
	}
};


// same indices as Scan_code_text[].  Indicates if a scancode is allowed to be bound.
int Config_allowed[] = {
	0, 0, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,

	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 1, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 0, 1, 0, 1, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};

#ifndef NDEBUG
int Show_controls_info = 0;

DCF_BOOL(show_controls_info, Show_controls_info);
#endif

static int Axes_origin[JOY_NUM_AXES];

static int joy_get_unscaled_reading(int raw, int axn)
{
	int rng;

	rng = JOY_AXIS_MAX - JOY_AXIS_MIN;
	raw -= JOY_AXIS_MIN;  // adjust for linear range starting at 0

	// cap at limits
	if (raw < 0)
		raw = 0;
	if (raw > rng)
		raw = rng;

	return (int) ((std::uint64_t) raw * (std::uint64_t) F1_0 / (std::uint64_t) rng);  // convert to 0 - F1_0 range, 64bit ints used to avoid uint overflow
}

int joy_get_scaled_reading(int raw)
{
	int x, d, dead_zone, rng;
	float percent, sensitivity_percent, non_sensitivity_percent;

	raw -= JOY_AXIS_CENTER;

	dead_zone = (JOY_AXIS_MAX - JOY_AXIS_MIN) * Joy_dead_zone_size / 100;

	if (raw < -dead_zone) {
		rng = JOY_AXIS_CENTER - JOY_AXIS_MIN - dead_zone;
		d = -raw - dead_zone;
	} else if (raw > dead_zone) {
		rng = JOY_AXIS_MAX - JOY_AXIS_CENTER - dead_zone;
		d = raw - dead_zone;
	} else
		return 0;

	if (d > rng)
		d = rng;

	Assert(Joy_sensitivity >= 0 && Joy_sensitivity <= 9);

	// compute percentages as a range between 0 and 1
	sensitivity_percent = (float) Joy_sensitivity / 9.0f;
	non_sensitivity_percent = (float) (9 - Joy_sensitivity) / 9.0f;

	// find percent of max axis is at
	percent = (float) d / (float) rng;

	// work sensitivity on axis value
	percent = (percent * sensitivity_percent + percent * percent * percent * percent * percent * non_sensitivity_percent);

	x = (int) ((float) F1_0 * percent);

	//nprintf(("AI", "d=%6i, sens=%3i, percent=%6.3f, val=%6i, ratio=%6.3f\n", d, Joy_sensitivity, percent, (raw<0) ? -x : x, (float) d/x));

	if (raw < 0)
		return -x;

	return x;
}

void control_config_detect_axis_reset()
{
	joystick_read_raw_axis(JOY_NUM_AXES, Axes_origin);
}

int control_config_detect_axis()
{
	int i, d, axis = -1, delta = 16384;
	int axes_values[JOY_NUM_AXES];
	int dx, dy, dz, fudge = 7;

	joystick_read_raw_axis(JOY_NUM_AXES, axes_values);
	for (i=0; i<JOY_NUM_AXES; i++) {
		d = abs(axes_values[i] - Axes_origin[i]);
		if (d > delta) {
			axis = i;
			delta = d;
		}
	}

	if ( (axis == -1) && Use_mouse_to_fly ) {
		mouse_get_delta( &dx, &dy, &dz );

		if ( (dx > fudge) || (dx < -fudge) ) {
			axis = 0;
		} else if ( (dy > fudge) || (dy < -fudge) ) {
			axis = 1;
		} else if ( (dz > fudge) || (dz < -fudge) ) {
			axis = 2;
		}
	}
		
	return axis;
}


void control_config_conflict_check()
{
	int i;	// Index of the first conflicted action.
	int j;	// Index of the second conflicted action
	int k;	// Index of the first conflicted binding
	int l;	// Index of the second conflicted binding

	for (auto it = Conflicts.begin(); it != Conflicts.end(); ++it) {
		it->clear();
	}

	for (i=0; i<NUM_TABS; i++) {
		Conflicts_tabs[i] = 0;
	}

	for (i = 0; i < Control_config.size() - 1; ++i) {
		// Skip first if disabled
		if (Control_config[i].disabled) {
			continue;
		}

		for (j = i + 1; j < Control_config.size(); ++j) {
			// Skip second if disabled
			if (Control_config[j].disabled) {
				continue;
			}

			auto &first = Control_config[i].c_id;
			auto &second = Control_config[j].c_id;

			// z64: Disabled binds are skipped now, so we don't have to worry about any "false" conflicts anymore
			// TODO: Refactor this crap, z64!
			for (k = 0; k < MAX_BINDINGS; ++k) {
				// Find shared bindings
				if (first[k].first < 0) {
					// check next one
					continue;
				}

				for (l = 0; l < MAX_BINDINGS; ++l) {
					if (second[l].first < 0) {
						// check next one
						continue;
					}

					if (first[k] != second[l]) {
						// No conflict, check next one
						continue;
					} // Else, Conflict found!

					Conflicts[i].other_id[k] = pair<int, short>(j, first[k].first);
					Conflicts[j].other_id[l] = pair<int, short>(i, second[k].first);	// Technically the same thing as first[k].first. Hopefully this is less confusing.

					auto &first_tab = Control_config[i].tab;
					auto &second_tab = Control_config[j].tab;
					Conflicts_tabs[first_tab] = 1;
					Conflicts_tabs[second_tab] = 1;
				}
			}
		}
	}
		

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++) {
		Conflicts_axes[i] = -1;
	}

	for (i=0; i<NUM_JOY_AXIS_ACTIONS-1; i++) {
		for (j=i+1; j<NUM_JOY_AXIS_ACTIONS; j++) {
			if ((Axis_map_to[i] >= 0) && (Axis_map_to[i] == Axis_map_to[j])) {
				Conflicts_axes[i] = j;
				Conflicts_axes[j] = i;
				Conflicts_tabs[SHIP_TAB] = 1;
 			}
		}
	}
}

// do list setup required prior to rendering and checking for the controls listing.  Called when list changes
void control_config_list_prepare()
{
	int j, y, z;
	int font_height = gr_get_font_height();

	Num_cc_lines = y = z = 0;
	while (z < CCFG_MAX) {
		if (Control_config[z].tab == Tab && !Control_config[z].disabled) {
			if (Control_config[z].hasXSTR) {
				Cc_lines[Num_cc_lines].label = XSTR(Control_config[z].text, CONTROL_CONFIG_XSTR + z);
			} else {
				Cc_lines[Num_cc_lines].label = Control_config[z].text;
			}

			Cc_lines[Num_cc_lines].cc_index = z;
			Cc_lines[Num_cc_lines++].y = y;
			y += font_height + 2;
		}

		z++;
	}

	if (Tab == SHIP_TAB) {
		for (j=0; j<NUM_JOY_AXIS_ACTIONS; j++) {
			Cc_lines[Num_cc_lines].label = Joy_axis_action_text[j];
			Cc_lines[Num_cc_lines].cc_index = j | JOY_AXIS;
			Cc_lines[Num_cc_lines++].y = y;
			y += font_height + 2;
		}
	}
}

int cc_line_query_visible(int n)
{
	int y;

	if ((n < 0) || (n >= Num_cc_lines)) {
		return 0;
	}
	
	y = Cc_lines[n].y - Cc_lines[Scroll_offset].y;
	if ((y < 0) || (y + gr_get_font_height() > Control_list_coords[gr_screen.res][CONTROL_H_COORD])){
		return 0;
	}

	return 1;
}


// undo the most recent binding changes
int control_config_undo_last()
{
	if (Undo_controls.size() == 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	pair<const void*, const void*> _item = Undo_controls.undo();

	if (_item.second == &Control_config[0]) {
		// Is a button
		const Config_item* item = static_cast<const Config_item*>(_item.second);

		Tab = item->tab;
		// TODO: Find the selected line and item this op is for
	} else if (_item.second == &Axis_map_to[0]) {
		// Is an axis

		Tab = SHIP_TAB;
		// TODO: Find the selected line and item this op is for
	} else {
		// Can't happen!
		Int3();
	}

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_USER_SELECT);
	return 0;
}

void control_config_bind_axis(int a, int axis)
{
	Undo_controls.save(a, &Axis_map_to[0]);
	Axis_map_to[a] = axis;
}

void control_config_bind_key(int i, int key)
{
	Undo_controls.save(Control_config[i], &Control_config[0]);
	Control_config[i].bind(cid(CID_KEYBOARD, key));
}

void control_config_bind_joy(int i, int joy)
{
	Undo_controls.save(Control_config[i], &Control_config[0]);
	Control_config[i].bind(cid(CID_JOY, joy));
}

int control_config_remove_binding()
{
	int z;

	if (Selected_line < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	z = Cc_lines[Selected_line].cc_index;
	if (z & JOY_AXIS) {
		z &= ~JOY_AXIS;
		if (Axis_map_to[z] < 0) {
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		Undo_controls.save(Axis_map_to[z], Axis_map_to);
		Axis_map_to[z] = -1;
		control_config_conflict_check();
		control_config_list_prepare();
		gamesnd_play_iface(SND_USER_SELECT);
		Selected_item = -1;
		return 0;
	}

	if (Control_config[z].empty()) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	Undo_controls.save(Control_config[z], &Control_config[0]);
	Control_config[z].unbind(Selected_item);

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_USER_SELECT);
	Selected_item = -1;
	return 0;
}

int control_config_clear_other()
{
	int z, i, j, total = 0;

	if (Selected_line < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	z = Cc_lines[Selected_line].cc_index;
	if (z & JOY_AXIS) {
		z &= ~JOY_AXIS;
		if (Axis_map_to[z] < 0) {
			// Nothing to work on!
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		// Count number of 'others'
		for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++) {
			if ((Axis_map_to[i] == Axis_map_to[z]) && (i != z)) {
				total++;
			}
		}

		if (!total) {
			// There are no others!
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		Undo_stack Stack;
		for (i = 0; i < NUM_JOY_AXIS_ACTIONS; i++) {
			if ((Axis_map_to[i] == Axis_map_to[z]) && (i != z)) {
				Stack.save(Axis_map_to[i], Axis_map_to);
				Axis_map_to[i] = -1;
			}
		}
		Undo_controls.save_stack(Stack);

	} else {

		if (Control_config[z].empty()) {
			// Nothing to work on!
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		// Count number of 'others'
		for (i = 0; i < CCFG_MAX; i++) {
			if (i == z) {
				continue;
			}

			for (j = 0; j < MAX_BINDINGS; ++j) {
				if (Control_config[i].find_bind(Control_config[z].c_id[j]) >= 0) {
					total++;
				}
			}
		}

		if (!total) {
			// There are no others!
			gamesnd_play_iface(SND_GENERAL_FAIL);
			return -1;
		}

		Undo_stack Stack;
		Stack.reserve(total);
		for (i = 0; i < CCFG_MAX; ++i) {
			int id = -1;

			// First, check if this action has any common bindings
			for (j = 0; j < MAX_BINDINGS; ++j) {
				id = Control_config[i].find_bind(Control_config[z].c_id[j]);

				if (id >= 0) {
					// Found a binding!
					break;
				}
			}

			// If it does, then save its current state, and unbind all matches
			if (id >= 0) {
				Stack.save(Control_config[i], &Control_config[0]);

				for (j = 0; j < MAX_BINDINGS; ++j) {
					Control_config[i].unbind(Control_config[z].c_id[j]);
				}
				Control_config[i].cleanup();
			}
		}
		Undo_controls.save_stack(Stack);
	}

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_USER_SELECT);
	return 0;
}

int control_config_clear_all()
{
	int i, j, total = 0;

	// First, determine if anything needs to be cleared
	for (i = 0; i < CCFG_MAX; ++i) {
		if (!Control_config[i].empty()) {
			total++;
		}
	}

	if (!total) {
		// Nothing to clear...
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	// Save non-empty actions to the undo system, and clear them
	Undo_stack Stack;
	Stack.reserve(total);
	for (i = 0; i < CCFG_MAX; ++i) {
		if (!Control_config[i].empty()) {
			Stack.save(Control_config[i], &Control_config[0]);
			Control_config[i].clear();
		}
	}
	Undo_controls.save_stack(Stack);

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_RESET_PRESSED);
	return 0;
}

int control_config_axis_default(int axis)
{
	Assert(axis >= 0);

	if ( axis > 1 ) {
		if (Axis_map_to_defaults[axis] < 0) {
			return -1;
		}

		auto joystick = io::joystick::getCurrentJoystick();
		if (joystick == nullptr || Axis_map_to_defaults[axis] >= joystick->numAxes()) {
			return -1;
		}
	}

	return Axis_map_to_defaults[axis];
}

int control_config_do_reset()
{
	int i, j, total = 0;
	Config_item_preset* preset;
	bool cycling_presets = false;
	
	// If there are presets, then we'll cycle to the next preset and reset to that
	if (!Control_config_presets.empty()) {
		cycling_presets = true;
		
		if (++Defaults_cycle_pos >= Control_config_presets.size())
			Defaults_cycle_pos = 0;
		
		preset = &Control_config_presets[Defaults_cycle_pos].action[0];
	} else {
		// If there are no presets, then we'll always reset to the hardcoded defaults
		preset = &Control_config[0];
	}
	
	// first, determine how many bindings need to be changed
	for (i = 0; i < CCFG_MAX; ++i) {
		for (j = 0; j < MAX_BINDINGS; ++j) {
			if (Control_config[i].c_id[j] != preset[i].default_id[j]) {
				total++;
				break;
			}
		}
	}

	// TODO: Axes only have one preset, ever!
	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++) {
		if ((Axis_map_to[i] != control_config_axis_default(i)) || (Invert_axis[i] != Invert_axis_defaults[i])) {
			total++;
		}
	}

	if (!total && !cycling_presets) {
		// Fail if there's nothing to change, and we're not cycling presets
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	// now, back up the old bindings so we can undo if we want to
	Undo_stack Stack;
	Stack.reserve(total);
	for (i = 0; i < CCFG_MAX; ++i) {
		for (j = 0; j < MAX_BINDINGS; ++j) {
			if (Control_config[i].c_id[j] != preset[i].default_id[j]) {
				Stack.save(Control_config[i], &Control_config[0]);
				break;
			}
		}
	}

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++) {
		if (Axis_map_to[i] != control_config_axis_default(i)) {
			Stack.save(Axis_map_to[i], Axis_map_to);
		}

		if (Invert_axis[i] != Invert_axis_defaults[i]) {
			Stack.save(Invert_axis[i], Invert_axis);
		}
	}
	Undo_controls.save_stack(Stack);

	Assert(j == total);

	// Now, actually do the resetting
	if (cycling_presets)
		control_config_reset_defaults(Defaults_cycle_pos);
	else
		control_config_reset_defaults();

	control_config_conflict_check();
	control_config_list_prepare();
	gamesnd_play_iface(SND_RESET_PRESSED);
	return 0;
}

void control_config_reset_defaults(int presetnum)
{
	int i;
	Config_item_preset *preset;

	if (presetnum >= 0)
		preset = &Control_config_presets[presetnum].action[0];
	else
		preset = &Control_config[0];

	// Reset keyboard defaults
	for (i=0; i<CCFG_MAX; i++) {
		// Note that key_default and joy_default are NOT overwritten here;
		// they should retain the values of the first preset because
		// for example the key-pressed SEXP works off the defaults of the first preset
		Control_config[i].c_id[0] = preset[i].default_id[0];
		Control_config[i].c_id[1] = preset[i].default_id[1];
		Control_config[i].c_id[2] = preset[i].default_id[2];
	}

	for (i=0; i<NUM_JOY_AXIS_ACTIONS; i++) {
		Axis_map_to[i] = control_config_axis_default(i);
		Invert_axis[i] = Invert_axis_defaults[i];
	}
}

void control_config_scroll_screen_up()
{
	if (Scroll_offset) {
		Scroll_offset--;
		Assert(Selected_line > Scroll_offset);
		while (!cc_line_query_visible(Selected_line)) {
			Selected_line--;
		}

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
}

void control_config_scroll_line_up()
{
	if (Selected_line) {
		Selected_line--;
		if (Selected_line < Scroll_offset) {
			Scroll_offset = Selected_line;
		}

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
}

void control_config_scroll_screen_down()
{
	if (Cc_lines[Num_cc_lines - 1].y + gr_get_font_height() > Cc_lines[Scroll_offset].y + Control_list_coords[gr_screen.res][CONTROL_H_COORD]) {
		Scroll_offset++;
		while (!cc_line_query_visible(Selected_line)) {
			Selected_line++;
			Assert(Selected_line < Num_cc_lines);
		}

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
}

void control_config_scroll_line_down()
{
	if (Selected_line < Num_cc_lines - 1) {
		Selected_line++;
		Assert(Selected_line > Scroll_offset);
		while (!cc_line_query_visible(Selected_line)) {
			Scroll_offset++;
		}

		Selected_item = -1;
		gamesnd_play_iface(SND_SCROLL);

	} else {
		gamesnd_play_iface(SND_GENERAL_FAIL);
	}
}

void control_config_toggle_modifier(int bit)
{
	int k, z;

	z = Cc_lines[Selected_line].cc_index;
	Assert(!(z & JOY_AXIS));
	k = Control_config[z].find_bind(cid(CID_KEYBOARD, -1));
	if (k < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	control_config_bind_key(z, Control_config[z].c_id[k].second ^ bit);
	control_config_conflict_check();
	gamesnd_play_iface(SND_USER_SELECT);
}

void control_config_toggle_invert()
{
	int z;

	z = Cc_lines[Selected_line].cc_index;
	Assert(z & JOY_AXIS);
	z &= ~JOY_AXIS;
	Undo_controls.save(Invert_axis[z], Invert_axis);
	Invert_axis[z] = !Invert_axis[z];
}

void control_config_do_bind()
{
	int i;

	game_flush();
//	if ((Selected_line < 0) || (Cc_lines[Selected_line].cc_index & JOY_AXIS)) {
	if (Selected_line < 0) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return;
	}

	for (i=0; i<NUM_BUTTONS; i++) {
		if (i != CANCEL_BUTTON) {
			CC_Buttons[gr_screen.res][i].button.reset_status();
			CC_Buttons[gr_screen.res][i].button.disable();
		}
	}
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.enable();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.set_hotkey(KEY_ESC);

	for (i=0; i<JOY_TOTAL_BUTTONS; i++){
		joy_down_count(i, 1);  // clear checking status of all joystick buttons
	}

	control_config_detect_axis_reset();

	Binding_mode = 1;
	Bind_time = timer_get_milliseconds();
	Search_mode = 0;
	Last_key = -1;
	Axis_override = -1;
	gamesnd_play_iface(SND_USER_SELECT);
}

void control_config_do_search()
{
	int i;

	for (i=0; i<NUM_BUTTONS; i++){
		if (i != CANCEL_BUTTON) {
			CC_Buttons[gr_screen.res][i].button.reset_status();
			CC_Buttons[gr_screen.res][i].button.disable();
		}
	}

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.enable();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.set_hotkey(KEY_ESC);

	for (i=0; i<JOY_TOTAL_BUTTONS; i++){
		joy_down_count(i, 1);  // clear checking status of all joystick buttons
	}

	Binding_mode = 0;
	Search_mode = 1;
	Last_key = -1;
	gamesnd_play_iface(SND_USER_SELECT);
}

void control_config_do_cancel(int fail = 0)
{
	int i;

	game_flush();

	for (i=0; i<NUM_BUTTONS; i++){
		if ( (i != CANCEL_BUTTON) && (i != INVERT_AXIS) ){
			CC_Buttons[gr_screen.res][i].button.enable();
		}
	}

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.reset_status();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.disable();
	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.set_hotkey(-1);
	CC_Buttons[gr_screen.res][BIND_BUTTON].button.reset_status();
	CC_Buttons[gr_screen.res][SEARCH_MODE].button.reset_status();

	Binding_mode = Search_mode = 0;
	if (fail){
		gamesnd_play_iface(SND_GENERAL_FAIL);
	} else {
		gamesnd_play_iface(SND_USER_SELECT);
	}
}

int control_config_accept()
{
	int i;

	for (i=0; i<NUM_TABS; i++) {
		if (Conflicts_tabs[i]) {
			break;
		}
	}

	if (i < NUM_TABS) {
		gamesnd_play_iface(SND_GENERAL_FAIL);
		return -1;
	}

	hud_squadmsg_save_keys();  // rebuild map for saving/restoring keys in squadmsg mode
	gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
	gamesnd_play_iface(SND_COMMIT_PRESSED);
	return 0;
}

void control_config_cancel_exit()
{
	int i;

	for (i=0; i<CCFG_MAX; i++) {
		Control_config[i] = Control_config_backup[i];
	}

	gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
}

void control_config_button_pressed(int n)
{
	switch (n) {
		case TARGET_TAB:
		case SHIP_TAB:
		case WEAPON_TAB:
		case COMPUTER_TAB:
			Tab = n;
			Scroll_offset = Selected_line = 0;
			control_config_list_prepare();
			gamesnd_play_iface(SND_SCREEN_MODE_PRESSED);
			break;

		case BIND_BUTTON:
			control_config_do_bind();
			break;

		case SEARCH_MODE:
			control_config_do_search();
			break;

		case SHIFT_TOGGLE:
			control_config_toggle_modifier(KEY_SHIFTED);
			gamesnd_play_iface(SND_USER_SELECT);
			break;

		case ALT_TOGGLE:
			control_config_toggle_modifier(KEY_ALTED);
			gamesnd_play_iface(SND_USER_SELECT);
			break;

		case INVERT_AXIS:
			control_config_toggle_invert();
			gamesnd_play_iface(SND_USER_SELECT);
			break;

		case SCROLL_UP_BUTTON:
			control_config_scroll_screen_up();
			break;

		case SCROLL_DOWN_BUTTON:
			control_config_scroll_screen_down();
			break;

		case ACCEPT_BUTTON:
			control_config_accept();
			break;

		case CLEAR_BUTTON:
			control_config_remove_binding();
			break;

		case HELP_BUTTON:
			launch_context_help();
			gamesnd_play_iface(SND_HELP_PRESSED);
			break;

		case RESET_BUTTON:
			control_config_do_reset();
			break;

		case UNDO_BUTTON:
			control_config_undo_last();
			break;

		case CANCEL_BUTTON:
			control_config_do_cancel();
			break;

		case CLEAR_OTHER_BUTTON:
			control_config_clear_other();
			break;

		case CLEAR_ALL_BUTTON:
			control_config_clear_all();
			break;		
	}
}

const char *control_config_tooltip_handler(const char *str)
{
	int i;

	if (!stricmp(str, NOX("@conflict"))) {
		for (i=0; i<NUM_TABS; i++) {
			if (Conflicts_tabs[i]) {
				return XSTR( "Conflict!", 205);
			}
		}
	}

	return NULL;
}

void control_config_init()
{
	int i;
	ui_button_info *b;

	// make backup of all controls
	for (i=0; i<CCFG_MAX; i++) {
		Control_config_backup[i] = Control_config[i];
	}

	Defaults_cycle_pos = 0;

	common_set_interface_palette(NOX("ControlConfigPalette"));  // set the interface palette
	Ui_window.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled, 0);
	Ui_window.set_mask_bmap(Conflict_background_bitmap_mask_fname[gr_screen.res]);
	Ui_window.tooltip_handler = control_config_tooltip_handler;

	// load in help overlay bitmap	
	Control_config_overlay_id = help_overlay_get_index(CONTROL_CONFIG_OVERLAY);
	help_overlay_set_state(Control_config_overlay_id,gr_screen.res,0);

	// reset conflict flashing
	Conflict_stamp = -1;

	for (i=0; i<NUM_BUTTONS; i++) {
		b = &CC_Buttons[gr_screen.res][i];

		if (b->hotspot < 0) {  // temporary
			b->button.create(&Ui_window, NOX("Clear other"), b->x, b->y, 150, 30, 0, 1);  // temporary
			b->button.set_highlight_action(common_play_highlight_sound);
			continue;
		}

		b->button.create(&Ui_window, "", b->x, b->y, 60, 30, ((i == SCROLL_UP_BUTTON) || (i == SCROLL_DOWN_BUTTON)), 1);

		// set up callback for when a mouse first goes over a button
		b->button.set_highlight_action(common_play_highlight_sound);		
		if (i<4) {
			b->button.set_bmaps(b->filename, 5, 1);		// a bit of a hack here, but buttons 0-3 need 4 frames loaded
		} else {
			b->button.set_bmaps(b->filename);
		}
		b->button.link_hotspot(b->hotspot);
	}	

	// create all text
	for(i=0; i<CC_NUM_TEXT; i++){
		Ui_window.add_XSTR(&CC_text[gr_screen.res][i]);
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		List_buttons[i].create(&Ui_window, "", 0, 0, 60, 30, 0, 1);
		List_buttons[i].hide();
		List_buttons[i].disable();
	}

	// set up hotkeys for buttons so we draw the correct animation frame when a key is pressed
	CC_Buttons[gr_screen.res][SCROLL_UP_BUTTON].button.set_hotkey(KEY_PAGEUP);
	CC_Buttons[gr_screen.res][SCROLL_DOWN_BUTTON].button.set_hotkey(KEY_PAGEDOWN);
	CC_Buttons[gr_screen.res][BIND_BUTTON].button.set_hotkey(KEY_ENTER);
	CC_Buttons[gr_screen.res][CLEAR_OTHER_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_DELETE);
	CC_Buttons[gr_screen.res][UNDO_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_Z);
	CC_Buttons[gr_screen.res][CLEAR_BUTTON].button.set_hotkey(KEY_DELETE);
	CC_Buttons[gr_screen.res][ACCEPT_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_ENTER);
	CC_Buttons[gr_screen.res][HELP_BUTTON].button.set_hotkey(KEY_F1);
	CC_Buttons[gr_screen.res][RESET_BUTTON].button.set_hotkey(KEY_CTRLED | KEY_R);
	CC_Buttons[gr_screen.res][INVERT_AXIS].button.set_hotkey(KEY_I);

	CC_Buttons[gr_screen.res][CANCEL_BUTTON].button.disable();
	CC_Buttons[gr_screen.res][CLEAR_OTHER_BUTTON].button.disable();

	Background_bitmap = bm_load(Conflict_background_bitmap_fname[gr_screen.res]);	

	Scroll_offset = Selected_line = 0;
	Undo_controls.clear();
	control_config_conflict_check();

	// setup strings					
	Joy_axis_action_text[0] = vm_strdup(XSTR("Turn (Yaw) Axis", 1016));
	Joy_axis_action_text[1] = vm_strdup(XSTR("Pitch Axis", 1017));
	Joy_axis_action_text[2] = vm_strdup(XSTR("Bank Axis", 1018));
	Joy_axis_action_text[3] = vm_strdup(XSTR("Absolute Throttle Axis", 1019));
	Joy_axis_action_text[4] = vm_strdup(XSTR("Relative Throttle Axis", 1020));
	Joy_axis_text[0] = vm_strdup(XSTR("Joystick/Mouse X Axis", 1021));
	Joy_axis_text[1] = vm_strdup(XSTR("Joystick/Mouse Y Axis", 1022));
	Joy_axis_text[2] = vm_strdup(XSTR("Joystick Z Axis", 1023));
	Joy_axis_text[3] = vm_strdup(XSTR("Joystick rX Axis", 1024));
	Joy_axis_text[4] = vm_strdup(XSTR("Joystick rY Axis", 1025));
	Joy_axis_text[5] = vm_strdup(XSTR("Joystick rZ Axis", 1026));
	Mouse_button_text[0] = vm_strdup("");
	Mouse_button_text[1] = vm_strdup(XSTR("Left Button", 1027));
	Mouse_button_text[2] = vm_strdup(XSTR("Right Button", 1028));
	Mouse_button_text[3] = vm_strdup(XSTR("Mid Button", 1029));
	Mouse_button_text[4] = vm_strdup("");
	Mouse_axis_text[0] = vm_strdup(XSTR("L/R", 1030));
	Mouse_axis_text[1] = vm_strdup(XSTR("U/B", 1031));
	Invert_text[0] = vm_strdup(XSTR("N", 1032));
	Invert_text[1] = vm_strdup(XSTR("Y", 1033));

	control_config_list_prepare();
}

void control_config_close()
{
	int idx;
	
	Undo_controls.clear();
	
	if (Background_bitmap){
		bm_release(Background_bitmap);
	}

	Ui_window.destroy();
	common_free_interface_palette();		// restore game palette
	hud_squadmsg_save_keys();				// rebuild map for saving/restoring keys in squadmsg mode
	game_flush();

	if (Game_mode & GM_MULTIPLAYER) {
		Pilot.save_player();
	} else {
		Pilot.save_savefile();
	}

	// free strings	
	for(idx=0; idx<NUM_JOY_AXIS_ACTIONS; idx++){
		if(Joy_axis_action_text[idx] != NULL){
			vm_free(Joy_axis_action_text[idx]);
			Joy_axis_action_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_AXIS_TEXT; idx++){
		if(Joy_axis_text[idx] != NULL){
			vm_free(Joy_axis_text[idx]);
			Joy_axis_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_MOUSE_TEXT; idx++){
		if(Mouse_button_text[idx] != NULL){
			vm_free(Mouse_button_text[idx]);
			Mouse_button_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_MOUSE_AXIS_TEXT; idx++){
		if(Mouse_axis_text[idx] != NULL){
			vm_free(Mouse_axis_text[idx]);
			Mouse_axis_text[idx] = NULL;
		}
	}
	for(idx=0; idx<NUM_INVERT_TEXT; idx++){
		if(Invert_text[idx] != NULL){
			vm_free(Invert_text[idx]);
			Invert_text[idx] = NULL;
		}
	}
}

void control_config_do_frame(float frametime)
{
	const char *str;
	char buf[256];
	int i, j, k, w, x, y, z, line, conflict;
	int font_height = gr_get_font_height();
	int select_tease_line = -1;  // line mouse is down on, but won't be selected until button released
	static float timer = 0.0f;
	color *c;
	static int bound_timestamp = 0;
	static char bound_string[40];
	
	timer += frametime;

	if (Binding_mode) {
		if (Cc_lines[Selected_line].cc_index & JOY_AXIS) {
			int bind = 0;

			z = Cc_lines[Selected_line].cc_index & ~JOY_AXIS;
			i = control_config_detect_axis();
			if (i >= 0) {
				Axis_override = i;
				bind = 1;
			}

			k = game_poll();
			Ui_window.use_hack_to_get_around_stupid_problem_flag = 1;
			Ui_window.process(0);

			if (k == KEY_ESC) {
				strcpy_s(bound_string, XSTR( "Canceled", 206));
				bound_timestamp = timestamp(2500);
				control_config_do_cancel();

			} else {
				if (k == KEY_ENTER) {
					bind = 1;
				}

				for (i=0; i<JOY_TOTAL_BUTTONS; i++) {
					if (joy_down_count(i, 1)) {
						bind = 1;
					}
				}

				if (bind) {
					if (Axis_override >= 0) {
						control_config_bind_axis(z, Axis_override);
						strcpy_s(bound_string, Joy_axis_text[Axis_override]);
						font::force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
						bound_timestamp = timestamp(2500);
						control_config_conflict_check();
						control_config_list_prepare();
						control_config_do_cancel();

					} else {
						control_config_do_cancel(1);
					}
				}
			}

		} else {
			if (help_overlay_active(Control_config_overlay_id)) {
				CC_Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
				Ui_window.set_ignore_gadgets(1);
			}

			k = game_poll();
			Ui_window.use_hack_to_get_around_stupid_problem_flag = 1;
			Ui_window.process(0);

			if ( (k > 0) || B1_JUST_RELEASED ) {
				if (help_overlay_active(Control_config_overlay_id)) {
					help_overlay_set_state(Control_config_overlay_id, gr_screen.res, 0);
					Ui_window.set_ignore_gadgets(0);
					k = 0;
				}
			}

			if ( !help_overlay_active(Control_config_overlay_id) ) {
				Ui_window.set_ignore_gadgets(0);
			}

			if (k == KEY_ESC) {
				strcpy_s(bound_string, XSTR( "Canceled", 206));
				bound_timestamp = timestamp(2500);
				control_config_do_cancel();

			} else {
				switch (k & KEY_MASK) {
					case KEY_LSHIFT:
					case KEY_RSHIFT:
					case KEY_LALT:
					case KEY_RALT:
						Last_key = k & KEY_MASK;
						k = 0;
						break;
				}

				if (Cc_lines[Selected_line].cc_index == BANK_WHEN_PRESSED || Cc_lines[Selected_line].cc_index == GLIDE_WHEN_PRESSED) {
					if ( (Last_key >= 0) && (k <= 0) && !keyd_pressed[Last_key] ) {
						k = Last_key;
					}
				}

				if ((k > 0) && !Config_allowed[k & KEY_MASK]) {
					popup(PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR( "That is a non-bindable key.  Please try again.", 207));
					k = 0;
				}

				k &= (KEY_MASK | KEY_SHIFTED | KEY_ALTED);
				if (k > 0) {
					z = Cc_lines[Selected_line].cc_index;
					Assert(!(z & JOY_AXIS));
					control_config_bind_key(z, k);

					strcpy_s(bound_string, textify_scancode(k));
					font::force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
					bound_timestamp = timestamp(2500);
					control_config_conflict_check();
					control_config_list_prepare();
					control_config_do_cancel();
				}

				for (i=0; i<JOY_TOTAL_BUTTONS; i++) {
					if (joy_down_count(i, 1)) {
						z = Cc_lines[Selected_line].cc_index;
						Assert(!(z & JOY_AXIS));
						control_config_bind_joy(z, i);

						strcpy_s(bound_string, Joy_button_text[i]);
						font::force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
						bound_timestamp = timestamp(2500);
						control_config_conflict_check();
						control_config_list_prepare();
						control_config_do_cancel();
						break;
					}
				}

				if (Bind_time + 375 < timer_get_milliseconds()) {
					for (i=0; i<NUM_BUTTONS; i++){
						if ( (CC_Buttons[gr_screen.res][i].button.is_mouse_on()) && (CC_Buttons[gr_screen.res][i].button.enabled()) ){
							break;
						}
					}

					if (i == NUM_BUTTONS) {  // no buttons pressed
						for (i=0; i<MOUSE_NUM_BUTTONS; i++) {
							if (mouse_down(1 << i)) {
								z = Cc_lines[Selected_line].cc_index;
								Assert(!(z & JOY_AXIS));
								control_config_bind_joy(z, i);

								strcpy_s(bound_string, Joy_button_text[i]);
								font::force_fit_string(bound_string, 39, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
								bound_timestamp = timestamp(2500);
								control_config_conflict_check();
								control_config_list_prepare();
								control_config_do_cancel();
								for (j=0; j<NUM_BUTTONS; j++){
									CC_Buttons[gr_screen.res][j].button.reset();
								}

								break;
							}
						}
					}
				}
			}
		}

	} else if (Search_mode) {
		if (help_overlay_active(Control_config_overlay_id)) {
			CC_Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
			Ui_window.set_ignore_gadgets(1);
		}

		k = game_poll();
		Ui_window.use_hack_to_get_around_stupid_problem_flag = 1;
		Ui_window.process(0);

		if ( (k > 0) || B1_JUST_RELEASED ) {
			if ( help_overlay_active(Control_config_overlay_id) ) {
				help_overlay_set_state(Control_config_overlay_id, gr_screen.res, 0);
				Ui_window.set_ignore_gadgets(0);
				k = 0;
			}
		}

		if ( !help_overlay_active(Control_config_overlay_id) ) {
			Ui_window.set_ignore_gadgets(0);
		}

		if (k == KEY_ESC) {
			control_config_do_cancel();

		} else {
			if ((k > 0) && !Config_allowed[k & KEY_MASK]) {
				k = 0;
			}

			k &= (KEY_MASK | KEY_SHIFTED | KEY_ALTED);
			z = -1;
			if (k > 0) {
				for (i=0; i<CCFG_MAX; i++) {
					if (Control_config[i].get_bind(CID_KEYBOARD) >= 0) {
						z = i;
						break;
					}
				}
			}

			for (i=0; i<JOY_TOTAL_BUTTONS; i++) {
				if (joy_down_count(i, 1)) {
					j = i;
					for (i=0; i<CCFG_MAX; i++) { //-V535
						if (Control_config[i].get_bind(CID_JOY) >= 0) {
							z = i;
							break;
						}
					}
					break;
				}
			}

			// check if not on enabled button
			for (j=0; j<NUM_BUTTONS; j++){
				if ( (CC_Buttons[gr_screen.res][j].button.is_mouse_on()) && (CC_Buttons[gr_screen.res][j].button.enabled()) ){
					break;
				}
			}

			if (j == NUM_BUTTONS) {  // no buttons pressed
				for (j=0; j<MOUSE_NUM_BUTTONS; j++) {
					if (mouse_down(1 << j)) {
						for (i=0; i<CCFG_MAX; i++) {
							if (Control_config[i].get_bind(CID_MOUSE) >= 0) {
								z = i;
								for (size_t buttonid=0; buttonid<NUM_BUTTONS; buttonid++){
									CC_Buttons[gr_screen.res][buttonid].button.reset();
								}
								break;
							}
						}
						break;
					}
				}
			}

			if (z >= 0) {
				Tab = Control_config[z].tab;
				control_config_list_prepare();
				Selected_line = Scroll_offset = 0;
				for (i=0; i<Num_cc_lines; i++) {
					if (Cc_lines[i].cc_index == z) {
						Selected_line = i;
						break;
					}
				}
				while (!cc_line_query_visible(Selected_line)) {
					Scroll_offset++;
					Assert(Scroll_offset < Num_cc_lines);
				}
			}
		}

	} else {
		z = Cc_lines[Selected_line].cc_index & JOY_AXIS;
		CC_Buttons[gr_screen.res][ALT_TOGGLE].button.enable(!z);
		CC_Buttons[gr_screen.res][SHIFT_TOGGLE].button.enable(!z);
		CC_Buttons[gr_screen.res][INVERT_AXIS].button.enable(z);

		if (!z) {
			z = Cc_lines[Selected_line].cc_index;
			k = Control_config[z].get_bind(CID_KEYBOARD);
			if ( (k == KEY_LALT) || (k == KEY_RALT) || (k == KEY_LSHIFT) || (k == KEY_RSHIFT) ) {
				CC_Buttons[gr_screen.res][ALT_TOGGLE].button.enable(0);
				CC_Buttons[gr_screen.res][SHIFT_TOGGLE].button.enable(0);
			}
		}

		CC_Buttons[gr_screen.res][UNDO_BUTTON].button.enable(Undo_controls.size() > 0);

		if ( help_overlay_active(Control_config_overlay_id) ) {
			CC_Buttons[gr_screen.res][HELP_BUTTON].button.reset_status();
			Ui_window.set_ignore_gadgets(1);
		}

		k = Ui_window.process();

		if ( (k > 0) || B1_JUST_RELEASED ) {
			if ( help_overlay_active(Control_config_overlay_id) ) {
				help_overlay_set_state(Control_config_overlay_id, gr_screen.res, 0);
				Ui_window.set_ignore_gadgets(0);
				k = 0;
			}
		}

		if ( !help_overlay_active(Control_config_overlay_id) ) {
			Ui_window.set_ignore_gadgets(0);
		}

		switch (k) {
			case KEY_DOWN:  // select next line
				control_config_scroll_line_down();
				break;

			case KEY_UP:  // select previous line
				control_config_scroll_line_up();
				break;

			case KEY_SHIFTED | KEY_TAB:  // activate previous tab
				Tab--;
				if (Tab < 0) {
					Tab = NUM_TABS - 1;
				}

				Scroll_offset = Selected_line = 0;
				control_config_list_prepare();
				gamesnd_play_iface(SND_SCREEN_MODE_PRESSED);
				break;

			case KEY_TAB:  // activate next tab
				Tab++;
				if (Tab >= NUM_TABS) {
					Tab = 0;
				}

				Scroll_offset = Selected_line = 0;
				control_config_list_prepare();
				gamesnd_play_iface(SND_SCREEN_MODE_PRESSED);
				break;

			case KEY_LEFT:
				Selected_item--;
				if (Selected_item == -2) {
					Selected_item = 1;
					if (Cc_lines[Selected_line].jw < 1) {
						Selected_item = 0;
						if (Cc_lines[Selected_line].kw < 1) {
							Selected_item = -1;
						}
					}
				}

				gamesnd_play_iface(SND_SCROLL);
				break;

			case KEY_RIGHT:
				Selected_item++;
				if ((Selected_item == 1) && (Cc_lines[Selected_line].jw < 1)) {
					Selected_item = -1;
				} else if (!Selected_item && (Cc_lines[Selected_line].kw < 1)) {
					Selected_item = -1;
				} else if (Selected_item > 1) {
					Selected_item = -1;
				}
				gamesnd_play_iface(SND_SCROLL);
				break;

			case KEY_BACKSP:  // undo
				control_config_undo_last();
				break;

			case KEY_ESC:
				control_config_cancel_exit();
				break;
		}	// end switch
	}

	for (i=0; i<NUM_BUTTONS; i++){
		if (CC_Buttons[gr_screen.res][i].button.pressed()){
			control_config_button_pressed(i);
		}
	}

	for (i=0; i<LIST_BUTTONS_MAX; i++) {
		if (List_buttons[i].is_mouse_on()) {
			select_tease_line = i + Scroll_offset;
		}
	
		if (List_buttons[i].pressed()) {
			Selected_line = i + Scroll_offset;
			Selected_item = -1;
			List_buttons[i].get_mouse_pos(&x, &y);
			if ((x >= Cc_lines[Selected_line].kx) && (x < Cc_lines[Selected_line].kx + Cc_lines[Selected_line].kw)) {
				Selected_item = 0;
			}

			if ((x >= Cc_lines[Selected_line].jx) && (x < Cc_lines[Selected_line].jx + Cc_lines[Selected_line].jw)) {
				Selected_item = 1;
			}

			gamesnd_play_iface(SND_USER_SELECT);
		}

		if (List_buttons[i].double_clicked()) {
			control_config_do_bind();
		}
	}

	GR_MAYBE_CLEAR_RES(Background_bitmap);
	if (Background_bitmap >= 0) {
		gr_set_bitmap(Background_bitmap);
		gr_bitmap(0, 0, GR_RESIZE_MENU);
	} 

	// highlight tab with conflict
	Ui_window.draw();
	for (i=z=0; i<NUM_TABS; i++) {
		if (Conflicts_tabs[i]) {
			CC_Buttons[gr_screen.res][i].button.draw_forced(4);
			z++;
		}
	}

	if (z) {
		// maybe switch from bright to normal
		if((Conflict_stamp == -1) || timestamp_elapsed(Conflict_stamp)){
			Conflict_bright = !Conflict_bright;

			Conflict_stamp = timestamp(CONFLICT_FLASH_TIME);
		}

		// set color and font
		font::set_font(font::FONT2);
		if(Conflict_bright){
			gr_set_color_fast(&Color_bright_red);
		} else {
			gr_set_color_fast(&Color_red);
		}

		// setup the conflict string
		char conflict_str[512] = "";
		strncpy(conflict_str, XSTR("Conflict!", 205), 511);
		int sw, sh;
		gr_get_string_size(&sw, &sh, conflict_str);

		gr_string((gr_screen.max_w / 2) - (sw / 2), Conflict_warning_coords[gr_screen.res][1], conflict_str, GR_RESIZE_MENU);

		font::set_font(font::FONT1);
	} else {
		// might as well always reset the conflict stamp
		Conflict_stamp = -1;
	}

	for (i=0; i<NUM_TABS; i++) {
		if (CC_Buttons[gr_screen.res][i].button.button_down()) {
			break;
		}
	}

	if (i == NUM_TABS) {
		CC_Buttons[gr_screen.res][Tab].button.draw_forced(2);
	}

	if (Search_mode) {
		CC_Buttons[gr_screen.res][SEARCH_MODE].button.draw_forced(2);
	}

	if (Selected_line >= 0) {
		z = Cc_lines[Selected_line].cc_index;
		if (z & JOY_AXIS) {
			if (Invert_axis[z & ~JOY_AXIS]) {
				CC_Buttons[gr_screen.res][INVERT_AXIS].button.draw_forced(2);
			}

		} else {
			z = Control_config[Selected_line].get_bind(CID_KEYBOARD);
			if (z >= 0) {
				if (z & KEY_SHIFTED) {
					CC_Buttons[gr_screen.res][SHIFT_TOGGLE].button.draw_forced(2);
				}
				if (z & KEY_ALTED) {
					CC_Buttons[gr_screen.res][ALT_TOGGLE].button.draw_forced(2);
				}
			}
		}
	}

	if (Binding_mode) {
		CC_Buttons[gr_screen.res][BIND_BUTTON].button.draw_forced(2);
	}

	z = Cc_lines[Selected_line].cc_index;
	x = Conflict_wnd_coords[gr_screen.res][CONTROL_X_COORD] + Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD] / 2;
	y = Conflict_wnd_coords[gr_screen.res][CONTROL_Y_COORD] + Conflict_wnd_coords[gr_screen.res][CONTROL_H_COORD] / 2;
	if (Binding_mode) {
		int t;

		t = (int) (timer * 3);
		if (t % 2) {
			gr_set_color_fast(&Color_text_normal);
			gr_get_string_size(&w, NULL, XSTR( "?", 208));
			gr_printf_menu(x - w / 2, y - font_height / 2, XSTR( "?", 208));
		}

	} else if (!(z & JOY_AXIS) && Conflicts[z].has_conflict()) {
		i = 0;
		for (int i = 0; i < MAX_BINDINGS; ++i) {
			if (Conflicts[z].other_id[i].first >= 0) {
				break;
			}
		}
		Assert(i != MAX_BINDINGS);

		// Reuse i to index the action we're conflicting with
		// TODO: z64! refactor this entire function!
		i = Conflicts[z].other_id[i].first;

		gr_set_color_fast(&Color_text_normal);
		str = XSTR( "Control conflicts with:", 209);
		gr_get_string_size(&w, NULL, str);
		gr_printf_menu(x - w / 2, y - font_height, str);

		if (Control_config[i].hasXSTR) {
			strcpy_s(buf, XSTR(Control_config[i].text, CONTROL_CONFIG_XSTR + i));
		} else {
			strcpy_s(buf, Control_config[i].text);
		}

		font::force_fit_string(buf, 255, Conflict_wnd_coords[gr_screen.res][CONTROL_W_COORD]);
		gr_get_string_size(&w, NULL, buf);
		gr_printf_menu(x - w / 2, y, buf);

	} else if (*bound_string) {
		gr_set_color_fast(&Color_text_normal);
		gr_get_string_size(&w, NULL, bound_string);
		gr_printf_menu(x - w / 2, y - font_height / 2, bound_string);
		if (timestamp_elapsed(bound_timestamp)) {
			*bound_string = 0;
		}
	}

	if (Cc_lines[Num_cc_lines - 1].y + font_height > Cc_lines[Scroll_offset].y + Control_list_coords[gr_screen.res][CONTROL_H_COORD]) {
		gr_set_color_fast(&Color_white);
		gr_printf_menu(Control_more_coords[gr_screen.res][CONTROL_X_COORD], Control_more_coords[gr_screen.res][CONTROL_Y_COORD], XSTR( "More...", 210));
	}

	conflict = 0;
	line = Scroll_offset;
	while (cc_line_query_visible(line)) {
		z = Cc_lines[line].cc_index;
		y = Control_list_coords[gr_screen.res][CONTROL_Y_COORD] + Cc_lines[line].y - Cc_lines[Scroll_offset].y;

		List_buttons[line - Scroll_offset].update_dimensions(Control_list_coords[gr_screen.res][CONTROL_X_COORD], y, Control_list_coords[gr_screen.res][CONTROL_W_COORD], font_height);
		List_buttons[line - Scroll_offset].enable(!Binding_mode);

		Cc_lines[line].kw = Cc_lines[line].jw = 0;

		if (line == Selected_line){
			c = &Color_text_selected;
		} else if (line == select_tease_line) {
			c = &Color_text_subselected;
		} else {
			c = &Color_text_normal;
		}

		gr_set_color_fast(c);
		if (Cc_lines[line].label) {
			strcpy_s(buf, Cc_lines[line].label);
			font::force_fit_string(buf, 255, Control_list_ctrl_w[gr_screen.res]);
			gr_printf_menu(Control_list_coords[gr_screen.res][CONTROL_X_COORD], y, buf);
		}

		if (!(z & JOY_AXIS)) {
			k = Control_config[z].get_bind(CID_KEYBOARD);
			j = Control_config[z].get_bind(CID_JOY);
			x = Control_list_key_x[gr_screen.res];
			*buf = 0;

			if ((k < 0) && (j < 0)) {
				gr_set_color_fast(&Color_grey);
				gr_printf_menu(x, y, XSTR( "None", 211));

			} else {
				if (k >= 0) {
					strcpy_s(buf, textify_scancode(k));
					if (Conflicts[z].key >= 0) {
						if (c == &Color_text_normal)
							gr_set_color_fast(&Color_text_error);
						else {
							gr_set_color_fast(&Color_text_error_hi);
							conflict++;
						}

					} else if (Selected_item == 1) {
						gr_set_color_fast(&Color_text_normal);

					} else {
						gr_set_color_fast(c);
					}

					gr_printf_menu(x, y, buf);

					Cc_lines[line].kx = x - Control_list_coords[gr_screen.res][CONTROL_X_COORD];
					gr_get_string_size(&w, NULL, buf);
					Cc_lines[line].kw = w;
					x += w;

					if (j >= 0) {
						gr_set_color_fast(&Color_text_normal);
						gr_printf_menu(x, y, XSTR( ", ", 212));
						gr_get_string_size(&w, NULL, XSTR( ", ", 212));
						x += w;
					}
				}

				if (j >= 0) {
					strcpy_s(buf, Joy_button_text[j]);
					if (Conflicts[z].joy >= 0) {
						if (c == &Color_text_normal) {
							gr_set_color_fast(&Color_text_error);
						} else {
							gr_set_color_fast(&Color_text_error_hi);
							conflict++;
						}

					} else if (!Selected_item) {
						gr_set_color_fast(&Color_text_normal);

					} else {
						gr_set_color_fast(c);
					}

					font::force_fit_string(buf, 255, Control_list_key_w[gr_screen.res] + Control_list_key_x[gr_screen.res] - x);
					gr_printf_menu(x, y, buf);

					Cc_lines[line].jx = x - Control_list_coords[gr_screen.res][CONTROL_X_COORD];
					gr_get_string_size(&Cc_lines[line].jw, NULL, buf);
				}
			}

		} else {
			x = Control_list_key_x[gr_screen.res];
			j = Axis_map_to[z & ~JOY_AXIS];
			if (Binding_mode && (line == Selected_line)) {
				j = Axis_override;
			}

			if (j < 0) {
				gr_set_color_fast(&Color_grey);
				gr_printf_menu(x, y, XSTR( "None", 211));

			} else {
				if (Conflicts_axes[z & ~JOY_AXIS] >= 0) {
					if (c == &Color_text_normal) {
						gr_set_color_fast(&Color_text_error);

					} else {
						gr_set_color_fast(&Color_text_error_hi);
						conflict++;
					}

				} else if (!Selected_item) {
					gr_set_color_fast(&Color_text_normal);

				} else {
					gr_set_color_fast(c);
				}

				gr_string(x, y, Joy_axis_text[j], GR_RESIZE_MENU);
			}
		}

		line++;
	}

	CC_Buttons[gr_screen.res][CLEAR_OTHER_BUTTON].button.enable(conflict);

	i = line - Scroll_offset;
	while (i < LIST_BUTTONS_MAX) {
		List_buttons[i++].disable();
	}

	// If multiple controls presets are provided, display which one is in use
	if (Control_config_presets.size() > 1) {
		SCP_string preset_str;
		int matching_preset = -1;

		for (i=0; i<(int)Control_config_presets.size(); i++) {
			bool this_preset_matches = true;
			config_item *this_preset = Control_config_presets[i];

			for (j=0; j<CCFG_MAX; j++) {
				if (!Control_config[j].disabled && Control_config[j].key_id != this_preset[j].key_default) {
					this_preset_matches = false;
					break;
				}
			}

			if (this_preset_matches) {
				matching_preset = i;
				break;
			}
		}

		if (matching_preset >= 0) {
			sprintf(preset_str, "Controls: %s", Control_config_preset_names[matching_preset].c_str());
		} else {
			sprintf(preset_str, "Controls: custom");
			
		}

		gr_get_string_size(&w, NULL, preset_str.c_str());
		gr_set_color_fast(&Color_text_normal);

		if (gr_screen.res == GR_640) {
			gr_string(16, (24 - font_height) / 2, preset_str.c_str(), GR_RESIZE_MENU);
		} else {
			gr_string(24, (40 - font_height) / 2, preset_str.c_str(), GR_RESIZE_MENU);
		}
	}

	// blit help overlay if active
	help_overlay_maybe_blit(Control_config_overlay_id, gr_screen.res);

	gr_flip();
}

void clear_key_binding(short key)
{
	int i;

	for (i=0; i<CCFG_MAX; i++) {
		if (Control_config[i].key_id == key) {
			Control_config[i].key_id = -1;
		}
	}
}

float check_control_timef(int id)
{
	float t1, t2;

	// if type isn't continuous, we shouldn't be using this function, cause it won't work.
	Assert(Control_config[id].type == CC_TYPE_CONTINUOUS);

	// first, see if control actually used (makes sure modifiers match as well)
	if (!check_control(id)) {
		Control_config[id].continuous_ongoing = false;

		return 0.0f;
	}

	t1 = key_down_timef(Control_config[id].key_id);
	if (t1) {
		control_used(id);
	}

	t2 = joy_down_time(Control_config[id].joy_id);
	if (t2) {
		control_used(id);
	}

	if (t1 + t2) {
		// We want to set this to true only after visiting control_used() (above)
		// to allow it to tell the difference between an ongoing continuous action
		// started before and a continuous action being started right now.
		Control_config[id].continuous_ongoing = true;

		return t1 + t2;
	}

	return 1.0f;
}

void control_check_indicate()
{
#ifndef NDEBUG
	if (Show_controls_info) {
		gr_set_color_fast(&HUD_color_debug);
		gr_printf_no_resize(gr_screen.center_offset_x + gr_screen.center_w - 154, gr_screen.center_offset_y + 5, NOX("Ctrls checked: %d"), Control_check_count);
	}
#endif

	Control_check_count = 0;
}

int check_control_used(int id, int key)
{
	int z, mask;
	static int last_key = 0;

	Control_check_count++;
	if (key < 0) {
		key = last_key;
	}

	last_key = key;

	// if we're in multiplayer text enter (for chat) mode, check to see if we should ignore controls
	if ((Game_mode & GM_MULTIPLAYER) && multi_ignore_controls()){
		return 0;
	}

	auto &Control = Control_config[id];

	if (Control.disabled)
		return 0;

	switch (Control.type) {
	case CC_TYPE_CONTINUOUS:
		for (int i = 0; i < MAX_BINDINGS; ++i) {
			auto &c_id = Control.c_id[i];

			if (c_id.first == -1) {
				continue;
			}
			
			if (((c_id.first == CID_MOUSE) && (mouse_down(c_id.second) || mouse_down_count(c_id.second))) ||
				((c_id.first == CID_JOY) && (joy_down(c_id.second) || joy_down_count(c_id.second, 1)))) {
				control_used(id);
				return 1;
			}
		}

		// check what current modifiers are pressed
		mask = 0;
		if (keyd_pressed[KEY_LSHIFT] || key_down_count(KEY_LSHIFT) || keyd_pressed[KEY_RSHIFT] || key_down_count(KEY_RSHIFT)) {
			mask |= KEY_SHIFTED;
		}

		if (keyd_pressed[KEY_LALT] || key_down_count(KEY_LALT) || keyd_pressed[KEY_RALT] || key_down_count(KEY_RALT)) {
			mask |= KEY_ALTED;
		}

		z = Control.key_id;
		if (z >= 0) {
			if ((z != KEY_LALT) && (z != KEY_RALT) && (z != KEY_LSHIFT) && (z != KEY_RSHIFT)) {
				// if current modifiers don't match action's modifiers, don't register control active.
				if ((z & (KEY_SHIFTED | KEY_ALTED)) != mask) {
					return 0;
				}
			}

			z &= KEY_MASK;

			if (keyd_pressed[z] || key_down_count(z)) {
				control_used(id);
				return 1;
			}
		}
		break;

	case CC_TYPE_TRIGGER:
		for (int i = 0; i < MAX_BINDINGS; ++i) {
			auto &c_id = Control.c_id[i];

			if (c_id.first == -1) {
				continue;
			}

			if ((c_id.first == CID_KEYBOARD) && (c_id.second == key) ||
				((c_id.first == CID_MOUSE) && mouse_down_count(c_id.second)) ||
				((c_id.first == CID_JOY) && joy_down_count(c_id.second, 1))) {
				control_used(id);
				return 1;
			}
		}
		break;

	default:
		// Can't happen
		Int3();
		break;
	}

	return 0;
}

int check_control(int id, int key) 
{
	if (check_control_used(id, key)) {
		if (Ignored_keys[id]) {
			if (Ignored_keys[id] > 0) {
				Ignored_keys[id]--;
			}
			return 0;
		}
		return 1;
	}

	if (Control_config[id].continuous_ongoing) {
		// If we reach this point, then it means this is a continuous control
		// which has just been released

		Script_system.SetHookVar("Action", 's', Control_config[id].text);
		Script_system.RunCondition(CHA_ONACTIONSTOPPED, '\0', NULL, NULL, id);
		Script_system.RemHookVar("Action");

		Control_config[id].continuous_ongoing = false;
	}

	return 0;
}

void control_get_axes_readings(int *h, int *p, int *b, int *ta, int *tr)
{
	int axes_values[JOY_NUM_AXES];

	joystick_read_raw_axis(JOY_NUM_AXES, axes_values);

	//	joy_get_scaled_reading will return a value represents the joystick pos from -1 to +1 (fixed point)
	*h = 0;
	if (Axis_map_to[0] >= 0) {
		*h = joy_get_scaled_reading(axes_values[Axis_map_to[0]]);
	}

	*p = 0;
	if (Axis_map_to[1] >= 0) {
		*p = joy_get_scaled_reading(axes_values[Axis_map_to[1]]);
	}

	*b = 0;
	if (Axis_map_to[2] >= 0) {
		*b = joy_get_scaled_reading(axes_values[Axis_map_to[2]]);
	}

	*ta = 0;
	if (Axis_map_to[3] >= 0) {
		*ta = joy_get_unscaled_reading(axes_values[Axis_map_to[3]], Axis_map_to[3]);
	}

	*tr = 0;
	if (Axis_map_to[4] >= 0) {
		*tr = joy_get_scaled_reading(axes_values[Axis_map_to[4]]);
	}

	if (Invert_axis[0]) {
		*h = -(*h);
	}
	if (Invert_axis[1]) {
		*p = -(*p);
	}
	if (Invert_axis[2]) {
		*b = -(*b);
	}
	if (Invert_axis[3]) {
		*ta = F1_0 - *ta;
	}
	if (Invert_axis[4]) {
		*tr = -(*tr);
	}

	return;
}

int Last_frame_timestamp;
void control_used(int id)
{
	// if we have set this key to be ignored, ignore it
	if (Ignored_keys[id]) {
		return;
	}

	// This check needs to be done because the control code might call this function more than once per frame,
	// and we don't want to run the hooks more than once per frame
	if (Control_config[id].used < Last_frame_timestamp) {
		if (!Control_config[id].continuous_ongoing) {
			Script_system.SetHookVar("Action", 's', Control_config[id].text);
			Script_system.RunCondition(CHA_ONACTION, '\0', NULL, NULL, id);
			Script_system.RemHookVar("Action");

			if (Control_config[id].type == CC_TYPE_CONTINUOUS)
				Control_config[id].continuous_ongoing = true;
		}

		Control_config[id].used = timestamp();
	}
}

void control_config_clear_used_status()
{
	int i;

	for (i=0; i<CCFG_MAX; i++) {
		Control_config[i].used = 0;
	}
}

void control_config_clear()
{
	int i, j;

	for (i=0; i<CCFG_MAX; i++) {
		for (j = 0; j < MAX_BINDINGS; ++j) {
			Control_config[i].c_id[j] = std::pair<short, short>(-1, -1);
		}
	}
}

int control_config_handle_conflict()
{
	return 0;
}
