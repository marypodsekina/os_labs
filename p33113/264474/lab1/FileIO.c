#include "FileIO.h"

void *FileWriterThread(void *rawParams) {

    struct FileWriterThreadParams *params = (struct FileWriterThreadParams *) rawParams;
    union IOBlock block;
    void *address = params->startAddress;
    FILE *file;
    int random_offset;
    char *file_name = malloc(256);
    snprintf(file_name, 256, "labfile_%d", params->file_id);
    printf("\n%s\r", file_name);

    memset(block.raw, 0, blockSize);

    while (*params->infinityLoop) {

        address = params->startAddress;
        pthread_mutex_lock(params->fileMutex);

        file = fopen(file_name, "wb");
        for (size_t i = 0; i < params->blocksCount; ++i) {

            random_offset = (rand()%params->blocksCount)*blockNumbersSize;
            memcpy(block.numbers, address + random_offset, blockNumbersSize);
            fwrite(block.raw, sizeof(char), blockSize, file);

        }

        fclose(file);
        pthread_mutex_unlock(params->fileMutex);

    }
    return NULL;
}

void *FileReaderThread(void *rawParams) {

    struct FileReaderThreadParams *params = (struct FileReaderThreadParams *) rawParams;
    union IOBlock block;
    FILE *file;
    char *file_name;
    int min = INT_MAX;

    while (*params->infinityLoop) {

        for (size_t i = 0; i < params->filesCount; ++i) {
            params->sum = 0;
            file_name = malloc(256);

            snprintf(file_name, 256, "labfile_%d", params->fileWriterThreadParams[i].file_id);

            pthread_mutex_lock(params->fileWriterThreadParams[i].fileMutex);
            file = fopen(file_name, "rb");
//            memset(block.raw, 0, blockSize);

            for (size_t j = 0; j < params->fileWriterThreadParams[i].blocksCount; ++j) {

                fread(block.raw, sizeof(char), blockSize, file);

                for (size_t k = 0; k < sizeof(block.numbers) / sizeof(int); ++k) {

                    params->sum += block.numbers[k];

                }

            }
            fclose(file);
            pthread_mutex_unlock(params->fileWriterThreadParams[i].fileMutex);
        }
    }
    return NULL;
}
