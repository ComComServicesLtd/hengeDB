#ifndef SERIESDATA_H
#define SERIESDATA_H

#include "types.h"
#include "seriesindex.h"

#define getSeriesName() char filename[FILENAME_SIZE]; sprintf(filename,FS_FORMAT,series->id,this->id); size_t size;
#define series_buffer_size (series->indexPoints*header.valueSize)
#define series_buffer_used (series->indexPoint*header.valueSize)



class SeriesIndex;

class SeriesData {

public:
    SeriesData(uint64_t seriesID, SeriesIndex *parent);
    bool init();
    void shutdown();

    ~SeriesData();

    void append(void* data); // data size must match theseries data value size, e.g. 4 for a 32 byte float
    // returns 0 (success) or error code

    bool flush();

    void allocateMemory();

    bool checkFile();

    SeriesIndex *series;


    char* buffer = NULL;


    seriesHeader header;

private:
    FILE *file;
    uint64_t id;
};



#endif // SERIESDATA_H


//Index Number, timestamp
//seriesid value
//seriesid value


//battery voltage
//120v ac
//door contacts
//ac ct's
//one wire temp sensors
