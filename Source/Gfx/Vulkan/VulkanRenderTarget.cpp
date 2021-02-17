#include "VulkanRenderTarget.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include <vector>

namespace Blast {
    VulkanRenderTarget::VulkanRenderTarget(VulkanContext *context, const GfxRenderTargetDesc &desc)
    :GfxRenderTarget(desc) {
        mContext = context;

        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        VkAttachmentReference depthStencilAttachmentRef;

        uint32_t attachmentIdx = 0;
        for (uint32_t i = 0; i < mNumColorAttachments; i++) {
            VkAttachmentDescription attachmentDesc = {};
            attachmentDesc.flags = 0;
            attachmentDesc.format = toVulkanFormat(mColor[i].texture->getFormat());
            attachmentDesc.samples = toVulkanSampleCount(mColor[i].texture->getSampleCount());
            attachmentDesc.loadOp = toVulkanLoadOp(mColor[i].loadOp);
            attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachmentDesc);

            VkAttachmentReference colorReference;
            colorReference.attachment = attachmentIdx;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentRefs.push_back(colorReference);

            attachmentIdx++;
        }
        if (mHasDepth) {
            VkAttachmentDescription attachmentDesc = {};
            attachmentDesc.flags = 0;
            attachmentDesc.format = toVulkanFormat(mDepthStencil.texture->getFormat());
            attachmentDesc.samples = toVulkanSampleCount(mDepthStencil.texture->getSampleCount());
            attachmentDesc.loadOp = toVulkanLoadOp(mDepthStencil.depthLoadOp);
            attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.stencilLoadOp = toVulkanLoadOp(mDepthStencil.stencilLoadOp);
            attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachmentDesc);

            depthStencilAttachmentRef.attachment = attachmentIdx;
            depthStencilAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachmentIdx++;
        }

        VkSubpassDescription subpass = {};
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = NULL;
        subpass.colorAttachmentCount = mNumColorAttachments;
        subpass.pResolveAttachments = NULL;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = NULL;

        if (mNumColorAttachments > 0) {
            subpass.pColorAttachments = colorAttachmentRefs.data();
        } else {
            subpass.pColorAttachments = nullptr;
        }

        if (mHasDepth) {
            subpass.pDepthStencilAttachment = &depthStencilAttachmentRef;
        } else {
            subpass.pDepthStencilAttachment = nullptr;
        }

        VkRenderPassCreateInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = NULL;
        renderPassInfo.flags = 0;
        renderPassInfo.attachmentCount = attachmentIdx;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 0;
        renderPassInfo.pDependencies = NULL;

        VK_ASSERT(vkCreateRenderPass(mContext->getDevice(), &renderPassInfo, nullptr, &(mRenderPass)));

        std::vector<VkImageView> attachmentViews;
        for (int i = 0; i < mNumColorAttachments; i++) {
            VulkanTexture* vulkanTexture = (VulkanTexture*)mColor[i].texture;
            attachmentViews.push_back(vulkanTexture->getView());
        }

        if(mHasDepth) {
            VulkanTexture* vulkanTexture = (VulkanTexture*)mDepthStencil.texture;
            attachmentViews.push_back(vulkanTexture->getView());
        }

        VkFramebufferCreateInfo framebufferInfo;
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.flags = 0;
        framebufferInfo.attachmentCount = attachmentViews.size();
        framebufferInfo.pAttachments = attachmentViews.data();
        framebufferInfo.width = mColor[0].texture->getWidth();
        framebufferInfo.height = mColor[0].texture->getHeight();
        framebufferInfo.layers = 1;
        framebufferInfo.renderPass = mRenderPass;

        VK_ASSERT(vkCreateFramebuffer(mContext->getDevice(), &framebufferInfo, nullptr, &mFramebuffer));
    }

    VulkanRenderTarget::~VulkanRenderTarget() {
        vkDestroyFramebuffer(mContext->getDevice(), mFramebuffer, nullptr);
        vkDestroyRenderPass(mContext->getDevice(), mRenderPass, nullptr);
    }
}