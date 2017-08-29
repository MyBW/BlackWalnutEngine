
#ifndef __OgreWindowEventUtils_H__
#define __OgreWindowEventUtils_H__

#include "BWRenderWindow.h"
class ListenerTest;

#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>



	class  WindowEventListener
	{
	public:
		virtual ~WindowEventListener() {}

		/**
		@Remarks
			Window has moved position
		@param rw
			The RenderWindow which created this events
		*/
		virtual void windowMoved(BWRenderWindow* rw)
                { (void)rw; }

		/**
		@Remarks
			Window has resized
		@param rw
			The RenderWindow which created this events
		*/
		virtual void windowResized(BWRenderWindow* rw)
                { (void)rw; }

		/**
		@Remarks
			Window is closing (Only triggered if user pressed the [X] button)
		@param rw
			The RenderWindow which created this events
		@return True will close the window(default).
		*/
		virtual bool windowClosing(BWRenderWindow* rw)
		{ (void)rw; return true; }

		/**
		@Remarks
			Window has been closed (Only triggered if user pressed the [X] button)
		@param rw
			The RenderWindow which created this events
		@note
			The window has not actually close yet when this event triggers. It's only closed after
			all windowClosed events are triggered. This allows apps to deinitialise properly if they
			have services that needs the window to exist when deinitialising.
		*/
		virtual void windowClosed(BWRenderWindow* rw)
                { (void)rw; }

		/**
		@Remarks
			Window has lost/gained focus
		@param rw
			The RenderWindow which created this events
		*/
		virtual void windowFocusChange(BWRenderWindow* rw)
                { (void)rw; }
	};

	/**
	@Remarks
		Utility class to handle Window Events/Pumping/Messages
	*/
	class  WindowEventUtilities
	{
	public:
		/**
		@Remarks
			Call this once per frame if not using Root:startRendering(). This will update all registered
			RenderWindows (If using external Windows, you can optionally register those yourself)
		*/
		static void messagePump();

		/**
		@Remarks
			Add a listener to listen to renderwindow events (multiple listener's per renderwindow is fine)
			The same listener can listen to multiple windows, as the Window Pointer is sent along with
			any messages.
		@param window
			The RenderWindow you are interested in monitoring
		@param listner
			Your callback listener
		*/
		static void addWindowEventListener( BWRenderWindow* window, WindowEventListener* listener );

		/**
		@Remarks
			Remove previously added listener
		@param window
			The RenderWindow you registered with
		@param listner
			The listener registered
		*/
		static void removeWindowEventListener(BWRenderWindow* window, WindowEventListener* listener);

		/**
		@Remarks
			Called by RenderWindows upon creation for Ogre generated windows. You are free to add your
			external windows here too if needed.
		@param window
			The RenderWindow to monitor
		*/
		static void _addRenderWindow(BWRenderWindow* window);

		/**
		@Remarks
			Called by RenderWindows upon creation for Ogre generated windows. You are free to add your
			external windows here too if needed.
		@param window
			The RenderWindow to remove from list
		*/
		static void _removeRenderWindow(BWRenderWindow* window);

		static void _addListenerTest(ListenerTest * listenertest);

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		//! Internal winProc (RenderWindow's use this when creating the Win32 Window)
		static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE && !defined __OBJC__
        //! Internal UPP Window Handler (RenderWindow's use this when creating the OS X Carbon Window
        static OSStatus _CarbonWindowHandler(EventHandlerCallRef nextHandler, EventRef event, void* wnd);
#endif

		//These are public only so GLXProc can access them without adding Xlib headers header
		typedef std::multimap<BWRenderWindow*, WindowEventListener*> WindowEventListeners;
		static WindowEventListeners _msListeners;

		typedef std::vector<BWRenderWindow*> Windows;
		static Windows _msWindows;

		static ListenerTest* listenerTest;
	};
#endif
