#pragma once

typedef void (*ITERATEFILECALLBACK)(const char* filename, void* args);

bool iterateFiles(const char* dirname, const char* ext, ITERATEFILECALLBACK callback, void* args);

void getFileNameNoExtensionA(const char* filename, char* outfilename, size_t size );

void getFileExtensionA(const char* filename, char* outfilename, size_t size );
