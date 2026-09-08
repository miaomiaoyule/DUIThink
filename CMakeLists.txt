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

# Sources must be UTF-8. Do NOT use -finput-charset=GBK: it breaks UTF-8
# headers such as MMPlatformTypes.h (SAL/_In_ shims) and cascades into
# thousands of bogus errors. Convert remaining GBK files via tools/gbk_to_utf8.py.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()
if(MSVC)
	add_compile_options(/utf-8)
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
# libwebp (decode + demux for animated WebP)
# Vendored tree is incomplete for upstream CMake; build from VS source lists.
# ---------------------------------------------------------------------------
set(WEBP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdDepend/webp")
set(WEBP_DEC_SRCS
	${WEBP_ROOT}/src/dec/alpha_dec.c
	${WEBP_ROOT}/src/dec/buffer_dec.c
	${WEBP_ROOT}/src/dec/frame_dec.c
	${WEBP_ROOT}/src/dec/idec_dec.c
	${WEBP_ROOT}/src/dec/io_dec.c
	${WEBP_ROOT}/src/dec/quant_dec.c
	${WEBP_ROOT}/src/dec/tree_dec.c
	${WEBP_ROOT}/src/dec/vp8l_dec.c
	${WEBP_ROOT}/src/dec/vp8_dec.c
	${WEBP_ROOT}/src/dec/webp_dec.c
)
set(WEBP_DSP_DEC_SRCS
	${WEBP_ROOT}/src/dsp/alpha_processing.c
	${WEBP_ROOT}/src/dsp/cpu.c
	${WEBP_ROOT}/src/dsp/dec.c
	${WEBP_ROOT}/src/dsp/dec_clip_tables.c
	${WEBP_ROOT}/src/dsp/filters.c
	${WEBP_ROOT}/src/dsp/lossless.c
	${WEBP_ROOT}/src/dsp/rescaler.c
	${WEBP_ROOT}/src/dsp/upsampling.c
	${WEBP_ROOT}/src/dsp/yuv.c
	# x86_64 always defines __SSE2__; portable C calls these Init*SSE2 hooks
	${WEBP_ROOT}/src/dsp/alpha_processing_sse2.c
	${WEBP_ROOT}/src/dsp/dec_sse2.c
	${WEBP_ROOT}/src/dsp/filters_sse2.c
	${WEBP_ROOT}/src/dsp/lossless_sse2.c
	${WEBP_ROOT}/src/dsp/rescaler_sse2.c
	${WEBP_ROOT}/src/dsp/upsampling_sse2.c
	${WEBP_ROOT}/src/dsp/yuv_sse2.c
)
set(WEBP_UTILS_DEC_SRCS
	${WEBP_ROOT}/src/utils/bit_reader_utils.c
	${WEBP_ROOT}/src/utils/color_cache_utils.c
	${WEBP_ROOT}/src/utils/filters_utils.c
	${WEBP_ROOT}/src/utils/huffman_utils.c
	${WEBP_ROOT}/src/utils/palette.c
	${WEBP_ROOT}/src/utils/quant_levels_dec_utils.c
	${WEBP_ROOT}/src/utils/rescaler_utils.c
	${WEBP_ROOT}/src/utils/random_utils.c
	${WEBP_ROOT}/src/utils/thread_utils.c
	${WEBP_ROOT}/src/utils/utils.c
)
add_library(webpdecoder STATIC
	${WEBP_DEC_SRCS}
	${WEBP_DSP_DEC_SRCS}
	${WEBP_UTILS_DEC_SRCS}
)
target_include_directories(webpdecoder PUBLIC ${WEBP_ROOT} ${WEBP_ROOT}/src)
target_compile_definitions(webpdecoder PRIVATE WEBP_USE_THREAD=1)
if(UNIX)
	target_link_libraries(webpdecoder PUBLIC pthread)
endif()

add_library(webpdemux STATIC
	${WEBP_ROOT}/src/demux/anim_decode.c
	${WEBP_ROOT}/src/demux/demux.c
)
target_include_directories(webpdemux PUBLIC ${WEBP_ROOT} ${WEBP_ROOT}/src)
target_link_libraries(webpdemux PUBLIC webpdecoder)

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
if(TARGET webpdemux)
	target_link_libraries(DUIThink PUBLIC webpdemux)
endif()
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

# ---------------------------------------------------------------------------
# DUIThinkDemo_C++ (Linux / SDL)
# ---------------------------------------------------------------------------
set(DEMO_ROOT "${CMAKE_SOURCE_DIR}/Demo/DUIThinkDemo_C++/DUIThinkDemo_C++")
set(DEMO_PROJ_DIR "${CMAKE_SOURCE_DIR}/Demo/DUIThinkDemo_C++/DUIThinkDemo_C++.DuiProj")
set(DEMO_SRCS
	${DEMO_ROOT}/DUIThinkDemo_C++.cpp
	${DEMO_ROOT}/StdAfx.cpp
	${DEMO_ROOT}/DlgDemo.cpp
	${DEMO_ROOT}/DlgDpi.cpp
	${DEMO_ROOT}/DlgChatTip.cpp
	${DEMO_ROOT}/BaseControl/AnimateView/AnimateView.cpp
	${DEMO_ROOT}/BaseControl/ButtonView/ButtonView.cpp
	${DEMO_ROOT}/BaseControl/CheckBoxView/CheckBoxView.cpp
	${DEMO_ROOT}/BaseControl/ComboxView/ComboxView.cpp
	${DEMO_ROOT}/BaseControl/EditView/EditView.cpp
	${DEMO_ROOT}/BaseControl/ProgressView/ProgressView.cpp
	${DEMO_ROOT}/BaseControl/RadioBoxView/RadioBoxView.cpp
	${DEMO_ROOT}/BaseControl/RichEditView/RichEditView.cpp
	${DEMO_ROOT}/BaseControl/SliderView/SliderView.cpp
	${DEMO_ROOT}/BaseControl/StaticView/StaticView.cpp
	${DEMO_ROOT}/QQ/DlgQQChat.cpp
	${DEMO_ROOT}/QQ/DlgQQFace.cpp
	${DEMO_ROOT}/QQ/QQView.cpp
	${DEMO_ROOT}/Usage/AnimateDlg/AnimateDlgView.cpp
	${DEMO_ROOT}/Usage/DpiDlg/DpiDlgView.cpp
	${DEMO_ROOT}/Usage/SvgImage/SvgImageView.cpp
	${DEMO_ROOT}/Usage/SwitchSkin/SwitchSkinView.cpp
)

add_executable(DUIThinkDemo_C++ ${DEMO_SRCS})
target_include_directories(DUIThinkDemo_C++ PRIVATE
	${DEMO_ROOT}
	"${CMAKE_SOURCE_DIR}/Demo/DUIThinkDemo_C++"
)
target_link_libraries(DUIThinkDemo_C++ PRIVATE DUIThink)
if(DUITHINK_PLATFORM_SDL AND TARGET SDL3::SDL3)
	target_link_libraries(DUIThinkDemo_C++ PRIVATE SDL3::SDL3)
endif()
if(UNIX)
	# Always load libDUIThink.so from the executable directory (the VM pack),
	# never from the WSL build tree path baked in by CMake.
	set_target_properties(DUIThinkDemo_C++ PROPERTIES
		BUILD_RPATH "$ORIGIN"
		INSTALL_RPATH "$ORIGIN"
		BUILD_WITH_INSTALL_RPATH TRUE
	)
	target_link_libraries(DUIThinkDemo_C++ PRIVATE dl)
	target_link_options(DUIThinkDemo_C++ PRIVATE -rdynamic)
endif()

# Portable relative path: resolved against SDL_GetBasePath() (exe directory).
target_compile_definitions(DUIThinkDemo_C++ PRIVATE
	"DUITHINK_DEMO_PROJ_FILE=\"DUIThinkDemo_C++.DuiProj/DUIThinkDemo_C++.DuiProj\""
)

# Keep DuiProj next to the binary for relative lookups.
if(UNIX)
	add_custom_command(TARGET DUIThinkDemo_C++ POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink
			"${DEMO_PROJ_DIR}"
			"$<TARGET_FILE_DIR:DUIThinkDemo_C++>/DUIThinkDemo_C++.DuiProj"
		COMMENT "Link Demo DuiProj next to DUIThinkDemo_C++"
	)
endif()