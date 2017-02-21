/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

#include <stdio.h>
#include <stdarg.h>
#include <string>

#include "cfile/cfile.h"
#include "controlconfig/controlsconfig.h"
#include "def_files/def_files.h"
#include "globalincs/systemvars.h"
#include "globalincs/vmallocator.h"
#include "io/joy.h"
#include "io/key.h"
#include "io/mouse.h"
#include "localization/localize.h"
#include "parse/parselo.h"

using namespace io::joystick;
// z64: These enumerations MUST equal to those in controlsconfig.cpp...
// z64: Really need a better way than this.
enum CC_tab {
	TARGET_TAB			=0,
	SHIP_TAB			=1,
	WEAPON_TAB			=2,
	COMPUTER_TAB		=3
};

int Failed_key_index;

// Joystick configuration
int Joy_dead_zone_size = 10;
int Joy_sensitivity = 9;

// assume control keys are used as modifiers until we find out 
int Shift_is_modifier;
int Ctrl_is_modifier;
int Alt_is_modifier;

int Axis_enabled[JOY_NUM_AXES] = { 1, 1, 1, 0, 0, 0 };
int Axis_enabled_defaults[JOY_NUM_AXES] = { 1, 1, 1, 0, 0, 0 };
int Invert_axis[JOY_NUM_AXES] = { 0, 0, 0, 0, 0, 0 };
int Invert_axis_defaults[JOY_NUM_AXES] = { 0, 0, 0, 0, 0, 0 };

//! Holds the key mappings. See controls_config_common_init() for hard-coded bindings
Control_LUT Control_config;

const char *Scan_code_text_german[] = {
	"",				"Esc",				"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"Akzent '",				"Eszett",				"R\x81""cktaste",		"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Z",				"U",				"I",
	"O",				"P",				"\x9A",				"+",				"Eingabe",			"Strg Links",			"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				"\x99",
	"\x8E",				"`",				"Shift",			"#",				"Y",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"-",				"Shift",			"Num *",
	"Alt",				"Leertaste",			"Hochstell",			"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",				"Pause",			"Rollen",			"Num 7",
	"Num 8",			"Num 9",			"Num -",			"Num 4",			"Num 5",			"Num 6",			"Num +",			"Num 1",
	"Num 2",			"Num 3",			"Num 0",			"Num ,",			"",				"",				"",				"F11",
	"F12",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Num Eingabe",			"Strg Rechts",			"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Num /",			"",				"Druck",
	"Alt",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Num Lock",			"",				"Pos 1",
	"Pfeil Hoch",			"Bild Hoch",			"",				"Pfeil Links",			"",				"Pfeil Rechts",			"",				"Ende",
	"Pfeil Runter", 			"Bild Runter",			"Einfg",			"Entf",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

const char *Joy_button_text_german[] = {
	"Knopf 1",		"Knopf 2",		"Knopf 3",		"Knopf 4",		"Knopf 5",		"Knopf 6",
	"Knopf 7",		"Knopf 8",		"Knopf 9",		"Knopf 10",		"Knopf 11",		"Knopf 12",
	"Knopf 13",		"Knopf 14",		"Knopf 15",		"Knopf 16",		"Knopf 17",		"Knopf 18",
	"Knopf 19",		"Knopf 20",		"Knopf 21",		"Knopf 22",		"Knopf 23",		"Knopf 24",
	"Knopf 25",		"Knopf 26",		"Knopf 27",		"Knopf 28",		"Knopf 29",		"Knopf 30",
	"Knopf 31",		"Knopf 32",		"Hut Hinten",	"Hut Vorne",	"Hut Links",	"Hut Rechts"
};

const char *Scan_code_text_french[] = {
	"",				"\x90""chap",			"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"-",				"=",				"Fl\x82""che Ret.",			"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Y",				"U",				"I",
	"O",				"P",				"[",				"]",				"Entr\x82""e",			"Ctrl Gauche",			"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				";",
	"'",				"`",				"Maj.",			"\\",				"Z",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"/",				"Maj.",			"Pav\x82 *",
	"Alt",				"Espace",			"Verr. Maj.",			"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",				"Pause",			"Arret defil",		"Pav\x82 7",
	"Pav\x82 8",			"Pav\x82 9",			"Pav\x82 -",			"Pav\x82 4",			"Pav\x82 5",			"Pav\x82 6",			"Pav\x82 +",			"Pav\x82 1",
	"Pav\x82 2",			"Pav\x82 3",			"Pav\x82 0",			"Pav\x82 .",			"",				"",				"",				"F11",
	"F12",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Pav\x82 Entr",			"Ctrl Droite",		"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Pav\x82 /",			"",				"Impr \x82""cran",
	"Alt",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Verr num",			"",				"Orig.",
	"Fl\x82""che Haut",			"Page Haut",			"",				"Fl\x82""che Gauche",			"",				"Fl\x82""che Droite",			"",			"Fin",
	"Fl\x82""che Bas", 			"Page Bas",			"Inser",			"Suppr",			"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

const char *Joy_button_text_french[] = {
	"Bouton 1",		"Bouton 2",		"Bouton 3",		"Bouton 4",		"Bouton 5",		"Bouton 6",
	"Bouton 7",		"Bouton 8",		"Bouton 9",		"Bouton 10",		"Bouton 11",		"Bouton 12",
	"Bouton 13",		"Bouton 14",		"Bouton 15",		"Bouton 16",		"Bouton 17",		"Bouton 18",
	"Bouton 19",		"Bouton 20",		"Bouton 21",		"Bouton 22",		"Bouton 23",		"Bouton 24",
	"Bouton 25",		"Bouton 26",		"Bouton 27",		"Bouton 28",		"Bouton 29",		"Bouton 30",
	"Bouton 31",		"Bouton 32",		"Chapeau Arri\x8Are",		"Chapeau Avant",		"Chapeau Gauche",		"Chapeau Droite"
};

const char *Scan_code_text_polish[] = {
	"",				"Esc",			"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"-",				"=",				"Backspace",	"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Y",				"U",				"I",
	"O",				"P",				"[",				"]",				"Enter",			"Lewy Ctrl",	"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				";",
	"'",				"`",				"LShift",			"\\",				"Z",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"/",				"PShift",			"Num *",
	"Alt",			"Spacja",		"CapsLock",	"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",			"Pause",			"Scrlock",	"Num 7",
	"Num 8",			"Num 9",			"Num -",			"Num 4",			"Num 5",			"Num 6",			"Num +",			"Num 1",
	"Num 2",			"Num 3",			"Num 0",			"Num .",			"",				"",				"",				"F11",
	"F12",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Num Enter",	"Prawy Ctrl",	"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Num /",			"",				"PrntScr",
	"Alt",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Num Lock",		"",				"Home",
	"Kursor G\xF3ra",		"Page Up",		"",				"Kursor Lewo",	"",				"Kursor Prawo",	"",				"End",
	"Kursor D\xF3\xB3",  "Page Down",	"Insert",		"Delete",		"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

const char *Joy_button_text_polish[] = {
	"Przyc.1",		"Przyc.2",		"Przyc.3",		"Przyc.4",		"Przyc.5",		"Przyc.6",
	"Przyc.7",		"Przyc.8",		"Przyc.9",		"Przyc.10",	"Przyc.11",	"Przyc.12",
	"Przyc.13",	"Przyc.14",	"Przyc.15",	"Przyc.16",	"Przyc.17",	"Przyc.18",
	"Przyc.19",	"Przyc.20",	"Przyc.21",	"Przyc.22",	"Przyc.23",	"Przyc.24",
	"Przyc.25",	"Przyc.26",	"Przyc.27",	"Przyc.28",	"Przyc.29",	"Przyc.30",
	"Przyc.31",	"Przyc.32",	"Hat Ty\xB3",		"Hat Prz\xF3\x64",	"Hat Lewo",		"Hat Prawo"
};

//!	This is the text that is displayed on the screen for the keys a player selects
const char *Scan_code_text_english[] = {
	"",				"Esc",			"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"-",				"=",				"Backspace",	"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Y",				"U",				"I",
	"O",				"P",				"[",				"]",				"Enter",			"Left Ctrl",	"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				";",
	"'",				"`",				"Shift",			"\\",				"Z",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"/",				"Shift",			"Pad *",
	"Alt",			"Spacebar",		"Caps Lock",	"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",			"Pause",			"Scroll Lock",	"Pad 7",
	"Pad 8",			"Pad 9",			"Pad -",			"Pad 4",			"Pad 5",			"Pad 6",			"Pad +",			"Pad 1",
	"Pad 2",			"Pad 3",			"Pad 0",			"Pad .",			"",				"",				"",				"F11",
	"F12",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Pad Enter",	"Right Ctrl",	"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Pad /",			"",				"Print Scrn",
	"Alt",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Num Lock",		"",				"Home",
	"Up Arrow",		"Page Up",		"",				"Left Arrow",	"",				"Right Arrow",	"",				"End",
	"Down Arrow",  "Page Down",	"Insert",		"Delete",		"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

const char *Joy_button_text_english[] = {
	"Button 1",		"Button 2",		"Button 3",		"Button 4",		"Button 5",		"Button 6",
	"Button 7",		"Button 8",		"Button 9",		"Button 10",	"Button 11",	"Button 12",
	"Button 13",	"Button 14",	"Button 15",	"Button 16",	"Button 17",	"Button 18",
	"Button 19",	"Button 20",	"Button 21",	"Button 22",	"Button 23",	"Button 24",
	"Button 25",	"Button 26",	"Button 27",	"Button 28",	"Button 29",	"Button 30",
	"Button 31",	"Button 32",	"Hat Back",		"Hat Forward",	"Hat Left",		"Hat Right"
};

const char **Scan_code_text = Scan_code_text_english;
const char **Joy_button_text = Joy_button_text_english;

SCP_vector<Preset_table> Control_config_presets;

SCP_map<SCP_string, int> mKeyNameToVal;
SCP_map<SCP_string, int> mMouseNameToVal;
SCP_map<SCP_string, CC_type> mCCTypeNameToVal;
SCP_map<SCP_string, char> mCCTabNameToVal;

/*!
 * @brief Parses controlconfigdefault.tbl, and overrides the default control configuration for each valid entry in the .tbl
 */
void control_config_common_load_overrides();

/*!
 * @brief Helper function to LoadEnumsIntoMaps(), Loads the Keyboard definitions/enumerations into mKeyNameToVal
 */
void LoadEnumsIntoKeyMap(void);

/*!
 * @breif Helpfer function to LoadEnumsIntoMaps(), Loads the Mouse definitions/enumarations into mMouseNameToVal
 */
void LoadEnumsIntoMouseMap(void);

/*!
 * @brief Helper function to LoadEnumsIntoMaps(), Loads the Control Types enumerations into mCCTypeNameToVal
 */
void LoadEnumsIntoCCTypeMap(void);

/*! 
 * @brief Helper function to LoadEnumsIntoMaps(), Loads the Control Tabs enumerations into mCCTabNameToVal
 */
void LoadEnumsIntoCCTabMap(void);

/*!
 * @brief Loads the various control configuration maps.
 *
 * @details This allows the parsing functions to appropriately map string tokns to their associated enumerations. The
 *   string tokens in the controlconfigdefaults.tbl match directly to their names in the C++ code, such as "KEY_5" in 
 *   the .tbl mapping to the #define KEY_5 value
 */
void LoadEnumsIntoMaps();

/*!
 * @brief Function used to sort bindings within Config_item's
 */
bool compare_binds(cid &left, cid &right);


Preset_table::Preset_table(SCP_string &_name, Control_LUT& preset)
	: name(_name)
{
	size_t size = preset.size();
	Assert(size >= CCFG_MAX);

	action.resize(size);

	std::copy(preset.begin(), preset.end(), action.begin());
}


bool  Config_item::bind(cid &control) {
	if (control == cid(-1, -1)) {
		return false;
	}

	for (uint i = 0; i < MAX_BINDINGS; ++i) {
		if (control.first == c_id[i].first) {
			// Ok, we have a controller already bound. Map to the new button
			c_id[i].second = control.second;
			return true;

		} else if (c_id[i].first == -1) {
			// Found an empty slot, use it
			c_id[i] = control;
			cleanup();
			return true;
		} // Else, check the next slot
	}

	// Could not find an available slot
	return false;
}

void Config_item::cleanup(bool defaults) {
	if (defaults) {
		std::sort(default_id, default_id + MAX_BINDINGS, compare_binds);

	} else {
		std::sort(c_id, c_id + MAX_BINDINGS, compare_binds);
	}
}

void Config_item::clear() {
	std::fill(c_id, c_id + MAX_BINDINGS, cid(-1, -1));
}

Config_item::Config_item()
	: used(-1), type(0), disabled(true), continuous_ongoing(false), tab(0), hasXSTR(false), text("Non-initialized Control")
{
	default_id[0] = cid(-1, -1);
	default_id[1] = cid(-1, -1);
	default_id[2] = cid(-1, -1);

	std::copy(default_id, default_id + MAX_BINDINGS, c_id);
}

Config_item::Config_item(short default0, short default1, short default2, char type, bool disabled, char tab, bool hasXSTR, const char* text)
	: used(-1), type(type), disabled(disabled), continuous_ongoing(false), tab(tab), hasXSTR(hasXSTR), text(text)
{
	default_id[0] = cid(CID_KEYBOARD, default0);

	if (default1 != -1) {
		default_id[1] = cid(CID_MOUSE, default1);
	} else {
		default_id[1] = cid(-1, -1);
	}

	if (default2 != -1) {
		default_id[2] = cid(CID_JOY, default2);
	} else {
		default_id[2] = cid(-1, -1);
	}

	cleanup(true);
	std::copy(default_id, default_id + MAX_BINDINGS, c_id);
}

bool Config_item::empty() {
	for (uint i = 0; i < MAX_BINDINGS; ++i) {
		if (c_id[i].first != -1) {
			return false;
		}
	}

	return true;
}

bool Config_item::defaults() {
	for (uint i = 0; i < MAX_BINDINGS; ++i) {
		if (c_id[i] != default_id[i]) {
			return false;
		}
	}

	return true;
}

int Config_item::find_bind(cid &control) {
	for (int i = 0; i < MAX_BINDINGS; ++i) {
		if (c_id[i] == control) {
			return i;
		}
	}

	return -1;
}

void Config_item::reset() {
	std::copy(default_id, default_id + MAX_BINDINGS, c_id);
}

bool Config_item::unbind(cid &control) {
	for (uint i = 0; i < MAX_BINDINGS; ++i) {
		if ((c_id[i].first == control.first) &&
			((c_id[i].second == control.second) || (control.second == -1))) {
			c_id[i] = cid(-1, -1);
			return true;
		}
	}

	return false;
}

bool Config_item::unbind(uint id) {
	Assert((MAX_BINDINGS > id) && (id >= 0));

	if (c_id[id].first == -1) {
		return false;

	} else {
		c_id[id] = cid(-1, -1);
		return true;
	}
}


config_item_loader::config_item_loader(Control_LUT *dest)
	: dest(dest) {};

config_item_loader& config_item_loader::operator()(char tab, const char* text, bool hasXSTR, char type, bool disabled, short default0, short default1, short default2)
{
	dest->emplace_back(Config_item(default0, default1, default2, type, disabled, tab, hasXSTR, text));
	return *this;
}

config_item_loader& config_item_loader::operator()(char tab, const char* text, bool hasXSTR, char type, bool disabled, short default0, short default1)
{
	return this->operator()(tab, text, hasXSTR, type, disabled, default0, default1, -1);
}


config_item_loader& config_item_loader::operator()(char tab, const char* text, bool hasXSTR, char type, bool disabled, short default0)
{
	return this->operator()(tab, text, hasXSTR, type, disabled, default0, -1, -1);
}

bool compare_binds(cid &left, cid &right) {
	if (left.first == -1) {
		return false;

	} else if (right.first == -1) {
		return true;

	} else if (left.first < right.first) {
		return true;

	} else if ((left.first == right.first) && (left.second < right.second)) {
		return true;
	}

	return false;
}

int translate_key_to_index(const char *key, bool find_override)
{
	int i, index = -1, alt = 0, shift = 0, max_scan_codes;

	max_scan_codes = sizeof(Scan_code_text_english) / sizeof(char *);

	// look for modifiers
	Assert(key);
	if (!strnicmp(key, "Alt", 3)) {
		alt = 1;
		key += 3;
		if (*key)
			key++;
	}

	if (!strnicmp(key, "Shift", 5)) {
		shift = 1;
		key += 5;
		if (*key)
			key++;
	}

	// look up index for default key
	if (*key) {
		for (i=0; i<max_scan_codes; i++)
			if (!stricmp(key, Scan_code_text_english[i])) {
				index = i;
				break;
			}

		if (i == max_scan_codes)
			return -1;

		if (shift)
			index |= KEY_SHIFTED;
		if (alt)
			index |= KEY_ALTED;

		// convert scancode to Control_config index
		auto it = Control_config.begin();

		// Find the first active match
		if (find_override) {
			while (it != Control_config.end()) {
				it = find_control(cid(CID_KEYBOARD, index), it);

				if (!it->disabled) {
					break;
				}
			}
		} else {
			while (it != Control_config.end()) {
				it = find_control_default(cid(CID_KEYBOARD, index), it);

				if (!it->disabled) {
					break;
				}
			}
		}

		if (it == Control_config.end())
			return -1;

		return std::distance(Control_config.begin(), it);
	}

	return -1;
}

char *translate_key(char *key)
{
	int index = -1, key_code = -1, joy_code = -1;
	const char *key_text = NULL;
	const char *joy_text = NULL;

	static char text[40] = {"None"};	// TODO Replace this with an SCP_string
	SCP_string temp;

	index = translate_key_to_index(key, false);
	if (index < 0) {
		return NULL;
	}

	cid *c_id = Control_config[index].c_id;

	int count = 0;	// Number of bindings for this action
	for (auto i = 0; i < MAX_BINDINGS; ++i) {
		if (c_id[i].first != -1)
			count++;
	}

	if (count == 1) {
		temp = textify(c_id[0]);

	} else if (count == 2) {
		temp = textify(c_id[0]) + " or " + textify(c_id[1]);

	} else if (count > 2) {

		for (auto i = 0; i < MAX_BINDINGS; ++i) {
			temp += textify(c_id[i]);
			count -= 1;

			if (count > 1) {
				temp += ", ";

			} else if (count == 1) {
				temp += ", or ";

			} else if (count == 0) {
				break;
			}
		}
	} else {
		// Nothing bound
		temp = "None";
	}

	Failed_key_index = index;

	strcpy_s(text, temp.c_str());

	return text;
}

SCP_string textify(cid c_id)
{
	SCP_string retval;

	if (c_id.first == CID_KEYBOARD) {
		retval.assign(textify_scancode(c_id.second));

	} else if (c_id.first == CID_MOUSE) {
		retval = textify_mouse(c_id.second);

	} else if (c_id.first >= CID_JOY) {
		retval = textify_joy(c_id);
	}

	return retval;
}

SCP_string textify_mouse(int code)
{
	SCP_string retval;

	switch (code) {
	case MOUSE_LEFT_BUTTON:
		retval = "L MB";
		break;
	case MOUSE_RIGHT_BUTTON:
		retval = "R MB";
		break;
	case MOUSE_MIDDLE_BUTTON:
		retval = "M MB";
		break;
	case MOUSE_X1_BUTTON:
		retval = "X1 MB";
		break;
	case MOUSE_X2_BUTTON:
		retval += "X2 MB";
		break;
	case MOUSE_WHEEL_UP:
		retval += "MW Up";
		break;
	case MOUSE_WHEEL_DOWN:
		retval += "MW Down";
		break;
	case MOUSE_WHEEL_LEFT:
		retval += "MW Left";
		break;
	case MOUSE_WHEEL_RIGHT:
		retval += "MW Right";
		break;
	default:
		// Shouldn't happen
		Assertion(false, "Unknown mouse button!");
		retval += "Unknown MB";
		break;
	}
}

SCP_string textify_joy(cid c_id)
{
	SCP_string retval;

	// TODO: Get the string name per joystick. For now we'll just use Joy0, Joy1, etc.
	int joy = c_id.first -= CID_JOY;
	int button = c_id.second;

	retval = "Joy";
	retval += std::to_string(c_id.second - CID_JOY);

	if ((button < 0) || (button >= JOY_TOTAL_BUTTONS)) {
		// Shouldn't happen
		Assertion(false, "textify_joy: Unknown joystick button id.");
		retval += "Button Unknown";

	} else if (button >= JOY_NUM_BUTTONS) {
		// Is hat
		retval += " Hat ";
		switch (button) {
		case JOY_NUM_BUTTONS + HAT_UP:
			retval += "Up";
			break;
		case JOY_NUM_BUTTONS + HAT_DOWN:
			retval += "Down";
			break;
		case JOY_NUM_BUTTONS + HAT_LEFT:
			retval += "Left";
			break;
		case JOY_NUM_BUTTONS + HAT_RIGHT:
			retval += "Right";
			break;
		default:
			// Shouldn't happen
			Assertion(false, "textify_joy: Unknown hat position.");
			retval += "Unknown";
			break;
		}

	} else {
		// (button >= 0) && (button < JOY_NUM_BUTTONS)
		// Is button
		retval += std::to_string(button);
	}
	
	return retval;
}

const char *textify_scancode(int code)
{
	static char text[40];

	if (code < 0)
		return "None";

	int keycode = code & KEY_MASK;

	*text = 0;
	if (code & KEY_ALTED && !(keycode == KEY_LALT || keycode == KEY_RALT)) {
		if(Lcl_gr){		
			strcat_s(text, "Alt-");
		} else if(Lcl_fr){		
			strcat_s(text, "Alt-");
		} else {		
			strcat_s(text, "Alt-");
		}		
	}

	if (code & KEY_SHIFTED && !(keycode == KEY_LSHIFT || keycode == KEY_RSHIFT)) {		
		if(Lcl_gr){
			strcat_s(text, "Shift-");
		} else if(Lcl_fr){		
			strcat_s(text, "Maj.-");
		} else {		
			strcat_s(text, "Shift-");
		}
	}

	strcat_s(text, Scan_code_text[keycode]);
	return text;
}

void control_config_common_init()
{
	Control_config.reserve(CCFG_MAX);
	config_item_loader config_item_loader(&Control_config);
	config_item_loader
		// Tab,      "Put Description Here"                   XSTR, type,            disabled, keyboard,                     M, J
		// Targeting a ship
		(TARGET_TAB, "Target Next Ship",                      true, CC_TYPE_TRIGGER, false,                           KEY_T)
		(TARGET_TAB, "Target Previous Ship",                  true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_T)
		(TARGET_TAB, "Target Next Closest Hostile Ship",      true, CC_TYPE_TRIGGER, false,                           KEY_H, 2, 2)
		(TARGET_TAB, "Target Previous Closest Hostile Ship",  true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_H)
		(TARGET_TAB, "Toggle Auto Targeting",                 true, CC_TYPE_TRIGGER, false, KEY_ALTED |               KEY_H)
		(TARGET_TAB, "Target Next Closest Friendly Ship",     true, CC_TYPE_TRIGGER, false,                           KEY_F)
		(TARGET_TAB, "Target Previous Closest Friendly Ship", true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_F)
		(TARGET_TAB, "Target Ship in Reticle",                true, CC_TYPE_TRIGGER, false,                           KEY_Y, -1, 4)
		(TARGET_TAB, "Target Target's Nearest Attacker",      true, CC_TYPE_TRIGGER, false,                           KEY_G)
		(TARGET_TAB, "Target Last Ship to Send Transmission", true, CC_TYPE_TRIGGER, false, KEY_ALTED |               KEY_Y)
		(TARGET_TAB, "Turn Off Targeting",                    true, CC_TYPE_TRIGGER, false, KEY_ALTED |               KEY_T)

		// Targeting a subsystem
		(TARGET_TAB, "Target Subsystem in Reticle",      true, CC_TYPE_TRIGGER, false,                           KEY_V)
		(TARGET_TAB, "Target Next Subsystem",            true, CC_TYPE_TRIGGER, false,                           KEY_S)
		(TARGET_TAB, "Target Previous Subsystem",        true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_S)
		(TARGET_TAB, "Turn Off Targeting of Subsystems", true, CC_TYPE_TRIGGER, false, KEY_ALTED |               KEY_S)

		// Speed matching
		(COMPUTER_TAB, "Match Target Speed",         true, CC_TYPE_TRIGGER, false,             KEY_M)
		(COMPUTER_TAB, "Toggle Auto Speed Matching", true, CC_TYPE_TRIGGER, false, KEY_ALTED | KEY_M)

		// Weapons
		(WEAPON_TAB, "Fire Primary Weapon",                true, CC_TYPE_CONTINUOUS, false,            KEY_LCTRL,     0, 0)
		(WEAPON_TAB, "Fire Secondary Weapon",              true, CC_TYPE_CONTINUOUS, false,            KEY_SPACEBAR,  1, 1)
		(WEAPON_TAB, "Cycle Forward Primary Weapon",       true, CC_TYPE_TRIGGER, false,               KEY_PERIOD)
		(WEAPON_TAB, "Cycle Backward Primary Weapon",      true, CC_TYPE_TRIGGER, false,               KEY_COMMA)
		(WEAPON_TAB, "Cycle Secondary Weapon Bank",        true, CC_TYPE_TRIGGER, false,               KEY_DIVIDE)
		(WEAPON_TAB, "Cycle Secondary Weapon Firing Rate", true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_DIVIDE)
		(WEAPON_TAB, "Launch Countermeasure",              true, CC_TYPE_TRIGGER, false,               KEY_X,        -1, 3)

		// Flight controls
		(SHIP_TAB,   "Forward Thrust", true, CC_TYPE_CONTINUOUS, false, KEY_A)
		(SHIP_TAB,   "Reverse Thrust", true, CC_TYPE_CONTINUOUS, false, KEY_Z)
		(SHIP_TAB,   "Bank Left",      true, CC_TYPE_CONTINUOUS, false, KEY_PAD7)
		(SHIP_TAB,   "Bank Right",     true, CC_TYPE_CONTINUOUS, false, KEY_PAD9)
		(SHIP_TAB,   "Pitch Forward",  true, CC_TYPE_CONTINUOUS, false, KEY_PAD8)
		(SHIP_TAB,   "Pitch Backward", true, CC_TYPE_CONTINUOUS, false, KEY_PAD2)
		(SHIP_TAB,   "Turn Left",      true, CC_TYPE_CONTINUOUS, false, KEY_PAD4)
		(SHIP_TAB,   "Turn Right",     true, CC_TYPE_CONTINUOUS, false, KEY_PAD6)

		// Throttle
		(SHIP_TAB,   "Set Throttle to Zero",        true, CC_TYPE_TRIGGER, false, KEY_BACKSP)
		(SHIP_TAB,   "Set Throttle to Max",         true, CC_TYPE_TRIGGER, false, KEY_SLASH)
		(SHIP_TAB,   "Set Throttle to One-Third",   true, CC_TYPE_TRIGGER, false, KEY_LBRACKET)
		(SHIP_TAB,   "Set Throttle to Two-Thirds",  true, CC_TYPE_TRIGGER, false, KEY_RBRACKET)
		(SHIP_TAB,   "Increase Throttle 5 Percent", true, CC_TYPE_TRIGGER, false, KEY_EQUAL)
		(SHIP_TAB,   "Decrease Throttle 5 Percent", true, CC_TYPE_TRIGGER, false, KEY_MINUS)

		// Squadmate messaging
		(COMPUTER_TAB, "Attack My Target",    true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_A)
		(COMPUTER_TAB, "Disarm My Target",    true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_Z)
		(COMPUTER_TAB, "Disable My Target",   true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_D)
		(COMPUTER_TAB, "Attack My Subsystem", true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_V)
		(COMPUTER_TAB, "Capture My Target",   true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_X)
		(COMPUTER_TAB, "Engage Enemy",        true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_E)
		(COMPUTER_TAB, "Form on My Wing",     true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_W)
		(COMPUTER_TAB, "Ignore My Target",    true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_I)
		(COMPUTER_TAB, "Protect My Target",   true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_P)
		(COMPUTER_TAB, "Cover Me",            true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_C)
		(COMPUTER_TAB, "Return to Base",      true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_J)
		(COMPUTER_TAB, "Rearm Me",            true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_R)

		(TARGET_TAB, "Target Closest Attacking Ship", true, CC_TYPE_TRIGGER, false, KEY_R, -1, 6)

		// Views
		(COMPUTER_TAB, "Chase View",                  true, CC_TYPE_TRIGGER,    false, KEY_PADMULTIPLY)
		(COMPUTER_TAB, "External View",               true, CC_TYPE_TRIGGER,    false, KEY_PADPERIOD)
		(COMPUTER_TAB, "Toggle External Camera Lock", true, CC_TYPE_TRIGGER,    false, KEY_PADENTER)
		(COMPUTER_TAB, "Free Look View",              true, CC_TYPE_CONTINUOUS, false, KEY_PAD0)
		(COMPUTER_TAB, "Current Target View",         true, CC_TYPE_TRIGGER,    false, KEY_PADDIVIDE)
		(COMPUTER_TAB, "Increase View Distance",      true, CC_TYPE_CONTINUOUS, false, KEY_PADPLUS)
		(COMPUTER_TAB, "Decrease View Distance",      true, CC_TYPE_CONTINUOUS, false, KEY_PADMINUS)
		(COMPUTER_TAB, "Center View",                 true, CC_TYPE_CONTINUOUS, false, KEY_PAD5)
		(COMPUTER_TAB, "View Up",                     true, CC_TYPE_CONTINUOUS, false, -1,             -1, 33)
		(COMPUTER_TAB, "View Rear",                   true, CC_TYPE_CONTINUOUS, false, -1,             -1, 32)
		(COMPUTER_TAB, "View Left",                   true, CC_TYPE_CONTINUOUS, false, -1,             -1, 34)
		(COMPUTER_TAB, "View Right",                  true, CC_TYPE_CONTINUOUS, false, -1,             -1, 35)

		(COMPUTER_TAB, "Cycle Radar Range",            true, CC_TYPE_TRIGGER,    false,             KEY_RAPOSTRO)
		(COMPUTER_TAB, "Communications Menu",          true, CC_TYPE_TRIGGER,    false,             KEY_C)
		(-1,           "Show Objectives",              true, CC_TYPE_TRIGGER,    false,             -1)
		(COMPUTER_TAB, "Enter Subspace (End Mission)", true, CC_TYPE_TRIGGER,    false, KEY_ALTED | KEY_J)
		(TARGET_TAB,   "Target Target's Target",       true, CC_TYPE_TRIGGER,    false,             KEY_J)
		(SHIP_TAB,     "Afterburner",                  true, CC_TYPE_CONTINUOUS, false,             KEY_TAB,     -1, 5)

		(COMPUTER_TAB, "Increase Weapon Energy",   true, CC_TYPE_TRIGGER, false,             KEY_INSERT)
		(COMPUTER_TAB, "Decrease Weapon Energy",   true, CC_TYPE_TRIGGER, false,             KEY_DELETE)
		(COMPUTER_TAB, "Increase Shield Energy",   true, CC_TYPE_TRIGGER, false,             KEY_HOME)
		(COMPUTER_TAB, "Decrease Shield Energy",   true, CC_TYPE_TRIGGER, false,             KEY_END)
		(COMPUTER_TAB, "Increase Engine Energy",   true, CC_TYPE_TRIGGER, false,             KEY_PAGEUP)
		(COMPUTER_TAB, "Decrease Engine Energy",   true, CC_TYPE_TRIGGER, false,             KEY_PAGEDOWN)
		(COMPUTER_TAB, "Equalize Energy Settings", true, CC_TYPE_TRIGGER, false, KEY_ALTED | KEY_D)

		(COMPUTER_TAB, "Equalize Shields",              true, CC_TYPE_TRIGGER, false,               KEY_Q, -1, 7)
		(COMPUTER_TAB, "Augment Forward Shield",        true, CC_TYPE_TRIGGER, false,               KEY_UP)
		(COMPUTER_TAB, "Augment Rear Shield",           true, CC_TYPE_TRIGGER, false,               KEY_DOWN)
		(COMPUTER_TAB, "Augment Left Shield",           true, CC_TYPE_TRIGGER, false,               KEY_LEFT)
		(COMPUTER_TAB, "Augment Right Shield",          true, CC_TYPE_TRIGGER, false,               KEY_RIGHT)
		(COMPUTER_TAB, "Transfer Energy Laser->Shield", true, CC_TYPE_TRIGGER, false,               KEY_SCROLLOCK)
		(COMPUTER_TAB, "Transfer Energy Shield->Laser", true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_SCROLLOCK)

		(SHIP_TAB,     "Glide When Pressed",         false, CC_TYPE_CONTINUOUS, false, -1)
		(SHIP_TAB,     "Bank When Pressed",          true,  CC_TYPE_CONTINUOUS, false, -1)
		(      -1,     "Show Nav Map",               true,  CC_TYPE_TRIGGER,    false, -1)
		(COMPUTER_TAB, "Add or Remove Escort",       true,  CC_TYPE_TRIGGER,    false, KEY_ALTED |               KEY_E)
		(COMPUTER_TAB, "Clear Escort List",          true,  CC_TYPE_TRIGGER,    false, KEY_ALTED | KEY_SHIFTED | KEY_E)
		(TARGET_TAB,   "Target Next Escort Ship",    true,  CC_TYPE_TRIGGER,    false,                           KEY_E)
		(TARGET_TAB,   "Target Closest Repair Ship", true,  CC_TYPE_TRIGGER,    false, KEY_ALTED |               KEY_R)

		(TARGET_TAB, "Target Next Uninspected Cargo",     true, CC_TYPE_TRIGGER, false,               KEY_U)
		(TARGET_TAB, "Target Previous Uninspected Cargo", true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_U)
		(TARGET_TAB, "Target Newest Ship in Area",        true, CC_TYPE_TRIGGER, false,               KEY_N)
		(TARGET_TAB, "Target Next Live Turret",           true, CC_TYPE_TRIGGER, false,               KEY_K)
		(TARGET_TAB, "Target Previous Live Turret",       true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_K)

		(TARGET_TAB, "Target Next Hostile Bomb or Bomber",     true, CC_TYPE_TRIGGER, false,             KEY_B)
		(TARGET_TAB, "Target Previous Hostile Bomb or Bomber", true, CC_TYPE_TRIGGER, false, KEY_SHIFTED | KEY_B)

		// Multiplayer messaging
		(COMPUTER_TAB, "(Multiplayer) Message All",             true, CC_TYPE_TRIGGER, false,                           KEY_1)
		(COMPUTER_TAB, "(Multiplayer) Message Friendly",        true, CC_TYPE_TRIGGER, false,                           KEY_2)
		(COMPUTER_TAB, "(Multiplayer) Message Hostile",         true, CC_TYPE_TRIGGER, false,                           KEY_3)
		(COMPUTER_TAB, "(Multiplayer) Message Target",          true, CC_TYPE_TRIGGER, false,                           KEY_4)
		(COMPUTER_TAB, "(Multiplayer) Observer Zoom to Target", true, CC_TYPE_TRIGGER, false, KEY_ALTED |               KEY_X)
		(COMPUTER_TAB, "Increase Time Compression",             true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_PERIOD)
		(COMPUTER_TAB, "Decrease Time Compression",             true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_COMMA)
		(COMPUTER_TAB, "Toggle High HUD Contrast",              true, CC_TYPE_TRIGGER, false,                           KEY_L)
		(COMPUTER_TAB, "(Multiplayer) Toggle Network Info",     true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_N)
		(COMPUTER_TAB, "(Multiplayer) Self Destruct",           true, CC_TYPE_TRIGGER, false,             KEY_SHIFTED | KEY_END)

		// Misc
		(COMPUTER_TAB, "Toggle HUD",                       true,  CC_TYPE_TRIGGER,    false,             KEY_SHIFTED | KEY_O)
		(SHIP_TAB,     "Right Thrust",                     true,  CC_TYPE_CONTINUOUS, false,             KEY_SHIFTED | KEY_3)
		(SHIP_TAB,     "Left Thrust",                      true,  CC_TYPE_CONTINUOUS, false,             KEY_SHIFTED | KEY_1)
		(SHIP_TAB,     "Up Thrust",                        true,  CC_TYPE_CONTINUOUS, false,             KEY_SHIFTED | KEY_PADPLUS)
		(SHIP_TAB,     "Down Thrust",                      true,  CC_TYPE_CONTINUOUS, false,             KEY_SHIFTED | KEY_PADENTER)
		(COMPUTER_TAB, "Toggle HUD Wireframe Target View", true,  CC_TYPE_TRIGGER,    false, KEY_ALTED | KEY_SHIFTED | KEY_Q)
		(COMPUTER_TAB, "Top-Down View",                    false, CC_TYPE_TRIGGER,    false,                           -1)
		(COMPUTER_TAB, "Target Padlock View",              false, CC_TYPE_TRIGGER,    false,                           -1)

		// Auto navigation
		(COMPUTER_TAB, "Toggle Auto Pilot",                false, CC_TYPE_TRIGGER, false, KEY_ALTED | KEY_A)
		(COMPUTER_TAB, "Cycle Nav Points",                 false, CC_TYPE_TRIGGER, false, KEY_ALTED | KEY_N)
		(SHIP_TAB,     "Toggle Gliding",                   false, CC_TYPE_TRIGGER, false, KEY_ALTED | KEY_G)
		(WEAPON_TAB,   "Cycle Primary Weapon Firing Rate", false, CC_TYPE_TRIGGER, false,             KEY_O)
	;

	control_config_common_load_overrides();
	if(Lcl_gr){
		Scan_code_text = Scan_code_text_german;
		Joy_button_text = Joy_button_text_german;
	} else if(Lcl_fr){
		Scan_code_text = Scan_code_text_french;
		Joy_button_text = Joy_button_text_french;
	} else if(Lcl_pl){
		Scan_code_text = Scan_code_text_polish;
		Joy_button_text = Joy_button_text_polish;
	} else {
		Scan_code_text = Scan_code_text_english;
		Joy_button_text = Joy_button_text_english;
	}
}

void control_config_common_close()
{
	// only need to worry control presets for now
	// z64555: Don't need to worry about control presets anymore, they've been classified. D:
}

Control_LUT::iterator find_control(cid c_id, Control_LUT::iterator begin, Control_LUT::iterator end)
{
	auto it = begin;
	for (; it != end; ++it) {
		for (size_t i = 0; i < MAX_BINDINGS; ++i) {
			if (it->c_id[i] == c_id) {
				// Found ya!
				return it;
			}

			if (c_id.first == CID_KEYBOARD) {
				// Keyabord mappings are always first
				continue;
			}
		}
	}

	return end;
}

Control_LUT::iterator find_control_default(cid c_id, Control_LUT::iterator begin, Control_LUT::iterator end) {
	auto it = begin;
	for (; it != end; ++it) {
		for (size_t i = 0; i < MAX_BINDINGS; ++i) {
			if (it->default_id[i] == c_id) {
				// Found ya!
				return it;
			}

			if (c_id.first == CID_KEYBOARD) {
				// Keyabord mappings are always first
				continue;
			}
		}
	}

	return end;
}

void LoadEnumsIntoKeyMap(void) {
	// Dirty macro hack :D
#define ADD_ENUM_TO_KEY_MAP(Enum) mKeyNameToVal[#Enum] = (Enum);

	ADD_ENUM_TO_KEY_MAP(KEY_SHIFTED)
		/*
		ADD_ENUM_TO_KEY_MAP(KEY_ALTED)
		ADD_ENUM_TO_KEY_MAP(KEY_CTRLED)
		ADD_ENUM_TO_KEY_MAP(KEY_DEBUGGED)
		ADD_ENUM_TO_KEY_MAP(KEY_DEBUGGED1)
		ADD_ENUM_TO_KEY_MAP(KEY_MASK)

		ADD_ENUM_TO_KEY_MAP(KEY_DEBUG_KEY)
		*/
		ADD_ENUM_TO_KEY_MAP(KEY_0)
		ADD_ENUM_TO_KEY_MAP(KEY_1)
		ADD_ENUM_TO_KEY_MAP(KEY_2)
		ADD_ENUM_TO_KEY_MAP(KEY_3)
		ADD_ENUM_TO_KEY_MAP(KEY_4)
		ADD_ENUM_TO_KEY_MAP(KEY_5)
		ADD_ENUM_TO_KEY_MAP(KEY_6)
		ADD_ENUM_TO_KEY_MAP(KEY_7)
		ADD_ENUM_TO_KEY_MAP(KEY_8)
		ADD_ENUM_TO_KEY_MAP(KEY_9)

		ADD_ENUM_TO_KEY_MAP(KEY_A)
		ADD_ENUM_TO_KEY_MAP(KEY_B)
		ADD_ENUM_TO_KEY_MAP(KEY_C)
		ADD_ENUM_TO_KEY_MAP(KEY_D)
		ADD_ENUM_TO_KEY_MAP(KEY_E)
		ADD_ENUM_TO_KEY_MAP(KEY_F)
		ADD_ENUM_TO_KEY_MAP(KEY_G)
		ADD_ENUM_TO_KEY_MAP(KEY_H)
		ADD_ENUM_TO_KEY_MAP(KEY_I)
		ADD_ENUM_TO_KEY_MAP(KEY_J)
		ADD_ENUM_TO_KEY_MAP(KEY_K)
		ADD_ENUM_TO_KEY_MAP(KEY_L)
		ADD_ENUM_TO_KEY_MAP(KEY_M)
		ADD_ENUM_TO_KEY_MAP(KEY_N)
		ADD_ENUM_TO_KEY_MAP(KEY_O)
		ADD_ENUM_TO_KEY_MAP(KEY_P)
		ADD_ENUM_TO_KEY_MAP(KEY_Q)
		ADD_ENUM_TO_KEY_MAP(KEY_R)
		ADD_ENUM_TO_KEY_MAP(KEY_S)
		ADD_ENUM_TO_KEY_MAP(KEY_T)
		ADD_ENUM_TO_KEY_MAP(KEY_U)
		ADD_ENUM_TO_KEY_MAP(KEY_V)
		ADD_ENUM_TO_KEY_MAP(KEY_W)
		ADD_ENUM_TO_KEY_MAP(KEY_X)
		ADD_ENUM_TO_KEY_MAP(KEY_Y)
		ADD_ENUM_TO_KEY_MAP(KEY_Z)

		ADD_ENUM_TO_KEY_MAP(KEY_MINUS)
		ADD_ENUM_TO_KEY_MAP(KEY_EQUAL)
		ADD_ENUM_TO_KEY_MAP(KEY_DIVIDE)
		ADD_ENUM_TO_KEY_MAP(KEY_SLASH)
		ADD_ENUM_TO_KEY_MAP(KEY_SLASH_UK)
		ADD_ENUM_TO_KEY_MAP(KEY_COMMA)
		ADD_ENUM_TO_KEY_MAP(KEY_PERIOD)
		ADD_ENUM_TO_KEY_MAP(KEY_SEMICOL)

		ADD_ENUM_TO_KEY_MAP(KEY_LBRACKET)
		ADD_ENUM_TO_KEY_MAP(KEY_RBRACKET)

		ADD_ENUM_TO_KEY_MAP(KEY_RAPOSTRO)
		ADD_ENUM_TO_KEY_MAP(KEY_LAPOSTRO)

		ADD_ENUM_TO_KEY_MAP(KEY_ESC)
		ADD_ENUM_TO_KEY_MAP(KEY_ENTER)
		ADD_ENUM_TO_KEY_MAP(KEY_BACKSP)
		ADD_ENUM_TO_KEY_MAP(KEY_TAB)
		ADD_ENUM_TO_KEY_MAP(KEY_SPACEBAR)

		ADD_ENUM_TO_KEY_MAP(KEY_NUMLOCK)
		ADD_ENUM_TO_KEY_MAP(KEY_SCROLLOCK)
		ADD_ENUM_TO_KEY_MAP(KEY_CAPSLOCK)

		ADD_ENUM_TO_KEY_MAP(KEY_LSHIFT)
		ADD_ENUM_TO_KEY_MAP(KEY_RSHIFT)

		ADD_ENUM_TO_KEY_MAP(KEY_LALT)
		ADD_ENUM_TO_KEY_MAP(KEY_RALT)

		ADD_ENUM_TO_KEY_MAP(KEY_LCTRL)
		ADD_ENUM_TO_KEY_MAP(KEY_RCTRL)

		ADD_ENUM_TO_KEY_MAP(KEY_F1)
		ADD_ENUM_TO_KEY_MAP(KEY_F2)
		ADD_ENUM_TO_KEY_MAP(KEY_F3)
		ADD_ENUM_TO_KEY_MAP(KEY_F4)
		ADD_ENUM_TO_KEY_MAP(KEY_F5)
		ADD_ENUM_TO_KEY_MAP(KEY_F6)
		ADD_ENUM_TO_KEY_MAP(KEY_F7)
		ADD_ENUM_TO_KEY_MAP(KEY_F8)
		ADD_ENUM_TO_KEY_MAP(KEY_F9)
		ADD_ENUM_TO_KEY_MAP(KEY_F10)
		ADD_ENUM_TO_KEY_MAP(KEY_F11)
		ADD_ENUM_TO_KEY_MAP(KEY_F12)

		ADD_ENUM_TO_KEY_MAP(KEY_PAD0)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD1)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD2)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD3)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD4)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD5)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD6)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD7)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD8)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD9)
		ADD_ENUM_TO_KEY_MAP(KEY_PADMINUS)
		ADD_ENUM_TO_KEY_MAP(KEY_PADPLUS)
		ADD_ENUM_TO_KEY_MAP(KEY_PADPERIOD)
		ADD_ENUM_TO_KEY_MAP(KEY_PADDIVIDE)
		ADD_ENUM_TO_KEY_MAP(KEY_PADMULTIPLY)
		ADD_ENUM_TO_KEY_MAP(KEY_PADENTER)

		ADD_ENUM_TO_KEY_MAP(KEY_INSERT)
		ADD_ENUM_TO_KEY_MAP(KEY_HOME)
		ADD_ENUM_TO_KEY_MAP(KEY_PAGEUP)
		ADD_ENUM_TO_KEY_MAP(KEY_DELETE)
		ADD_ENUM_TO_KEY_MAP(KEY_END)
		ADD_ENUM_TO_KEY_MAP(KEY_PAGEDOWN)
		ADD_ENUM_TO_KEY_MAP(KEY_UP)
		ADD_ENUM_TO_KEY_MAP(KEY_DOWN)
		ADD_ENUM_TO_KEY_MAP(KEY_LEFT)
		ADD_ENUM_TO_KEY_MAP(KEY_RIGHT)

		ADD_ENUM_TO_KEY_MAP(KEY_PRINT_SCRN)
		ADD_ENUM_TO_KEY_MAP(KEY_PAUSE)
		ADD_ENUM_TO_KEY_MAP(KEY_BREAK)

#undef ADD_ENUM_TO_KEY_MAP
}

void LoadEnumsIntoMouseMap(void)
{
	// Dity macro hack :D
#define ADD_ENUM_TO_MOUSE_MAP(Enum) mMouseNameToVal[#Enum] = (Enum)
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_LEFT_BUTTON	);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_RIGHT_BUTTON);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_MIDDLE_BUTTON);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_X1_BUTTON);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_X2_BUTTON);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_WHEEL_UP);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_WHEEL_DOWN);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_WHEEL_LEFT);
	ADD_ENUM_TO_MOUSE_MAP(MOUSE_WHEEL_RIGHT);
#undef ADD_ENUM_TO_MOUSE_MAP
}

void LoadEnumsIntoCCTypeMap(void) {
	// Dirty macro hack :D
#define ADD_ENUM_TO_CCTYPE_MAP(Enum) mCCTypeNameToVal[#Enum] = (Enum);

	ADD_ENUM_TO_CCTYPE_MAP(CC_TYPE_TRIGGER)
		ADD_ENUM_TO_CCTYPE_MAP(CC_TYPE_CONTINUOUS)

#undef ADD_ENUM_TO_CCTYPE_MAP
}

void LoadEnumsIntoCCTabMap(void) {
	// Dirty macro hack :D
#define ADD_ENUM_TO_CCTAB_MAP(Enum) mCCTabNameToVal[#Enum] = (Enum);

	ADD_ENUM_TO_CCTAB_MAP(TARGET_TAB)
	ADD_ENUM_TO_CCTAB_MAP(SHIP_TAB)
	ADD_ENUM_TO_CCTAB_MAP(WEAPON_TAB)
	ADD_ENUM_TO_CCTAB_MAP(COMPUTER_TAB)

#undef ADD_ENUM_TO_CCTAB_MAP
}

void LoadEnumsIntoMaps() {
	LoadEnumsIntoKeyMap();
	LoadEnumsIntoMouseMap();
	LoadEnumsIntoCCTypeMap();
	LoadEnumsIntoCCTabMap();
}

void control_config_common_load_overrides()
{
	LoadEnumsIntoMaps();

	try {
		if (cf_exists_full("controlconfigdefaults.tbl", CF_TYPE_TABLES)) {
			read_file_text("controlconfigdefaults.tbl", CF_TYPE_TABLES);
		} else {
			read_file_text_from_default(defaults_get_file("controlconfigdefaults.tbl"));
		}

		reset_parse();

		// start parsing
		// TODO: Split this out into more helps. Too many tabs!
		bool first = true;
		while(optional_string("#ControlConfigOverride")) {
			Control_LUT cfg_preset;
			std::copy(Control_config.begin(), Control_config.end(), cfg_preset.begin());
			

			SCP_string preset_name;
			if (optional_string("$Name:")) {
				stuff_string_line(preset_name);
			} else {
				preset_name = "<unnamed preset>";
			}

			while (required_string_either("#End","$Bind Name:")) {
				const int iBufferLength = 64;
				char szTempBuffer[iBufferLength];

				required_string("$Bind Name:");
				stuff_string(szTempBuffer, F_NAME, iBufferLength);

				const size_t cCntrlAryLength = sizeof(Control_config) / sizeof(Control_config[0]);
				for (size_t i = 0; i < cCntrlAryLength; ++i) {
					Config_item& r_ccConfig = cfg_preset[i];

					if (!strcmp(szTempBuffer, r_ccConfig.text)) {
						int iTemp;

						if (optional_string("$Key Default:")) {
							if (optional_string("NONE")) {
								r_ccConfig.default_id[0] = cid(-1,-1);
							} else {
								stuff_string(szTempBuffer, F_NAME, iBufferLength);
								r_ccConfig.default_id[0] = cid(CID_KEYBOARD, (short) mKeyNameToVal[szTempBuffer]);
							}
						}

						if (optional_string("$Mouse Default:")) {
							stuff_string(szTempBuffer, F_NAME, iBufferLength);
							r_ccConfig.default_id[1] = cid(CID_MOUSE, (short) mMouseNameToVal[szTempBuffer]);
						}

						if (optional_string("$Joy Default:")) {
							stuff_int(&iTemp);
							r_ccConfig.default_id[2] = cid(CID_JOY, (short)iTemp);
						}

						if (optional_string("$Key Mod Shift:")) {
							stuff_int(&iTemp);
							r_ccConfig.default_id[0].second |= (iTemp == 1) ? KEY_SHIFTED : 0;
						}

						if (optional_string("$Key Mod Alt:")) {
							stuff_int(&iTemp);
							r_ccConfig.default_id[0].second |= (iTemp == 1) ? KEY_ALTED : 0;
						}

						if (optional_string("$Key Mod Ctrl:")) {
							stuff_int(&iTemp);
							r_ccConfig.default_id[0].second |= (iTemp == 1) ? KEY_CTRLED : 0;
						}

						if (optional_string("$Category:")) {
							if (!first) {
								Warning("controlconfigdefaults.tbl", get_line_num(), "Ignoring $Category: entry, only the first override block may use this!");
							}
							stuff_string(szTempBuffer, F_NAME, iBufferLength);
							r_ccConfig.tab = (char)mCCTabNameToVal[szTempBuffer];
						}

						if (optional_string("$Has XStr:")) {
							if (!first) {
								Warning("controlconfigdefaults.tbl", get_line_num(), "Ignoring $Has XStr: entry, only the first override block may use this!");
							}
							stuff_int(&iTemp);
							r_ccConfig.hasXSTR = (iTemp == 1);
						}

						if (optional_string("$Type:")) {
							if (!first) {
								Warning("controlconfigdefaults.tbl", get_line_num(), "Ignoring $Type: entry, only the first override block may use this!");
							}
							stuff_string(szTempBuffer, F_NAME, iBufferLength);
							r_ccConfig.type = (char)mCCTypeNameToVal[szTempBuffer];
						}

						// Deprecated
						if (optional_string("+Disable")) {
							Warning("controlconfigdefaults.tbl", get_line_num(), "Found deprecated entry '+Disable,' Please use '$Disable: true' instead.");
							if (!first) {
								Warning("controlconfigdefaults.tbl", get_line_num(), "Ignoring +Disable entry, only the first override block may use this!");
							}
							r_ccConfig.disabled = true;
						}

						if (optional_string("$Disable:")) {
							if (!first) {
								Warning("controlconfigdefaults.tbl", get_line_num(), "Ignoring $Disable: entry, only the first override block may use this!");
							}
							stuff_boolean(&r_ccConfig.disabled);
						}

						// Nerf the buffer now.
						szTempBuffer[0] = '\0';
					} else if ((i + 1) == cCntrlAryLength) {
						error_display(1, "Bind Name not found: %s\n", szTempBuffer);
						advance_to_eoln(NULL);
						ignore_white_space();
						return;
					}
				}
			}

			// Overwrite the control config with the first preset that was found
			if (first) {
				std::copy(cfg_preset.begin(), cfg_preset.end(), Control_config);
				first = false;
			}

			Control_config_presets.push_back(Preset_table(preset_name, cfg_preset));
			required_string("#End");
		}
	}
	catch (const parse::ParseException& e)
	{
		mprintf(("TABLES: Unable to parse 'controlconfigdefaults.tbl'!  Error message = %s.\n", e.what()));
		return;
	}
}
