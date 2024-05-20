from ctypes import wintypes
from ctypes import windll
import ctypes
import sys

#Defining private types for kernel.
class __SECURITY_ATTRIBUTES(ctypes.Structure):
    _fields_ = [('nLength', wintypes.DWORD),
                ('lpSecurityDescriptor', wintypes.LPVOID),
                ('bInheritHandle', wintypes.BOOL)]

__INVALID_HANDLE_VALUE = wintypes.HANDLE(-1).value

__PAGE_READWRITE = 0x04
__FILE_MAP_READ = 0x04
__FILE_MAP_WRITE = 0x02

__LPSECURITY_ATTRIBUTES = ctypes.POINTER(__SECURITY_ATTRIBUTES)
__SIZE_T = ctypes.c_size_t
__kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)

__CreateFileMappingW = __kernel32.CreateFileMappingW
__CreateFileMappingW.argtypes = wintypes.HANDLE, __LPSECURITY_ATTRIBUTES, wintypes.DWORD, wintypes.DWORD, wintypes.DWORD, wintypes.LPCWSTR
__CreateFileMappingW.restype = wintypes.LPVOID
__MapViewOfFile = __kernel32.MapViewOfFile
__MapViewOfFile.restype = wintypes.LPVOID
__MapViewOfFile.argtypes = wintypes.HANDLE, wintypes.DWORD, wintypes.DWORD, wintypes.DWORD, __SIZE_T
__UnmapViewOfFile = __kernel32.UnmapViewOfFile
__UnmapViewOfFile.argtypes = wintypes.LPCVOID,
__UnmapViewOfFile.restype = wintypes.BOOL
__CloseHandle = __kernel32.CloseHandle
__CloseHandle.argtypes = wintypes.HANDLE,
__CloseHandle.restype = wintypes.BOOL

#Private variables and methods
__mapHandle = None
__mapData = None
__mapName = None

#Public variables and methods
class TYPES:
    CHAR_P = 0

def checkForArgs():
    """
    The point of this method is to check if the program was launched with one of the specific arguments.

    Return
    -
    Returns `True` if it was launched with arguments and `False` if without them.
    """
    if len(sys.argv) > 1:
        if sys.argv[1] == 'char':
            return True
    #No arguments or they are not correct.
    return False

def receiveInput():
    """
    Gets data from the command arguments. Make sure to check if the program was launched with required arguments.

    Returns
    -
    Returns the argument data. -1 if there's not enough arguments.
    """
    if len(sys.argv) < 5:
        return -1
    
    return ' '.join(sys.argv[4:])

def receiveMaxCharNumb():
    """
    A must for sending character types! Receives the maximum number of characters allowed to send from launch arguments.
    
    Returns
    -
    Returns the maximum character number. -1 if there's not enough arguments.
    """
    if len(sys.argv) > 2:
        return sys.argv[2]
    return -1

def getMapName():
    """
    Receives the file map object's name from the argument list.

    Returns
    -
    Returns the file map object's name. -1 if there's not enough arguments.
    """
    if len(sys.argv) > 3:
        return sys.argv[3]
    return -1

def send(type, data):
    """
    Send data of the specified type.

    Parameters
    -
    `type` is the c type of the data. Use `types` class.
    `data` is the data of the specified type.
    
    Returns
    -
    `True` on success and `False` on failure.
    """
    if type < 0: return False
    if type > 0: return False #increment the value according to the numb of types in the class.
    if data == -1: return False
    
    #Get the maximum string length
    length = int(receiveMaxCharNumb())
    __mapName = getMapName()

    #Access the file map object.
    __mapHandle = __CreateFileMappingW(__INVALID_HANDLE_VALUE, None, __PAGE_READWRITE, 0, length, __mapName)
    #TODO Check: failure, whether it was created or accessed. (must be accessed)    
    
    #Access the pointer.
    __mapData = __MapViewOfFile(__mapHandle, __FILE_MAP_WRITE, 0, 0, length)
        #Check if pointer accessed.
    if __mapData == 0:
        return False

    #copy data to the pointer.
    char_p = data.encode('utf-8')
    actual_length = len(char_p)
    if actual_length < length:
        length = actual_length
    else:
        data[length] = '\0'
        char_p = data.encode('utf-8')

    ctypes.memmove(__mapData, char_p, length)
    
    #Clean up
    __UnmapViewOfFile(__mapData)
    __CloseHandle(__mapHandle)
    return True