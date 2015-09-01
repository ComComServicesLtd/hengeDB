#include "seriesmanager.h"

#include "types.h"

seriesmanager::seriesmanager()
{

}


// Create a series,
// id [IN] of series to create
// indexID[IN] optionalally specify an already existing index to use for this series
//int seriesmanager::create(uint64_t id, uint64_t indexID){

//    if index file is not specfied
//        create a new index file starting at the highest free index number

//    if index file is specified
//        get offset, take index points, ((index file size – header size) / timestamp size in bytes)
//        create series with index id, offset, format and size



//}

// Create an index and all it's child series to the disk
// [IN] id of index, optional, if unspecified all indexes will be flushed
//int seriesmanager::flush(uint64_t index = 0){


//    for(int idx = 0; idx < indexCount; idx++){ // loop through each index file
//    write indexes to index file                // append the additional index information to the file

//    for(int ser = 0; ser < index[idx].seriesCount;ser++){ // loop through each file for 'idx' index

//    ensure data points to be written to file do not exceed the number of index points written, trim if neccessary or fill with void points

//    write data points to time series file


//}



seriesmanager::~seriesmanager()
{

}

