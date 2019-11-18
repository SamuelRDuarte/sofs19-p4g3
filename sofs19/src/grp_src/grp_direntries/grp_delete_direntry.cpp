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
    uint32_t grpDeleteDirEntry(int pih, const char *name)
    {
        soProbe(203, "%s(%d, %s)\n", __FUNCTION__, pih, name);

        //return binDeleteDirEntry(pih, name);
        SOInode* ip = soGetInodePointer(pih);
        SODirEntry ref[DPB];

        uint32_t numBlocks = (ip->size) / BlockSize 
        uint32_t result;

        for(uint32_t i = 0; i < numBlocks; i++) {
            soReadFileBlock(pih, i, ref);

            for(uint32_t j = 0; j < DPB; j++) {
                if(strcmp(ref[j].name, name) == 0) {
                    memset(ref[j].name, '\0', SOFS19_MAX_NAME + 1);
                    ref[j].in = NullReference;
                    soWriteFileBlock(pih, i, ref);
                    return resutl;
                }
            }
        }
        throw SOException(ENOENT, __FUNCTION__);
    }
};

