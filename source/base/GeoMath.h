#include "Vector.h"
#include "Matrix.h"
#include "Box.h"
#include "Constants.h"
#include "Functions.h"

namespace vct {

using three::Vector2;
using three::Vector3;
using three::Vector4;
using three::Vector2i;
using three::Vector3i;
using three::Vector4i;
using three::Vector2u;
using three::Vector3u;
using three::Vector4u;
using three::Matrix4;
using three::Box3;

using Extent2i = Vector2i;

static_assert(sizeof(Vector2) == 2 * sizeof(float));

} // namespace vct
