#include "grp_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

#include <iostream>

namespace sofs19
{
    static uint32_t grpAllocIndirectFileBlock(SOInode * ip, uint32_t afbn);
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn);

    /* ********************************************************* */

    uint32_t grpAllocFileBlock(int ih, uint32_t fbn)
    {
        soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

        SOInode* ip = soGetInodePointer(ih);
        uint32_t doubleIndirectStart = N_INDIRECT * RPB + N_DIRECT;
        uint32_t doubleIndirectEnd = RPB * RPB * N_DOUBLE_INDIRECT + doubleIndirectStart;
        uint32_t allBlock;

        // exit condition - invalid fbn
        if (fbn < 0 || fbn >= doubleIndirectEnd)
            throw SOException(EINVAL, __FUNCTION__);

        if (fbn < N_DIRECT) {
            allBlock = soAllocDataBlock();
            ip->d[fbn] = allBlock ;
            ip->blkcnt += 1;
        }

        if (fbn >= N_DIRECT && fbn < doubleIndirectStart)
            allBlock = grpAllocIndirectFileBlock(ip, fbn - N_DIRECT);

        if (fbn >= doubleIndirectStart && fbn < doubleIndirectEnd)
            allBlock = grpAllocDoubleIndirectFileBlock(ip, fbn - doubleIndirectStart);

        soSaveInode(ih);
        return allBlock;
        
        /* change the following two lines by your code */
        //return binAllocFileBlock(ih, fbn);
    }

    /* ********************************************************* */
    /*
    */
    static uint32_t grpAllocIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

        // calculate indirect list index
        uint32_t i1index = (afbn / RPB); //% RPB;

        // calculate direct list index
        uint32_t ref = afbn % RPB; // ref inclusive

        // data block frame
        uint32_t db[RPB];
        uint32_t allBlock;

        // indirect list index is empty
        if (ip->i1[i1index] == NullReference) {

            // create direct block and alloc it to indirect position
            allBlock = soAllocDataBlock();
            for (uint32_t i = 0; i < RPB; i++) {
                db[i] = NullReference;
            }
            soWriteDataBlock(allBlock, db);
            // read direct block
            ip->i1[i1index] = allBlock;
            soReadDataBlock(ip->i1[i1index], &db);

            allBlock = soAllocDataBlock();
            db[ref] = allBlock;
            soWriteDataBlock(ip->i1[i1index], db);
            ip->blkcnt += 2;
        }
        else {

            soReadDataBlock(ip->i1[i1index], &db);
            allBlock = soAllocDataBlock();
            db[ref] = allBlock;
            soWriteDataBlock(ip->i1[i1index], db);
            ip->blkcnt += 1;
        }

        return allBlock;
        
        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;
    }

    /* ********************************************************* */
    /*
    */
    static uint32_t grpAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
    {
        soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

        uint32_t i2index = afbn / RPB / RPB;
        uint32_t db[RPB];
        uint32_t allBlock;

        // double indirect list index is empty
        if (ip->i2[i2index] == NullReference) {

            // create indirect block and alloc it to double indirect position
            allBlock = soAllocDataBlock();
            for (uint32_t i = 0; i < RPB; i++) {
                db[i] = NullReference;
            }
            soWriteDataBlock(allBlock,db);
            // read indirect block
            ip->i2[i2index] = allBlock;
            soReadDataBlock(ip->i2[i2index], &db);
            
            allBlock = grpAllocIndirectFileBlock(ip, afbn);
            soWriteDataBlock(allBlock, db);
            ip->blkcnt += 1;
        }
        else {

            soReadDataBlock(ip->i2[i2index], &db);
            allBlock = grpAllocIndirectFileBlock(ip, afbn);
            soWriteDataBlock(allBlock, db);
        }

        return allBlock;

        /* change the following two lines by your code */
        //throw SOException(ENOSYS, __FUNCTION__); 
        //return 0;
    }
};

