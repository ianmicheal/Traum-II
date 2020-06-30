/*Generated by glloader.lua*/
#define _GL1BIND_IMP_
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdlib.h>
#include "public/gl1bind.h"

#ifndef _WIN32
#include <dlfcn.h>
static void *
_getsymb (const char *symb)
{
	static void *lib = NULL;
	static void *gs = NULL;
	if (NULL == lib)
	{/*For some reason glXGetProcAddress is not required to be exported*/
		lib = dlopen (NULL, RTLD_LAZY|RTLD_LOCAL);
		if (lib) gs = dlsym ("glXGetProcAddress");
		else return NULL;
	}
	if (gs) return ((void*(*)(const GLubyte*))gs) (symb);
	else return dlsym (lib, symb);
}
#else
#include <windows.h>
static void *
_getsymb (const char *symb)
{
	static HMODULE lib = NULL;
	static FARPROC gs = NULL;
	if (NULL == lib)
	{
		lib = LoadLibrary ("OpenGL32.dll");
		if (lib) gs = GetProcAddress (lib, "wglGetProcAddress");
		else return NULL;
	}
	PROC x = ((PROC (*) (const char *))gs) (symb);
	if (NULL == x) x = GetProcAddress (lib, symb);
	return *(void **)&x;
}
#endif
#define GPA(symb) _getsymb (symb)

GLenum (APIENTRY *_dl_glGetError) ( void );
const GLubyte* (APIENTRY *_dl_glGetString) ( GLenum name );
void (APIENTRY *_dl_glActiveTexture) ( GLenum texture );
void (APIENTRY *_dl_glAlphaFunc) ( GLenum func, GLclampf ref );
void (APIENTRY *_dl_glBindBuffer) (GLenum target, GLuint buffer);
void (APIENTRY *_dl_glBindTexture) ( GLenum target, GLuint texture );
void (APIENTRY *_dl_glBlendFunc) ( GLenum sfactor, GLenum dfactor );
void (APIENTRY *_dl_glBufferData) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
void (APIENTRY *_dl_glBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
void (APIENTRY *_dl_glBufferSubDataARB) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const void *data);
void (APIENTRY *_dl_glClear) ( GLbitfield mask );
void (APIENTRY *_dl_glClearColor) ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
void (APIENTRY *_dl_glClearDepthf) (GLfloat d);
void (APIENTRY *_dl_glClearStencil) ( GLint s );
void (APIENTRY *_dl_glClientActiveTexture) ( GLenum texture );
void (APIENTRY *_dl_glColor4f) ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
void (APIENTRY *_dl_glColorMask) ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
void (APIENTRY *_dl_glColorPointer) ( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
void (APIENTRY *_dl_glCompressedTexImage2D) ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data );
void (APIENTRY *_dl_glCompressedTexSubImage2D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data );
void (APIENTRY *_dl_glCopyTexImage2D) ( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
void (APIENTRY *_dl_glCopyTexSubImage2D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
void (APIENTRY *_dl_glCullFace) ( GLenum mode );
void (APIENTRY *_dl_glDeleteBuffers) (GLsizei n, const GLuint *buffers);
void (APIENTRY *_dl_glDeleteTextures) ( GLsizei n, const GLuint *textures);
void (APIENTRY *_dl_glDepthFunc) ( GLenum func );
void (APIENTRY *_dl_glDepthMask) ( GLboolean flag );
void (APIENTRY *_dl_glDepthRange) ( GLclampd near_val, GLclampd far_val );
void (APIENTRY *_dl_glDisable) ( GLenum cap );
void (APIENTRY *_dl_glDisableClientState) ( GLenum cap );
void (APIENTRY *_dl_glDrawArrays) ( GLenum mode, GLint first, GLsizei count );
void (APIENTRY *_dl_glDrawElements) ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );
void (APIENTRY *_dl_glEnable) ( GLenum cap );
void (APIENTRY *_dl_glEnableClientState) ( GLenum cap );
void (APIENTRY *_dl_glFinish) ( void );
void (APIENTRY *_dl_glFlush) ( void );
void (APIENTRY *_dl_glFogf) ( GLenum pname, GLfloat param );
void (APIENTRY *_dl_glFogfv) ( GLenum pname, const GLfloat *params );
void (APIENTRY *_dl_glFrontFace) ( GLenum mode );
void (APIENTRY *_dl_glFrustum) ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val );
void (APIENTRY *_dl_glGenBuffers) (GLsizei n, GLuint *buffers);
void (APIENTRY *_dl_glGenTextures) ( GLsizei n, GLuint *textures );
void (APIENTRY *_dl_glGetFloatv) ( GLenum pname, GLfloat *params );
void (APIENTRY *_dl_glGetIntegerv) ( GLenum pname, GLint *params );
void (APIENTRY *_dl_glHint) ( GLenum target, GLenum mode );
void (APIENTRY *_dl_glLightModelf) ( GLenum pname, GLfloat param );
void (APIENTRY *_dl_glLightModelfv) ( GLenum pname, const GLfloat *params );
void (APIENTRY *_dl_glLightf) ( GLenum light, GLenum pname, GLfloat param );
void (APIENTRY *_dl_glLightfv) ( GLenum light, GLenum pname, const GLfloat *params );
void (APIENTRY *_dl_glLineWidth) ( GLfloat width );
void (APIENTRY *_dl_glLoadIdentity) ( void );
void (APIENTRY *_dl_glLoadMatrixf) ( const GLfloat *m );
void (APIENTRY *_dl_glLogicOp) ( GLenum opcode );
void (APIENTRY *_dl_glMaterialf) ( GLenum face, GLenum pname, GLfloat param );
void (APIENTRY *_dl_glMaterialfv) ( GLenum face, GLenum pname, const GLfloat *params );
void (APIENTRY *_dl_glMatrixMode) ( GLenum mode );
void (APIENTRY *_dl_glMultMatrixf) ( const GLfloat *m );
void (APIENTRY *_dl_glMultiTexCoord4f) ( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
void (APIENTRY *_dl_glNormal3f) ( GLfloat nx, GLfloat ny, GLfloat nz );
void (APIENTRY *_dl_glNormalPointer) ( GLenum type, GLsizei stride, const GLvoid *ptr );
void (APIENTRY *_dl_glOrtho) ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val );
void (APIENTRY *_dl_glPixelStorei) ( GLenum pname, GLint param );
void (APIENTRY *_dl_glPointParameterf) (GLenum pname, GLfloat param);
void (APIENTRY *_dl_glPointParameterfv) (GLenum pname, const GLfloat *params);
void (APIENTRY *_dl_glPointSize) ( GLfloat size );
void (APIENTRY *_dl_glPolygonOffset) ( GLfloat factor, GLfloat units );
void (APIENTRY *_dl_glPopMatrix) ( void );
void (APIENTRY *_dl_glPushMatrix) ( void );
void (APIENTRY *_dl_glReadPixels) ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
void (APIENTRY *_dl_glRotatef) ( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
void (APIENTRY *_dl_glSampleCoverage) ( GLclampf value, GLboolean invert );
void (APIENTRY *_dl_glScalef) ( GLfloat x, GLfloat y, GLfloat z );
void (APIENTRY *_dl_glScissor) ( GLint x, GLint y, GLsizei width, GLsizei height);
void (APIENTRY *_dl_glShadeModel) ( GLenum mode );
void (APIENTRY *_dl_glStencilFunc) ( GLenum func, GLint ref, GLuint mask );
void (APIENTRY *_dl_glStencilMask) ( GLuint mask );
void (APIENTRY *_dl_glStencilOp) ( GLenum fail, GLenum zfail, GLenum zpass );
void (APIENTRY *_dl_glTexCoordPointer) ( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
void (APIENTRY *_dl_glTexEnvf) ( GLenum target, GLenum pname, GLfloat param );
void (APIENTRY *_dl_glTexEnvfv) ( GLenum target, GLenum pname, const GLfloat *params );
void (APIENTRY *_dl_glTexImage2D) ( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
void (APIENTRY *_dl_glTexParameterf) ( GLenum target, GLenum pname, GLfloat param );
void (APIENTRY *_dl_glTexParameteri) ( GLenum target, GLenum pname, GLint param );
void (APIENTRY *_dl_glTexSubImage2D) ( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
void (APIENTRY *_dl_glTranslatef) ( GLfloat x, GLfloat y, GLfloat z );
void (APIENTRY *_dl_glVertexPointer) ( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
void (APIENTRY *_dl_glViewport) ( GLint x, GLint y, GLsizei width, GLsizei height );
int gl1bind_load (void)
{
	if (NULL == (*(void **)&_dl_glActiveTexture = GPA("glActiveTexture"))) return 1;
	if (NULL == (*(void **)&_dl_glAlphaFunc = GPA("glAlphaFunc"))) return 2;
	if (NULL == (*(void **)&_dl_glBindBuffer = GPA("glBindBuffer"))) return 3;
	if (NULL == (*(void **)&_dl_glBindTexture = GPA("glBindTexture"))) return 4;
	if (NULL == (*(void **)&_dl_glBlendFunc = GPA("glBlendFunc"))) return 5;
	if (NULL == (*(void **)&_dl_glBufferData = GPA("glBufferData"))) return 6;
	if (NULL == (*(void **)&_dl_glBufferSubData = GPA("glBufferSubData"))) return 7;
	if (NULL == (*(void **)&_dl_glBufferSubDataARB = GPA("glBufferSubDataARB"))) return 8;
	if (NULL == (*(void **)&_dl_glClear = GPA("glClear"))) return 9;
	if (NULL == (*(void **)&_dl_glClearColor = GPA("glClearColor"))) return 10;
	if (NULL == (*(void **)&_dl_glClearDepthf = GPA("glClearDepthf"))) return 11;
	if (NULL == (*(void **)&_dl_glClearStencil = GPA("glClearStencil"))) return 12;
	if (NULL == (*(void **)&_dl_glClientActiveTexture = GPA("glClientActiveTexture"))) return 13;
	if (NULL == (*(void **)&_dl_glColor4f = GPA("glColor4f"))) return 14;
	if (NULL == (*(void **)&_dl_glColorMask = GPA("glColorMask"))) return 15;
	if (NULL == (*(void **)&_dl_glColorPointer = GPA("glColorPointer"))) return 16;
	if (NULL == (*(void **)&_dl_glCompressedTexImage2D = GPA("glCompressedTexImage2D"))) return 17;
	if (NULL == (*(void **)&_dl_glCompressedTexSubImage2D = GPA("glCompressedTexSubImage2D"))) return 18;
	if (NULL == (*(void **)&_dl_glCopyTexImage2D = GPA("glCopyTexImage2D"))) return 19;
	if (NULL == (*(void **)&_dl_glCopyTexSubImage2D = GPA("glCopyTexSubImage2D"))) return 20;
	if (NULL == (*(void **)&_dl_glCullFace = GPA("glCullFace"))) return 21;
	if (NULL == (*(void **)&_dl_glDeleteBuffers = GPA("glDeleteBuffers"))) return 22;
	if (NULL == (*(void **)&_dl_glDeleteTextures = GPA("glDeleteTextures"))) return 23;
	if (NULL == (*(void **)&_dl_glDepthFunc = GPA("glDepthFunc"))) return 24;
	if (NULL == (*(void **)&_dl_glDepthMask = GPA("glDepthMask"))) return 25;
	if (NULL == (*(void **)&_dl_glDepthRange = GPA("glDepthRange"))) return 26;
	if (NULL == (*(void **)&_dl_glDisable = GPA("glDisable"))) return 27;
	if (NULL == (*(void **)&_dl_glDisableClientState = GPA("glDisableClientState"))) return 28;
	if (NULL == (*(void **)&_dl_glDrawArrays = GPA("glDrawArrays"))) return 29;
	if (NULL == (*(void **)&_dl_glDrawElements = GPA("glDrawElements"))) return 30;
	if (NULL == (*(void **)&_dl_glEnable = GPA("glEnable"))) return 31;
	if (NULL == (*(void **)&_dl_glEnableClientState = GPA("glEnableClientState"))) return 32;
	if (NULL == (*(void **)&_dl_glFinish = GPA("glFinish"))) return 33;
	if (NULL == (*(void **)&_dl_glFlush = GPA("glFlush"))) return 34;
	if (NULL == (*(void **)&_dl_glFogf = GPA("glFogf"))) return 35;
	if (NULL == (*(void **)&_dl_glFogfv = GPA("glFogfv"))) return 36;
	if (NULL == (*(void **)&_dl_glFrontFace = GPA("glFrontFace"))) return 37;
	if (NULL == (*(void **)&_dl_glFrustum = GPA("glFrustum"))) return 38;
	if (NULL == (*(void **)&_dl_glGenBuffers = GPA("glGenBuffers"))) return 39;
	if (NULL == (*(void **)&_dl_glGenTextures = GPA("glGenTextures"))) return 40;
	if (NULL == (*(void **)&_dl_glGetError = GPA("glGetError"))) return 41;
	if (NULL == (*(void **)&_dl_glGetFloatv = GPA("glGetFloatv"))) return 42;
	if (NULL == (*(void **)&_dl_glGetIntegerv = GPA("glGetIntegerv"))) return 43;
	if (NULL == (*(void **)&_dl_glGetString = GPA("glGetString"))) return 44;
	if (NULL == (*(void **)&_dl_glHint = GPA("glHint"))) return 45;
	if (NULL == (*(void **)&_dl_glLightModelf = GPA("glLightModelf"))) return 46;
	if (NULL == (*(void **)&_dl_glLightModelfv = GPA("glLightModelfv"))) return 47;
	if (NULL == (*(void **)&_dl_glLightf = GPA("glLightf"))) return 48;
	if (NULL == (*(void **)&_dl_glLightfv = GPA("glLightfv"))) return 49;
	if (NULL == (*(void **)&_dl_glLineWidth = GPA("glLineWidth"))) return 50;
	if (NULL == (*(void **)&_dl_glLoadIdentity = GPA("glLoadIdentity"))) return 51;
	if (NULL == (*(void **)&_dl_glLoadMatrixf = GPA("glLoadMatrixf"))) return 52;
	if (NULL == (*(void **)&_dl_glLogicOp = GPA("glLogicOp"))) return 53;
	if (NULL == (*(void **)&_dl_glMaterialf = GPA("glMaterialf"))) return 54;
	if (NULL == (*(void **)&_dl_glMaterialfv = GPA("glMaterialfv"))) return 55;
	if (NULL == (*(void **)&_dl_glMatrixMode = GPA("glMatrixMode"))) return 56;
	if (NULL == (*(void **)&_dl_glMultMatrixf = GPA("glMultMatrixf"))) return 57;
	if (NULL == (*(void **)&_dl_glMultiTexCoord4f = GPA("glMultiTexCoord4f"))) return 58;
	if (NULL == (*(void **)&_dl_glNormal3f = GPA("glNormal3f"))) return 59;
	if (NULL == (*(void **)&_dl_glNormalPointer = GPA("glNormalPointer"))) return 60;
	if (NULL == (*(void **)&_dl_glOrtho = GPA("glOrtho"))) return 61;
	if (NULL == (*(void **)&_dl_glPixelStorei = GPA("glPixelStorei"))) return 62;
	if (NULL == (*(void **)&_dl_glPointParameterf = GPA("glPointParameterf"))) return 63;
	if (NULL == (*(void **)&_dl_glPointParameterfv = GPA("glPointParameterfv"))) return 64;
	if (NULL == (*(void **)&_dl_glPointSize = GPA("glPointSize"))) return 65;
	if (NULL == (*(void **)&_dl_glPolygonOffset = GPA("glPolygonOffset"))) return 66;
	if (NULL == (*(void **)&_dl_glPopMatrix = GPA("glPopMatrix"))) return 67;
	if (NULL == (*(void **)&_dl_glPushMatrix = GPA("glPushMatrix"))) return 68;
	if (NULL == (*(void **)&_dl_glReadPixels = GPA("glReadPixels"))) return 69;
	if (NULL == (*(void **)&_dl_glRotatef = GPA("glRotatef"))) return 70;
	if (NULL == (*(void **)&_dl_glSampleCoverage = GPA("glSampleCoverage"))) return 71;
	if (NULL == (*(void **)&_dl_glScalef = GPA("glScalef"))) return 72;
	if (NULL == (*(void **)&_dl_glScissor = GPA("glScissor"))) return 73;
	if (NULL == (*(void **)&_dl_glShadeModel = GPA("glShadeModel"))) return 74;
	if (NULL == (*(void **)&_dl_glStencilFunc = GPA("glStencilFunc"))) return 75;
	if (NULL == (*(void **)&_dl_glStencilMask = GPA("glStencilMask"))) return 76;
	if (NULL == (*(void **)&_dl_glStencilOp = GPA("glStencilOp"))) return 77;
	if (NULL == (*(void **)&_dl_glTexCoordPointer = GPA("glTexCoordPointer"))) return 78;
	if (NULL == (*(void **)&_dl_glTexEnvf = GPA("glTexEnvf"))) return 79;
	if (NULL == (*(void **)&_dl_glTexEnvfv = GPA("glTexEnvfv"))) return 80;
	if (NULL == (*(void **)&_dl_glTexImage2D = GPA("glTexImage2D"))) return 81;
	if (NULL == (*(void **)&_dl_glTexParameterf = GPA("glTexParameterf"))) return 82;
	if (NULL == (*(void **)&_dl_glTexParameteri = GPA("glTexParameteri"))) return 83;
	if (NULL == (*(void **)&_dl_glTexSubImage2D = GPA("glTexSubImage2D"))) return 84;
	if (NULL == (*(void **)&_dl_glTranslatef = GPA("glTranslatef"))) return 85;
	if (NULL == (*(void **)&_dl_glVertexPointer = GPA("glVertexPointer"))) return 86;
	if (NULL == (*(void **)&_dl_glViewport = GPA("glViewport"))) return 87;
	return 0;
}