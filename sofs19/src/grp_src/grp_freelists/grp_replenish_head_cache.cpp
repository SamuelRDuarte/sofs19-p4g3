/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2019
 */

#include "grp_freelists.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs19
{
    void grpReplenishHeadCache(void)
    {
        soProbe(443, "%s()\n", __FUNCTION__);

        /* change the following line by your code */
        //binReplenishHeadCache();
        //
        SOSuperBlock *sb = soGetSuperBlockPointer();
        
        if(sb->dz_free == 0)
            throw SOException(ENOSPC, __FUNCTION__);
        if(sb->head_cache.idx != HEAD_CACHE_SIZE)
             return;

        if(sb->dz_free ==sb->tail_cache.idx) {
            if(HEAD_CACHE_SIZE >= sb->tail_cache.idx) {
                uint32_t cnt = sb->tail_cache.idx;
                uint32_t idx = HEAD_CACHE_SIZE - cnt;
                uint32_t nbytes = cnt * sizeof (uint32_t);
                uint32_t *addr1 = sb->head_cache.ref + idx;
                uint32_t *addr2 = sb->tail_cache.ref;
                memcpy(addr1,addr2,nbytes);
                memset(addr2,0xFF,nbytes);
                sb->head_cache.idx = idx;
                sb->tail_cache.idx = 0;
            }
            else {
                uint32_t cnt = sb->tail_cache.idx - HEAD_CACHE_SIZE;
                uint32_t nbytes1 = HEAD_CACHE_SIZE * sizeof (uint32_t);
                uint32_t nbytes2 = cnt * sizeof (uint32_t);
                uint32_t *addr1 = sb->head_cache.ref;
                uint32_t *addr2 = sb->tail_cache.ref;
                uint32_t *addr3 = sb->tail_cache.ref + HEAD_CACHE_SIZE;
                uint32_t *addr4 = sb->tail_cache.ref + cnt;
                memcpy(addr1, addr2, nbytes1);
                memcpy(addr2,addr3, nbytes2);
                memset(addr4, 0xFF, nbytes1);
                sb->head_cache.idx = 0;
                sb->tail_cache.idx = cnt; 
            }
            soSaveSuperBlock();
            return;
        }
        else {
            uint32_t head_blk = sb->head_blk;;
            uint32_t head_idx = sb->head_idx;;

            uint32_t ref[RPB];
            soReadDataBlock(head_blk, ref);
            uint32_t cnt = sb->dz_free - sb->tail_cache.idx;
            if(cnt > RPB - head_idx)
                cnt = RPB - head_idx;
            if(cnt > HEAD_CACHE_SIZE)
                cnt = HEAD_CACHE_SIZE;

            uint32_t cidx = HEAD_CACHE_SIZE - cnt;

            for (uint32_t i = 0; i < cnt; i++) {
                sb->head_cache.ref[cidx++] = ref[head_idx];
                ref[head_idx++] = NullReference;
            }
            sb->head_cache.idx = HEAD_CACHE_SIZE - cnt;

            if (ref[0] == NullReference) {
                if (head_idx == sb->tail_idx) {
                    sb->head_blk = sb->tail_blk = NullReference;
                    sb->head_idx = sb->tail_idx = NullReference;
                    ref[0] =  NullReference;
                }
                else {
                    sb->head_idx += cnt;
                }
                
            }
            else {
                sb->head_idx += cnt;
                if(sb->head_idx == RPB) {
                    sb->head_blk = ref[0];
                    sb->head_idx = 1;
                }
            }
        
            if( sb->head_blk != head_blk) {
                soFreeDataBlock(head_blk);
            }
            else {
                soWriteDataBlock(head_blk, ref);
            }
        soSaveSuperBlock();
        }
    }
};

