/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace sofs19
{
    /* only fill the current block to its end */
    void grpDepleteTailCache(void)
    {
        soProbe(444, "%s()\n", __FUNCTION__);

        /* change the following line by your code */
        SOSuperBlock *sbp = soGetSuperBlockPointer();

        if(sbp -> dz_free == 0)
            throw SOException(ENOSPC, __FUNCTION__);
        if (sbp -> tail_cache.idx != TAIL_CACHE_SIZE)
            return;

        uint32_t block = sbp -> tail_idx / RPB ;
        uint32_t block_used_refs = sbp -> tail_idx % RPB;
        uint32_t *bp;
        soReadDataBlock(block, bp);
        uint32_t block_free_refs;

        if(block == sbp -> head_idx / RPB ) {
            if (sbp->head_idx % RPB > sbp->tail_idx % RPB) {
                block_free_refs = (sbp->head_idx % RPB) - (sbp->tail_idx % RPB);
            } else {
                block_free_refs = RPB - block_used_refs;
            }
        } else{
            block_free_refs = RPB - block_used_refs;
        }

        if(sbp -> tail_cache.idx > block_free_refs){
            memcpy(&(bp[block_used_refs]), &(sbp -> tail_cache), block_free_refs * sizeof(uint32_t));

            if(sbp -> dz_free - 1 > block) {
                sbp->tail_idx += block_free_refs;
            } else{
                sbp -> tail_idx = 0;
            }

            memcpy(&(sbp -> tail_cache), &(sbp -> tail_cache.ref[block_free_refs]), ((sbp -> tail_cache.idx) - block_free_refs) * sizeof(uint32_t));
            sbp -> tail_cache.idx -= block_free_refs ;

            for(uint32_t i = 0 ; i < block_free_refs ; ++i ){
                sbp -> tail_cache.ref[(sbp -> tail_cache.idx) + i] = NullReference;
            }
        } else{
            memcpy(&(bp[block_used_refs]), &(sbp -> tail_cache), (sbp -> tail_cache.idx) * sizeof(uint32_t));
            sbp -> tail_idx += sbp -> tail_cache.idx;
            sbp -> tail_cache.idx = 0;

            for( uint32_t i = 0 ; i < TAIL_CACHE_SIZE ; ++i ){
                sbp -> tail_cache.ref[i] = NullReference;
            }
        }

        soWriteDataBlock(block, bp);
        soSaveSuperBlock();
    }
};

