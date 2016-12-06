#ifndef COMMAND_LINE_ONLY
//#include "ui/GraphicalUI.h"
#endif

#include "RayTracer.h"
#include "ui/CommandLineUI.h"
#include "fileio/bitmap.h"

using namespace std;

RayTracer* theRayTracer;
TraceUI* traceUI;

// usage : ray [option] in.ray out.bmp
// Simply keying in ray will invoke a graphics mode version.
// Use "ray --help" to see the detailed usage.
//
// Graphics mode will be substantially slower than text mode because of
// event handling overhead.
int main(int argc, char **argv) {

	traceUI = new CommandLineUI( argc, argv );

	theRayTracer=new RayTracer();

	traceUI->setRayTracer(theRayTracer);
	return traceUI->run();
}
