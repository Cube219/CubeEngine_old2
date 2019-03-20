#include "BaseTypes.h"


#include <iostream>

void* operator new[](size_t size, const char* /*name*/, int /*flags*/,
	unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* /*name*/,
	int flags, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	//return _aligned_offset_malloc(size, alignment, alignmentOffset);

	// TODO: 일단은 alignment 무시...
	// _aligned_free로 해제해야 하는데 확인을 할 수가 없기 때문
	// 나중에 찾아볼까?
	return malloc(size);
}
