#include "pch.h"
#include "shader-engine.h"
#include <iostream>
#include "chunk-constants.h"
#include "dexassy.h"
#include "resutil.h"
#include "settings.h"
#include "ingame-prerendered.h"
using namespace blooDot::InGamePreRendered;

using namespace blooDot::Res;

/// <summary>
/// Original inspiration for this part came from code by Augusto Ruiz
/// https://github.com/AugustoRuiz/sdl2glsl (MIT Licence)
/// </summary>
namespace blooDot::ShaderEngine
{
	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLGETSHADERIVPROC glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLVALIDATEPROGRAMPROC glValidateProgram;
	PFNGLGETPROGRAMIVPROC glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORM1FPROC glUniform1f;

	bool InitGLExtensions()
	{
		glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
		glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
		glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
		glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
		glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
		glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
		glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
		glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)SDL_GL_GetProcAddress("glValidateProgram");
		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");
		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)SDL_GL_GetProcAddress("glGetProgramInfoLog");
		glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
		glUniform1f = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");

		return true
			&& _AssertValidpFnGl(glCreateShader, "glCreateShader")
			&& _AssertValidpFnGl(glShaderSource, "glShaderSource")
			&& _AssertValidpFnGl(glCompileShader, "glCompileShader")
			&& _AssertValidpFnGl(glGetShaderiv, "glGetShaderiv")
			&& _AssertValidpFnGl(glGetShaderInfoLog, "glGetShaderInfoLog")
			&& _AssertValidpFnGl(glDeleteShader, "glDeleteShader")
			&& _AssertValidpFnGl(glAttachShader, "glAttachShader")
			&& _AssertValidpFnGl(glCreateProgram, "glCreateProgram")
			&& _AssertValidpFnGl(glLinkProgram, "glLinkProgram")
			&& _AssertValidpFnGl(glValidateProgram, "glValidateProgram")
			&& _AssertValidpFnGl(glGetProgramiv, "glGetProgramiv")
			&& _AssertValidpFnGl(glGetProgramInfoLog, "glGetProgramInfoLog")
			&& _AssertValidpFnGl(glUseProgram, "glUseProgram")
			&& _AssertValidpFnGl(glDeleteProgram, "glDeleteProgram")
			&& _AssertValidpFnGl(glGetUniformLocation, "glGetUniformLocation")
			&& _AssertValidpFnGl(glUniform1f, "glUniform1f");
	}

	GLint BindUniform(GLuint programId, const char* uniformName)
	{
		return glGetUniformLocation(programId, uniformName);
	}

	void SetUniform(GLint uniformId, GLfloat value)
	{
		glUniform1f(uniformId, value);
	}

	GLuint CompileProgram(int chunkKeyVertex, int chunkKeyFragment)
	{
		std::string vertexSource;
		std::string fragmtSource;

		if (!LoadText(chunkKeyVertex, vertexSource))
		{
			return 0;
		}

		if (!LoadText(chunkKeyFragment, fragmtSource))
		{
			return 0;
		}

		GLuint programId = glCreateProgram();
		GLuint vtexShaderId = _CompileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
		GLuint fragShaderId = _CompileShader(fragmtSource.c_str(), GL_FRAGMENT_SHADER);

		if (vtexShaderId && fragShaderId)
		{
			glAttachShader(programId, vtexShaderId);
			glAttachShader(programId, fragShaderId);
			glLinkProgram(programId);
			glValidateProgram(programId);
			GLint logLen;
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0)
			{
				char* log = (char*)malloc(logLen * sizeof(char));
				glGetProgramInfoLog(programId, logLen, &logLen, log);
				std::cout 
					<< "Shader linker output:\n"
					<< log
					<< "\n";

				free(log);
			}
		}

		if (vtexShaderId)
		{
			glDeleteShader(vtexShaderId);
		}

		if (fragShaderId)
		{
			glDeleteShader(fragShaderId);
		}

		return programId;
	}

	void PresentBackBuffer(SDL_Window* win, SDL_Renderer* renderer, GLuint programId, GLuint uniformId, GLfloat uniformsValue)
	{
		GLint oldProgramId = 0;

		if (programId != 0)
		{
			glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgramId);
			glUseProgram(programId);
			if (uniformsValue > 0)
			{
				SetUniform(uniformId, uniformsValue);
			}
		}

		/* mostly rectangular screen, so two triangles will do */
		GLfloat minx, miny, maxx, maxy;
		GLfloat minu, maxu, minv, maxv;

		minx = 0.0f;
		miny = (float)blooDot::Settings::GetPhysicalArenaHeight();
		maxx = (float)blooDot::Settings::GetPhysicalArenaWidth();
		maxy = 0.0f;
		minu = 0.0f;
		maxu = 1.0f;
		minv = 0.0f;
		maxv = 1.0f;

		glBegin(GL_TRIANGLE_STRIP);
		{
			glTexCoord2f(minu, minv);
			glVertex2f(minx, miny);
			glTexCoord2f(maxu, minv);
			glVertex2f(maxx, miny);
			glTexCoord2f(minu, maxv);
			glVertex2f(minx, maxy);
			glTexCoord2f(maxu, maxv);
			glVertex2f(maxx, maxy);
			glEnd();
		}

		/* overlay control(s):
		 * empirically, the area where an overlay is drawn,
		 * needs to be cleared with transparency first, to
		 * properly draw "over" the shader output */
		SDL_Rect src = { 0,0,static_cast<int>(InGameSkipButtonWidth) + 2 * static_cast<int>(InGameControlMargin),static_cast<int>(InGameButtonHeight) + 2 * static_cast<int>(InGameControlMargin)};
		SDL_Rect dst = { static_cast<int>(maxx) - src.w - 33,33,src.w,src.h };
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0x00);
		SDL_RenderFillRect(renderer, &dst);
		SDL_RenderCopy(
			renderer,
			blooDot::InGamePreRendered::PreRendered,
			&src,
			&dst
		);
		
		SDL_GL_SwapWindow(win);
		if (programId != 0)
		{
			glUseProgram(oldProgramId);
		}
	}

	void DeleteGPUProgram(GLuint programId)
	{
		glDeleteProgram(programId);
	}

	bool _AssertValidpFnGl(void* pFn, std::string name)
	{
		if (pFn)
		{
			return true;
		}

		std::cerr
			<< "Failed to bind GPU function \""
			<< name
			<< "\"\n";

		return false;
	}

	GLuint _CompileShader(const char* source, GLuint shaderType)
	{
		GLuint result = glCreateShader(shaderType);
		glShaderSource(result, 1, &source, NULL);
		glCompileShader(result);
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(result, GL_COMPILE_STATUS, &shaderCompiled);
		if (shaderCompiled != GL_TRUE)
		{
			std::cout
				<< "Failed to compiler shader:\n"
				<< result
				<< "\n";

			GLint logLength;
			glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
				GLchar* log = (GLchar*)malloc(logLength);

				glGetShaderInfoLog(result, logLength, &logLength, log);
				std::cout 
					<< "Shader compiler output:\n"
					<< log
					<< "\n";

				free(log);
			}

			glDeleteShader(result);
			result = 0;
		}
		else
		{
			std::cout << "Successfully compiled shader #" << result << "\n";
		}

		return result;
	}
}