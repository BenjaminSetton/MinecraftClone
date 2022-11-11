#ifndef _HEAP_OVERRIDES_H
#define _HEAP_OVERRIDES_H

#if defined (ENABLE_MEMORY_TRACKING)

#include <source_location>

void* operator new(size_t sz, std::source_location location);		// Regular new
void* operator new[](size_t sz, std::source_location location);		// Placement new[]

void operator delete(void* ptr);									// Regular delete
void operator delete(void* ptr, std::source_location location);		// Placement delete

void operator delete[](void* ptr);									// Regular delete[]
void operator delete[](void* ptr, std::source_location location);	// Placement delete[]

#define OG_NEW    new(std::source_location::current())

// This define does nothing, it's just defined as a matching pair for OG_NEW
#define OG_DELETE delete

#else

#define OG_NEW new

#endif

#endif