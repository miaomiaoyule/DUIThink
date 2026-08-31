cmake_minimum_required(VERSION 3.16)
project(DUIThinkCrossPlatform LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(DUITHINK_PLATFORM_SDL "Build with SDL3 windowing backend" ON)

# ---------------------------------------------------------------------------
# SDL3
# ---------------------------------------------------------------------------
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
	# allow vendored / manual SDK
	set(SDL3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdDepend/SDL3" CACHE PATH "SDL3 headers")
	find_library(SDL3_LIBRARY NAMES SDL3 SDL3.dll libSDL3.so SDL3.dylib
		PATHS
			"${CMAKE_SOURCE_DIR}/lib"
			"${CMAKE_SOURCE_DIR}/ThirdDepend/SDL3"
			ENV SDL3_DIR
		PATH_SUFFIXES lib lib64)
	if(SDL3_LIBRARY)
		add_library(SDL3::SDL3 UNKNOWN IMPORTED)
		set_target_properties(SDL3::SDL3 PROPERTIES
			IMPORTED_LOCATION "${SDL3_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR}")
		set(SDL3_FOUND TRUE)
	endif()
endif()

if(DUITHINK_PLATFORM_SDL AND NOT SDL3_FOUND)
	message(WARNING "SDL3 not found. Configure SDL3_LIBRARY / SDL3_INCLUDE_DIR, or install SDL3.")
endif()

add_compile_definitions(UNICODE _UNICODE)
if(DUITHINK_PLATFORM_SDL)
	add_compile_definitions(DuiPlatform_SDL)
endif()
if(UNIX AND NOT APPLE)
	add_compile_definitions(_LINUX)
endif()
if(APPLE)
	add_compile_definitions(__APPLE__)
endif()
if(ANDROID)
	add_compile_definitions(__ANDROID__)
endif()

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/DUIThink
	${CMAKE_SOURCE_DIR}/MMHelper
	${CMAKE_SOURCE_DIR}/ThirdDepend
	${CMAKE_SOURCE_DIR}/ThirdDepend/tinyxml2
	${CMAKE_SOURCE_DIR}/ThirdDepend/SDL3
)

# ---------------------------------------------------------------------------
# tinyxml2
# ---------------------------------------------------------------------------
add_library(tinyxml2 STATIC ThirdDepend/tinyxml2/tinyxml2.cpp)

# ---------------------------------------------------------------------------
# MMHelper
# ---------------------------------------------------------------------------
file(GLOB_RECURSE SRC_MMHELPER CONFIGURE_DEPENDS "MMHelper/*.cpp")
list(FILTER SRC_MMHELPER EXCLUDE REGEX ".*_win32\\.cpp$")

# Windows-only modules stay compiled but stubbed by #ifndef DuiPlatform_SDL
add_library(MMHelper STATIC ${SRC_MMHELPER})
target_compile_definitions(MMHelper PUBLIC MMHELPLIB)
if(DUITHINK_PLATFORM_SDL AND TARGET SDL3::SDL3)
	target_link_libraries(MMHelper PUBLIC SDL3::SDL3)
endif()
if(UNIX)
	target_link_libraries(MMHelper PUBLIC pthread dl)
endif()

# ---------------------------------------------------------------------------
# DUIThink
# ---------------------------------------------------------------------------
file(GLOB_RECURSE SRC_DUITHINK CONFIGURE_DEPENDS "DUIThink/*.cpp")

# Win32-only controls / platform
if(DUITHINK_PLATFORM_SDL)
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIWndWin32\\.cpp$")
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIRichEditCtrl\\.cpp$")
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIWebBrowserCtrl\\.cpp$")
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIEditCtrl\\.cpp$")
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIHotKeyCtrl\\.cpp$")
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIWkeBrowserCtrl\\.cpp$")
else()
	list(FILTER SRC_DUITHINK EXCLUDE REGEX ".*/DUIWndSDL\\.cpp$")
endif()

add_library(DUIThink SHARED ${SRC_DUITHINK})
target_compile_definitions(DUIThink PRIVATE DUITHINKSDK MMHELPLIB)
target_link_libraries(DUIThink PUBLIC MMHelper tinyxml2)
if(DUITHINK_PLATFORM_SDL AND TARGET SDL3::SDL3)
	target_link_libraries(DUIThink PUBLIC SDL3::SDL3)
endif()

# ---------------------------------------------------------------------------
# smoke test entry
# ---------------------------------------------------------------------------
add_executable(DuiLinuxTest linux_main.cpp)
target_link_libraries(DuiLinuxTest PRIVATE DUIThink)
if(DUITHINK_PLATFORM_SDL AND TARGET SDL3::SDL3)
	target_link_libraries(DuiLinuxTest PRIVATE SDL3::SDL3)
endif()
