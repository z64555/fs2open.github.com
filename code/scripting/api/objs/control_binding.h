#pragma once
#ifndef FS2_OPEN_CONTROLBINDING_H
#define FS2_OPEN_CONTROLBINDING_H

#include "scripting/ade_api.h"
#include "../../../controlconfig/controlsconfig.h"

namespace scripting {
namespace api {

class cci_h {
 private:
	IoActionId idx;
 public:
	cci_h();
	cci_h(int n_id);

	bool IsValid();

	IoActionId Get();
};

DECLARE_ADE_OBJ(l_ControlBinding, cci_h);


}
}

#endif // FS2_OPEN_CONTROLBINDING_H
