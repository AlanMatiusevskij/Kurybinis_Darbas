#include "pyclink.h"
#include<iostream>

//Declaring pirvate functions and variables.
//Map is shortened for File Map Object.
namespace pyclink{
    LPCWSTR mapName = L"PyC-Link-directLink";
    std::string mapNameCharP = "PyC-Link-directLink";
    char *mapData;

    STARTUPINFOW start_info = {};
    PROCESS_INFORMATION process_info = {};

    std::thread thread;

    bool directLink(const char* path, const char* data, char* *returnedData, int maxCharNumb, bool threaded);
}

bool pyclink::communicate(const char* path, const char* data, char* *returnedData, int maxCharNumb, bool threaded){
    if(threaded){
        try{
            thread = std::thread(pyclink::directLink, path, data, returnedData, maxCharNumb, threaded);
        }
        catch(const std::system_error &error){
            return true;
        }
        return false;
    }
    return pyclink::directLink(path, data, returnedData, maxCharNumb, threaded);
}

bool pyclink::directLink(const char* path, const char* input, char* *returnedData, int maxCharNumb, bool threaded){
    //Create a file map object handle    
    HANDLE mapHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(char)*maxCharNumb, mapName);
    //TODO must be new and check for failure

    //Set arguments for python; path variable becomes a command;
    //Convert to char*
    path = pyclink::append_char_p(path, " ");
    path = pyclink::append_char_p(path, "char");
    path = pyclink::append_char_p(path, " ");
    path = pyclink::append_char_p(path, pyclink::int_to_string(maxCharNumb).c_str());
    path = pyclink::append_char_p(path, " ");
    path = pyclink::append_char_p(path, mapNameCharP.c_str());
    path = pyclink::append_char_p(path, " ");
    path = pyclink::append_char_p(path, input);
    //Convert to LPWSTR
    LPWSTR command = pyclink::charToLPWSTR(path);

    //Launch python
    if(!CreateProcessW(nullptr, command, nullptr, nullptr, 0, 0, nullptr, nullptr, &start_info, &process_info)){
        delete(path);
        delete[] command;
        return true;
    }

    //Handle allocated memory
    delete(path);
    delete[] command;

    //Wait for python to finish.
    WaitForSingleObject(process_info.hProcess, INFINITE);

    //Get data pointer form the file map object handle.
    mapData = (char*)MapViewOfFile(mapHandle, FILE_MAP_READ, 0, 0, 0);
    *returnedData = new char[strlen(mapData) + 1];
    memcpy_s(*returnedData, strlen(mapData)*sizeof(char) + 1, mapData, strlen(mapData)*sizeof(char) + 1);

    //Clean up
    UnmapViewOfFile(mapData);
    CloseHandle(mapHandle);
    if(threaded) thread.detach();
    
    return false;
}

wchar_t* pyclink::charToLPWSTR(const char* charArr){
    int length = MultiByteToWideChar(CP_UTF8, 0, charArr, -1, nullptr, 0);
    wchar_t* wideChar = new wchar_t[length];

    MultiByteToWideChar(CP_UTF8, 0, charArr, -1, wideChar, length);

    return wideChar;
}

char* pyclink::append_char_p(const char* array, const char* to_add){
    size_t length = strlen(array) + strlen(to_add);
    char* _return = new char[length + 1];

    strcpy(_return, array);
    for(int i = 0; i < strlen(to_add); i++)
        _return[i + strlen(array)] = to_add[i];
    _return[length] = '\0';

    return _return;        
}

std::string pyclink::int_to_string(int numb){
    std::string tmp, converted;
    while(numb > 0){
        tmp.push_back(48 + numb%10);
        numb/=10;
    }
    converted.resize(tmp.size());
    for(int i = tmp.size()-1; i >= 0; i--)
        converted[tmp.size()-1 - i] = tmp[i];
    return converted;
}

void pyclink::changeMapName(const char* newMapName){
    pyclink::mapNameCharP = newMapName;
    pyclink::mapName = pyclink::charToLPWSTR(newMapName);
    return;
}