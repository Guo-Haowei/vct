#include "rendering_misc.h"

#define DEFINE_DVAR
#include "rendering_dvars.h"

namespace vct {

void register_rendering_dvars() {
#define REGISTER_DVAR
#include "rendering_dvars.h"
}

}  // namespace vct