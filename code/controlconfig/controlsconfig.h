/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/


#ifndef CONTROLS_CONFIG_H
#define CONTROLS_CONFIG_H

#include "globalincs/pstypes.h"

#define CONTROL_CONFIG_XSTR	507
#define MAX_BINDINGS 3

class Config_item;
enum IoActionId;

typedef std::pair<short, short> cid;            // Controller/Button pair. First = Controller ID, Second = Button ID on the controller
typedef SCP_vector<Config_item> Control_LUT;    // Control Configuration container. Contains control info and Controller->Action mappings (Input index is an IoActionId)
typedef SCP_vector<IoActionId>  Action_LUT;     // Action container. Containts Action->Controller mappings. (Input index is a button id of the controller this map is for)

class Config_item
{
public:
	cid default_id[MAX_BINDINGS];   //!< default bindings for this action
	cid c_id[MAX_BINDINGS];     //!< controller ID's currently bound
	int used;                   //!< has control been used yet in mission?  If so, this is the timestamp
	char type;                  //!< manner control should be checked in
	bool disabled;              //!< whether this action should be available at all
	bool continuous_ongoing;    //!< whether this action is a contiuous one and is currently ongoing

	// GUI members
	char tab;           //!< what tab (category) this control belongs to
	bool hasXSTR;       //!< whether we should translate the text with an XSTR
	const char* text;   //!< describes the action in the config screen

	Config_item();

	Config_item(short default0, short default1, short default2, char type, bool disabled, char tab, bool hasXSTR, const char* text);
};


/**
* @brief Predicate class used to construct new Config_items
*/
class config_item_loader
{
public:
	/**
	* @brief Loader's constructor. This is how we tell it which container the items will go into!
	*/
	config_item_loader(Control_LUT *dest);

	/**
	* @brief Creates a new instance of Config_item and adds it to the end of dest
	*
	* @note Returns a reference to iself, so you can chain calls together in a style similar to array initialization.
	* @sa Config_item
	*/
	config_item_loader& operator()(char tab, const char* text, bool hasXSTR, char type, bool disabled, short default0, short default1, short default2);

	/**
	* @brief Version of operator() which takes in two cid's
	*
	* @sa Config_item
	*/
	config_item_loader& operator()(char tab, const char* text, bool hasXSTR, char type, bool disabled, short default0, short default1);

	/**
	* @brief Version of operator() which takes in one cid
	*
	* @sa Config_item
	*/
	config_item_loader& operator()(char tab, const char* text, bool hasXSTR, char type, bool disabled, short default0);

private:
	config_item_loader();

	Control_LUT *dest;
};


/*!
 * These are used to index a corresponding joystick axis value from an array.
 * Currently only used by ::Axis_map_to[] and ::Axis_map_to_defaults[]
 */
enum Joy_axis_index {
	JOY_X_AXIS		=0,
	JOY_Y_AXIS,
	JOY_Z_AXIS,
	JOY_RX_AXIS,
	JOY_RY_AXIS,
	JOY_RZ_AXIS
};


/*!
 * These are used to index a corresponding (analog) action, namely controlling the orientation angles and throttle.
 */
enum Joy_axis_action_index {
	JOY_HEADING_AXIS	=0,
	JOY_PITCH_AXIS,
	JOY_BANK_AXIS,
	JOY_ABS_THROTTLE_AXIS,
	JOY_REL_THROTTLE_AXIS,

	/*!
	 * This must always be below the last defined item
	 */
	NUM_JOY_AXIS_ACTIONS			//!< The total number of actions an axis may map to
};


/*!
 * Control Configuration Types. Namely differ in how the control is activated
 */
enum CC_type {
	CC_TYPE_TRIGGER			=0,		//!< A normal, one-shot type control that is activated when a key is or button is pressed
	CC_TYPE_CONTINUOUS				//!< A continous control that is activated as long as the key or button is held down
};


/*!
 * All available actions
 * This is the value of the id field in config_item
 * The first group of items are ship targeting.
 */
enum IoActionId  {
	TARGET_NEXT										=0,		//!< target next
	TARGET_PREV										=1,		//!< target previous
	TARGET_NEXT_CLOSEST_HOSTILE						=2,		//!< target the next hostile target
	TARGET_PREV_CLOSEST_HOSTILE						=3,		//!< target the previous closest hostile
	TOGGLE_AUTO_TARGETING							=4,		//!< toggle auto-targeting
	TARGET_NEXT_CLOSEST_FRIENDLY					=5,		//!< target the next friendly ship
	TARGET_PREV_CLOSEST_FRIENDLY					=6,		//!< target the closest friendly ship
	TARGET_SHIP_IN_RETICLE							=7,		//!< target ship closest to center of reticle
	TARGET_CLOSEST_SHIP_ATTACKING_TARGET			=8,		//!< target the closest ship attacking current target
	TARGET_LAST_TRANMISSION_SENDER					=9,		//!< TARGET_LAST_TRANMISSION_SENDER
	STOP_TARGETING_SHIP								=10,	//!< stop targeting ship

	//!< @n
	//!< Targeting a Ship's Subsystem
	//!< ------------------------------
	TARGET_SUBOBJECT_IN_RETICLE						=11,	//!< target ships subsystem in reticle
	TARGET_NEXT_SUBOBJECT							=12,	//!< target next subsystem on current target
	TARGET_PREV_SUBOBJECT							=13,	//!< TARGET_PREV_SUBOBJECT
	STOP_TARGETING_SUBSYSTEM						=14,	//!< stop targeting subsystems on ship

	//!< @n
	//!< Speed Matching
	//!< ----------------
	MATCH_TARGET_SPEED								=15,	//!< match target speed
	TOGGLE_AUTO_MATCH_TARGET_SPEED					=16,	//!< toggle auto-match target speed

	//!< @n
	//!< Weapons
	//!< ---------
	FIRE_PRIMARY									=17,	//!< FIRE_PRIMARY
	FIRE_SECONDARY									=18,	//!< FIRE_SECONDARY
	CYCLE_NEXT_PRIMARY								=19,	//!< cycle to next primary weapon
	CYCLE_PREV_PRIMARY								=20,	//!< cycle to previous primary weapon
	CYCLE_SECONDARY									=21,	//!< cycle to next secondary weapon
	CYCLE_NUM_MISSLES								=22,	//!< cycle number of missiles fired from secondary bank
	LAUNCH_COUNTERMEASURE							=23,	//!< LAUNCH_COUNTERMEASURE

	//!< @n
	//!< Controls
	//!< ----------
	FORWARD_THRUST									=24,	//!< FORWARD_THRUST
	REVERSE_THRUST									=25,	//!< REVERSE_THRUST
	BANK_LEFT										=26,	//!< BANK_LEFT
	BANK_RIGHT										=27,	//!< BANK_RIGHT
	PITCH_FORWARD									=28,	//!< PITCH_FORWARD
	PITCH_BACK										=29,	//!< PITCH_BACK
	YAW_LEFT										=30,	//!< YAW_LEFT
	YAW_RIGHT										=31,	//!< YAW_RIGHT

	//!< @n
	//!< Throttle Control
	//!< ------------------
	ZERO_THROTTLE									=32,	//!< ZERO_THROTTLE
	MAX_THROTTLE									=33,	//!< MAX_THROTTLE
	ONE_THIRD_THROTTLE								=34,	//!< ONE_THIRD_THROTTLE
	TWO_THIRDS_THROTTLE								=35,	//!< TWO_THIRDS_THROTTLE
	PLUS_5_PERCENT_THROTTLE							=36,	//!< PLUS_5_PERCENT_THROTTLE
	MINUS_5_PERCENT_THROTTLE						=37,	//!< MINUS_5_PERCENT_THROTTLE

	//!< @n
	//!< Squadmate Messaging Keys
	//!< --------------------------
	ATTACK_MESSAGE									=38,	//!< wingman message: attack current target
	DISARM_MESSAGE									=39,	//!< wingman message: disarm current target
	DISABLE_MESSAGE									=40,	//!< wingman message: disable current target
	ATTACK_SUBSYSTEM_MESSAGE						=41,	//!< wingman message: disable current target
	CAPTURE_MESSAGE									=42,	//!< wingman message: capture current target
	ENGAGE_MESSAGE									=43,	//!< wingman message: engage enemy
	FORM_MESSAGE									=44,	//!< wingman message: form on my wing
	IGNORE_MESSAGE									=45,	//!< IGNORE_MESSAGE
	PROTECT_MESSAGE									=46,	//!< wingman message: protect current target
	COVER_MESSAGE									=47,	//!< wingman message: cover me
	WARP_MESSAGE									=48,	//!< wingman message: warp out
	REARM_MESSAGE									=49,	//!< REARM_MESSAGE
	TARGET_CLOSEST_SHIP_ATTACKING_SELF				=50,	//!< target closest ship that is attacking player

	//!< @n
	//!< Views
	//!< -------
	VIEW_CHASE										=51,	//!< VIEW_CHASE
	VIEW_EXTERNAL									=52,	//!< VIEW_EXTERNAL
	VIEW_EXTERNAL_TOGGLE_CAMERA_LOCK				=53,	//!< VIEW_EXTERNAL_TOGGLE_CAMERA_LOCK
	VIEW_SLEW										=54,	//!< VIEW_SLEW
	VIEW_OTHER_SHIP									=55,	//!< VIEW_OTHER_SHIP
	VIEW_DIST_INCREASE								=56,	//!< VIEW_DIST_INCREASE
	VIEW_DIST_DECREASE								=57,	//!< VIEW_DIST_DECREASE
	VIEW_CENTER										=58,	//!< VIEW_CENTER
	PADLOCK_UP										=59,	//!< PADLOCK_UP
	PADLOCK_DOWN									=60,	//!< PADLOCK_DOWN
	PADLOCK_LEFT									=61,	//!< PADLOCK_LEFT
	PADLOCK_RIGHT									=62,	//!< PADLOCK_RIGHT

	//!< @n
	//!< Misc Controls 1
	//!< -----------------
	RADAR_RANGE_CYCLE								=63,	//!< cycle to next radar range
	SQUADMSG_MENU									=64,	//!< toggle the squadmate messaging menu
	SHOW_GOALS										=65,	//!< show the mission goals screen
	END_MISSION										=66,	//!< end the mission
	TARGET_TARGETS_TARGET							=67,	//!< target your target's target
	AFTERBURNER										=68,	//!< AFTERBURNER

	//!< @n
	//!< ETS
	//!< -----
	INCREASE_WEAPON									=69,	//!< increase weapon recharge rate
	DECREASE_WEAPON									=70,	//!< decrease weapon recharge rate
	INCREASE_SHIELD									=71,	//!< increase shield recharge rate
	DECREASE_SHIELD									=72,	//!< decrease shield recharge rate
	INCREASE_ENGINE									=73,	//!< increase energy to engines
	DECREASE_ENGINE									=74,	//!< decrease energy to engines
	ETS_EQUALIZE									=75,	//!< equalize recharge rates
	SHIELD_EQUALIZE									=76,	//!< equalize shield energy to all quadrants
	SHIELD_XFER_TOP									=77,	//!< transfer shield energy to front
	SHIELD_XFER_BOTTOM								=78,	//!< transfer shield energy to rear
	SHIELD_XFER_LEFT								=79,	//!< transfer shield energy to left
	SHIELD_XFER_RIGHT								=80,	//!< transfer shield energy to right
	XFER_SHIELD										=81,	//!< transfer energy to shield from weapons
	XFER_LASER										=82,	//!< transfer energy to weapons from shield

	//!< @n
	//!< Misc Controls 2
	//!< -----------------
	GLIDE_WHEN_PRESSED								=83, 	//!< GLIDE_WHEN_PRESSED
													//!< @remark Backslash -- this was a convenient place for Glide When Pressed, since Show Damage Popup isn't used
	BANK_WHEN_PRESSED								=84,	//!< BANK_WHEN_PRESSED
	SHOW_NAVMAP										=85,	//!< SHOW_NAVMAP
	ADD_REMOVE_ESCORT								=86,	//!< ADD_REMOVE_ESCORT
	ESCORT_CLEAR									=87,	//!< ESCORT_CLEAR

	TARGET_NEXT_ESCORT_SHIP							=88,	//!< TARGET_NEXT_ESCORT_SHIP
	TARGET_CLOSEST_REPAIR_SHIP						=89,	//!< target the closest repair ship
	TARGET_NEXT_UNINSPECTED_CARGO					=90,	//!< TARGET_NEXT_UNINSPECTED_CARGO
	TARGET_PREV_UNINSPECTED_CARGO					=91,	//!< TARGET_PREV_UNINSPECTED_CARGO
	TARGET_NEWEST_SHIP								=92,	//!< TARGET_NEWEST_SHIP

	TARGET_NEXT_LIVE_TURRET							=93, 	//!< TARGET_NEXT_LIVE_TURRET
	TARGET_PREV_LIVE_TURRET							=94,	//!< TARGET_PREV_LIVE_TURRET

	TARGET_NEXT_BOMB								=95,	//!< TARGET_NEXT_BOMB
	TARGET_PREV_BOMB								=96,	//!< TARGET_PREV_BOMB

	//!< @n
	//!< Multiplayer messaging keys
	//!< ----------------------------
	MULTI_MESSAGE_ALL								=97,	//!< message all netplayers
	MULTI_MESSAGE_FRIENDLY							=98,	//!< message all friendlies
	MULTI_MESSAGE_HOSTILE							=99,	//!< message all hostiles
	MULTI_MESSAGE_TARGET							=100,	//!< message targeted ship (if player)

	//!< @n
	//!< Multiplayer misc keys
	//!< -----------------------
	MULTI_OBSERVER_ZOOM_TO							=101,	//!< if i'm an observer, zoom to my targeted object

	TIME_SPEED_UP									=102,	//!< TIME_SPEED_UP
	TIME_SLOW_DOWN									=103,	//!< TIME_SLOW_DOWN

	TOGGLE_HUD_CONTRAST								=104,	//!< toggle between high and low HUD contrast

	MULTI_TOGGLE_NETINFO							=105,	//!< toggle network info

	MULTI_SELF_DESTRUCT								=106,	//!< self destruct (multiplayer only)

	//!< @n
	//!< Misc Controls 3
	//!< -----------------
	TOGGLE_HUD										=107,	//!< TOGGLE_HUD
	RIGHT_SLIDE_THRUST								=108,	//!< RIGHT_SLIDE_THRUST
	LEFT_SLIDE_THRUST								=109,	//!< LEFT_SLIDE_THRUST
	UP_SLIDE_THRUST									=110,	//!< UP_SLIDE_THRUST
	DOWN_SLIDE_THRUST								=111,	//!< DOWN_SLIDE_THRUST
	HUD_TARGETBOX_TOGGLE_WIREFRAME					=112,	//!< HUD_TARGETBOX_TOGGLE_WIREFRAME
	VIEW_TOPDOWN									=113,	//!< VIEW_TOPDOWN
	VIEW_TRACK_TARGET								=114,	//!< VIEW_TRACK_TARGET

	//!< @n
	//!< AutoPilot - Kazan
	//!< -------------------
	AUTO_PILOT_TOGGLE								=115,	//!< Autopilot key control
	NAV_CYCLE										=116,	//!< NAV_CYCLE

	//!< @n
	//!< Gliding
	//!< ---------
	TOGGLE_GLIDING									=117,	//!< TOGGLE_GLIDING

	//!< @n
	//!< Additional weapon controls
	//!< ----------------------------
	CYCLE_PRIMARY_WEAPON_SEQUENCE					=118,	//!< cycle num primaries to fire at once

	/*!
	 * This must always be below the last defined item
	 */
	CCFG_MAX                                  //!<  The total number of defined control actions (or last define + 1)
};


// Controller ID's for each binding
const short CID_KEYBOARD = 0;
const short CID_MOUSE = 1;
const short CID_JOY = 2;


extern int Failed_key_index;
extern int Invert_heading;
extern int Invert_pitch;
extern int Invert_roll;
extern int Invert_thrust;
extern int Disable_axis2;
extern int Disable_axis3;

extern int Axis_map_to[];
extern int Invert_axis[];
extern int Invert_axis_defaults[];

extern int Joy_dead_zone_size;
extern int Joy_sensitivity;

extern int Control_config_overlay_id;

extern Action_LUT Mouse_config;                 //!< Reverse LUT for the mouse buttons
extern SCP_vector< Action_LUT > Joy_configs;    //!< Reverse LUT for the joystick(s) buttons

extern Control_LUT Control_config;                          //!< Forward LUT for keys and buttons that the player is using.
extern SCP_vector<Control_LUT> Control_config_presets;      //!< tabled control presets; pointers to config_item arrays
extern SCP_vector<SCP_string> Control_config_preset_names;  //!< names for Control_config_presets (identical order of items)
extern const char **Scan_code_text;
extern const char **Joy_button_text;

/*!
 * @brief Initializes the controls configuration module. Call this at game startup.
 */
void control_config_common_init();

/*!
 * @brief Closes the controls configuration module. Call this at game shutdown
 */
void control_config_common_close();

/*!
* @brief Marks the control with the given Control_config index as used, and updates its timestamp
*/
void control_used(int id);

/*!
* @brief Clears all bindings set by the user. (Resets every control to their defaults)
*/
void control_config_clear();

/*!
 * @brief Clears the given key combo from any and all controls it may be bound to
 */
void clear_key_binding(short key);

/*!
 * @brief Sets all controls to default values in the given preset
 *
 * @param[in] presetnum The index in Control_config_presets[] of which preset to use
 *
 * @details If presetnum is -1 or not given, the hard-coded values are used.
 */
void control_config_reset_defaults(int presetnum=-1);

/*!
* @brief Find the first control that has c_id mapped
*
* @param[in] c_id  The cid to check
* @param[in] begin Optional. Begin iterator of the LUT
* @param[in] end   Optional. End iterator of the LUT
*
* @returns iterator to the first control with the 'c_id', or
* @returns iterator equal to 'end'
*
* @note To find the IoActionId in Control_config from the returned iterator, call std::distance(Control_config.begin(), retval)
* @note This can return a disabled control
*/
Control_LUT::iterator find_control(cid c_id, Control_LUT::iterator begin = Control_config.begin(), const Control_LUT::iterator end = Control_config.end());

/*!
 * @brief Find the first control that has c_id mapped as a default
 *
 * @param[in] c_id  The cid to check
 * @param[in] begin Optional. Begin iterator of the LUT
 * @param[in] end   Optional. End iterator of the LUT
 *
 * @returns iterator to the first control with the 'c_id', or
 * @returns iterator equal to 'end'
 *
 * @note To find the IoActionId in Control_config from the returned iterator, call std::distance(Control_config.begin(), retval)
 * @note This can return a disabled control
 */
Control_LUT::iterator find_control_default(cid c_id, Control_LUT::iterator begin = Control_config.begin(), Control_LUT::iterator end = Control_config.end());

/*!
 * @brief Translates a keycode into its Control_config index
 * @param[in] key           The keycode to translate
 * @param[in] find_override Should we find the user-overridden control (true), or its default (false)?
 *
 * @returns Index in Control_config of the control that has this keycode
 */
int translate_key_to_index(const char *key, bool find_override=true);

/*!
 * @brief Translates a system default action (via key) to the key that's currently bound to it.
 *
 * @param[in] key Human-readable string of the default system keycode
 * 
 * @returns Human-readable string of the key assigned to the action, or
 * @returns '\0' If no key is bound to the action
 */
char *translate_key(char *key);

/*!
 * @brief Returns the Human-readable string of the given key code
 */
const char *textify_scancode(int code);

/*!
 * @brief Returns the Human-reable string of the given mapping
 */
SCP_string textify(cid c_id);

/*!
 * @brief Returns the Human-readable string of the given mouse button
 */
SCP_string textify_mouse(int code);

/*!
 * @brief Returns the Human-readable string of the given joystick control
 */
SCP_string textify_joy(cid c_id);

/*!
 * @brief Returns the time a continous control has been active
 *
 * @param[in] Index in Control_config of the control to check
 *
 * @returns Time, in seconds, the control has been active, or
 * @returns 0.0f if inactive
 */
float check_control_timef(int id);

/*!
* @brief DEBUG: Displays the number of controls checked per frame
*/
void control_check_indicate();

/*!
 * @brief Wrapper for check_control_used. Allows the game to ignore the key if told to do so by the 'ignore-key' SEXP
 */
int check_control(int id, int key = -1);

/*!
 * @brief Get values for all axis actions
 */
void control_get_axes_readings(int *h, int *p, int *b, int *ta, int *tr);

/*!
 * @brief Clears the used status (timestamps) of all controls
 */
void control_config_clear_used_status();

/*!
 * @brief Applies sensitivity and deadzone curve to a reading that's directly from a joystick
 */
int joy_get_scaled_reading(int raw);

//  Menu stuff below
// vvvvvvvvvvvvvvvvvv

/*!
* @brief Inits the controls menu
*/
void control_config_init();

/**
* @brief Processes a frame of the controls menu
*/
void control_config_do_frame(float frametime);

/*!
* @brief Closes the controls menu, saving any changes
*/
void control_config_close();

/*!
* @brief Closes the controls menu, rejecting any changes
*/
void control_config_cancel_exit();

#endif
