#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <iostream>
#include <time.h>
#include <chrono>

#include <stdio.h>
#include <string.h>


#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>


Window* findWindows( Display* display, ulong* winCount ) {
    Atom actualType;
    int format;
    ulong bytesAfter;
    unsigned char* list = NULL;

    Status status = XGetWindowProperty(
                        display,
                        DefaultRootWindow( display ),
                        XInternAtom( display, "_NET_CLIENT_LIST", False ),
                        0L,
                        ~0L,
                        False,
                        XA_WINDOW,
                        &actualType,
                        &format,
                        winCount,
                        &bytesAfter,
                        &list
                    );

    if( status != Success ) {
        *winCount = 0;
        return NULL;
    }

    return reinterpret_cast< Window* >( list );
}


char* getWindowName( Display* display, Window win ) {
    Atom actualType;
    int format;
    ulong count, bytesAfter;
    unsigned char* name = NULL;
    Status status = XGetWindowProperty(
                        display,
                        win,
                        XInternAtom( display, "_NET_WM_NAME", False ),
                        0L,
                        ~0L,
                        False,
                        XInternAtom( display, "UTF8_STRING", False ),
                        &actualType,
                        &format,
                        &count,
                        &bytesAfter,
                        &name
                    );

    if( status != Success ) {
        return NULL;
    }

    if( name == NULL ) {
        Status status = XGetWindowProperty(
                            display,
                            win,
                            XInternAtom( display, "WM_NAME", False ),
                            0L,
                            ~0L,
                            False,
                            AnyPropertyType,
                            &actualType,
                            &format,
                            &count,
                            &bytesAfter,
                            &name
                        );

        if( status != Success ) {
            return NULL;
        }
    }

    return reinterpret_cast< char* >( name );
}

Display* display = XOpenDisplay( NULL );
std::vector<char*> arr;
int ScreenNumber = DefaultScreen ( display );
ulong count = 0;
Window* wins = findWindows( display, &count );
XWindowAttributes attrs;
Window wis;
XImage* ximg;
XShmSegmentInfo shminfo;
unsigned int * data_img;

extern "C" void bar()
{
            //cv::waitKey(0);

            for( ulong i = 0; i < count; ++i ) {
                Window w = wins[ i ];
                if( char* name = getWindowName( display, w ) ) {
                     arr.push_back(name);
                     std::cout << i << " : " << name << std::endl;
                }
            }
            std::cout << arr.size() << std::endl;
            int age;
            std::cin >> age;
            
            std::cout << arr[age] << std::endl;

            wis = wins[age];

                            XGetWindowAttributes( display, wis, &attrs );
                            Screen* screen = attrs.screen;
                            XMapWindow(display, wis);
                            ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &shminfo, attrs.width, attrs.height);
                            shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|0777);
//                            shminfo.shmaddr = (char*)shmat(shminfo.shmid, 0, 0);
                            shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
                            data_img = (unsigned int*) shminfo.shmaddr;
                            shminfo.readOnly = false;
                            if(shminfo.shmid < 0)
                                puts("Fatal shminfo error!");;
                            Status s1 = XShmAttach(display, &shminfo);
                            printf("XShmAttach() %s\n", s1 ? "success!" : "failure!");

}

extern "C" int getW ()
{
            
            return attrs.width;
}
extern "C" int getH ()
{
            
            return attrs.height;

}
extern "C" void close_win()
{
                    std::cout << ">>>>>" << std::endl;
                    XShmDetach(display, &shminfo);
                    XDestroyImage(ximg);
                    shmdt(shminfo.shmaddr);
                    XCloseDisplay(display);
                    puts("Exit success!"); 
}

extern "C" void fdata(unsigned int* fdata)
{
            
                            XShmGetImage(display, wis, ximg, 0, 0, 0x00ffffff);
                            memcpy(fdata, ximg->data, sizeof(unsigned int) * attrs.height * attrs.width);
                            
}

//g++ -c -fPIC cap_app_lib_2.cpp -o lib -lX11 -lXext -I/usr/include/python3.6 -I/usr/lib/python3.6 && g++ -shared -Wl,-soname,lib.so -o lib.so lib -lX11 -lXext

