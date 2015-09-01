#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <map>
#include <memory>
#include <stdio.h>

using namespace std;

// HOW IT WORKS :)

// Class IndexManager manages all the indexes (Classs SeriesIndex)

// Each index manages it's list of data series (Class SeriesData)

// When a write is performed the series manager routes the write to the correct index which takes it from there

// When an index receives a write request on a Data Series it passes it to the relevant data series object which performs the write

// Writes are performed freely but must be kept in sync with the index which means if a series is


#define t_NC  0 // INVLAID ? NON IMPLEMENTED

#define t_uint8 1
#define t_int8  2

#define t_uint16  3
#define t_int16   4

#define t_uint32  5
#define t_int32   6

#define t_uint64  7
#define t_int64   8

#define t_float32  9
#define t_float64  10


// RESERVED CHARACTERS ARE FOR NULL DATA PLACEHOLDERS,
// If a data point for a givin series is not received before a commit happens the value will be filled with the appropriate nan fill for it's format
static uint8_t NULL_FILL[11][8] = {
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // NOT IMPLEMENTED

    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // 8 bit unsigned int, cannot use 255
    {0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // 8 bit signed int, cannot use 127

    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //16 bit unsigned int, cannot use 65535
    {0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //16 bit signed int, cannot use 32767

    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //32 bit unsigned int, cannot use 4294967295
    {0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //32 bit signed int, cannot use 2147483647

    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //64 bit unsigned int, cannot use 4294967295
    {0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, //64 bit signed int, cannot use 2147483647

    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // IEE32 Floating Point, cannot use NAN
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} // IEE64 Double Floating Point, cannot use NAN
};

static uint8_t FORMAT_SIZE[11] = {
    1, // Invalid
    1, // uint8_t
    1, // int8_t
    2, // uint16_t
    2, // int16_t
    4, // uint32_t
    4, // int32_t
    8, // int64_t
    8, // uint64_t
    4, // IEE32 Float
    8  // IEE64 Float
};








// INDEX

// FILE STORAGE
#define FS_FORMAT "/dev/shm/test/%lu/%lu"
#define FS_FORMAT_INDEX_FOLDER "/dev/shm/test/%lu"
#define FS_FORMAT_INDEX_FILE "/dev/shm/test/%lu/index"
#define FILENAME_SIZE 256


// Standard unix timestamps with 1 second resolution
#define index_default_timestamp_size 4
#define index_default_timestamp_ticks 1


#define RP_INFINITE 0
#define RP_SECONDS 1
#define RP_POINTS 2

#define SAFE

// SERIES
#define DEFAULT_BUFFER_POINTS 1024 //Default buffer size is 1k bytes, if more points are received before flush is called the buffer will be allocated up to the max buffer size of 10M before requesting being individually flushed
#define MAX_BUFFER_POINTS 10000000 //Max 10M Point Buffer

#define series_default_value_format t_float32 // float
#define series_default_value_size 32  // float32
#define index_default_timestamp_retention_policy RP_INFINITE
#define index_default_timestamp_retention_value 0

#pragma pack(1)


#define series_version 1
struct seriesHeader {
    uint32_t version; // version number (unused)
    uint64_t indexID; // Index ID for series
    uint64_t indexOffset; // point offset from index
    uint32_t valueFormat; // Data Format
    uint32_t valueSize; // value size in bytes
    uint64_t checksum; // checksum of data (unused)
};

#define index_version 1
struct indexHeader {
    uint16_t version; // version number (unused)
    uint64_t owner_id; // database owner
    uint32_t ticksPerSecond; // number of ticks for 1 second, 1 = 1 second (standard unix timestamp)
    uint32_t timestampSize; //in bytes = 32 for unix timestamp (32 bit)
    uint32_t retentionPolicy; // 0 = infinite, 1 = delete after RetentionValue exceeded in seconds, 2=delete after retntionValue exceeded in points
    uint64_t retentionValue; // maximum
    uint64_t checksum; // checksum of data (unused)
};

#pragma push



#endif // TYPES_H

