/*
 * Def_Files.cpp
 *
 * You may not sell or otherwise commercially exploit the source or things you
 * create based on the source.
 */




#include <string.h>
#include "globalincs/pstypes.h"

//Struct used to hold data about file defaults
typedef struct def_file
{
	char* filename;
	char *contents;
} def_file;

//:PART 1:
//**********
//**********

// These files are generated from the actual files, if you add a new file add the #include below

#include "def_files/ai_profiles.tbl.h"

#include "def_files/autopilot.tbl.h"

#include "def_files/blur-f.sdr.h"

#include "def_files/brightpass-f.sdr.h"

#include "def_files/controlconfigdefaults.tbl.h"

#include "def_files/fonts.tbl.h"

#include "def_files/fxaa-f.sdr.h"

#include "def_files/fxaa-v.sdr.h"

#include "def_files/fxaapre-f.sdr.h"

#include "def_files/game_settings.tbl.h"

#include "def_files/iff_defs.tbl.h"

#include "def_files/ls-f.sdr.h"

#include "def_files/main-f.sdr.h"

#include "def_files/main-v.sdr.h"

#include "def_files/objecttypes.tbl.h"

#include "def_files/particle-f.sdr.h"

#include "def_files/particle-v.sdr.h"

#include "def_files/post-f.sdr.h"

#include "def_files/post-v.sdr.h"

#include "def_files/post_processing.tbl.h"

#include "def_files/species_defs.tbl.h"

#include "def_files/video-f.sdr.h"

#include "def_files/video-v.sdr.h"
//:PART 2:
//**********
def_file Default_files[] =
{
	{ "ai_profiles.tbl",		(char*) Default_ai_profiles_tbl},
	{ "autopilot.tbl",			(char*) Default_autopilot_tbl},
	{ "blur-f.sdr",				(char*) Default_blur_f_sdr},
	{ "brightpass-f.sdr",		(char*) Default_brightpass_f_sdr},
	{ "controlconfigdefaults.tbl",	(char*) Default_controlconfigdefaults_tbl},
	{ "fonts.tbl",				(char*) Default_fonts_tbl},
	{ "fxaa-f.sdr",				(char*) Default_fxaa_f_sdr},
	{ "fxaa-v.sdr",				(char*) Default_fxaa_v_sdr},
	{ "fxaapre-f.sdr",			(char*) Default_fxaapre_f_sdr},
	{ "game_settings.tbl",		(char*) Default_game_settings_tbl},
	{ "iff_defs.tbl",			(char*) Default_iff_defs_tbl},
	{ "ls-f.sdr",				(char*) Default_ls_f_sdr},
	{ "main-f.sdr",				(char*) Default_main_f_sdr},
	{ "main-v.sdr",				(char*) Default_main_v_sdr},
	{ "objecttypes.tbl",		(char*) Default_objecttypes_tbl},
	{ "particle-f.sdr",			(char*) Default_particle_f_sdr},
	{ "particle-v.sdr",			(char*) Default_particle_v_sdr},
	{ "post-f.sdr",				(char*) Default_post_f_sdr},
	{ "post-v.sdr",				(char*) Default_post_v_sdr},
	{ "post_processing.tbl",	(char*) Default_post_processing_tbl},
	{ "species_defs.tbl",		(char*) Default_species_defs_tbl},
	{ "video-f.sdr",			(char*) Default_video_f_sdr},
	{ "video-v.sdr",			(char*) Default_video_v_sdr},
};

static int Num_default_files = sizeof(Default_files) / sizeof(def_file);
//**********

char *defaults_get_file(char *filename)
{
	for(int i = 0; i < Num_default_files; i++)
	{
		if(!stricmp(Default_files[i].filename, filename))
		{
			return Default_files[i].contents;
		}
	}

	//WMC - This is really bad, because it means we have a default table missing.
	Error(LOCATION, "Default table '%s' missing from executable - contact a coder.", filename);
	return NULL;
}

//:PART 3:
//**********
//=========================================================================

