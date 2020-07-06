#include "Vector.h"

namespace vct {

using three::Vector2;
using three::Vector3;
using three::Vector4;

using three::Vector2i;
using Extent2i = Vector2i;

static_assert(sizeof(Vector2) == 2 * sizeof(float));

} // namespace vct
