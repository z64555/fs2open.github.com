# top-level files
set (file_root
)

# Base files
set (file_root_base
	base/wxFRED_base.cpp
	base/wxFRED_base.h
	base/wxFRED_base.xrc
)

# Editors files
set (file_root_editors
	editors/dlgAsteroidFieldEditor.cpp
	editors/dlgAsteroidFieldEditor.h
	editors/dlgBackgroundEditor.cpp
	editors/dlgBackgroundEditor.h
	editors/dlgEventsEditor.cpp
	editors/dlgEventsEditor.h
	editors/dlgFictionViewer.cpp
	editors/dlgFictionViewer.h
	editors/dlgMissionObjectivesEditor.cpp
	editors/dlgMissionObjectivesEditor.h
	editors/dlgMissionSpecsEditor.cpp
	editors/dlgMissionSpecsEditor.h
	editors/dlgObjectEditor.cpp
	editors/dlgObjectEditor.h
	editors/dlgReinforcementsEditor.cpp
	editors/dlgReinforcementsEditor.h
	editors/dlgReinforcementsPicker.cpp
	editors/dlgReinforcementsPicker.h
	editors/dlgSetGlobalShipFlagsEditor.cpp
	editors/dlgSetGlobalShipFlagsEditor.h
	editors/dlgShieldSystemEditor.cpp
	editors/dlgShieldSystemEditor.h
	editors/dlgSoundEnvironment.cpp
	editors/dlgSoundEnvironment.h
	editors/dlgVoiceActingManager.cpp
	editors/dlgVoiceActingManager.h
	editors/frmBriefingEditor.cpp
	editors/frmBriefingEditor.h
	editors/frmCampaignEditor.cpp
	editors/frmCampaignEditor.h
	editors/frmCommandBriefingEditor.cpp
	editors/frmCommandBriefingEditor.h
	editors/frmDebriefingEditor.cpp
	editors/frmDebriefingEditor.h
	editors/frmShipsEditor.cpp
	editors/frmShipsEditor.h
	editors/frmTeamLoadoutEditor.cpp
	editors/frmTeamLoadoutEditor.h
	editors/frmWaypointEditor.cpp
	editors/frmWaypointEditor.h
	editors/frmWingEditor.cpp
	editors/frmWingEditor.h
)

# Help files
set (file_root_help
	help/dlgAboutBox.cpp
	help/dlgAboutBox.h
	help/dlgSexpHelp.cpp
	help/dlgSexpHelp.h
)

# Misc files
set (file_root_misc
	misc/dlgMissionStats.cpp
	misc/dlgMissionStats.h
)

set(file_root_res
	res/wxFREDicon.rc
)

# the source groups
source_group(""                                   FILES ${file_root})
source_group("Base"                               FILES ${file_root_base})
source_group("Editors"                            FILES ${file_root_editors})
source_group("Help"                               FILES ${file_root_help})
source_group("Misc"                               FILES ${file_root_misc})
source_group("Resources"                          FILES ${file_root_res})

# append all files to the file_root
set (file_root
	${file_root}
	${file_root_base}
	${file_root_editors}
	${file_root_help}
	${file_root_misc}
	${file_root_res}
)
