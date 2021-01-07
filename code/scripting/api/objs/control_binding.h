#pragma once
#ifndef FS2_OPEN_CONTROLBINDING_H
#define FS2_OPEN_CONTROLBINDING_H

#include "scripting/ade_api.h"
#include "controlconfig/controlsconfig.h"

namespace scripting {
namespace api {

/*
 * A class wrapping the Control_Config's ActionID's to Lua. Lua can request a control-binding object by name and will get this class.
 * This class, only holding the IoActionId as an identifier, can then be used to access and modify the control binding with this id.
 */
class cci_h {
 private:
	IoActionId idx; //!< The ActionId thos lua object references.
 public:
	cci_h();
	cci_h(int n_id);

	/*
	* @returns If this object holds a valid ActionId
	*/
	bool IsValid();

	IoActionId Get();
};

DECLARE_ADE_OBJ(l_ControlBinding, cci_h);


}
}

#endif // FS2_OPEN_CONTROLBINDING_H
