#pragma once
#include <stdint.h>
#include "../Utility/Flag.h"

namespace Blast {

    enum StencilOp {
        STENCIL_OP_KEEP,
        STENCIL_OP_ZERO,
        STENCIL_OP_REPLACE,
        STENCIL_OP_INCR_SAT,
        STENCIL_OP_DECR_SAT,
        STENCIL_OP_INVERT,
        STENCIL_OP_INCR,
        STENCIL_OP_DECR,
    };

    enum BlendConstant {
        BLEND_ZERO,
        BLEND_ONE,
        BLEND_SRC_COLOR,
        BLEND_INV_SRC_COLOR,
        BLEND_SRC_ALPHA,
        BLEND_INV_SRC_ALPHA,
        BLEND_DEST_ALPHA,
        BLEND_INV_DEST_ALPHA,
        BLEND_DEST_COLOR,
        BLEND_INV_DEST_COLOR,
        BLEND_SRC_ALPHA_SAT,
        BLEND_BLEND_FACTOR,
        BLEND_INV_BLEND_FACTOR,
        BLEND_SRC1_COLOR,
        BLEND_INV_SRC1_COLOR,
        BLEND_SRC1_ALPHA,
        BLEND_INV_SRC1_ALPHA,
    };

    enum ColorWriteEnable {
        COLOR_WRITE_DISABLE = 0,
        COLOR_WRITE_ENABLE_RED = 1,
        COLOR_WRITE_ENABLE_GREEN = 2,
        COLOR_WRITE_ENABLE_BLUE = 4,
        COLOR_WRITE_ENABLE_ALPHA = 8,
        COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) | COLOR_WRITE_ENABLE_BLUE) | COLOR_WRITE_ENABLE_ALPHA)
    };

    enum BlendOp {
        BLEND_OP_ADD,
        BLEND_OP_SUBTRACT,
        BLEND_OP_REV_SUBTRACT,
        BLEND_OP_MIN,
        BLEND_OP_MAX,
    };

    enum FillMode {
        FILL_WIREFRAME,
        FILL_SOLID,
    };

    enum CullMode {
        CULL_NONE,
        CULL_FRONT,
        CULL_BACK,
    };

    enum AddressMode {
        ADDRESS_MODE_MIRROR,
        ADDRESS_MODE_REPEAT,
        ADDRESS_MODE_CLAMP_TO_EDGE,
        ADDRESS_MODE_CLAMP_TO_BORDER
    };

    enum MipMapMode {
        MIPMAP_MODE_NEAREST = 0,
        MIPMAP_MODE_LINEAR
    };

    enum SampleCount {
        SAMPLE_COUNT_1 = 1,
        SAMPLE_COUNT_2 = 2,
        SAMPLE_COUNT_4 = 4,
        SAMPLE_COUNT_8 = 8,
        SAMPLE_COUNT_16 = 16,
    };

    enum Format {
        FORMAT_UNKNOWN,

        FORMAT_R32G32B32A32_FLOAT,
        FORMAT_R32G32B32A32_UINT,
        FORMAT_R32G32B32A32_SINT,

        FORMAT_R32G32B32_FLOAT,
        FORMAT_R32G32B32_UINT,
        FORMAT_R32G32B32_SINT,

        FORMAT_R16G16B16A16_FLOAT,
        FORMAT_R16G16B16A16_UNORM,
        FORMAT_R16G16B16A16_UINT,
        FORMAT_R16G16B16A16_SNORM,
        FORMAT_R16G16B16A16_SINT,

        FORMAT_R32G32_FLOAT,
        FORMAT_R32G32_UINT,
        FORMAT_R32G32_SINT,
        FORMAT_R32G8X24_TYPELESS,
        FORMAT_D32_FLOAT_S8X24_UINT,

        FORMAT_R10G10B10A2_UNORM,
        FORMAT_R10G10B10A2_UINT,
        FORMAT_R11G11B10_FLOAT,
        FORMAT_R8G8B8A8_UNORM,
        FORMAT_R8G8B8A8_UNORM_SRGB,
        FORMAT_R8G8B8A8_UINT,
        FORMAT_R8G8B8A8_SNORM,
        FORMAT_R8G8B8A8_SINT,
        FORMAT_B8G8R8A8_UNORM,
        FORMAT_B8G8R8A8_UNORM_SRGB,
        FORMAT_R16G16_FLOAT,
        FORMAT_R16G16_UNORM,
        FORMAT_R16G16_UINT,
        FORMAT_R16G16_SNORM,
        FORMAT_R16G16_SINT,
        FORMAT_R32_TYPELESS,
        FORMAT_D32_FLOAT,
        FORMAT_R32_FLOAT,
        FORMAT_R32_UINT,
        FORMAT_R32_SINT,
        FORMAT_R24G8_TYPELESS,
        FORMAT_D24_UNORM_S8_UINT,

        FORMAT_R8G8_UNORM,
        FORMAT_R8G8_UINT,
        FORMAT_R8G8_SNORM,
        FORMAT_R8G8_SINT,
        FORMAT_R16_TYPELESS,
        FORMAT_R16_FLOAT,
        FORMAT_D16_UNORM,
        FORMAT_R16_UNORM,
        FORMAT_R16_UINT,
        FORMAT_R16_SNORM,
        FORMAT_R16_SINT,

        FORMAT_R8_UNORM,
        FORMAT_R8_UINT,
        FORMAT_R8_SNORM,
        FORMAT_R8_SINT,

        FORMAT_BC1_UNORM,
        FORMAT_BC1_UNORM_SRGB,
        FORMAT_BC2_UNORM,
        FORMAT_BC2_UNORM_SRGB,
        FORMAT_BC3_UNORM,
        FORMAT_BC3_UNORM_SRGB,
        FORMAT_BC4_UNORM,
        FORMAT_BC4_SNORM,
        FORMAT_BC5_UNORM,
        FORMAT_BC5_SNORM,
        FORMAT_BC6H_UF16,
        FORMAT_BC6H_SF16,
        FORMAT_BC7_UNORM,
        FORMAT_BC7_UNORM_SRGB,

        FORMAT_X8_D24_UNORM_PACK32,
        FORMAT_D32_SFLOAT,
        FORMAT_S8_UINT,
        FORMAT_D16_UNORM_S8_UINT,
        FORMAT_D32_SFLOAT_S8_UINT
    };

    enum ResourceUsage {
        RESOURCE_USAGE_UNKNOWN = 0,
        RESOURCE_USAGE_GPU_ONLY = 1,
        RESOURCE_USAGE_CPU_ONLY = 2,
        RESOURCE_USAGE_CPU_TO_GPU = 3,
        RESOURCE_USAGE_GPU_TO_CPU = 4,
        RESOURCE_USAGE_COUNT,
        RESOURCE_USAGE_MAX_ENUM = 0x7FFFFFFF
    };

    enum ResourceState {
        RESOURCE_STATE_UNDEFINED = 0,
        RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
        RESOURCE_STATE_INDEX_BUFFER = 0x2,
        RESOURCE_STATE_RENDER_TARGET = 0x4,
        RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
        RESOURCE_STATE_DEPTH_WRITE = 0x10,
        RESOURCE_STATE_DEPTH_READ = 0x20,
        RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
        RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
        RESOURCE_STATE_STREAM_OUT = 0x100,
        RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
        RESOURCE_STATE_COPY_DEST = 0x400,
        RESOURCE_STATE_COPY_SOURCE = 0x800,
        RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
        RESOURCE_STATE_PRESENT = 0x1000,
        RESOURCE_STATE_COMMON = 0x2000,
        RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x4000,
    };
    MAKE_ENUM_FLAG(uint32_t, ResourceState)

    enum ResourceType {
        RESOURCE_TYPE_UNDEFINED = 0,
        RESOURCE_TYPE_SAMPLER = 0x01,
        RESOURCE_TYPE_TEXTURE = (RESOURCE_TYPE_SAMPLER << 1),
        RESOURCE_TYPE_RW_TEXTURE = (RESOURCE_TYPE_TEXTURE << 1),
        RESOURCE_TYPE_RW_BUFFER = (RESOURCE_TYPE_RW_TEXTURE << 1),
        RESOURCE_TYPE_UNIFORM_BUFFER = (RESOURCE_TYPE_RW_BUFFER << 1),
        RESOURCE_TYPE_VERTEX_BUFFER = (RESOURCE_TYPE_UNIFORM_BUFFER << 1),
        RESOURCE_TYPE_INDEX_BUFFER = (RESOURCE_TYPE_VERTEX_BUFFER << 1),
        RESOURCE_TYPE_INDIRECT_BUFFER = (RESOURCE_TYPE_INDEX_BUFFER << 1),
        RESOURCE_TYPE_TEXTURE_CUBE = (RESOURCE_TYPE_TEXTURE | (RESOURCE_TYPE_INDIRECT_BUFFER << 1)),
    };
    MAKE_ENUM_FLAG(uint32_t, ResourceType)

    enum LoadAction {
        LOAD_ACTION_DONTCARE,
        LOAD_ACTION_LOAD,
        LOAD_ACTION_CLEAR,
        MAX_LOAD_ACTION
    };
}
