#include "eventfilter.h"
#include "mainwindow.h"
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <QtX11Extras/QX11Info>


void EventFilter::setup(QWidget *target)
{
    this->target = target;

    Display * display = QX11Info::display();
    keycodePrevious = XKeysymToKeycode(display, XK_A);
    keycodeNext = XKeysymToKeycode(display, XK_S);
    XGrabKey(display, keycodePrevious, AnyModifier, DefaultRootWindow(display), 1, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, keycodeNext, AnyModifier, DefaultRootWindow(display), 1, GrabModeAsync, GrabModeAsync);
}

bool EventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "xcb_generic_event_t")
    {
        xcb_generic_event_t* xcbevent = static_cast<xcb_generic_event_t *>(message);

        switch(xcbevent->response_type)
        {
        case XCB_KEY_PRESS:
            xcb_key_press_event_t * keypress_event = static_cast<xcb_key_press_event_t *>(message);
            if(keypress_event->detail == keycodePrevious)
            {
                qobject_cast<MainWindow*>(target->topLevelWidget())->previous();
            }
            if(keypress_event->detail == keycodeNext)
            {
                qobject_cast<MainWindow*>(target->topLevelWidget())->next();
            }
        }
    }
    return false;
}
