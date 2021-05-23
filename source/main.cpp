//
//  main.cpp
//  Cornell University Game Library (CUGL)
//
//  This is the main entry class for your application.  You may need to modify
//  it slightly for your application class or platform.
//
//  CUGL zlib License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 2/21/21

// Include your application class
#include "App.h"

using namespace cugl;

/**
 * The main entry point of any CUGL application.
 *
 * This class creates the application and runs it until done.  You may
 * need to tailor this to your application, particularly the application
 * settings.  However, never modify anything below the line marked.
 *
 * @return the exit status of the application
 */
int main(int argc, char * argv[]) {
    // Change this to your application class
    LabApp app;
    
    // Set the properties of your application
    app.setName("Dire Deal");
    app.setOrganization("Catana Games");
    app.setHighDPI(true);
    app.setFPS(60.0f);

    // VARY THIS TO TRY OUT YOUR SCENE GRAPH
    // app.setSize(1560, 720); // iphone11
    // app.setSize(1280, 720); // 16x9,  Android phones, PC Gaming
    app.setSize(683, 512); // ipad pro (smaller)
    //app.setSize(1366, 1024); // ipad pro

    /// DO NOT MODIFY ANYTHING BELOW THIS LINE
    if (!app.init()) {
        return 1;
    }
    
    app.onStartup();
    while (app.step());
    app.onShutdown();

    exit(0);    // Necessary to quit on mobile devices
    return 0;   // This line is never reached
}
