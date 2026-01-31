#if ETERNAL_PLATFORM_ANDROID

#include "Core/Game.hpp"
#include "Core/SystemFactory.hpp"
#include "Core/SystemCreateInformation.hpp"
#include "Android/AndroidGraphicsContext.hpp"
#include "Platform/Android/AndroidSystem.hpp"
#include "File/Android/AndroidAssetManagerFile.hpp"
#include <pthread.h>
#include <unistd.h>
#include <android/input.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/native_app_glue/android_native_app_glue.h>

namespace Eternal
{
	namespace Core
	{
		struct AndroidMutexAutoLock
		{
			AndroidMutexAutoLock(_In_ android_app* InAndroidApplication)
				: AndroidApplication(InAndroidApplication)
			{
				ETERNAL_ASSERT(InAndroidApplication);

				if (pthread_mutex_lock(&AndroidApplication->mutex))
				{
					ETERNAL_BREAK();
					AndroidApplication = nullptr;
				}
			}

			~AndroidMutexAutoLock()
			{
				if (AndroidApplication)
				{
					if (pthread_mutex_unlock(&AndroidApplication->mutex))
					{
						ETERNAL_BREAK();
					}
				}
			}

			operator bool() const
			{
				return AndroidApplication != nullptr;
			}

			android_app* AndroidApplication = nullptr;
		};

		//////////////////////////////////////////////////////////////////////////

		static void AndroidApplicationWriteCommand(_In_ android_app* InAndroidApplication, _In_ int8_t InCommand)
		{
			ETERNAL_ASSERT(write(InAndroidApplication->msgwrite, &InCommand, sizeof(int8_t)) == sizeof(int8_t));
		}

		static int8_t AndroidApplicationReadCommand(_In_ android_app* InAndroidApplication)
		{
			int8_t Command = -1;
			ETERNAL_ASSERT(read(InAndroidApplication->msgread, &Command, sizeof(int8_t)) == sizeof(int8_t));
			return Command;
		}

		//////////////////////////////////////////////////////////////////////////

		template<typename AndroidApplicationFunctor>
		static bool AndroidApplicationSafeExecuteAndSignal(_In_ android_app* InAndroidApplication, _In_ AndroidApplicationFunctor InAndroidApplicationFunction)
		{
			AndroidMutexAutoLock AutoLock(InAndroidApplication);
			if (!AutoLock)
				return false;

			InAndroidApplicationFunction(InAndroidApplication);
			int ConditionBroadcastResult = pthread_cond_broadcast(&InAndroidApplication->cond);
			ETERNAL_ASSERT(!ConditionBroadcastResult);
			return ConditionBroadcastResult == 0;
		}

		template<typename AndroidApplicationFunctor, typename AndroidApplicationWaitFunctor>
		static bool AndroidApplicationSafeExecuteAndWait(_In_ android_app* InAndroidApplication, _In_ AndroidApplicationFunctor InAndroidApplicationFunction, _In_ AndroidApplicationWaitFunctor InAndroidApplicationWaitFunction)
		{
			AndroidMutexAutoLock AutoLock(InAndroidApplication);
			if (!AutoLock)
				return false;

			InAndroidApplicationFunction(InAndroidApplication);

			while (!InAndroidApplicationWaitFunction(InAndroidApplication))
			{
				if (pthread_cond_wait(&InAndroidApplication->cond, &InAndroidApplication->mutex))
				{
					ETERNAL_BREAK();
					return false;
				}
			}

			return true;
		}

		//////////////////////////////////////////////////////////////////////////

		static void AndroidApplicationSetActivityState(_In_ android_app* InAndroidApplication, _In_ int8_t InCommand)
		{
			AndroidApplicationSafeExecuteAndSignal(
				InAndroidApplication,
				[InCommand](_In_ android_app* InAndroidApplication)
				{
					InAndroidApplication->activityState = InCommand;
				}
			);
		}

		static void AndroidApplicationSendActivityStateCommand(_In_ android_app* InAndroidApplication, _In_ int8_t InCommand)
		{
			AndroidApplicationSafeExecuteAndWait(
				InAndroidApplication,
				[InCommand](_In_ android_app* InAndroidApplication)
				{
					AndroidApplicationWriteCommand(InAndroidApplication, InCommand);
				},
				[InCommand](_In_ android_app* InAndroidApplication) -> bool
				{
					return InAndroidApplication->activityState == InCommand;
				}
			);
		}

		static void AndroidApplicationSetWindow(_In_ android_app* InAndroidApplication, _In_ ANativeWindow* InWindow)
		{
			AndroidApplicationSafeExecuteAndWait(
				InAndroidApplication,
				[InWindow](_In_ android_app* InAndroidApplication)
				{
					if (InAndroidApplication->pendingWindow)
						AndroidApplicationWriteCommand(InAndroidApplication, APP_CMD_TERM_WINDOW);

					InAndroidApplication->pendingWindow = InWindow;

					if (InWindow)
						AndroidApplicationWriteCommand(InAndroidApplication, APP_CMD_INIT_WINDOW);
				},
				[](_In_ android_app* InAndroidApplication) -> bool
				{
					return InAndroidApplication->window == InAndroidApplication->pendingWindow;
				}
			);
		}

		static void AndroidApplicationSetInput(_In_ android_app* InAndroidApplication, _In_ AInputQueue* InInputQueue)
		{
			AndroidApplicationSafeExecuteAndWait(
				InAndroidApplication,
				[InInputQueue](_In_ android_app* InAndroidApplication)
				{
					InAndroidApplication->pendingInputQueue = InInputQueue;
					AndroidApplicationWriteCommand(InAndroidApplication, APP_CMD_INPUT_CHANGED);
				},
				[](_In_ android_app* InAndroidApplication) -> bool
				{
					return InAndroidApplication->inputQueue == InAndroidApplication->pendingInputQueue;
				}
			);
		}

		//////////////////////////////////////////////////////////////////////////

		static void AndroidApplicationCommandProcess(_In_ android_app* InAndroidApplication, _In_ android_poll_source* InAndroidPollSource)
		{
			int8_t Command = AndroidApplicationReadCommand(InAndroidApplication);
			InAndroidApplication->onAppCmd(InAndroidApplication, Command);
		}

		static void AndroidApplicationInputProcess(_In_ android_app* InAndroidApplication, _In_ android_poll_source* InAndroidPollSource)
		{
			ETERNAL_BREAK();
		}

		//////////////////////////////////////////////////////////////////////////

		static void AndroidActivityOnStart(_In_ ANativeActivity* InActivity)
		{
			AndroidApplicationSendActivityStateCommand(reinterpret_cast<android_app*>(InActivity->instance), APP_CMD_START);
		}

		static void AndroidActivityOnResume(_In_ ANativeActivity* InActivity)
		{
			AndroidApplicationSendActivityStateCommand(reinterpret_cast<android_app*>(InActivity->instance), APP_CMD_RESUME);
		}

		static void* AndroidActivityOnSaveInstanceState(_In_ ANativeActivity* InActivity, _Out_ size_t* OutSize)
		{
			ETERNAL_BREAK();
		}

		static void AndroidActivityOnPause(_In_ ANativeActivity* InActivity)
		{
			AndroidApplicationSendActivityStateCommand(reinterpret_cast<android_app*>(InActivity->instance), APP_CMD_PAUSE);
		}

		static void AndroidActivityOnStop(_In_ ANativeActivity* InActivity)
		{
			AndroidApplicationSendActivityStateCommand(reinterpret_cast<android_app*>(InActivity->instance), APP_CMD_STOP);
		}

		static void AndroidActivityOnDestroy(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}

		static void AndroidActivityOnWindowFocusChanged(_In_ ANativeActivity* InActivity, _In_ int HasFocus)
		{
			AndroidApplicationSendActivityStateCommand(reinterpret_cast<android_app*>(InActivity->instance), HasFocus ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);
		}

		static void AndroidActivityOnNativeWindowCreated(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			AndroidApplicationSetWindow(reinterpret_cast<android_app*>(InActivity->instance), InWindow);
		}

		static void AndroidActivityOnNativeWindowResized(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			//ETERNAL_BREAK();
		}

		static void AndroidActivityOnNativeWindowRedrawNeeded(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			//ETERNAL_BREAK();
		}

		static void AndroidActivityOnNativeWindowDestroyed(_In_ ANativeActivity* InActivity, _In_ ANativeWindow* InWindow)
		{
			AndroidApplicationSetWindow(reinterpret_cast<android_app*>(InActivity->instance), nullptr);
		}

		static void AndroidActivityOnInputQueueCreated(_In_ ANativeActivity* InActivity, _In_ AInputQueue* InQueue)
		{
			AndroidApplicationSetInput(reinterpret_cast<android_app*>(InActivity->instance), InQueue);
		}

		static void AndroidActivityOnInputQueueDestroyed(_In_ ANativeActivity* InActivity, _In_ AInputQueue* InQueue)
		{
			AndroidApplicationSetInput(reinterpret_cast<android_app*>(InActivity->instance), nullptr);
		}

		static void AndroidActivityOnContentRectChanged(_In_ ANativeActivity* InActivity, _In_ const ARect* InRectangle)
		{
			//ETERNAL_BREAK();
		}

		static void AndroidActivityOnConfigurationChanged(_In_ ANativeActivity* InActivity)
		{
			ETERNAL_BREAK();
		}

		static void AndroidActivityOnLowMemory(_In_ ANativeActivity* InActivity)
		{
			AndroidApplicationSendActivityStateCommand(reinterpret_cast<android_app*>(InActivity->instance), APP_CMD_LOW_MEMORY);
		}

		//////////////////////////////////////////////////////////////////////////

		void AndroidApplicationOnAppCmd(_In_ android_app* InAndroidApplication, _In_ int32_t InCommand)
		{
			switch (InCommand)
			{
			case APP_CMD_INPUT_CHANGED:
			{
				AndroidApplicationSafeExecuteAndSignal(
					InAndroidApplication,
					[](_In_ android_app* InAndroidApplication)
					{
						if (InAndroidApplication->inputQueue)
							AInputQueue_detachLooper(InAndroidApplication->inputQueue);
						
						InAndroidApplication->inputQueue = InAndroidApplication->pendingInputQueue;
						
						if (InAndroidApplication->inputQueue)
							AInputQueue_attachLooper(
								InAndroidApplication->inputQueue,
								InAndroidApplication->looper,
								LOOPER_ID_INPUT,
								nullptr,
								&InAndroidApplication->inputPollSource
							);
					}
				);
			} break;
			case APP_CMD_INIT_WINDOW:
			{
				AndroidApplicationSafeExecuteAndSignal(
					InAndroidApplication,
					[](_In_ android_app* InAndroidApplication)
					{
						InAndroidApplication->window = InAndroidApplication->pendingWindow;
						AndroidGraphicsContextCreateInformation* ContextCreateInformation = static_cast<AndroidGraphicsContextCreateInformation*>(
							reinterpret_cast<Game*>(InAndroidApplication->userData)->_GameCreateInformation.SystemInformation.ContextInformation
						);
						ContextCreateInformation->NativeWindow		= InAndroidApplication->window;
						ContextCreateInformation->Settings.Width	= ANativeWindow_getWidth(InAndroidApplication->window);
						ContextCreateInformation->Settings.Height	= ANativeWindow_getHeight(InAndroidApplication->window);
						*reinterpret_cast<Game*>(InAndroidApplication->userData)->GetRunningPointer() = false;
					}
				);
			} break;
			case APP_CMD_TERM_WINDOW:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_WINDOW_RESIZED:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_WINDOW_REDRAW_NEEDED:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_CONTENT_RECT_CHANGED:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_GAINED_FOCUS:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_LOST_FOCUS:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_CONFIG_CHANGED:
			{
			} break;
			case APP_CMD_LOW_MEMORY:
			{
			} break;
			case APP_CMD_START:
			case APP_CMD_RESUME:
			case APP_CMD_PAUSE:
			case APP_CMD_STOP:
			{
				AndroidApplicationSetActivityState(InAndroidApplication, InCommand);
			} break;
			case APP_CMD_SAVE_STATE:
			{
				ETERNAL_BREAK();
			} break;
			case APP_CMD_DESTROY:
			{
				ETERNAL_BREAK();
			} break;
			default:
				break;
			}
		}

		static int32_t AndroidApplicationOnInputEvent(_In_ android_app* InApplication, _In_ AInputEvent* InEvent)
		{
			ETERNAL_BREAK();
		}

		//////////////////////////////////////////////////////////////////////////

		int AndroidLooperCallbackFunction(_In_ int InFileDescriptor, _In_ int InEvents, _In_ void* InData)
		{
			ETERNAL_BREAK();
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
			AndroidApplication->cmdPollSource.process	= AndroidApplicationCommandProcess;

			AndroidApplication->inputPollSource.id		= LOOPER_ID_INPUT;
			AndroidApplication->inputPollSource.app		= AndroidApplication;
			AndroidApplication->inputPollSource.process	= AndroidApplicationInputProcess;

			AndroidApplication->looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
			if (ALooper_addFd(
				AndroidApplication->looper,
				AndroidApplication->msgread,
				LOOPER_ID_MAIN,
				ALOOPER_EVENT_INPUT,
				nullptr, //AndroidLooperCallbackFunction,
				&AndroidApplication->cmdPollSource
			) == -1)
			{
				ETERNAL_BREAK();
				return nullptr;
			}


			if (!AndroidApplicationSafeExecuteAndSignal(
				AndroidApplication,
				[](_In_ android_app* InAndroidApplication)
				{
					InAndroidApplication->running = 1;
				}
			))
			{
				return nullptr;
			}

			Game* InGame = reinterpret_cast<Game*>(AndroidApplication->userData);

			while (*InGame->GetRunningPointer())
			{
				AndroidSystem::AndroidLooperProcess(AndroidApplication);
			}

			*InGame->GetRunningPointer() = true;

			InGame->_System = CreateSystem(InGame->_GameCreateInformation.SystemInformation);
			reinterpret_cast<Game*>(AndroidApplication->userData)->Run();

			return nullptr;
		}

		void RunGame(_In_ GameCreateInformation& InGameCreateInformation, _Inout_ Game* InOutGame)
		{
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

			AndroidAssetManagerFile::RegisterAndroidAssetManager(AndroidNativeActivity->assetManager);

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

			{
				AndroidMutexAutoLock AutoLock(AndroidApplication);
				if (!AutoLock)
					return;

				while (!AndroidApplication->running)
				{
					if (pthread_cond_wait(&AndroidApplication->cond, &AndroidApplication->mutex))
					{
						ETERNAL_BREAK();
					}
				}
			}

			AndroidNativeActivity->instance = AndroidApplication;
		}
	}
}

#endif
