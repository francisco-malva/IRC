#include <stdint.h>
#include <string.h>

#include "args.h"



#define MAX_ARGS (8)

struct {
	const char* Name;
	const char* Value;
} typedef ArgEntry;

uint8_t ArgCount;


ArgEntry ArgEntries[MAX_ARGS];


enum MATCH_STATE {
	MATCH_LOOKING_FOR_ARG,
	MATCH_LOOKING_FOR_VALUE
};

void ArgsInit(int argc, char* argv[])
{
	const char* name = NULL, * value = NULL;

	ArgCount = 0;
	uint8_t match = MATCH_LOOKING_FOR_ARG;

	int argi = 0;

	while (argi < argc && ArgCount < MAX_ARGS) {

		const char* arg = argv[argi];

		switch (match) {
		case MATCH_LOOKING_FOR_ARG:

			if (arg[0] == '-' && strlen(arg) >= 2) {
				match = MATCH_LOOKING_FOR_VALUE;

				name = arg + 1;

				match = MATCH_LOOKING_FOR_VALUE;
			}
			break;
		case MATCH_LOOKING_FOR_VALUE:

			value = arg;

			ArgEntries[ArgCount].Name = name;
			ArgEntries[ArgCount].Value = value;
			++ArgCount;

			match = MATCH_LOOKING_FOR_ARG;
			break;
		}

		++argi;
	}
}

void ArgsShutdown(void)
{
	ArgCount = 0;
	memset(ArgEntries, 0, sizeof(ArgEntries));
}

const char* ArgsGet(char* name)
{

	for (size_t i = 0; i < ArgCount; i++) {
		if (strcmp(name, ArgEntries[i].Name) == 0) {
			return ArgEntries[i].Value;
		}
	}

	return NULL;
}
