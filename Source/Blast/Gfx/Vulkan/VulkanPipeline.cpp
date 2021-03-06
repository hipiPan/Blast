#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanRenderTarget.h"
#include "VulkanShader.h"
#include "VulkanSampler.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace Blast {
    VulkanDescriptorSet::VulkanDescriptorSet(VulkanContext* context, VulkanRootSignature* rootSignature, const uint8_t& set) {
        mContext = context;
        mRootSignature = rootSignature;
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = mContext->getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &rootSignature->mSetLayoutMap[set];
        VK_ASSERT(vkAllocateDescriptorSets(mContext->getDevice(), &allocInfo, &mSet));
    }

    VulkanDescriptorSet::~VulkanDescriptorSet() {
        vkFreeDescriptorSets(mContext->getDevice(), mContext->getDescriptorPool(), 1, &mSet);
    }

    void VulkanDescriptorSet::setSampler(const uint8_t& reg, GfxSampler* sampler) {
        VulkanSampler* internelSampler = static_cast<VulkanSampler*>(sampler);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = internelSampler->getHandle();
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mSet;
        descriptorWrite.dstBinding = reg;
        descriptorWrite.pImageInfo = &imageInfo;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        vkUpdateDescriptorSets(mContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    void VulkanDescriptorSet::setTexture(const uint8_t& reg, GfxTexture* texture) {
        VulkanTexture* internelTexture = static_cast<VulkanTexture*>(texture);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageView = internelTexture->getSRV(0, 0);
        imageInfo.imageLayout = toVulkanImageLayout(internelTexture->getResourceState());
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mSet;
        descriptorWrite.dstBinding = reg;
        descriptorWrite.pImageInfo = &imageInfo;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        vkUpdateDescriptorSets(mContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    void VulkanDescriptorSet::setCombinedSampler(const uint8_t& reg, GfxTexture* texture, GfxSampler* sampler) {
        VulkanTexture* internelTexture = static_cast<VulkanTexture*>(texture);
        VulkanSampler* internelSampler = static_cast<VulkanSampler*>(sampler);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageView = internelTexture->getSRV(0, 0);
        imageInfo.sampler = internelSampler->getHandle();
        imageInfo.imageLayout = toVulkanImageLayout(internelTexture->getResourceState());
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mSet;
        descriptorWrite.dstBinding = reg;
        descriptorWrite.pImageInfo = &imageInfo;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        vkUpdateDescriptorSets(mContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    void VulkanDescriptorSet::setRWTexture(const uint8_t& reg, GfxTexture* texture) {
        VulkanTexture* internelTexture = static_cast<VulkanTexture*>(texture);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageView = internelTexture->getUAV(0, 0);
        imageInfo.imageLayout = toVulkanImageLayout(internelTexture->getResourceState());
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mSet;
        descriptorWrite.dstBinding = reg;
        descriptorWrite.pImageInfo = &imageInfo;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        vkUpdateDescriptorSets(mContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    void VulkanDescriptorSet::setUniformBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) {
        VulkanBuffer* internelBuffer = static_cast<VulkanBuffer*>(buffer);
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = internelBuffer->getHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = size;
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mSet;
        descriptorWrite.dstBinding = reg;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vkUpdateDescriptorSets(mContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    void VulkanDescriptorSet::setRWBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) {
        VulkanBuffer* internelBuffer = static_cast<VulkanBuffer*>(buffer);
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = internelBuffer->getHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = size;
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mSet;
        descriptorWrite.dstBinding = reg;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        vkUpdateDescriptorSets(mContext->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    VulkanRootSignature::VulkanRootSignature(VulkanContext *context, const GfxRootSignatureDesc &desc)
    :GfxRootSignature(desc) {
        mContext = context;

        std::map<int, std::vector<VkDescriptorSetLayoutBinding>> bindingsMap;
        for (int i = 0; i < desc.registers.size(); ++i) {
            const GfxRegisterInfo& registerInfo = desc.registers[i];
            VkDescriptorSetLayoutBinding binding = {};
            binding.pImmutableSamplers = nullptr;
            binding.binding = registerInfo.reg;
            binding.descriptorCount = registerInfo.size;
            binding.descriptorType = toVulkanDescriptorType(registerInfo.type);
            binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
            bindingsMap[registerInfo.set].push_back(binding);
        }

        std::vector<VkDescriptorSetLayout> layouts;
        for (auto& bindings : bindingsMap) {
            VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutCreateInfo.bindingCount = bindings.second.size();
            layoutCreateInfo.pBindings = bindings.second.data();
            VkDescriptorSetLayout layout = VK_NULL_HANDLE;
            VK_ASSERT(vkCreateDescriptorSetLayout(mContext->getDevice(), &layoutCreateInfo, nullptr, &layout));
            mSetLayoutMap[bindings.first] = layout;
            layouts.push_back(layout);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = layouts.size();
        pipelineLayoutInfo.pSetLayouts = layouts.data();

        VK_ASSERT(vkCreatePipelineLayout(mContext->getDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout));
    }

    VulkanRootSignature::~VulkanRootSignature() {
        for (auto& layout : mSetLayoutMap) {
            vkDestroyDescriptorSetLayout(mContext->getDevice(), layout.second, nullptr);
        }

        vkDestroyPipelineLayout(mContext->getDevice(), mPipelineLayout, nullptr);
    }

    GfxDescriptorSet * VulkanRootSignature::allocateSet(const uint8_t& set) {
        return new VulkanDescriptorSet(mContext, this, set);
    }

    static VkPipelineColorBlendStateCreateInfo toBlendDesc(const GfxBlendState& state, VkPipelineColorBlendAttachmentState* attachments) {
        int blendDescIndex = 0;
        for (int i = 0; i < MAX_RENDER_TARGET_ATTACHMENTS; ++i) {
            if (state.targetMask & (1 << i)) {
                VkBool32 blendEnable =
                        (toVulkanBlendFactor(state.srcFactors[blendDescIndex]) != VK_BLEND_FACTOR_ONE ||
                         toVulkanBlendFactor(state.dstFactors[blendDescIndex]) != VK_BLEND_FACTOR_ZERO ||
                         toVulkanBlendFactor(state.srcAlphaFactors[blendDescIndex]) != VK_BLEND_FACTOR_ONE ||
                         toVulkanBlendFactor(state.dstAlphaFactors[blendDescIndex]) != VK_BLEND_FACTOR_ZERO);

                attachments[i].blendEnable = blendEnable;
                attachments[i].colorWriteMask = state.masks[blendDescIndex];
                attachments[i].srcColorBlendFactor = toVulkanBlendFactor(state.srcFactors[blendDescIndex]);
                attachments[i].dstColorBlendFactor = toVulkanBlendFactor(state.dstFactors[blendDescIndex]);
                attachments[i].colorBlendOp = toVulkanBlendOp(state.blendOps[blendDescIndex]);
                attachments[i].srcAlphaBlendFactor = toVulkanBlendFactor(state.srcAlphaFactors[blendDescIndex]);
                attachments[i].dstAlphaBlendFactor = toVulkanBlendFactor(state.dstAlphaFactors[blendDescIndex]);
                attachments[i].alphaBlendOp = toVulkanBlendOp(state.blendAlphaOps[blendDescIndex]);
            }

            if (state.independentBlend)
                ++blendDescIndex;
        }

        VkPipelineColorBlendStateCreateInfo BlendStateInfo = {};
        BlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        BlendStateInfo.pNext = NULL;
        BlendStateInfo.flags = 0;
        BlendStateInfo.logicOpEnable = VK_FALSE;
        BlendStateInfo.logicOp = VK_LOGIC_OP_CLEAR;
        BlendStateInfo.pAttachments = attachments;
        BlendStateInfo.blendConstants[0] = 0.0f;
        BlendStateInfo.blendConstants[1] = 0.0f;
        BlendStateInfo.blendConstants[2] = 0.0f;
        BlendStateInfo.blendConstants[3] = 0.0f;

        return BlendStateInfo;
    }

    static VkPipelineDepthStencilStateCreateInfo toDepthDesc(const GfxDepthState& state) {
        VkPipelineDepthStencilStateCreateInfo DepthStencilStateInfo = {};
        DepthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        DepthStencilStateInfo.pNext = NULL;
        DepthStencilStateInfo.flags = 0;
        DepthStencilStateInfo.depthTestEnable = state.depthTest ? VK_TRUE : VK_FALSE;
        DepthStencilStateInfo.depthWriteEnable = state.depthWrite ? VK_TRUE : VK_FALSE;
        DepthStencilStateInfo.depthCompareOp = toVulkanCompareOp(state.depthFunc);
        DepthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
        DepthStencilStateInfo.stencilTestEnable = state.stencilTest ? VK_TRUE : VK_FALSE;

        DepthStencilStateInfo.front.failOp = toVulkanStencilOp(state.stencilFrontFail);
        DepthStencilStateInfo.front.passOp = toVulkanStencilOp(state.stencilFrontPass);
        DepthStencilStateInfo.front.depthFailOp = toVulkanStencilOp(state.depthFrontFail);
        DepthStencilStateInfo.front.compareOp = toVulkanCompareOp(state.stencilFrontFunc);
        DepthStencilStateInfo.front.compareMask = state.stencilReadMask;
        DepthStencilStateInfo.front.writeMask = state.stencilWriteMask;
        DepthStencilStateInfo.front.reference = 0;

        DepthStencilStateInfo.back.failOp = toVulkanStencilOp(state.stencilBackFail);
        DepthStencilStateInfo.back.passOp = toVulkanStencilOp(state.stencilBackPass);
        DepthStencilStateInfo.back.depthFailOp = toVulkanStencilOp(state.depthBackFail);
        DepthStencilStateInfo.back.compareOp = toVulkanCompareOp(state.stencilBackFunc);
        DepthStencilStateInfo.back.compareMask = state.stencilReadMask;
        DepthStencilStateInfo.back.writeMask = state.stencilWriteMask;
        DepthStencilStateInfo.back.reference = 0;

        DepthStencilStateInfo.minDepthBounds = 0;
        DepthStencilStateInfo.maxDepthBounds = 1;

        return DepthStencilStateInfo;
    }

    static VkPipelineRasterizationStateCreateInfo toRasterizerDesc(const GfxRasterizerState& state) {
        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
        rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateInfo.pNext = NULL;
        rasterizationStateInfo.flags = 0;
        rasterizationStateInfo.depthClampEnable = state.depthClampEnable ? VK_TRUE : VK_FALSE;
        rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateInfo.polygonMode = toVulkanFillMode(state.fillMode);
        rasterizationStateInfo.cullMode = toVulkanCullMode(state.cullMode);
        rasterizationStateInfo.frontFace = toVulkanFrontFace(state.frontFace);
        rasterizationStateInfo.depthBiasEnable = (state.depthBias != 0) ? VK_TRUE : VK_FALSE;
        rasterizationStateInfo.depthBiasConstantFactor = float(state.depthBias);
        rasterizationStateInfo.depthBiasClamp = 0.0f;
        rasterizationStateInfo.depthBiasSlopeFactor = state.slopeScaledDepthBias;
        rasterizationStateInfo.lineWidth = 1;

        return rasterizationStateInfo;
    }

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanContext *context, const GfxGraphicsPipelineDesc &desc)
        :GfxGraphicsPipeline(desc) {
        mContext = context;

        std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;
        if (desc.vertexShader) {
            VulkanShader* vulkanShader = static_cast<VulkanShader*>(desc.vertexShader);
            VkPipelineShaderStageCreateInfo shaderInfo = {};
            shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderInfo.module = vulkanShader->getHandle();
            shaderInfo.pName = "main";
            shaderInfos.push_back(shaderInfo);
        }

        if (desc.hullShader) {
            VulkanShader* vulkanShader = static_cast<VulkanShader*>(desc.hullShader);
            VkPipelineShaderStageCreateInfo shaderInfo = {};
            shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            shaderInfo.module = vulkanShader->getHandle();
            shaderInfo.pName = "main";
            shaderInfos.push_back(shaderInfo);
        }

        if (desc.domainShader) {
            VulkanShader* vulkanShader = static_cast<VulkanShader*>(desc.domainShader);
            VkPipelineShaderStageCreateInfo shaderInfo = {};
            shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            shaderInfo.module = vulkanShader->getHandle();
            shaderInfo.pName = "main";
            shaderInfos.push_back(shaderInfo);
        }

        if (desc.geometryShader) {
            VulkanShader* vulkanShader = static_cast<VulkanShader*>(desc.geometryShader);
            VkPipelineShaderStageCreateInfo shaderInfo = {};
            shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            shaderInfo.module = vulkanShader->getHandle();
            shaderInfo.pName = "main";
            shaderInfos.push_back(shaderInfo);
        }

        if (desc.pixelShader) {
            VulkanShader* vulkanShader = static_cast<VulkanShader*>(desc.pixelShader);
            VkPipelineShaderStageCreateInfo shaderInfo = {};
            shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderInfo.module = vulkanShader->getHandle();
            shaderInfo.pName = "main";
            shaderInfos.push_back(shaderInfo);
        }

        // 设置当前的管线的顶点布局
        uint32_t inputBindingCount = 0;
        VkVertexInputBindingDescription inputBindings[MAX_VERTEX_BINDINGS] = { { 0 } };
        uint32_t inputAttributeCount = 0;
        VkVertexInputAttributeDescription inputAttributes[MAX_VERTEX_ATTRIBS] = { { 0 } };
        uint32_t attribCount = desc.vertexLayout.attribCount > MAX_VERTEX_ATTRIBS ? MAX_VERTEX_ATTRIBS : desc.vertexLayout.attribCount;
        uint32_t bindingValue = UINT32_MAX;
        for (int i = 0; i < attribCount; ++i) {
            const GfxVertexAttrib* attrib = &(desc.vertexLayout.attribs[i]);

            if (bindingValue != attrib->binding) {
                bindingValue = attrib->binding;
                ++inputBindingCount;
            }

            inputBindings[inputBindingCount - 1].binding = bindingValue;
            if (attrib->rate == VERTEX_ATTRIB_RATE_INSTANCE) {
                inputBindings[inputBindingCount - 1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            } else {
                inputBindings[inputBindingCount - 1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
            inputBindings[inputBindingCount - 1].stride += mContext->getFormatStride(attrib->format);

            inputAttributes[inputAttributeCount].location = attrib->location;
            inputAttributes[inputAttributeCount].binding = attrib->binding;
            inputAttributes[inputAttributeCount].format = toVulkanFormat(attrib->format);
            inputAttributes[inputAttributeCount].offset = attrib->offset;
            ++inputAttributeCount;
        }
        VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
        vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateInfo.pNext = NULL;
        vertexInputStateInfo.flags = 0;
        vertexInputStateInfo.vertexBindingDescriptionCount = inputBindingCount;
        vertexInputStateInfo.pVertexBindingDescriptions = inputBindings;
        vertexInputStateInfo.vertexAttributeDescriptionCount = inputAttributeCount;
        vertexInputStateInfo.pVertexAttributeDescriptions = inputAttributes;

        // 设置当前管线的图元单位
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        switch (desc.rasterizerState.primitiveTopo) {
            case PRIMITIVE_TOPO_POINT_LIST: topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
            case PRIMITIVE_TOPO_LINE_LIST: topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
            case PRIMITIVE_TOPO_LINE_STRIP: topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
            case PRIMITIVE_TOPO_TRI_STRIP: topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
            case PRIMITIVE_TOPO_PATCH_LIST: topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
            case PRIMITIVE_TOPO_TRI_LIST: topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        }
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
        inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.pNext = NULL;
        inputAssemblyStateInfo.flags = 0;
        inputAssemblyStateInfo.topology = topology;
        inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

        // 设置当前管线的viewport,目前由外部动态设置
        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.pNext = NULL;
        viewportStateInfo.flags = 0;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = NULL;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = NULL;

        // 设置当前管线的多重采样
        VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {};
        multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateInfo.pNext = NULL;
        multisampleStateInfo.flags = 0;
        multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateInfo.minSampleShading = 0.0f;
        multisampleStateInfo.pSampleMask = 0;
        multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
        rasterizationStateInfo = toRasterizerDesc(desc.rasterizerState);

        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
        depthStencilStateInfo = toDepthDesc(desc.depthState);

        // 设置当前管线的混合状态
        VkPipelineColorBlendStateCreateInfo blendStateInfo = {};
        VkPipelineColorBlendAttachmentState blendAttachment[MAX_RENDER_TARGET_ATTACHMENTS];
        blendStateInfo = toBlendDesc(desc.blendState, blendAttachment);
        blendStateInfo.attachmentCount = desc.renderPass->getColorAttachmentCount();

        VkDynamicState dynamicStates[6];
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        dynamicStates[2] = VK_DYNAMIC_STATE_DEPTH_BIAS;
        dynamicStates[3] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
        dynamicStates[4] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
        dynamicStates[5] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext = NULL;
        dynamicStateInfo.flags = 0;
        dynamicStateInfo.dynamicStateCount = 6;
        dynamicStateInfo.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = NULL;
        pipelineInfo.flags = 0;
        pipelineInfo.stageCount = shaderInfos.size();
        pipelineInfo.pStages = shaderInfos.data();
        pipelineInfo.pVertexInputState = &vertexInputStateInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
        pipelineInfo.pTessellationState = NULL;
        pipelineInfo.pViewportState = &viewportStateInfo;
        pipelineInfo.pRasterizationState = &rasterizationStateInfo;
        pipelineInfo.pMultisampleState = &multisampleStateInfo;
        pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
        pipelineInfo.pColorBlendState = &blendStateInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = static_cast<VulkanRootSignature*>(desc.rootSignature)->getPipelineLayout();
        pipelineInfo.renderPass = static_cast<VulkanRenderPass*>(desc.renderPass)->getHandle();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;
        VK_ASSERT(vkCreateGraphicsPipelines(mContext->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline));

    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
        vkDestroyPipeline(mContext->getDevice(), mPipeline, nullptr);
    }
}