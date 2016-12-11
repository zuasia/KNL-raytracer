#include <iostream>
#include <time.h>
#include <stdarg.h>

#include <assert.h>

#include "CommandLineUI.h"
#include "../fileio/bitmap.h"

#include "../RayTracer.h"
#include "../CycleTimer.h"

using namespace std;

// The command line UI simply parses out all the arguments off
// the command line and stores them locally.
CommandLineUI::CommandLineUI( int argc, char* const* argv )
	: TraceUI()
{
	int i;

	progName=argv[0];

	while( (i = getopt( argc, argv, "tr:w:h:" )) != EOF )
	{
		switch( i )
		{
			case 'r':
				m_nDepth = atoi( optarg );
				break;

			case 'w':
				m_nSize = atoi( optarg );
				break;
			default:
			// Oops; unknown argument
			std::cerr << "Invalid argument: '" << i << "'." << std::endl;
			usage();
			exit(1);
		}
	}

	if( optind >= argc-1 )
	{
		std::cerr << "no input and/or output name." << std::endl;
		exit(1);
	}

	rayName = argv[optind];
	imgName = argv[optind+1];
}

int CommandLineUI::run()
{
	assert( raytracer != 0 );
	raytracer->loadScene( rayName );

	if( raytracer->sceneLoaded() )
	{
		int width = m_nSize;
		int height = (int)(width / raytracer->aspectRatio() + 0.5);

		raytracer->traceSetup( width, height );
		printf("loop = %d\n", height * width); 
		

		double startComputeTime, endComputeTime, computeDuration;
		for( int j = 0; j < height; ++j )
			for( int i = 0; i < width; ++i ) {
				startComputeTime = CycleTimer::currentSeconds();

				raytracer->tracePixel(i,j);

				endComputeTime = CycleTimer::currentSeconds();
				computeDuration = endComputeTime - startComputeTime;
				printf("Compute: %.3f ms\t\n", 1000.f * computeDuration);
			}
		

/*
		double startComputeTime = CycleTimer::currentSeconds();

		OMP("omp parallel") 
  		{
    		OMP("omp for")
			for( int j = 0; j < height * width; ++j )
			{
				int i = j % height;
				int m = (j - i)/height;


				raytracer->tracePixel(m,i);

			}
		}

		double endComputeTime = CycleTimer::currentSeconds();
		double computeDuration = endComputeTime - startComputeTime;
		printf("Compute: %.3f ms\t\n", 1000.f * computeDuration);
*/

		// save image
		unsigned char* buf;

		raytracer->getBuffer(buf, width, height);

		if (buf)
			writeBMP(imgName, width, height, buf);

        return 0;
	}
	else
	{
		std::cerr << "Unable to load ray file '" << rayName << "'" << std::endl;
		return( 1 );
	}
}

void CommandLineUI::alert( const string& msg )
{
	std::cerr << msg << std::endl;
}

void CommandLineUI::usage()
{
	std::cerr << "usage: " << progName << " [options] [input.ray output.bmp]" << std::endl;
	std::cerr << "  -r <#>      set recursion level (default " << m_nDepth << ")" << std::endl; 
	std::cerr << "  -w <#>      set output image width (default " << m_nSize << ")" << std::endl;
}
