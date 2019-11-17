#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs19
{
    uint32_t grpGetDirEntry(int pih, const char *name)
    {
        soProbe(201, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        SOInode* pip = soGetInodePointer(pih);

        if((pip->mode & S_IFDIR) != S_IFDIR ){
            throw SOException(ENOTDIR, __FUNCTION__);
        }

        uint32_t nd = pip->size / sizeof(SODirEntry);

        SODirEntry dir[DPB];

        for (uint32_t i = 0; i < nd; ++i) {
            uint32_t fbn = i / DPB;
            soReadFileBlock(pih, fbn, dir);

            for (uint32_t j = 0; j < DPB; ++j) {
                if(name == dir[j].name){
                    return dir[j].in;
                }
            }
        }

        return -1;
    }
};

