#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "iff.h"

int iff_parse_str(IFFChunk * ChunkInfo, const uint8_t * Buffer){
    /* No bounds checking yet */
    IFF_STR * StringData;
    unsigned Size = ChunkInfo->Size - 76;
    if(Size < 2)
        return 0;
    ChunkInfo->FormattedData = malloc(sizeof(IFF_STR));
    if(ChunkInfo->FormattedData == NULL)
        return 0;
    memset(ChunkInfo->FormattedData, 0, sizeof(IFF_STR));
    StringData = (IFF_STR*) ChunkInfo->FormattedData;
    StringData->Format = read_int16le(Buffer);
    Buffer += 2;
    if(Size-2 < 2) /* TSO allows this; as seen in the animations chunk in personglobals.iff */
        return 1;
    
    switch(StringData->Format){
    
    case 0: {
        unsigned i;
        IFFStringPairNode * PrevPair = NULL;
        
        StringData->LanguageSets[0].PairCount = read_uint16le(Buffer);
        Buffer += 2;
        if(StringData->LanguageSets[0].PairCount == 0)
            return 1;
        
        for(i=0; i<StringData->LanguageSets[0].PairCount; i++){
            IFFStringPairNode * CurrentPair;
            unsigned length;
            CurrentPair = malloc(sizeof(IFFStringPairNode));
            memset(CurrentPair, 0, sizeof(IFFStringPairNode));
            
            if(i == 0) StringData->LanguageSets[0].FirstPair = CurrentPair;
            else       PrevPair->NextPair = CurrentPair;
            CurrentPair->PrevPair = PrevPair;

            /* Key */
            length = read_uint8le(Buffer);
            if(length != 0){
                CurrentPair->Pair.Key = malloc(length+1);
                memcpy(CurrentPair->Pair.Key, Buffer+1, length);
                CurrentPair->Pair.Key[length] = 0x00;
            }
            Buffer += length+1;
            
            PrevPair = CurrentPair;
        }
        StringData->LanguageSets[0].LastPair = PrevPair;
    } return 1;

    case -1: {
        unsigned i;
        IFFStringPairNode * PrevPair = NULL;
        
        StringData->LanguageSets[0].PairCount = read_uint16le(Buffer);
        Buffer += 2;
        if(StringData->LanguageSets[0].PairCount == 0)
            return 1;
        
        for(i=0; i<StringData->LanguageSets[0].PairCount; i++){
            IFFStringPairNode * CurrentPair;
            unsigned length;
            CurrentPair = malloc(sizeof(IFFStringPairNode));
            memset(CurrentPair, 0, sizeof(IFFStringPairNode));
            
            if(i == 0) StringData->LanguageSets[0].FirstPair = CurrentPair;
            else       PrevPair->NextPair = CurrentPair;
            CurrentPair->PrevPair = PrevPair;

            /* Key */
            length = strlen((char*)Buffer);
            if(length != 0){
                CurrentPair->Pair.Key = malloc(length+1);
                strcpy(CurrentPair->Pair.Key, (char*)Buffer);
            }
            Buffer += length+1;
            
            PrevPair = CurrentPair;
        }
        StringData->LanguageSets[0].LastPair = PrevPair;
    } return 1;

    case -2: {
        unsigned i;
        IFFStringPairNode * PrevPair = NULL;
        
        StringData->LanguageSets[0].PairCount = read_uint16le(Buffer);
        Buffer += 2;
        if(StringData->LanguageSets[0].PairCount == 0)
            return 1;
        
        for(i=0; i<StringData->LanguageSets[0].PairCount; i++){
            IFFStringPairNode * CurrentPair;
            unsigned length;
            CurrentPair = malloc(sizeof(IFFStringPairNode));
            memset(CurrentPair, 0, sizeof(IFFStringPairNode));
            
            if(i == 0) StringData->LanguageSets[0].FirstPair = CurrentPair;
            else       PrevPair->NextPair = CurrentPair;
            CurrentPair->PrevPair = PrevPair;

            /* Key */
            length = strlen((char*)Buffer);
            if(length != 0){
                CurrentPair->Pair.Key = malloc(length+1);
                strcpy(CurrentPair->Pair.Key, (char*)Buffer);
            }
            Buffer += length+1;

            /* Value */
            length = strlen((char*)Buffer);
            if(length != 0){
                CurrentPair->Pair.Value = malloc(length+1);
                strcpy(CurrentPair->Pair.Value, (char*)Buffer);
            }
            Buffer += length+1;
            
            PrevPair = CurrentPair;
        }
        StringData->LanguageSets[0].LastPair = PrevPair;
    } return 1;

    case -3: {
        unsigned i, TotalPairCount;
        
        TotalPairCount = read_uint16le(Buffer);
        Buffer += 2;
        if(TotalPairCount == 0)
            return 1;

        for(i=0; i<TotalPairCount; i++){
            IFFStringPairNode * Pair;
            unsigned length;
            Pair = malloc(sizeof(IFFStringPairNode));
            memset(Pair, 0, sizeof(IFFStringPairNode));
            Pair->Pair.LanguageSet = read_uint8le(Buffer) - 1;
            Buffer++;
            
            /* Key */
            length = strlen((char*)Buffer);
            if(length != 0){
                Pair->Pair.Key = malloc(length+1);
                strcpy(Pair->Pair.Key, (char*)Buffer);
            }
            Buffer += length+1;

            /* Value */
            length = strlen((char*)Buffer);
            if(length != 0){
                Pair->Pair.Value = malloc(length+1);
                strcpy(Pair->Pair.Value, (char*)Buffer);
            }
            Buffer += length+1;
            
            /* Add the pair to the end of the associated language set */
            Pair->PrevPair = StringData->LanguageSets[0].LastPair;
            if(StringData->LanguageSets[0].PairCount == 0)
                StringData->LanguageSets[0].FirstPair = Pair;
            else
                StringData->LanguageSets[0].LastPair->NextPair = Pair;
            StringData->LanguageSets[0].PairCount++;
            StringData->LanguageSets[0].LastPair = Pair;
        }
    } return 1;

    case -4: {
        unsigned LanguageSet;
        unsigned LanguageSetCount = read_uint8le(Buffer);
        Buffer++;
        if(LanguageSetCount > 20) LanguageSetCount = 20;
        
        for(LanguageSet=0; LanguageSet<LanguageSetCount; LanguageSet++){
            unsigned i;
            IFFStringPairNode * PrevPair = NULL;
            
            StringData->LanguageSets[LanguageSet].PairCount = read_uint16le(Buffer);
            Buffer += 2;
            if(StringData->LanguageSets[LanguageSet].PairCount == 0)
                continue;
            
            for(i=0; i<StringData->LanguageSets[LanguageSet].PairCount; i++){
                IFFStringPairNode * CurrentPair;
                unsigned length;
                CurrentPair = malloc(sizeof(IFFStringPairNode));
                memset(CurrentPair, 0, sizeof(IFFStringPairNode));
                
                if(i == 0) StringData->LanguageSets[LanguageSet].FirstPair = CurrentPair;
                else       PrevPair->NextPair = CurrentPair;
                CurrentPair->PrevPair = PrevPair;
                
                Buffer++; /* Skip over LanguageSet */

                /* Key */
                length = read_uint8le(Buffer);
                if(length > 127){
                    length = (length & 127) | (read_uint8le(Buffer+1) << 7);
                    Buffer++;
                }
                if(length != 0){
                    CurrentPair->Pair.Key = malloc(length+1);
                    memcpy(CurrentPair->Pair.Key, Buffer+1, length);
                    CurrentPair->Pair.Key[length] = 0x00;
                }
                Buffer += length + 1;

                /* Value */
                length = read_uint8le(Buffer);
                if(length > 127){
                    length = (length & 127) | (read_uint8le(Buffer+1) << 7);
                    Buffer++;
                }
                if(length != 0){
                    CurrentPair->Pair.Value = malloc(length+1);
                    memcpy(CurrentPair->Pair.Value, Buffer+1, length);
                    CurrentPair->Pair.Value[length] = 0x00;
                }
                Buffer += length + 1;
                
                PrevPair = CurrentPair;
            }
            StringData->LanguageSets[LanguageSet].LastPair = PrevPair;
        }
    } return 1;
    
    }
    return 0;
}