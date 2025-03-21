/*
 * Copyright (c) 2017-2025 The Forge Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#pragma once

// Support external config file override
#if defined(EXTERNAL_CONFIG_FILEPATH)
#include EXTERNAL_CONFIG_FILEPATH
#elif defined(EXTERNAL_CONFIG_FILEPATH_NO_STRING)
// When invoking clanng from FastBuild the EXTERNAL_CONFIG_FILEPATH define doesn't get expanded to a string,
// quotes are removed, that's why we add this variation of the macro that turns the define back into a valid string
#define TF_EXTERNAL_CONFIG_STRINGIFY2(x) #x
#define TF_EXTERNAL_CONFIG_STRINGIFY(x)  TF_EXTERNAL_CONFIG_STRINGIFY2(x)

#include TF_EXTERNAL_CONFIG_STRINGIFY(EXTERNAL_CONFIG_FILEPATH_NO_STRING)

#undef TF_EXTERNAL_CONFIG_STRINGIFY
#undef TF_EXTERNAL_CONFIG_STRINGIFY2
#else

#include <stdint.h>
#include <stdlib.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

//////////////////////////////////////////////
//// Compiler setup
//////////////////////////////////////////////
#if INTPTR_MAX == 0x7FFFFFFFFFFFFFFFLL
#define PTR_SIZE 8
#elif INTPTR_MAX == 0x7FFFFFFF
#define PTR_SIZE 4
#else
#error unsupported platform
#endif

#ifdef __cplusplus
#define FORGE_CONSTEXPR constexpr
#define FORGE_EXTERN_C  extern "C"
#else
#define FORGE_CONSTEXPR
#define FORGE_EXTERN_C
#endif

#if !defined(QUOTE)
#define _QUOTE(x) #x
#define QUOTE(x)  _QUOTE(x)
#endif

#define TF_ARRAY_COUNT(array) (sizeof(array) / (sizeof(array[0]) * (sizeof(array) != PTR_SIZE || sizeof(array[0]) <= PTR_SIZE)))

#ifndef TF_MIN
#define TF_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef TF_MAX
#define TF_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#if !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif

#define UNREF_PARAM(x)         ((void)(x))
#define ALIGNAS(x)             __declspec(align(x))
#define DEFINE_ALIGNED(def, a) __declspec(align(a)) def
#define FORGE_CALLCONV         __cdecl
#define ALIGNOF(x)             __alignof(x)
#define THREAD_LOCAL           __declspec(thread)

#include <crtdbg.h>
#define COMPILE_ASSERT(exp) _STATIC_ASSERT(exp)

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#if defined(_M_X64)
#define ARCH_X64
#define ARCH_X86_FAMILY
#elif defined(_M_IX86)
#define ARCH_X86
#define ARCH_X86_FAMILY
#else
#error "Unsupported architecture for msvc compiler"
#endif

// Msvc removes trailing commas
#define OPT_COMMA_VA_ARGS(...) , __VA_ARGS__

#elif defined(__GNUC__) || defined(__clang__)
#include <assert.h>
#include <stdalign.h>
#include <sys/types.h>

#ifdef __OPTIMIZE__
// Some platforms define NDEBUG for Release builds
#if !defined(NDEBUG) && !defined(_DEBUG)
#define NDEBUG
#endif
#else
#if !defined(_DEBUG) && !defined(NDEBUG)
#define _DEBUG
#endif
#endif

#ifdef __APPLE__
#define NOREFS __unsafe_unretained
#endif

#define UNREF_PARAM(x)         ((void)(x))
#define ALIGNAS(x)             __attribute__((aligned(x)))
#define DEFINE_ALIGNED(def, a) __attribute__((aligned(a))) def
#define FORGE_CALLCONV
#define ALIGNOF(x)   __alignof__(x)
#define THREAD_LOCAL __thread

#if defined(__clang__) && !defined(__cplusplus)
#define COMPILE_ASSERT(exp) _Static_assert(exp, #exp)
#else
#define COMPILE_ASSERT(exp) static_assert(exp, #exp)
#endif

#if defined(__i386__)
#define ARCH_X86
#define ARCH_X86_FAMILY
#elif defined(__x86_64__)
#define ARCH_X64
#define ARCH_X86_FAMILY
#elif defined(__arm__)
#define ARCH_ARM
#define ARCH_ARM_FAMILY
#elif defined(__aarch64__)
#define ARCH_ARM64
#define ARCH_ARM_FAMILY
#else
#error "Unsupported architecture for gcc compiler"
#endif

#if defined(_WIN32)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#define OPT_COMMA_VA_ARGS(...) , ##__VA_ARGS__

#else
#error Unknown language dialect
#endif

#ifndef SSIZE_MAX
#if PTR_SIZE == 4
#define SSIZE_MAX INT32_MAX
COMPILE_ASSERT(sizeof(ssize_t) == sizeof(int32_t));
#elif PTR_SIZE == 8
#define SSIZE_MAX INT64_MAX
COMPILE_ASSERT(sizeof(ssize_t) == sizeof(int64_t));
#endif
#endif // !SSIZE_MAX

#if defined(_MSC_VER)
#define FORGE_EXPORT __declspec(dllexport)
#define FORGE_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) // clang & gcc
#define FORGE_EXPORT __attribute__((visibility("default")))
#define FORGE_IMPORT
#endif

#ifdef __cplusplus
#ifndef MAKE_ENUM_FLAG
#define MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)                                                                                      \
    inline FORGE_CONSTEXPR ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) { return ENUM_TYPE(((TYPE)a) | ((TYPE)b)); }        \
    inline ENUM_TYPE&                operator|=(ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)(((TYPE&)a) |= ((TYPE)b)); } \
    inline FORGE_CONSTEXPR ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) { return ENUM_TYPE(((TYPE)a) & ((TYPE)b)); }        \
    inline ENUM_TYPE&                operator&=(ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)(((TYPE&)a) &= ((TYPE)b)); } \
    inline FORGE_CONSTEXPR ENUM_TYPE operator~(ENUM_TYPE a) { return ENUM_TYPE(~((TYPE)a)); }                                \
    inline FORGE_CONSTEXPR ENUM_TYPE operator^(ENUM_TYPE a, ENUM_TYPE b) { return ENUM_TYPE(((TYPE)a) ^ ((TYPE)b)); }        \
    inline ENUM_TYPE&                operator^=(ENUM_TYPE& a, ENUM_TYPE b) { return (ENUM_TYPE&)(((TYPE&)a) ^= ((TYPE)b)); }
#endif
#else
#define MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)
#endif

//////////////////////////////////////////////
//// Platform setup
//////////////////////////////////////////////
#if defined(_WIN32)

#ifdef _GAMING_XBOX
#define XBOX
#ifdef _GAMING_XBOX_XBOXONE
#ifndef XBOXONE
#define XBOXONE
#endif
#endif
#ifdef _GAMING_XBOX_SCARLETT
#ifndef SCARLETT
#define SCARLETT
#endif
#endif
#elif !defined(_WINDOWS)
#define _WINDOWS
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef _WINDOWS
// Restrict compilation to Windows 7 APIs
#define NTDDI_VERSION NTDDI_WIN7
#define WINVER        _WIN32_WINNT_WIN7
#define _WIN32_WINNT  _WIN32_WINNT_WIN7

// Whitelist of compiler versions
#if (_MSC_VER == 1929) // VS 2019 all VC++ compilers
#else
#pragma message("Bad Visual Studio version: (" QUOTE(_MSC_VER) " " QUOTE(_MSC_FULL_VER) " " QUOTE(_MSC_BUILD) ").")
#error "Bad Visual Studio version"
#endif

#endif

#elif defined(__APPLE__)
#include <TargetConditionals.h>

#if defined(ARCH_ARM64)
#define TARGET_APPLE_ARM64
#endif

#if TARGET_OS_IPHONE
#define TARGET_IOS
#endif

#if TARGET_IPHONE_SIMULATOR
#define TARGET_IOS_SIMULATOR
#endif

#elif defined(__ANDROID__)
#define ANDROID
#elif defined(__ORBIS__)
#define ORBIS
#elif defined(__PROSPERO__)
#define PROSPERO
#elif defined(__linux__)
#define STEAMDECK
#endif

#ifndef MIN_MALLOC_ALIGNMENT
#if defined(__APPLE__)
#define MIN_MALLOC_ALIGNMENT 16
#elif defined(ANDROID) && defined(ARCH_ARM_FAMILY)
#define MIN_MALLOC_ALIGNMENT 8
#elif defined(ANDROID) && defined(ARCH_X86_FAMILY)
#define MIN_MALLOC_ALIGNMENT 8
#elif defined(NX64) && defined(ARCH_ARM_FAMILY)
#define MIN_MALLOC_ALIGNMENT 8
#elif PTR_SIZE == 4
#define MIN_MALLOC_ALIGNMENT 8
#elif PTR_SIZE == 8
#define MIN_MALLOC_ALIGNMENT 16
#endif
#endif

#ifdef ANDROID
// Allows to declare native functions that are called through Java base activity
// These events need to be declared as extern "C"
// Example usage:
//      extern "C" JNIEXPORT void JNICALL TF_ANDROID_JAVA_NATIVE_EVENT(<JavaClassName>, <functionName>) (JNIEnv* env, jobject obj <,
//      functionPrameters>);
#define TF_ANDROID_JAVA_NATIVE_EVENT(javaClass, javaFnName) Java_com_forge_unittest_##javaClass##_##javaFnName
#endif

//////////////////////////////////////////////
//// General options
//////////////////////////////////////////////
// #define ENABLE_MATH_CHECKS // Enables low level math library debug checks like SIMD variable alignment checks
#define ENABLE_FORGE_SCRIPTING
#define ENABLE_FORGE_RELOAD_SHADER
#define ENABLE_FORGE_UI
#define ENABLE_FORGE_FONTS
#define ENABLE_FORGE_INPUT
#define ENABLE_FORGE_ANIMATION_DEBUG
// #define ENABLE_FORGE_GPU_ANIMATION_DEBUG
#define ENABLE_FORGE_MATERIALS
#define ENABLE_ZIP_FILESYSTEM
#define ENABLE_SCREENSHOT
#define ENABLE_PROFILER
#define ENABLE_MESHOPTIMIZER
#define ENABLE_THREAD_PERFORMANCE_STATS
// #define ENABLE_VMA_LOG // Very verbose, prints for each allocation

// ENABLE_FORGE_ANDROID_SHADERC can be disabled if all shaders are compiled offline.
// This way we also avoid to link to this library
#define ENABLE_FORGE_ANDROID_SHADERC

#if TARGET_OS_IPHONE || (defined(MAC_OS_VERSION_11_0) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0)
// needed for GC haptics. Because we have to link CoreHaptics libs
// Comment to avoid linking CoreHaptics and using haptics feature.
#define ENABLE_FORGE_GC_HAPTICS
#endif
// Uncomment this to enable empty mounts
// used for absolute paths
// #define ENABLE_FS_EMPTY_MOUNT

#ifdef ENABLE_PROFILER
// Enable this if you want to have the profiler through a web browser, see PROFILE_WEBSERVER_PORT for server location
// #define ENABLE_PROFILER_WEBSERVER
#endif

// By default the UI uses 16bit indexes, enable define below to change it to 32bits
// #define FORGE_UI_USE_32BIT_INDEXES
#define FORGE_UI_MAX_VERTEXES (64 * 1024)
#define FORGE_UI_MAX_INDEXES  (128 * 1024)

// For allocating space in uniform block. Must match with shader and application.
// 804 aligns as multiple of the 67 bones used in the animation test closest to having a 64k uniform buffer
#ifndef MAX_SKELETON_BATCHER_BLOCK_INSTANCES
#define MAX_SKELETON_BATCHER_BLOCK_INSTANCES 804
#endif

#define VISIBILITY_BUFFER_MAX_GEOMETRY_SETS 3
// Number of bits required to represent all geometry set values
#define VISIBILITY_BUFFER_GEOMETRY_SET_BITS 2
#define VISIBILITY_BUFFER_MAX_NUM_BUFFERS   2

#if defined(AUTOMATED_TESTING)
// Used for automated testing, the app will exit after 240 frames
#define DEFAULT_AUTOMATION_FRAME_COUNT 240
// Used for automated testing, emulates 60fps to ensure screenshots always look the same
#define AUTOMATION_FIXED_FRAME_TIME    0.0167f
#endif

#if defined(TARGET_IOS) || (defined(ANDROID) && !defined(QUEST_VR)) || defined(NX64)
#define ENABLE_FORGE_TOUCH_INPUT
#endif

//////////////////////////////////////////////
//// Build related options
//////////////////////////////////////////////

// Compiling TheForge as a DLL:
//   - FORGE_API: is used to expose OS functionality, which usually should be accessible from all the program (Engine and Game) because it
//   contains the Assert/Logging/Memory functionality
//   - FORGE_RENDERER_API: is used to expose Renderer functionality, wich usually only the Engine needs access to (the Game should leverage
//   the Engine and never call the Renderer).
//                         This define allows to hide Renderer functionality to the Game, so that if someone tries to use it it'll get a
//                         linker error.
//   - FORGE_TOOL_API: is used to expose tool functionality, mostly tool filesystem.
#define FORGE_API
#define FORGE_RENDERER_API
#define FORGE_TOOL_API

#ifndef FORGE_DEBUG
#if defined(DEBUG) || defined(_DEBUG) || defined(AUTOMATED_TESTING)
#define FORGE_DEBUG
#endif
#endif

#if defined(FORGE_DEBUG)
#define IF_FORGE_DEBUG(...) __VA_ARGS__
#else
#define IF_FORGE_DEBUG(...)
#endif

#define ENABLE_LOGGING
#define DEFAULT_LOG_LEVEL eALL
#if !defined(NDEBUG)
#define ENABLE_MEMORY_TRACKING
#endif
// #define ENABLE_FORGE_STACKTRACE_DUMP

#ifdef AUTOMATED_TESTING
#if defined(NX64) || (defined(_WINDOWS) && defined(_M_X64)) || defined(ORBIS)
#define ENABLE_FORGE_STACKTRACE_DUMP
#endif
#endif

//////////////////////////////////////////////
//// External libraries options
//////////////////////////////////////////////
// Allows conversion of non-owning bstrings to owning bstrings
#define BSTR_ENABLE_STATIC_TO_DYNAMIC_CONVERSION

//////////////////////////////////////////////
//// Config validation
//////////////////////////////////////////////
#if !defined(ENABLE_LOGGING) && !defined(DEFAULT_LOG_LEVEL)
#define DEFAULT_LOG_LEVEL eNONE
#endif

#if defined(_DEBUG) && defined(NDEBUG)
#error "_DEBUG and NDEBUG are defined at the same time"
#endif
#endif

#define VR_API_OPENXR