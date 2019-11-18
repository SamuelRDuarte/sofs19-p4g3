#include "grp_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "fileblocks.h"
#include "bin_fileblocks.h"

#include <string.h>
#include <inttypes.h>

namespace sofs19
{
    void grpWriteFileBlock(int ih, uint32_t fbn, void *buf)
    {
        soProbe(332, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);

        //binWriteFileBlock(ih, fbn, buf);
        uint32_t bn = soGetFileBlock(ih, fbn);

        if(bn == NullReference){
            bn = soAllocFileBlock(ih, fbn);
        }

        soWriteDataBlock(bn, buf);
    }
};

