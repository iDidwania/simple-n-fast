#ifndef _UNWIND_H_
#define _UNWIND_H_

#include <stack>
#include "dbfiles.h"

/*
 * Unwind operation type.
 */
typedef enum unwind_op {
	WRITE_FLAGS,
	WRITE_NEXT_OFFSET,
	WRITE_PREV_OFFSET,
	FREE_PAGE
} unwind_op_t;

/*
 * Unwind block: operation + arguments.
 */
typedef struct unwind_block {
	unwind_op_t     op;         // unwind operation
	snf::file       *file;      // file manager: downcast to [Key|Value]File
	void            *page;      // Key/Value page to update
	int64_t         pageOff;    // Page offset
	int64_t         offset;     // New Offset to update or disk block to free
	int             flags;      // New flags
} unwind_block_t;

/**
 * Unwind stack.
 */
class UnwindStack
{
private:
	std::stack<unwind_block_t>	stk;

	void execute();
	void clear()
	{
		while (!stk.empty())
			stk.pop();
	}

public:
	UnwindStack()
	{
	}

	~UnwindStack()
	{
		clear();
	}

	void writeFlags(snf::file *, void *, int64_t, int);
	void writeNextOffset(snf::file *, void *, int64_t, int64_t);
	void writePrevOffset(snf::file *, void *, int64_t, int64_t);
	void freePage(snf::file *, int64_t);
	void unwind(int);
};

#endif // _UNWIND_H_
