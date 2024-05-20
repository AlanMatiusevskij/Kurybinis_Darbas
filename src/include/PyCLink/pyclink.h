#pragma once
#include<Windows.h>
#include<memoryapi.h>
#include<string>
#include<system_error>
#include<thread>

//Public variables
namespace pyclink{
    enum types{
        char_p,
    };

    /**
     * Function to send and receive `char` type data from the specified application programmed to receive and send data in the same way. The length can't exceed the `maxCharNumb` characters.
     * 
     * @param path is the path to the python application.
     * @param input is the char pointer data to send.
     * @param returnedData is a pointer to a char pointer type with the data returned by python.
     * @param maxCharNumb is the maximum number of characters that can be returned.
     * @param threaded `true` if the program should create a thread for sending and receiving data, `false` if not. Note that if it is set to `true`, the function will not return `true` on failure.
     * 
     * @return `true` on failure and `false` on success.
     */
    bool communicate(const char* path, const char* input, char** returnedData, int maxCharNumb, bool threaded);

    std::string int_to_string(int numb);

    /** TODO a memory leak?
     * Append char* to a given character array.
     * @param array the char* object to which `to_add` is appended.
     * @param to_add the data to append.
     * 
     * @return Returns a new char* with the appended characters.
    */
    char* append_char_p(const char* array, const char* to_add);

    /** TODO a memory leak?
     * Converts a char* to LPWSTR (wchar_t*) type.
     * @param charArr the pointer to a char array to convert.
     * 
     * @return returns converted array. 
    */
    wchar_t* charToLPWSTR(const char* charArr);

    /** TODO a memory leak?
     * Change the file map object's name. 
     * The name can't have spaces ` `!
     * By default it's `PyC-Link-directLink`.
    */
    void changeMapName(const char* newMapName);
}