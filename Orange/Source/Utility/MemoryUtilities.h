#ifndef MEMORY_UTILITIES_H
#define MEMORY_UTILITIES_H

namespace Orange
{
	namespace Memory
	{

		// This function should be called once at the very end of the
		// application's lifetime. Further changes need to be made to
		// make this a "checkpoint" system
		void FindMemoryLeaks();

	}
}

#endif