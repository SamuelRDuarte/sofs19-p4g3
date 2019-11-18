#include "grp_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>

namespace sofs19
{
    void grpFillReferenceDataBlocks(uint32_t ntotal, uint32_t itotal, uint32_t nbref)
    {
        soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, ntotal, itotal, nbref);

        /* change the following line by your code */
        //return binFillReferenceDataBlocks(ntotal, itotal, nbref);
        uint32_t nDataBlocks = ntotal - 1 - itotal/IPB - nbref; //número de blocos vazios
        if(nDataBlocks > 64) {
            uint32_t count = 1 + nbref + 64;

            for(int i=1; i<=nbref; i++){
                uint32_t block[RPB];
                if(i==nbref){
                    block[0] = NullReference;
                }
                else {
                    block[0] = i+1;
                }   
                for(int j = 1; j <= RPB; j++){
                    if(count > nDataBlocks + 1){
                        block[j] = NullReference;
                    }
                    else {
                        block[j] = count;
                        count++;
                    }
                }
                count--;
                soWriteRawBlock(i + 1 + itotal/IPB, &block);
                
            }
        }
    }
};

