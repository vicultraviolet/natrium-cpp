#if !defined(NA_PCH_BASE_HPP)
#define NA_PCH_BASE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>

#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <chrono>
#include <thread>
#include <atomic>
#include <limits>
#include <concepts>

#include <initializer_list>
#include <string_view>
#include <string>
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <tuple>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <bitset>

#include <fmt/format.h>
#include <fmt/chrono.h>

#include <nlohmann/json.hpp>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/hash.hpp>

#include "./Graphics/Vulkan.hpp"

#if !defined(NA_DISABLE_IMGUI)
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#endif // NA_DISABLE_IMGUI

#include "./Template/References.hpp"
#include "./Template/Handles.hpp"

#include "./Template/ArrayList.hpp"
#include "./Template/DoubleList.hpp"
#include "./Template/Arena.hpp"

// for convenience
#include "Natrium/Core/Logger.hpp"

#endif // NA_PCH_BASE_HPP
