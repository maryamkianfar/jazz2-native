#pragma once

#include "GLUniform.h"
#include "GLUniformBlock.h"
#include "GLAttribute.h"
#include "GLVertexFormat.h"
#include "../../Base/StaticHashMap.h"
#include "../../../Common.h"

#include <string>

#include <Containers/SmallVector.h>

using namespace Death::Containers;

namespace nCine
{
	class GLShader;

	/// A class to handle OpenGL shader programs
	class GLShaderProgram
	{
	public:
		enum class Introspection
		{
			Enabled,
			NoUniformsInBlocks,
			Disabled
		};

		enum class Status
		{
			NotLinked,
			CompilationFailed,
			LinkingFailed,
			Linked,
			LinkedWithDeferredQueries,
			LinkedWithIntrospection
		};

		enum class QueryPhase
		{
			Immediate,
			Deferred
		};

		GLShaderProgram();
		explicit GLShaderProgram(QueryPhase queryPhase);
		GLShaderProgram(const StringView& vertexFile, const StringView& fragmentFile, Introspection introspection, QueryPhase queryPhase);
		GLShaderProgram(const StringView& vertexFile, const StringView& fragmentFile, Introspection introspection);
		GLShaderProgram(const StringView& vertexFile, const StringView& fragmentFile);
		~GLShaderProgram();

		inline GLuint glHandle() const {
			return glHandle_;
		}
		inline Status status() const {
			return status_;
		}
		inline Introspection introspection() const {
			return introspection_;
		}
		inline QueryPhase queryPhase() const {
			return queryPhase_;
		}

		bool isLinked() const;

		/// Returns the length of the information log including the null termination character
		unsigned int retrieveInfoLogLength() const;
		/// Retrieves the information log and copies it in the provided string object
		void retrieveInfoLog(std::string& infoLog) const;

		/// Returns the total memory needed for all uniforms outside of blocks
		inline unsigned int uniformsSize() const {
			return uniformsSize_;
		}
		/// Returns the total memory needed for all uniforms inside of blocks
		inline unsigned int uniformBlocksSize() const {
			return uniformBlocksSize_;
		}

		bool attachShaderFromFile(GLenum type, const StringView& filename);
		bool attachShaderFromString(GLenum type, const char* string);
		bool attachShaderFromStrings(GLenum type, const char** strings);
		bool attachShaderFromStringsAndFile(GLenum type, const char** strings, const StringView& filename);
		bool link(Introspection introspection);
		void use();
		bool validate();
		
		/// Loads a shader program from a binary representation
		bool loadBinary(unsigned int binaryFormat, const void *buffer, int bufferSize);
		/// Returns the length in bytes of the binary representation of the shader program
		int binaryLength() const;
		/// Retrieves the binary representation of the shader program, if it is linked
		bool saveBinary(int bufferSize, unsigned int &binaryFormat, void *buffer) const;

		inline unsigned int numAttributes() const {
			return attributeLocations_.size();
		}
		inline bool hasAttribute(const char* name) const {
			return (attributeLocations_.find(String::nullTerminatedView(name)) != nullptr);
		}
		GLVertexFormat::Attribute* attribute(const char* name);

		inline void defineVertexFormat(const GLBufferObject* vbo) {
			defineVertexFormat(vbo, nullptr, 0);
		}
		inline void defineVertexFormat(const GLBufferObject* vbo, const GLBufferObject* ibo) {
			defineVertexFormat(vbo, ibo, 0);
		}
		void defineVertexFormat(const GLBufferObject* vbo, const GLBufferObject* ibo, unsigned int vboOffset);

		/// Deletes the current OpenGL shader program so that new shaders can be attached
		void reset();

		/// Returns a unique identification code to retrieve the corresponding compiled binary in the cache
		inline uint64_t hashName() const { return hashName_; }
		void setObjectLabel(const char* label);

		/// Returns the automatic log on errors flag
		inline bool logOnErrors() const {
			return shouldLogOnErrors_;
		}
		/// Sets the automatic log on errors flag
		/*! If the flag is true the shader program will automatically log compilation and linking errors. */
		inline void setLogOnErrors(bool shouldLogOnErrors) {
			shouldLogOnErrors_ = shouldLogOnErrors;
		}

	private:
		/// Max number of discoverable uniforms
		static constexpr unsigned int MaxNumUniforms = 32;

#if defined(NCINE_LOG)
		static constexpr unsigned int MaxInfoLogLength = 512;
		static char infoLogString_[MaxInfoLogLength];
#endif

		static GLuint boundProgram_;

		GLuint glHandle_;
		static const int AttachedShadersInitialSize = 4;
		SmallVector<std::unique_ptr<GLShader>, 0> attachedShaders_;
		uint64_t hashName_;
		Status status_;
		Introspection introspection_;
		QueryPhase queryPhase_;

		/// A flag indicating whether the shader program should automatically log errors (the information log)
		bool shouldLogOnErrors_;

		unsigned int uniformsSize_;
		unsigned int uniformBlocksSize_;

		static const int UniformsInitialSize = 8;
		SmallVector<GLUniform, 0> uniforms_;
		static const int UniformBlocksInitialSize = 4;
		SmallVector<GLUniformBlock, 0> uniformBlocks_;
		static const int AttributesInitialSize = 4;
		SmallVector<GLAttribute, 0> attributes_;

		StaticHashMap<String, int, GLVertexFormat::MaxAttributes> attributeLocations_;
		GLVertexFormat vertexFormat_;

		bool deferredQueries();
		bool checkLinking();
		void performIntrospection();

		void discoverUniforms();
		void discoverUniformBlocks(GLUniformBlock::DiscoverUniforms discover);
		void discoverAttributes();
		void initVertexFormat();

		/// Deleted copy constructor
		GLShaderProgram(const GLShaderProgram&) = delete;
		/// Deleted assignment operator
		GLShaderProgram& operator=(const GLShaderProgram&) = delete;

		friend class GLShaderUniforms;
		friend class GLShaderUniformBlocks;
	};

}
