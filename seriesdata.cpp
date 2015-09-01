#include "seriesdata.h"
#include <memory.h>


SeriesData::SeriesData(uint64_t seriesID, SeriesIndex *parent)
{
    series = parent;
    id = seriesID;


    if(!this->init()){
        shutdown();
    }

    allocateMemory();
}

void SeriesData::shutdown(){
    series->series.erase(this->id);
    printf("\tRemoving series\n\n");
    free(buffer);
    delete this;

}

bool SeriesData::init(){

    getSeriesName();

    printf("Loading series %lu from %s\n",this->id,filename);

    file=fopen(filename, "rb+");
    if(file != NULL){
        size = fread(&this->header,sizeof(this->header),1,file);
    } else {
        printf("\tSeries Data: Could not read open file");
        return false;
    }

    if(size != 1){
         printf("\tSeries Data:  Invalid file, incomplete header\n");
         fclose(file);
         return false;
    }



    if(!checkFile()){
        printf("\tSeries Data: Could not recover invalid file\n");
        fclose(file);
        return false;

    }

    fclose(file);

    printf("Series %lu loaded\n",this->id);
    printf("\tVersion %d\n",header.version);
    printf("\tindexID %d\n",header.indexID);
    printf("\tindexOffset %d\n",header.indexOffset);
    printf("\tvalueSize %d\n",header.valueSize);
    printf("\tvalueFormat %d\n\n\n",header.valueFormat);

    return true;

}


bool SeriesData::checkFile(){


        if(series->id != header.indexID){
         printf("checkFile() Failed, Invalid index ID or file in wrong directory\n");
         return false;
        }


        fseek(file, 0, SEEK_END);

        int64_t size = ftell(file);

        size -= sizeof(seriesHeader);




        if(size < 0){
            printf("checkFile() Failed, Invalid file size / Incomplete header\n");
            return false;
         }


        if(size == 0){ // header only, this is not necessarly an error but we need to make sure that the file is in sync with the index so we check and update if neccessary
            if(this->header.indexOffset != series->filePoint){

                printf("SeriesData::checkFile() Empty Series %lu Out of sync with index\n\tSeries Offset:%lu\n\tIndex Points:%lu\n\n",this->id,this->header.indexOffset,this->series->filePoint);
                fseek(file, 0, SEEK_SET); // Seek Begining

                this->header.indexOffset = series->filePoint;
                size = fwrite(&this->header,sizeof(seriesHeader),1,file); // overwrite header with updated information

                if(size != 1){
                    printf("checkFile() Failed to recover empty data file\n");
                    return false;
                }
            }
        }


        int remainder = (size % header.valueSize);


        if(remainder != 0){ // This would generally mean the the file system crashed mid write, we need to trim back to even increments of the value size field and then fill to the index position

            printf("SeriesData::checkFile() Series %lu is corrupt\n\tRemainder:%lu bytes\n\tData Size:%lu bytes\n\n",this->id,remainder,size);

            fseek(file,-remainder,SEEK_CUR);

            uint64_t points  = (size-remainder)/header.valueSize;


            /// 10,000 points in index, this file started @ 9000 points into index so indexoffset = 9,000
            /// we should have 10,000 - 9,000 points, = 1000,
            /// if we only have 500 points, we need to add 500
            ///
            printf("\tIndex Point: %lu\n\tindexOffset Points: %lu\n\n\n",this->series->filePoint,this->header.indexOffset);
            uint64_t targetPoints = this->series->filePoint - this->header.indexOffset;
            int64_t pointsToAdd = targetPoints - points;

            printf("\tTarget Points: %lu\n\tActual Points: %lu\n\tAdding %ld points\n\n\n",targetPoints,points,pointsToAdd);

            /// 1000 - 500 == 500
            ///

            if((pointsToAdd < 0 || pointsToAdd > 1000000) || remainder > header.valueSize){
                printf("checkFile() Failed to recover corrupt data file\n\n");
                return false;
            }


            for(uint64_t p = 0; p < pointsToAdd; p++){
                fwrite(NULL_FILL[header.valueFormat],header.valueSize,1,file); // Write null values for each point that's mising
            }

        }

        return true;


}


void SeriesData::allocateMemory(){
    buffer = (char*)realloc(buffer,series_buffer_size); // Allocate the buffer in memory

    if(buffer == NULL){
        printf("\tSeriesData::allocateMemory() Memory Allocation Failed\n");
        //series->remove(this->id);
        //return false;
        shutdown();
    }

    // Fill the new buffer space with Null values
    for(uint64_t i = series->indexPoint; i < series->indexPoints; i++){
        memcpy(buffer+(i*header.valueSize),NULL_FILL[header.valueFormat],header.valueSize);
    }
}


bool SeriesData::flush()
{

    // flush the memory buffer to file
    getSeriesName();

    printf("Flushing series data file %s\n",filename);
    file=fopen(filename, "a+b");
    if(file != NULL){
        size = fwrite(buffer, header.valueSize,series->indexPoint, file);
        fclose(file);

        if(size != series->indexPoint){
            printf("SeriesData::flush() on series %lu:%lu Failed, Partial Data Write, Full Disk?\n",series->id,this->id);
            //series->remove(this->id);
            //return false;
            shutdown();
        }


    } else {
        printf("SeriesData::flush() on series %lu:%lu Failed, Could not open file for flushing\n",series->id,this->id);
        //series->remove(this->id);
        //return false;
        shutdown();
    }


    // Fill the buffer with Null Values
    for(uint64_t i = 0; i < series->indexPoints; i++){
        memcpy(buffer+(i*header.valueSize),NULL_FILL[header.valueFormat],header.valueSize); // Fill each slot in the time series buffer with the appropriate null fill
    }

    return true;
}


void SeriesData::append(void *data){

#ifdef SAFE
    // See if our value is one of our values reserved for a null placeholder
    if(memcmp(data,NULL_FILL[header.valueFormat],header.valueSize) == 0) { // If yes
        printf("TODO: Unhandled requested for write of value reserved for NULL on series %lu\n",this->id);
    }
#endif

    // append the data to the file at the current index pos
    memcpy(&buffer[series_buffer_used],data,header.valueSize);
}


SeriesData::~SeriesData()
{
    free(buffer); // Free the buffer
}


// start of program will search database directory and will open an index for each
