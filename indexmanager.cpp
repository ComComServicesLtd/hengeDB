#include "indexmanager.h"
#include "types.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <chrono>
#include <iostream>



IndexManager::IndexManager()
{

    auto start = std::chrono::high_resolution_clock::now();

this->create(12345); // create index

load(12345);


indexes[12345]->create(1); // create series
indexes[12345]->create(2);
indexes[12345]->create(3);
indexes[12345]->create(4);
indexes[12345]->create(5);
indexes[12345]->create(6);
indexes[12345]->create(7);
indexes[12345]->create(8);
indexes[12345]->create(9);
indexes[12345]->create(10);





if(indexes[12345] != NULL){
   printf("Loaded Successfully\n");

   float value = 12.112;

   for(uint32_t i = 0; i < 10000000;i++){

       for(int s = 1; s <= 10; s++){
      // if(indexes[12345]->series[s] != NULL){
       indexes[12345]->series[s]->append(&value);
      // }
       }
      // printf("Closing Index with time %d\n",i);
       indexes[12345]->commit(&i);
      //  printf("...\n");

   }



   auto finish = std::chrono::high_resolution_clock::now();

   float time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() / 1000000000;

   std::cout << "Wrote 100M Points in " <<  time << "seconds\n";
   std::cout.flush();
 printf("Finished Successfully\n");

indexes[12345]->flush();

}


}

void IndexManager::flush(){

      for(auto it : indexes)
          it.second->flush();
}



void IndexManager::load(uint64_t indexID){
    if(indexes[indexID] == NULL){
    indexes[indexID] = new SeriesIndex(indexID,this);  //   indexes.insert(make_pair(indexID, new SeriesIndex(indexID,this)));
    } else {
        printf("IndexManager::load Could not create index, indexID: %lu already taken\n",indexID);
    }
}



uint64_t IndexManager::create(uint64_t indexID,
                              uint32_t ticksPerSecond,
                              uint32_t timestampSize,
                              uint32_t retentionPolicy,
                              uint64_t retentionValue,
                              uint64_t owner){



    if(indexes[indexID] != NULL){
         printf("IndexManager::create Index %lu already exists\n",indexID);
    }


      size_t size;
      char filename[FILENAME_SIZE];
      indexHeader newIndexHeader;

      newIndexHeader.version = index_version;
      newIndexHeader.ticksPerSecond = ticksPerSecond;
      newIndexHeader.timestampSize = timestampSize;
      newIndexHeader.retentionPolicy = retentionPolicy;
      newIndexHeader.retentionValue = retentionValue;
      newIndexHeader.owner_id = owner;


      sprintf(filename,FS_FORMAT_INDEX_FOLDER,indexID);

      struct stat st = {0};

      if (stat(filename, &st) == -1) {
          int mkr = mkdir(filename, 0700);
          if(mkr != 0){
               printf("IndexManager::create Could not create index folder '%s' \n\tmkdir returned %d\n",mkr);
               return 0;
          }
      } else {
          printf("IndexManager::create Index Folder '%s' already exists\n",filename);
      }


      sprintf(filename,FS_FORMAT_INDEX_FILE,indexID);

      FILE *file=fopen(filename, "wbax");

      if(file != NULL){
      size = fwrite(&newIndexHeader,sizeof(indexHeader),1,file);
      fclose(file);


      if(size == 1){

          load(indexID); // Load the newly created series
          return indexID;

      } else {

          printf("IndexManager::create Incomplete write detected, failed to create index %lu\n",indexID);
          return 0;
      }


      } else {
         printf("Could not create index\n");
         return 0;
      }

}


void IndexManager::remove(uint64_t indexID){
    printf("IndexManager:: Removing Index %lu\n",indexID);

    if( indexes[indexID] != NULL){
        delete indexes[indexID];
        indexes.erase(indexID);
    }
}



IndexManager::~IndexManager()
{
    // Deconstruct all the child classes
      for (Indexes_Iterator it=indexes.begin(); it!=indexes.end(); ++it)
        delete it->second;
}

