#pragma once

namespace nCine
{
	/// The interface class to query runtime OpenGL device capabilities
	class IGfxCapabilities
	{
	public:
		/// OpenGL version components
		enum class GLVersion
		{
			MAJOR,
			MINOR,
			RELEASE
		};

		/// OpenGL information strings
		struct GlInfoStrings
		{
			const char* vendor = nullptr;
			const char* renderer = nullptr;
			const char* glVersion = nullptr;
			const char* glslVersion = nullptr;
		};

		/// OpenGL queryable runtime integer values
		struct GLIntValues
		{
			enum Enum
			{
				MAX_TEXTURE_SIZE = 0,
				MAX_TEXTURE_IMAGE_UNITS,
				MAX_UNIFORM_BLOCK_SIZE,
				MAX_UNIFORM_BUFFER_BINDINGS,
				MAX_VERTEX_UNIFORM_BLOCKS,
				MAX_FRAGMENT_UNIFORM_BLOCKS,
				UNIFORM_BUFFER_OFFSET_ALIGNMENT,
				MAX_VERTEX_ATTRIB_STRIDE,
				MAX_COLOR_ATTACHMENTS,

				COUNT
			};
		};

		/// OpenGL queryable extensions
		struct GLExtensions
		{
			enum Enum
			{
				KHR_DEBUG = 0,
				ARB_TEXTURE_STORAGE,
				EXT_TEXTURE_COMPRESSION_S3TC,
				OES_COMPRESSED_ETC1_RGB8_TEXTURE,
				AMD_COMPRESSED_ATC_TEXTURE,
				IMG_TEXTURE_COMPRESSION_PVRTC,
				KHR_TEXTURE_COMPRESSION_ASTC_LDR,

				COUNT
			};
		};

		virtual ~IGfxCapabilities() = 0;

		/// Returns the OpenGL version numbers
		virtual int glVersion(GLVersion version) const = 0;
		/// Returns the OpenGL information strings structure
		virtual const GlInfoStrings& glInfoStrings() const = 0;
		/// Returns the value of a runtime OpenGL integer value
		virtual int value(GLIntValues::Enum valueName) const = 0;
		/// Returns true if the specified OpenGL extension is available
		virtual bool hasExtension(GLExtensions::Enum extensionName) const = 0;
	};

	inline IGfxCapabilities::~IGfxCapabilities() {}

	/// A fake graphics capabilities class that reports no available capabilities
	class NullGfxCapabilities : public IGfxCapabilities
	{
	public:
		inline int glVersion(GLVersion version) const override {
			return 0;
		}
		inline const GlInfoStrings& glInfoStrings() const override {
			return glInfoStrings_;
		};
		inline int value(GLIntValues::Enum valueName) const override {
			return 0;
		}
		inline bool hasExtension(GLExtensions::Enum extensionName) const override {
			return false;
		}

	private:
		GlInfoStrings glInfoStrings_;
	};

}
