#include <CoreServices/CoreServices.h>
#include <unistd.h>
#include <time.h>

#define SEP_NEWLINE '\n'
#define SEP_NULL '\0'

static char separator = SEP_NEWLINE;
static int run_once = 0;

static void callback(ConstFSEventStreamRef streamRef,
					 void *clientCallBackInfo,
					 size_t numEvents,
					 void *eventPaths,
					 const FSEventStreamEventFlags eventFlags[],
					 const FSEventStreamEventId eventIds[]) {

	for(int i=0; i < numEvents; i++) {
		printf("%s%c", ((char**)eventPaths)[i], separator);
	}

	fflush(NULL);
	
	if(run_once)
		CFRunLoopStop(CFRunLoopGetMain());
}

void usage() {
	printf("Sleep until a file in or below the watchdir is modified.\n");
	printf("Usage: fsevent_sleep /path/to/watchdir\n");
	exit(1);
}

int main (int argc, const char * argv[]) {
	close(STDIN_FILENO);
	
	// Show help
	if (argc < 2 || strncmp(argv[1], "-h", 2) == 0) {
		usage();
	}
	
	// Create event stream
	CFMutableArrayRef pathsToWatch = CFArrayCreateMutable(NULL, argc-1, NULL);
	
	for(int i=1; i<argc; i++) {
		CFArrayAppendValue(pathsToWatch, CFStringCreateWithCString(kCFAllocatorDefault, argv[i], kCFStringEncodingUTF8));
	}
	
    void *callbackInfo = NULL;
    FSEventStreamRef stream;
    CFAbsoluteTime latency = 1.0;
    stream = FSEventStreamCreate(
								 kCFAllocatorDefault,
								 callback,
								 callbackInfo,
								 pathsToWatch,
								 kFSEventStreamEventIdSinceNow,
								 latency,
								 kFSEventStreamCreateFlagNone
								 );
	
	// Add stream to run loop
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	FSEventStreamStart(stream);
	CFRunLoopRun();
	
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	// Exit
	return 0;
}
