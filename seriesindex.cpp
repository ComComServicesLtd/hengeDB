#include "seriesindex.h"
#include <memory.h>

#include <dirent.h>

SeriesIndex::SeriesIndex(uint64_t indexID, IndexManager *parent)
{
    id = indexID;
    manager = parent;


    if(!this->init()){
     shutdown();
    }

    allocateMemory();

    if(!loadSeries()){
        shutdown();
    }

}

void SeriesIndex::shutdown(){

    manager->indexes.erase(this->id);
    printf("\tRemoving index\n\n");


    for(auto it : series) // Deconstruct each time series data element
        delete it.second;


    free(buffer);
    delete this;
}

bool SeriesIndex::init(){


    getIndexName();

    printf("Loading index %lu from %s\n", this->id,filename);

    file=fopen(filename, "rb+");
    if(file != NULL){
        size = fread(&this->header,sizeof(this->header),1,file);

        fseek(file,0,SEEK_END); // Move to the end of file
        this->filePoint = (ftell(file) - sizeof(indexHeader))/this->header.timestampSize;

    } else {
        printf("Series Index: Failed to open file\n");
        return false;
    }

    if(size != 1){
        printf("Series Index: Invalid file, incomplete read\n");
        fclose(file);
        return false;
    }


    if(!checkFile()){
        printf("\tSeries Data: Could not recover invalid file\n");
        fclose(file);
        return false;
    }

    fclose(file);


    printf("Index %lu loaded\n",this->id);
    printf("\tPoints: %lu\n",this->filePoint);
    printf("\tOwner: %lu\n",header.owner_id);
    printf("\tTicksPerSecond: %d\n",header.ticksPerSecond);
    printf("\tTimestamp Size: %d\n",header.timestampSize);
    printf("\tRetention Policy: %d\n",header.retentionPolicy);
    printf("\tRetention Value: %lu\n\n\n",header.retentionValue);


    this->indexPoints = DEFAULT_BUFFER_POINTS;


    return true;
}

bool SeriesIndex::loadSeries(){

    /// LOAD ALL SERIES DATA OBJECTS

    printf("SeriesIndex :: Loading data files\n");


    getIndexFolder();


    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (filename)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        uint64_t seriesID = strtoul (ent->d_name, NULL, 0);
        if(seriesID > 0){
        this->load(seriesID);
        }
      }
      closedir(dir);
    } else {
    printf("Series Index: Invalid Folder, Could not open folder\n");
    return false;
    }

    return true;

}

bool SeriesIndex::checkFile(){
    return true; /// TODO!! CHECKING / CORRECTION FOR INDEX FILES
}


void SeriesIndex::allocateMemory(){
    buffer = (char*)realloc(buffer,index_buffer_size); // Allocate the buffer in memory

    if(buffer == NULL){
        printf("\SeriesIndex::allocateMemory() Memory Allocation Failed");
        //manager->remove(this->id);
        //return false;
        shutdown();
    }
}

void SeriesIndex::commit(uint32_t* time){ // sets the timestamp for the current position and moves the index to the next time slot allocating additional memory if neccessary
    if(this->indexPoint < this->indexPoints){
    // printf("SeriesIndex:: Buffer Used = %lu, index piont = %lu, timestamp size = %lu\n",index_buffer_used,indexPoint,header.timestampSize);
     memcpy(buffer + index_buffer_used,time,header.timestampSize);
     this->indexPoint += 1;

    } else if(this->indexPoints < MAX_BUFFER_POINTS) { // Can allocate more memory
        this->indexPoints += DEFAULT_BUFFER_POINTS;
        allocateMemory();
        for(auto it : series) // Reallocate memory for all child series
            it.second->allocateMemory();

    } else { // Cannot allocate more memory, must flush

        this->flush();
    }

}





bool SeriesIndex::flush(){

    // Flush each index first
    getIndexName();


    file=fopen(filename, "a+b");
    if(file != NULL){
        size = fwrite(buffer, header.timestampSize,this->indexPoint, file);
        fclose(file);
        if(size != this->indexPoint){
            printf("SeriesIndex::flush() Failed on series %lu, Partial Data Write, Full Disk? RET = %lu, EXP = %lu\n",this->id,size,this->indexPoint);
            //return false;
            shutdown();
        }
   } else {
        printf("Could not open file for flushing\n");
        shutdown();
        //return false;
   }



    // Flush eash series
    for(auto it : series)
        it.second->flush();

    filePoint += indexPoint;
    this->indexPoint = 0;

    return true;
}

void SeriesIndex::load(uint64_t seriesID){
    if(series[seriesID] == NULL){
        series[seriesID] = new SeriesData(seriesID,this);
    } else {
        printf("SeriesIndex::load Could not create series, seriesID: %lu already loaded\n",seriesID);
    }
}

uint64_t SeriesIndex::create(uint64_t seriesID,uint16_t format) {

    if(series[seriesID] != NULL){
        printf("SeriesIndex::create Series %lu already exists\n",seriesID);
        return 0;
    }


    size_t size;
    char filename[FILENAME_SIZE];
    seriesHeader newSeriesHeader;


    sprintf(filename,FS_FORMAT,this->id,seriesID);

    newSeriesHeader.version = series_version;
    newSeriesHeader.indexID=this->id;
    newSeriesHeader.valueFormat = format;
    newSeriesHeader.valueSize = FORMAT_SIZE[format];
    newSeriesHeader.indexOffset = this->filePoint;



    FILE *file=fopen(filename, "wbax");

    if(file != NULL){
        size = fwrite(&newSeriesHeader,sizeof(seriesHeader),1,file);
        fclose(file);





        if(size == 1){

            load(seriesID); // Load the newly created series
            return seriesID;

        } else {

            printf("SeriesIndex::create Incomplete write detected, failed to create series %lu\n",seriesID);
            return 0;

        }


    } else {
        printf("Could not create file for new series\n");
        return 0;
    }


}

void SeriesIndex::unload(uint64_t seriesID){
    printf("SeriesIndex:: Unloading Series %lu:%lu\n",this->id,seriesID);

    if( series[seriesID] != NULL){
        delete series[seriesID];
        series.erase(seriesID);
    }
}

SeriesIndex::~SeriesIndex()
{
  //  flush(); // flush the index and all series belonging to it

    for(auto it : series) // Deconstruct each time series data element
        delete it.second;


    free(buffer);
}

