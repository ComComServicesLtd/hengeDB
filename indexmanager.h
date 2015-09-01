#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H


#include "types.h"
#include "seriesindex.h"


class SeriesIndex;

typedef map<uint64_t,SeriesIndex*> Indexes;
typedef Indexes::iterator Indexes_Iterator;


class IndexManager
{
public:
    IndexManager();

    void flush();

    void load(uint64_t indexID);
    uint64_t create(uint64_t indexID = 0,
                    uint32_t ticksPerSecond = index_default_timestamp_ticks,
                    uint32_t timestampSize = index_default_timestamp_size,
                    uint32_t retentionPolicy = index_default_timestamp_retention_policy,
                    uint64_t retentionValue = index_default_timestamp_retention_value,
                    uint64_t owner = 0);

    void remove(uint64_t indexID);

    ~IndexManager();


//     IndexID  Index Object
    Indexes indexes;


};

#endif // INDEXMANAGER_H
