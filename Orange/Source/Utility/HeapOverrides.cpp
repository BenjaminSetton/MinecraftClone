
#include <unordered_map>
#include <source_location>

#include "MemoryUtilities.h"
#include "Utility.h"

struct ChangeInfo
{
	void* ptr;
	size_t size;
	const char* fileName;
	const char* functionName;
	size_t lineNum;
};

enum class HEAP_CHANGE
{
	ALLOCATION = 0,
	DEALLOCATION
};

// TODO - Revisit this system; I would like to NOT allocate 1 million size_t's for debug memory stuff :')
static constexpr size_t ALLOCATION_BUFFER_LIMIT = 1000000;
static ChangeInfo g_heapChanges[2][ALLOCATION_BUFFER_LIMIT];
static uint64_t g_currentChangeIndices[2] = { 0u, 0u };
static bool g_trackChanges = true;


void TrackHeapChange(HEAP_CHANGE change, void* ptr, size_t sz = 0, const char* fileName = "", const char* functionName = "", size_t lineNum = 0)
{
	if (!g_trackChanges) return;

	ChangeInfo ci;
	ci.ptr = ptr;
	ci.size = sz;
	ci.fileName = fileName;
	ci.functionName = functionName;
	ci.lineNum = lineNum;

	int changeIx = static_cast<int>(change);
	OG_ASSERT_MSG(g_currentChangeIndices[changeIx] < ALLOCATION_BUFFER_LIMIT, "Allocation buffer size exceeded! Consider increasing buffer limit or reducing number of allocations");
	g_heapChanges[changeIx][g_currentChangeIndices[changeIx]++] = ci;
}

void* operator new(size_t sz, std::source_location location)
{
	void* ptr = malloc(sz);
	TrackHeapChange(HEAP_CHANGE::ALLOCATION, ptr, sz, location.file_name(), location.function_name(), location.line());
	return ptr;
}
void* operator new[](size_t sz, std::source_location location)
{
	void* ptr = malloc(sz);
	TrackHeapChange(HEAP_CHANGE::ALLOCATION, ptr, sz, location.file_name(), location.function_name(), location.line());
	return ptr;
}

void operator delete(void* ptr)
{
	TrackHeapChange(HEAP_CHANGE::DEALLOCATION, ptr);
	free(ptr);
}

void operator delete(void* ptr, std::source_location location)
{
	// Doesn't do anything special, we just need a match for placement new
	UNUSED(location);
	free(ptr);
}

void operator delete[](void* ptr)
{
	TrackHeapChange(HEAP_CHANGE::DEALLOCATION, ptr);
	free(ptr);
}

void operator delete[](void* ptr, std::source_location location)
{
	// Doesn't do anything special, we just need a match for placement new[]
	UNUSED(location);
	free(ptr);
}

void Orange::Memory::FindMemoryLeaks()
{
	g_trackChanges = false;

	// Cache all the allocations and deallocations in a map for faster comparison O(N) vs O(N^2)
	//std::unordered_map<void*, size_t> allocMap;
	//for (uint32_t i = 0; i < g_currentChangeIndices[static_cast<uint32_t>(HEAP_CHANGE::ALLOCATION)]; i++)
	//{
	//	ChangeInfo& currentChange = g_heapChanges[static_cast<uint32_t>(HEAP_CHANGE::ALLOCATION)][i];
	//	allocMap[currentChange.ptr] = currentChange.size;
	//}

	// Create a map for deallocations
	std::unordered_map<void*, size_t> deAllocMap;
	for (uint32_t i = 0; i < g_currentChangeIndices[static_cast<uint32_t>(HEAP_CHANGE::DEALLOCATION)]; i++)
	{
		ChangeInfo& currentChange = g_heapChanges[static_cast<uint32_t>(HEAP_CHANGE::DEALLOCATION)][i];
		deAllocMap[currentChange.ptr] = currentChange.size;
	}
	
	// Compare allocations vs. deallocations to see if there are memory leaks
	std::vector<ChangeInfo*> memoryLeaks;
	for (uint32_t i = 0; i < g_currentChangeIndices[static_cast<uint32_t>(HEAP_CHANGE::ALLOCATION)]; i++)
	{
		ChangeInfo* currentChange = &g_heapChanges[static_cast<uint32_t>(HEAP_CHANGE::ALLOCATION)][i];
		void* allocPtr = currentChange->ptr;

		if (!deAllocMap.contains(allocPtr)) memoryLeaks.push_back(currentChange);
	}

	// TODO - Optionally spit these leaks out to a csv file
	OG_ASSERT_MSG(memoryLeaks.size() == 0, "Memory leaks found! Please check the vector above");
}