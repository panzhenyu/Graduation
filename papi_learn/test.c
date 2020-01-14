#include <papi.h>
#include <stdlib.h>
#include <stdio.h>

void handler_error (int retval)
{
	printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
	exit(1);
}
int main()
{
	int retval, EventSet = PAPI_NULL;
	unsigned int native = 0;
	PAPI_event_info_t info;

	/* Initialize the library */
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT)
	{
		printf("PAPI library init error!\n");
		exit(1);
	}

	/* Create an EvnetSet */
	retval = PAPI_create_eventset(&EventSet);
	if (retval != PAPI_OK) handler_error(retval);

	/* Find the first available native event */
	native = PAPI_NATIVE_MASK | 0;
	retval = PAPI_enum_event(&native, PAPI_ENUM_FIRST);
	if (retval != PAPI_OK) handler_error(retval);

	/* Add it to the eventset */
	retval = PAPI_add_event(EventSet, native);
	if (retval != PAPI_OK) handler_error(retval);

	/* Exit successfully */
	printf("Success!\n");

	exit(0);
}
