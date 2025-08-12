#if !defined(NA_ERROR_CODES_HPP)
#define NA_ERROR_CODES_HPP

namespace Na {
	enum class FileErrorCode : u8 {
		None = 0,
		NotFound, PermissionDenied, InvalidFormat, Unknown
	};

	enum class ShaderErrorCode : u8 {
		None = 0,
		CompilationFailed, InvalidStage, InvalidEntryPoint, Unknown
	};
} // namespace Na

#endif // NA_ERROR_CODES_HPP