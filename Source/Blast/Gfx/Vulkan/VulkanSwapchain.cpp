#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#if WIN32
#include <windows.h>
#endif
#include <cmath>

namespace Blast {
    VulkanSurface::VulkanSurface(VulkanContext* context, const GfxSurfaceDesc& desc)
    :GfxSurface(desc) {
        mContext = context;
#if WIN32
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = ::GetModuleHandle(nullptr);
        surfaceInfo.hwnd = (HWND)desc.originSurface;
        VK_ASSERT(vkCreateWin32SurfaceKHR(mContext->getInstance(), &surfaceInfo, nullptr, &mSurface));
#endif
        VulkanSwapchainSupportDetails swapChainSupport = querySwapChainSupport(mSurface);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

        mFormat = toGfxFormat(surfaceFormat.format);
        mPresentMode = presentMode;
        mSurfaceFormat = surfaceFormat;
    }

    VulkanSurface::~VulkanSurface() {
        vkDestroySurfaceKHR(mContext->getInstance(), mSurface, nullptr);
    }

    VkSurfaceCapabilitiesKHR VulkanSurface::getCapabilities() {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext->getPhyDevice(), mSurface, &capabilities);
        return capabilities;
    }

    VulkanSwapchainSupportDetails VulkanSurface::querySwapChainSupport(VkSurfaceKHR surface) {
        VulkanSwapchainSupportDetails details;

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->getPhyDevice(), surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->getPhyDevice(), surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->getPhyDevice(), surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->getPhyDevice(), surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanSurface::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSurface::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VulkanSwapchain::VulkanSwapchain(VulkanContext* context, const GfxSwapchainDesc& desc)
    :GfxSwapchain(desc) {
        mContext = context;

        VulkanSurface* internelSurface = static_cast<VulkanSurface*>(desc.surface);

        VkBool32 supportsPresent;
        VulkanQueue* GraphicsQueue = (VulkanQueue*)mContext->getQueue(QUEUE_TYPE_GRAPHICS);
        uint32_t queueFamilyIndice = GraphicsQueue->getFamilyIndex();
        vkGetPhysicalDeviceSurfaceSupportKHR(mContext->getPhyDevice(), queueFamilyIndice, internelSurface->getHandle(), &supportsPresent);
        if (!supportsPresent) {
            BLAST_LOGE("cannot find a graphics queue that also supports present operations.\n");
            return;
        }

        const auto caps = internelSurface->getCapabilities();

        const uint32_t maxImageCount = caps.maxImageCount;
        const uint32_t minImageCount = caps.minImageCount;
        uint32_t desiredImageCount = minImageCount + 1;
        if (maxImageCount != 0 && desiredImageCount > maxImageCount) {
            BLAST_LOGE("swap chain does not support %d images.\n", desiredImageCount);
            desiredImageCount = minImageCount;
        }
        mImageCount = desiredImageCount;

        VkExtent2D actualExtent;
        actualExtent.width = mWidth;
        actualExtent.height = mHeight;
        // actualExtent.width = max(caps.minImageExtent.width, min(caps.maxImageExtent.width, actualExtent.width));
        // actualExtent.height = max(caps.minImageExtent.height, min(caps.maxImageExtent.height, actualExtent.height));

        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = internelSurface->getHandle();
        swapchainInfo.minImageCount = mImageCount;
        swapchainInfo.imageFormat = internelSurface->getSurfaceFormat().format;
        swapchainInfo.imageColorSpace = internelSurface->getSurfaceFormat().colorSpace;
        swapchainInfo.imageExtent = actualExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 1;
        swapchainInfo.pQueueFamilyIndices = &queueFamilyIndice;
        swapchainInfo.preTransform = internelSurface->getCapabilities().currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = internelSurface->getPresentMode();
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
        if (desc.oldSwapchain) {
            VulkanSwapchain* oldSwapchain = static_cast<VulkanSwapchain*>(desc.oldSwapchain);
            swapchainInfo.oldSwapchain = oldSwapchain->mSwapchain;
        }

        VK_ASSERT(vkCreateSwapchainKHR(mContext->getDevice(), &swapchainInfo, nullptr, &mSwapchain));

        std::vector<VkImage> images;
        Format imageFormat = internelSurface->getFormat();
        vkGetSwapchainImagesKHR(mContext->getDevice(), mSwapchain, &mImageCount, nullptr);
        images.resize(mImageCount);
        vkGetSwapchainImagesKHR(mContext->getDevice(), mSwapchain, &mImageCount, images.data());

        for (int i = 0; i < mImageCount; i++) {
            GfxTextureDesc colorDesc;
            colorDesc.sampleCount = SAMPLE_COUNT_1;
            colorDesc.width = actualExtent.width;
            colorDesc.height = actualExtent.height;
            colorDesc.depth = 1;
            colorDesc.mipLevels = 1;
            colorDesc.arrayLayers = 1;
            colorDesc.format = imageFormat;
            colorDesc.state = RESOURCE_STATE_UNDEFINED;
            colorDesc.type = RESOURCE_TYPE_TEXTURE;
            colorDesc.usage = RESOURCE_USAGE_GPU_ONLY;
            colorDesc.colorAtt = true;

            VulkanTexture* colorTexture = new VulkanTexture(mContext, images[i], colorDesc);
            mColorImages.push_back(colorTexture);

            GfxTextureDesc depthStencilDesc;
            depthStencilDesc.sampleCount = SAMPLE_COUNT_1;
            depthStencilDesc.width = actualExtent.width;
            depthStencilDesc.height = actualExtent.height;
            depthStencilDesc.depth = 1;
            depthStencilDesc.mipLevels = 1;
            depthStencilDesc.arrayLayers = 1;
            depthStencilDesc.format = FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.state = RESOURCE_STATE_UNDEFINED;
            depthStencilDesc.type = RESOURCE_TYPE_TEXTURE;
            depthStencilDesc.usage = RESOURCE_USAGE_GPU_ONLY;
            depthStencilDesc.depthStencilAtt = true;

            VulkanTexture* depthStencilTexture = new VulkanTexture(mContext, depthStencilDesc);
            mDepthStencilImages.push_back(depthStencilTexture);
        }
    }

    VulkanSwapchain::~VulkanSwapchain() {
        for (int i = 0; i < mColorImages.size(); i++) {
            delete mColorImages[i];
        }
        mColorImages.clear();

        for (int i = 0; i < mDepthStencilImages.size(); i++) {
            delete mDepthStencilImages[i];
        }
        mDepthStencilImages.clear();
        vkDestroySwapchainKHR(mContext->getDevice(), mSwapchain, nullptr);
    }

    GfxTexture* VulkanSwapchain::getColorRenderTarget(uint32_t idx) {
        if (idx >= 0 && idx < mColorImages.size()) {
            return mColorImages[idx];
        }
        return nullptr;
    }

    GfxTexture* VulkanSwapchain::getDepthRenderTarget(uint32_t idx) {
        if (idx >= 0 && idx < mDepthStencilImages.size()) {
            return mDepthStencilImages[idx];
        }
        return nullptr;
    }

    uint32_t VulkanSwapchain::getImageCount() {
        return mImageCount;
    }
}