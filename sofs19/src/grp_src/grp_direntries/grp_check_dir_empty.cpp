#include "../../../include/direntries.h"
//#include "direntries.h"


#include "../../../include/core.h"
#include "../../../include/dal.h"
#include "../../../include/fileblocks.h"
#include "../../../include/bin/bin_direntries.h"

/*
#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"
*/

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs19
{
    /*
    Check directory emptiness.
    The directory is traversed to verified if the only existing entries are "." and "..".

    Parameters
        [in]	ih	handler of the inode to be checked

    Remarks
        -Assumes ih is a valid inode handler of a directory
        -when calling a function of any layer, the version with prefix so is used.
    */
    bool grpCheckDirEmpty(int ih)
    {
        soProbe(205, "%s(%d)\n", __FUNCTION__, ih);

        /* change the following line by your code */
        //return binCheckDirEmpty(ih);

        //Reference to the node being handled
        SOInode* ip = soGetInodePointer(ih);            // pointer para o inode a verificar

        uint32_t numBlocks = (ip->size) / BlockSize;    // file size in bytes / block size in bytes = numero de blocos no file

        // sabemos que existe numBlocks guardados no ih, mas podem estar espalhados
        uint32_t blocosVistos = 0;                      // esta variavel conta os blocos não null que encontramos, haverá numBlocks destes  
        uint32_t blocosCount = 0;                       // esta variavel conta todos os blocos dentro do ih

        uint32_t bloco;
        SODirEntry buffer[DPB];                         // buffer com DPB entradas

        while(blocosVistos < numBlocks) {               // enquanto o numero de blocosVistos nao for igual a numBlocks significa que ainda falta algum por ver
            bloco = soGetFileBlock(ih,blocosCount);

            if(bloco != NullReference) {                    // a referencia nao está a null, ou seja, encontramos um dos blocos que queremos verificar
                soReadFileBlock(ih,blocosCount,buffer);

                for(uint32_t i=2; i<DPB; i++) {             // começamos apartir do i=2 (pq i=0 é . e i=1 é ..) a ver se tem alguma posição com conteudo
                    if(strcmp(buffer[i].name,"\0") != 0){   // se houver alguma posiçao que nao esteja vazia, retorna false pois o directory nao está empty
                        return false;
                    }
                }
                blocosVistos++;                             // blocosVistos são os blocos que não têm nullreference, ou seja, que ocupam espaço no file          
            }
            blocosCount++;
        }
        return true; 
    }
};

