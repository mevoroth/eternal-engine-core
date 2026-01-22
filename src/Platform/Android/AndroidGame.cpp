#if ETERNAL_PLATFORM_ANDROID

#include "Core/Game.hpp"
#include "Core/SystemFactory.hpp"
#include "Core/SystemCreateInformation.hpp"
#include <pthread.h>
#include <unistd.h>
#include <android/input.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/native_app_glue/android_native_app_glue.h>

namespace Eternal
{
	namespace Core
	{
		static void AndroidActivityOnStart(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnResume(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}
		static void* AndroidActivityOnSaveInstanceState(_In_ ANativeActivity* InActivity, _Out_ size_t* OutSize)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnPause(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnStop(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnDestroy(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnWindowFocusChanged(_In_ ANativeActivity* InActivity, _In_ int HasFocus)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnNativeWindowCreated(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnNativeWindowResized(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnNativeWindowRedrawNeeded(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnNativeWindowDestroyed(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnInputQueueCreated(_In_ ANativeActivity* InActivity, _In_ AInputQueue* InQueue)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnInputQueueDestroyed(_In_ ANativeActivity* InActivity, _In_ AInputQueue* InQueue)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnContentRectChanged(_In_ ANativeActivity* InActivity, _In_ const ARect* InRectangle)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnConfigurationChanged(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}
		static void AndroidActivityOnLowMemory(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}

		//////////////////////////////////////////////////////////////////////////

		static void AndroidApplicationOnAppCmd(_In_ android_app* InApplication, _In_ int32_t InCommand)
		{
			ETERNAL_BREAK();
		}

		static int32_t AndroidApplicationOnInputEvent(_In_ android_app* InApplication, _In_ AInputEvent* InEvent)
		{
			ETERNAL_BREAK();
		}

		//////////////////////////////////////////////////////////////////////////

		static void AndroidApplicationCommandPollSourceProcess(_In_ android_app* InAndroidApplication, _In_ android_poll_source* InAndroidPollSource)
		{

		}

		static void AndroidApplicationInputPollSourceProcess(_In_ android_app* InAndroidApplication, _In_ android_poll_source* InAndroidPollSource)
		{

		}

		//////////////////////////////////////////////////////////////////////////

		int AndroidLooperCallbackFunction(_In_ int InFileDescriptor, _In_ int InEvents, _In_ void* InData)
		{
			return 1;
		}

		//////////////////////////////////////////////////////////////////////////

		void* AndroidApplicationEntry(_In_ void* InAndroidApplication)
		{
			android_app* AndroidApplication = reinterpret_cast<android_app*>(InAndroidApplication);

			AndroidApplication->config = AConfiguration_new();
			AConfiguration_fromAssetManager(AndroidApplication->config, AndroidApplication->activity->assetManager);

			AndroidApplication->cmdPollSource.id		= LOOPER_ID_MAIN;
			AndroidApplication->cmdPollSource.app		= AndroidApplication;
			AndroidApplication->cmdPollSource.process	= AndroidApplicationCommandPollSourceProcess;

			AndroidApplication->inputPollSource.id		= LOOPER_ID_INPUT;
			AndroidApplication->inputPollSource.app		= AndroidApplication;
			AndroidApplication->inputPollSource.process	= AndroidApplicationInputPollSourceProcess;

			AndroidApplication->looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
			if (ALooper_addFd(
				AndroidApplication->looper,
				AndroidApplication->msgread,
				LOOPER_ID_MAIN,
				ALOOPER_EVENT_INPUT,
				AndroidLooperCallbackFunction,
				&AndroidApplication->cmdPollSource
			) == -1)
			{
				ETERNAL_BREAK();
				return nullptr;
			}

			if (pthread_mutex_lock(&AndroidApplication->mutex))
			{
				ETERNAL_BREAK();
				return nullptr;
			}

			AndroidApplication->running = 1;
			int ConditionVariableBroadcastResult = pthread_cond_broadcast(&AndroidApplication->cond);

			if (pthread_mutex_unlock(&AndroidApplication->mutex) ||
				ConditionVariableBroadcastResult)
			{
				ETERNAL_BREAK();
				return nullptr;
			}

			//InOutGame->_System = CreateSystem(InGameCreateInformation.SystemInformation);

			Game* InGame = reinterpret_cast<Game*>(AndroidApplication->userData);

			InGame->_System = CreateSystem(InGame->_GameCreateInformation.SystemInformation);
			reinterpret_cast<Game*>(AndroidApplication->userData)->Run();

			return nullptr;
		}

		void RunGame(_In_ GameCreateInformation& InGameCreateInformation, _Inout_ Game* InOutGame)
		{
			ETERNAL_BREAK();

			InGameCreateInformation.SystemInformation.GameContext = InOutGame;

			ANativeActivity* AndroidNativeActivity	= InGameCreateInformation.SystemInformation.ExecutableInput.AndroidNativeActivity;
			void* AndroidSavedState					= InGameCreateInformation.SystemInformation.ExecutableInput.AndroidSavedState;
			size_t AndroidSavedStateSize			= InGameCreateInformation.SystemInformation.ExecutableInput.AndroidSavedStateSize;

			AndroidNativeActivity->callbacks->onStart						= AndroidActivityOnStart;
			AndroidNativeActivity->callbacks->onResume						= AndroidActivityOnResume;
			AndroidNativeActivity->callbacks->onSaveInstanceState			= AndroidActivityOnSaveInstanceState;
			AndroidNativeActivity->callbacks->onPause						= AndroidActivityOnPause;
			AndroidNativeActivity->callbacks->onStop						= AndroidActivityOnStop;
			AndroidNativeActivity->callbacks->onDestroy						= AndroidActivityOnDestroy;
			AndroidNativeActivity->callbacks->onWindowFocusChanged			= AndroidActivityOnWindowFocusChanged;
			AndroidNativeActivity->callbacks->onNativeWindowCreated			= AndroidActivityOnNativeWindowCreated;
			AndroidNativeActivity->callbacks->onNativeWindowResized			= AndroidActivityOnNativeWindowResized;
			AndroidNativeActivity->callbacks->onNativeWindowRedrawNeeded	= AndroidActivityOnNativeWindowRedrawNeeded;
			AndroidNativeActivity->callbacks->onNativeWindowDestroyed		= AndroidActivityOnNativeWindowDestroyed;
			AndroidNativeActivity->callbacks->onInputQueueCreated			= AndroidActivityOnInputQueueCreated;
			AndroidNativeActivity->callbacks->onInputQueueDestroyed			= AndroidActivityOnInputQueueDestroyed;
			AndroidNativeActivity->callbacks->onContentRectChanged			= AndroidActivityOnContentRectChanged;
			AndroidNativeActivity->callbacks->onConfigurationChanged		= AndroidActivityOnConfigurationChanged;
			AndroidNativeActivity->callbacks->onLowMemory					= AndroidActivityOnLowMemory;

			android_app* AndroidApplication = new android_app();
			AndroidApplication->onAppCmd		= AndroidApplicationOnAppCmd;
			AndroidApplication->onInputEvent	= AndroidApplicationOnInputEvent;
			AndroidApplication->activity		= AndroidNativeActivity;

			if (AndroidSavedState)
			{
				AndroidApplication->savedStateSize	= AndroidSavedStateSize;
				AndroidApplication->savedState		= malloc(AndroidSavedStateSize);
				mempcpy(AndroidApplication->savedState, AndroidSavedState, AndroidSavedStateSize);
			}

			{
				int MessagePipeline[2] = {};
				if (pipe(MessagePipeline))
				{
					ETERNAL_BREAK();
					return;
				}

				AndroidApplication->msgread		= MessagePipeline[0];
				AndroidApplication->msgwrite	= MessagePipeline[1];
			}

			AndroidApplication->userData		= InOutGame;

			if (pthread_mutex_init(&AndroidApplication->mutex, nullptr))
			{
				ETERNAL_BREAK();
				return;
			}

			if (pthread_cond_init(&AndroidApplication->cond, nullptr))
			{
				ETERNAL_BREAK();
				return;
			}

			pthread_attr_t ThreadAttributes = {};
			if (pthread_attr_init(&ThreadAttributes))
			{
				ETERNAL_BREAK();
				return;
			}

			if (pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_DETACHED))
			{
				ETERNAL_BREAK();
				return;
			}

			if (pthread_create(&AndroidApplication->thread, &ThreadAttributes, AndroidApplicationEntry, AndroidApplication))
			{
				ETERNAL_BREAK();
				return;
			}

			if (pthread_mutex_lock(&AndroidApplication->mutex))
			{
				ETERNAL_BREAK();
				return;
			}

			while (!AndroidApplication->running)
			{
				if (pthread_cond_wait(&AndroidApplication->cond, &AndroidApplication->mutex))
				{
					ETERNAL_BREAK();
				}
			}

			if (pthread_mutex_unlock(&AndroidApplication->mutex))
			{
				ETERNAL_BREAK();
				return;
			}

			AndroidNativeActivity->instance = AndroidApplication;

			//AndroidApp->activity =

			//void* userData;
			//void (*onAppCmd)(struct android_app* app, int32_t cmd);
			//int32_t(*onInputEvent)(struct android_app* app, AInputEvent * event);
			//ANativeActivity* activity;
			//void* savedState;
			//size_t savedStateSize;
			
			/*
			AConfiguration* config;

			ALooper* looper;
			AInputQueue* inputQueue;
			ANativeWindow* window;
			ARect contentRect;
			int activityState;
			int destroyRequested;
			pthread_mutex_t mutex;
			pthread_cond_t cond;

			int msgread;
			int msgwrite;

			pthread_t thread;

			struct android_poll_source cmdPollSource;
			struct android_poll_source inputPollSource;

			int running;
			int stateSaved;
			int destroyed;
			int redrawNeeded;
			AInputQueue* pendingInputQueue;
			ANativeWindow* pendingWindow;
			ARect pendingContentRect;
			*/
		}
	}
}

#endif
