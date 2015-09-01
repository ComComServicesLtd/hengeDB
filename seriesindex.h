#ifndef SERIESINDEX_H
#define SERIESINDEX_H


#include "types.h"
#include "seriesdata.h"
#include "indexmanager.h"

class SeriesData;
class SeriesIndex;
class IndexManager;


typedef map<uint64_t,SeriesData*> Series;
typedef Series::iterator Series_Iterator;




#define getIndexName() char filename[FILENAME_SIZE]; sprintf(filename,FS_FORMAT_INDEX_FILE,this->id); size_t size;
#define getIndexFolder() char filename[FILENAME_SIZE]; sprintf(filename,FS_FORMAT_INDEX_FOLDER,this->id); size_t size;

#define index_buffer_size (indexPoints*header.timestampSize)
#define index_buffer_used (indexPoint*header.timestampSize)



class SeriesIndex
{
public:
    SeriesIndex(uint64_t indexID, IndexManager *parent);
    bool init();
    bool loadSeries();
    bool checkFile();

    uint64_t create(uint64_t seriesID = 0,uint16_t format = t_float32); // create a series, if series id is blank an id is automatically assigned and returnd
    void allocateMemory();
    void unload(uint64_t seriesID);
    bool flush();
    void shutdown();

    void commit(uint32_t *time);

    ~SeriesIndex();

    uint64_t id;

    uint64_t indexPoint = 0;
    uint64_t indexPoints = 0;

    uint64_t filePoint = 0;

    char* buffer = NULL;
        Series series; // data series for this index

private:
    void load(uint64_t seriesID);





    indexHeader header;
    FILE *file;

    IndexManager *manager;



};






#endif // SERIESINDEX_H



