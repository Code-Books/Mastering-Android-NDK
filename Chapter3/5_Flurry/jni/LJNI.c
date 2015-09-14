/*
 * Copyright (C) 2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2015 Viktor Latypov (vl@linderdaum.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must display the names 'Sergey Kosarevsky' and
 *    'Viktor Latypov'in the credits of the application, if such credits exist.
 *    The authors of this work must be notified via email (sk@linderdaum.com) in
 *    this case of redistribution.
 *
 * 3. Neither the name of copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <jni.h>

JNIEnv* GetEnv();

/// global pointer to Java VM (also shared with OpenAL implementation)
JavaVM* javaVM = NULL;

jclass Class_JNILib;

jint JNI_OnLoad( JavaVM* vm, void* reserved )
{
	javaVM = vm;
	JNIEnv* env = GetEnv();

	Class_JNILib = ( *env )->FindClass( env, "com/packtpub/ndkmastering/AppActivity" );
	Class_JNILib = ( *env )->NewGlobalRef( env, Class_JNILib );

	return JNI_VERSION_1_2;
}

JNIEnv* GetEnv()
{
	JNIEnv* env = NULL;

	if ( javaVM ) { ( *javaVM )->GetEnv( javaVM, ( void** )&env, JNI_VERSION_1_2 ); }

	return env;
}

JNIEnv* GetEnvThread()
{
	JNIEnv* env = NULL;

	if ( javaVM ) { ( *javaVM )->AttachCurrentThread( javaVM, &env, NULL ); }

	return env;
}

jmethodID FindJavaStaticMethod( JNIEnv* env, jclass* Class, jmethodID* Method, const char* ClassName, const char* MethodName, const char* MethodSignature )
{
	*Class  = ( *env )->FindClass( env, ClassName );
	*Method = ( *env )->GetStaticMethodID( env, *Class, MethodName, MethodSignature );

	return *Method;
}

jmethodID FindJavaStaticMethodJNILib( JNIEnv* env, jmethodID* Method, const char* MethodName, const char* MethodSignature )
{
	*Method = ( *env )->GetStaticMethodID( env, Class_JNILib, MethodName, MethodSignature );

	return *Method;
}

#define JAVA_ENTER() JNIEnv* env; JavaEnter( &env ); jclass Class; jmethodID Method;
#define JAVA_ENTER_THREAD() JNIEnv* env; JavaEnterThread( &env ); jclass Class; jmethodID Method;
#define JAVA_LEAVE() JavaLeave( env );

void JavaEnter( JNIEnv** env )
{
	*env = GetEnv();

	( **env )->PushLocalFrame( *env, 4 );
}

void JavaEnterThread( JNIEnv** env )
{
	*env = GetEnvThread();

	( **env )->PushLocalFrame( *env, 4 );
}

void JavaLeave( JNIEnv* env )
{
	( *env )->PopLocalFrame( env, NULL );
}

void Android_TrackEvent( const char* EventID )
{
	JAVA_ENTER();

	jstring jstr = ( *env )->NewStringUTF( env, EventID );

	FindJavaStaticMethod( env, &Class, &Method, "com/packtpub/ndkmastering/AppActivity", "Callback_TrackEvent", "(Ljava/lang/String;)V" );

	( *env )->CallStaticVoidMethod( env, Class, Method, jstr );

	JAVA_LEAVE();
}

#undef JAVA_ENTER
#undef JAVA_LEAVE
