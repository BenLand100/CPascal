/**
 * I've had this on my harddrive forever, no idea where it came from, but I did
 * not write it, just modified it to be in a single file.
 */

//#define MEMORY_TRACE

#ifdef MEMORY_TRACE

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define LT_MALLOC  malloc
#define LT_FREE    free
#define LT_REALLOC realloc

/*
 * prime number for the address lookup hash table.
 * if you have _really_ many memory allocations, use a
 * higher value, like 343051 for instance.
 */
#define SOME_PRIME 343051
#define ADDR_HASH(addr) ((unsigned long) addr % SOME_PRIME)


#define MAGIC "\xAA\xBB\xCC\xDD\xEE\xF"
#define MAGIC_SIZE (sizeof(MAGIC)-1)


#define SAVEVALUE   0xAA

#define MEMCLEAN    0xEE

/**
 * Initial Number of memory allocations in our list.
 * Doubles for each re-allocation.
 */
#define INITIALSIZE 32768

static class LeakTracer {

    struct Leak {
        const void *addr;
        size_t size;
        const void *allocAddr;
        bool type;
        int nextBucket;
    };

    int newCount; // how many memory blocks do we have
    int leaksCount; // amount of entries in the leaks array
    int firstFreeSpot; // Where is the first free spot in the leaks array?
    int currentAllocated; // currentAllocatedMemory
    int maxAllocated; // maximum Allocated
    unsigned long totalAllocations; // total number of allocations. stats.
    unsigned int abortOn; // resons to abort program (see abortReason_t)

    
    bool initialized;
    bool destroyed; 

    Leak *leaks;

    int *leakHash; // fast lookup

    enum abortReason_t {
        OVERWRITE_MEMORY = 0x01, DELETE_NONEXISTENT = 0x02, NEW_DELETE_MISMATCH = 0x04
    };

public:

    LeakTracer() {
        initialize();
    }

    void initialize() {
        if (initialized) return;
        // fprintf(stderr, "LeakTracer::initialize()\n");
        initialized = true;
        newCount = 0;
        leaksCount = 0;
        firstFreeSpot = 1; // index '0' is special
        currentAllocated = 0;
        maxAllocated = 0;
        totalAllocations = 0;
        abortOn = OVERWRITE_MEMORY | DELETE_NONEXISTENT | NEW_DELETE_MISMATCH;
        leaks = 0;
        leakHash = 0;

        time_t t = time(NULL);
        fprintf(stderr, "# starting %s", ctime(&t));

        leakHash = (int*) LT_MALLOC(SOME_PRIME * sizeof (int));
        memset((void*) leakHash, 0x00, SOME_PRIME * sizeof (int));

#ifdef MAGIC
        fprintf(stderr, "# memory overrun protection of %d Bytes\n", MAGIC_SIZE);
#endif

#ifdef SAVEVALUE
        fprintf(stderr, "# initializing new memory with 0x%2X\n", SAVEVALUE);
#endif

#ifdef MEMCLEAN
        fprintf(stderr, "# sweeping deleted memory with 0x%2X\n", MEMCLEAN);
#endif

        if (getenv("LT_ABORTREASON")) {
            abortOn = atoi(getenv("LT_ABORTREASON"));
        }

#define PRINTREASON(x) if (abortOn & x) fprintf(stderr, "%s ", #x);
        fprintf(stderr, "# aborts on ");
        PRINTREASON(OVERWRITE_MEMORY);
        PRINTREASON(DELETE_NONEXISTENT);
        PRINTREASON(NEW_DELETE_MISMATCH);
        fprintf(stderr, "\n");
#undef PRINTREASON

        fprintf(stderr, "# not thread save; if you use threads, recompile with -DTHREAD_SAVE\n");
    }

    void* registerAlloc(size_t size, bool type);
    void registerFree(void *p, bool type);

    void hexdump(const unsigned char* area, int size);

    void progAbort(abortReason_t reason) {
        if (abortOn & reason) {
            fprintf(stderr, "# abort; DUMP of current state\n");
            fprintf(stderr, "LeakTracer aborting program\n");
            writeLeakReport();
            abort();
        }
    }

    void writeLeakReport();

    ~LeakTracer() {
        //fprintf(stderr, "LeakTracer::destroy()\n");
        time_t t = time(NULL);
        fprintf(stderr, "# finished %s", ctime(&t));
        writeLeakReport();
        free(leaks);
        destroyed = true;
    }
} leakTracer;

void* LeakTracer::registerAlloc(size_t size, bool type) {
    initialize();

    //fprintf(stderr, "LeakTracer::registerAlloc()\n");

    if (destroyed) {
        fprintf(stderr, "Oops, registerAlloc called after destruction of LeakTracer (size=%d)\n", size);
        return LT_MALLOC(size);
    }


    void *p = LT_MALLOC(size + MAGIC_SIZE);
    // Need to call the new-handler
    if (!p) {
        fprintf(stderr, "LeakTracer malloc %m\n");
        _exit(1);
    }

#ifdef SAVEVALUE
    memset(p, SAVEVALUE, size + MAGIC_SIZE);
#endif

#ifdef MAGIC
    memcpy((char*) p + size, MAGIC, MAGIC_SIZE);
#endif

    ++newCount;
    ++totalAllocations;
    currentAllocated += size;
    if (currentAllocated > maxAllocated)
        maxAllocated = currentAllocated;

    for (;;) {
        for (int i = firstFreeSpot; i < leaksCount; i++)
            if (leaks[i].addr == NULL) {
                leaks[i].addr = p;
                leaks[i].size = size;
                leaks[i].type = type;
                leaks[i].allocAddr = __builtin_return_address(1);
                firstFreeSpot = i + 1;
                // allow to lookup our index fast.
                int *hashPos = &leakHash[ ADDR_HASH(p) ];
                leaks[i].nextBucket = *hashPos;
                *hashPos = i;
                return p;
            }

        // Allocate a bigger array
        // Note that leaksCount starts out at 0.
        int new_leaksCount = (leaksCount == 0) ? INITIALSIZE
                : leaksCount * 2;
        leaks = (Leak*) LT_REALLOC(leaks, sizeof (Leak) * new_leaksCount);
        if (!leaks) {
            fprintf(stderr, "# LeakTracer realloc failed: %m\n");
            _exit(1);
        } else {
            fprintf(stderr, "# internal buffer now %d\n", new_leaksCount);
        }
        memset(leaks + leaksCount, 0x00, sizeof (Leak) * (new_leaksCount - leaksCount));
        leaksCount = new_leaksCount;
    }
}

void LeakTracer::hexdump(const unsigned char* area, int size) {
    fprintf(stderr, "# ");
    for (int j = 0; j < size; ++j) {
        fprintf(stderr, "%02x ", *(area + j));
        if (j % 16 == 15) {
            fprintf(stderr, "  ");
            for (int k = -15; k < 0; k++) {
                char c = (char) *(area + j + k);
                fprintf(stderr, "%c", isprint(c) ? c : '.');
            }
            fprintf(stderr, "\n# ");
        }
    }
    fprintf(stderr, "\n");
}

void LeakTracer::registerFree(void *p, bool type) {
    initialize();

    if (p == NULL)
        return;

    if (destroyed) {
        fprintf(stderr, "Oops, allocation destruction of LeakTracer (p=%p)\n", p);
        return;
    }

    int *lastPointer = &leakHash[ ADDR_HASH(p) ];
    int i = *lastPointer;

    while (i != 0 && leaks[i].addr != p) {
        lastPointer = &leaks[i].nextBucket;
        i = *lastPointer;
    }

    if (leaks[i].addr == p) {
        *lastPointer = leaks[i].nextBucket; // detach.
        newCount--;
        leaks[i].addr = NULL;
        currentAllocated -= leaks[i].size;
        if (i < firstFreeSpot)
            firstFreeSpot = i;

        if (leaks[i].type != type) {
            fprintf(stderr, "S %10p %10p  # new%s but delete%s "
                    "; size %d\n", leaks[i].allocAddr, __builtin_return_address(1), ((!type) ? "[]" : " normal"), ((type) ? "[]" : " normal"), leaks[i].size);

            progAbort(NEW_DELETE_MISMATCH);
        }
#ifdef MAGIC
        if (memcmp((char*) p + leaks[i].size, MAGIC, MAGIC_SIZE)) {
            fprintf(stderr, "O %10p %10p  "
                    "# memory overwritten beyond allocated"
                    " %d bytes\n", leaks[i].allocAddr, __builtin_return_address(1), leaks[i].size);
            fprintf(stderr, "# %d byte beyond area:\n", MAGIC_SIZE);
            hexdump((unsigned char*) p + leaks[i].size, MAGIC_SIZE);
            progAbort(OVERWRITE_MEMORY);
        }
#endif

#ifdef MEMCLEAN
        memset((unsigned char*) p, MEMCLEAN, leaks[i].size + MAGIC_SIZE);
#endif
        LT_FREE(p);
        return;
    }

    fprintf(stderr, "D %10p             # delete non alloc or twice pointer %10p\n", __builtin_return_address(1), p);
    progAbort(DELETE_NONEXISTENT);
}

void LeakTracer::writeLeakReport() {
    initialize();

    if (newCount > 0) {
        fprintf(stderr, "# LeakReport\n");
        fprintf(stderr, "# %10s | %9s  # Pointer Addr\n", "from new @", "size");
    }
    /*for (int i = 0; i < leaksCount ; i++)
        if (leaks[i].addr != NULL) {
            // This ought to be 64-bit safe?
            fprintf(stderr, "L %10p   %9ld  # %p\n", leaks[i].allocAddr, (long) leaks[i].size, leaks[i].addr);
        }*/
    fprintf(stderr, "# total allocation requests: %6ld ; max. mem used"
            " %d kBytes\n", totalAllocations, maxAllocated / 1024);
    fprintf(stderr, "# leak %6d Bytes\t:-%c\n", currentAllocated, (currentAllocated == 0) ? ')' : '(');
    if (currentAllocated > 50 * 1024) {
        fprintf(stderr, "# .. that is %d kByte!! A lot ..\n", currentAllocated / 1024);
    }
}

/** -- The actual new/delete operators -- **/

void* operator new(size_t size) {
    return leakTracer.registerAlloc(size, false);
}

void* operator new[] (size_t size) {
    return leakTracer.registerAlloc(size, true);
}

void operator delete (void *p) {
    leakTracer.registerFree(p, false);
}

void operator delete[] (void *p) {
    leakTracer.registerFree(p, true);
}

#endif
