#if !defined(NA_GRAPHICS_BARRIER_HPP)
#define NA_GRAPHICS_BARRIER_HPP

#include "Natrium/Core.hpp"

namespace Na::Graphics {
	enum class BarrierStageBits : u8 {
		None = 0,
		Earliest              = NA_BIT(0),
		VertexInput           = NA_BIT(1),
		VertexShader          = NA_BIT(2),
		FragmentShader        = NA_BIT(3),
		ColorAttachmentOutput = NA_BIT(4),
		ComputeShader         = NA_BIT(5),
		Transfer              = NA_BIT(6),

		All = Earliest | VertexInput | VertexShader | FragmentShader | ColorAttachmentOutput | ComputeShader | Transfer
	};

	enum class BarrierOperationBits : u16 {
		None = 0,

		ShaderRead  = NA_BIT(0),
		ShaderWrite = NA_BIT(1),

		TransferRead  = NA_BIT(2),
		TransferWrite = NA_BIT(3),

		IndexRead           = NA_BIT(4),
		VertexAttributeRead = NA_BIT(5),
		UniformRead         = NA_BIT(6),
		
		ColorAttachmentRead  = NA_BIT(7),
		ColorAttachmentWrite = NA_BIT(8),

		DepthStencilAttachmentRead  = NA_BIT(9),
		DepthStencilAttachmentWrite = NA_BIT(10),

		All = u16max
	};

	struct BarrierOperationInfo {
		BarrierStageBits stage;
		BarrierOperationBits op;
	};

	enum class DeviceImageState : u8 {
		None = 0,
		TransferSrc,
		TransferDst,
		Texture,
		StorageImage
	};

	struct DeviceImageBarrierInfo {
		DeviceImageState new_img_state;

		BarrierOperationInfo before; // Operations that must complete before the barrier
		BarrierOperationInfo after;  // Operations that will start after the barrier
	};

	inline BarrierStageBits operator|(BarrierStageBits lhs, BarrierStageBits rhs) { return (BarrierStageBits)((u8)lhs | (u8)rhs); }
	inline BarrierStageBits operator&(BarrierStageBits lhs, BarrierStageBits rhs) { return (BarrierStageBits)((u8)lhs & (u8)rhs); }
	inline BarrierStageBits operator^(BarrierStageBits lhs, BarrierStageBits rhs) { return (BarrierStageBits)((u8)lhs ^ (u8)rhs); }
	inline BarrierStageBits operator~(BarrierStageBits state) { return (BarrierStageBits)(~(u8)state); }

	inline BarrierStageBits& operator|=(BarrierStageBits& lhs, BarrierStageBits rhs) { lhs = lhs | rhs; return lhs; }
	inline BarrierStageBits& operator&=(BarrierStageBits& lhs, BarrierStageBits rhs) { lhs = lhs & rhs; return lhs; }
	inline BarrierStageBits& operator^=(BarrierStageBits& lhs, BarrierStageBits rhs) { lhs = lhs ^ rhs; return lhs; }

	inline bool operator==(BarrierStageBits lhs, BarrierStageBits rhs) { return (u8)lhs == (u8)rhs; }
	inline bool operator!=(BarrierStageBits lhs, BarrierStageBits rhs) { return (u8)lhs != (u8)rhs; }

	inline bool operator!(BarrierStageBits state) { return (u8)state == 0; }



	inline BarrierOperationBits operator|(BarrierOperationBits lhs, BarrierOperationBits rhs) { return (BarrierOperationBits)((u16)lhs | (u16)rhs); }
	inline BarrierOperationBits operator&(BarrierOperationBits lhs, BarrierOperationBits rhs) { return (BarrierOperationBits)((u16)lhs & (u16)rhs); }
	inline BarrierOperationBits operator^(BarrierOperationBits lhs, BarrierOperationBits rhs) { return (BarrierOperationBits)((u16)lhs ^ (u16)rhs); }
	inline BarrierOperationBits operator~(BarrierOperationBits state) { return (BarrierOperationBits)(~(u16)state); }

	inline BarrierOperationBits& operator|=(BarrierOperationBits& lhs, BarrierOperationBits rhs) { lhs = lhs | rhs; return lhs; }
	inline BarrierOperationBits& operator&=(BarrierOperationBits& lhs, BarrierOperationBits rhs) { lhs = lhs & rhs; return lhs; }
	inline BarrierOperationBits& operator^=(BarrierOperationBits& lhs, BarrierOperationBits rhs) { lhs = lhs ^ rhs; return lhs; }

	inline bool operator==(BarrierOperationBits lhs, BarrierOperationBits rhs) { return (u16)lhs == (u16)rhs; }
	inline bool operator!=(BarrierOperationBits lhs, BarrierOperationBits rhs) { return (u16)lhs != (u16)rhs; }

	inline bool operator!(BarrierOperationBits state) { return (u16)state == 0; }
} // namespace Na::Graphics

#endif // NA_GRAPHICS_BARRIER_HPP