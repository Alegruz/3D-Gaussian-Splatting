#include "3dgs/graphics/CommandBuffer.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/FrameResource.h"
#include "3dgs/graphics/SwapChain.h"
#include "3dgs/graphics/Texture.h"

namespace iiixrlab::graphics
{
    CommandBuffer::CommandBuffer(const CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mCommandBuffer(createInfo.CommandBuffer)
		, mFrameResourceOrNull(nullptr)
    {
        assert(mCommandBuffer != VK_NULL_HANDLE);
    }

    CommandBuffer::~CommandBuffer() noexcept
    {
        mDevice.DestroyCommandBuffer(mCommandBuffer);
    }

    void CommandBuffer::Barrier(const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask, const VkImageMemoryBarrier& imageMemoryBarriers) noexcept
    {
        vkCmdPipelineBarrier(mCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarriers);
    }

    void CommandBuffer::Begin(FrameResource& frameResource) noexcept
    {
		mFrameResourceOrNull = &frameResource;
        Texture& backBuffer = frameResource.GetBackBuffer();
        Texture& depthBuffer = frameResource.GetDepthBuffer();

        VkCommandBufferBeginInfo commandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };
        VkResult vr = vkBeginCommandBuffer(mCommandBuffer, &commandBufferBeginInfo);
        assert(vr == VK_SUCCESS);

		VkImageMemoryBarrier backBufferMemoryBarrier = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = backBuffer.GetImage(),
			.subresourceRange = 
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};

		Barrier(
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			backBufferMemoryBarrier);

		VkImageMemoryBarrier depthBufferMemoryBarrier = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = depthBuffer.GetImage(),
			.subresourceRange = 
			{
				.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};

		Barrier(
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			depthBufferMemoryBarrier);

		VkRenderingAttachmentInfo colorAttachmentInfo =
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = backBuffer.GetColorAttachmentViewOrNull(),
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearValue{.color = {0.0f, 0.0f, 0.0f, 1.0f}},
		};

		VkRenderingAttachmentInfo depthAttachmentInfo =
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = depthBuffer.GetDepthStencilViewOrNull(),
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = VkClearValue{.depthStencil = {1.0f, 0}},
		};

        const SwapChain& swapChain = frameResource.GetSwapChain();
        const VkExtent2D& extent = swapChain.GetExtent();

		VkRenderingInfo renderingInfo =
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderArea = VkRect2D{.offset = VkOffset2D{.x = 0, .y = 0}, .extent = extent},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
			.pDepthAttachment = &depthAttachmentInfo,
		};

		vkCmdBeginRendering(mCommandBuffer, &renderingInfo);

        VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);

        VkRect2D scissor =
        {
            .offset = {0, 0},
            .extent = extent,
        };
        vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
    }

    void CommandBuffer::BindDescriptorSets(const VkPipelineLayout pipelineLayout, const VkDescriptorSet& descriptorSet) noexcept
    {
        vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    }
	
	void CommandBuffer::BindPipeline(const VkPipeline pipeline) noexcept
	{
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

    void CommandBuffer::End() noexcept
    {
        Texture& backBuffer = mFrameResourceOrNull->GetBackBuffer();
		
        VkResult vr = VK_SUCCESS;

		vkCmdEndRendering(mCommandBuffer);
		
		VkImageMemoryBarrier backBufferMemoryBarrier = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = 0,
			.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = backBuffer.GetImage(),
			.subresourceRange = 
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};

		Barrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_NONE, backBufferMemoryBarrier);

		vr = vkEndCommandBuffer(mCommandBuffer);
        assert(vr == VK_SUCCESS);
    }

    void CommandBuffer::Reset() noexcept
    {
        VkResult vr = vkResetCommandBuffer(mCommandBuffer, 0);
        assert(vr == VK_SUCCESS);
    }
}   // namespace iiixrlab::graphics