//Emir Erbasan May 18' - June 18'
//header file for memory offsets of Super Hexagon...
//writing-reading in memory and calculating some stuff
//like current player slot
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <TlHelp32.h>

using namespace std;

DWORD dwGetModuleBaseAddress(DWORD dwProcessIdentifier, TCHAR *lpszModuleName)
{
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessIdentifier);
   DWORD dwModuleBaseAddress = 0;
   if(hSnapshot != INVALID_HANDLE_VALUE)
   {
      MODULEENTRY32 ModuleEntry32 = {0};
      ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
      if(Module32First(hSnapshot, &ModuleEntry32))
      {
         do
         {
            if(_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
               dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
               break;
            }
         }
         while(Module32Next(hSnapshot, &ModuleEntry32));
      }
      CloseHandle(hSnapshot);
   }
   return dwModuleBaseAddress;
}

enum offset{
    button = 0x4095D,
    num_slots = 0x1BC,
    num_walls = 0x2930,
    first_wall = 0x220,
    player_angle  = 0x2958,
    player_angle_2 = 0x2954,
    world_angle = 0x1AC,
    is_menu_on = 0x10A94
};

int readMemory(HANDLE handle,long address){
    int value;
    ReadProcessMemory(handle, (void *) address, (void *) &value, sizeof(value), 0);
    return value;
}

void writeMemory(HANDLE handle,long address,int value){
    WriteProcessMemory(handle, (void *) address, (void *) &value, sizeof(value), 0);
}

int getPlayerSlot(int angle,int slots){
    int playerSlot = (float) angle / 360.0 * slots;
    return playerSlot;
}

void moveLeft(HANDLE handle,long address){
    writeMemory(handle,address,1);
}

void moveRight(HANDLE handle, long address){
    writeMemory(handle,address,16777216);
}

void moveStop(HANDLE handle, long address){
    writeMemory(handle,address,0);
}

void setPlayerSlot(HANDLE handle, long angleadr1 , long angleadr2, int slot ,int slotCount){
    int angle = 360 / slotCount * (slot % slotCount) + (180 / slotCount);
    writeMemory(handle,angleadr1,angle);
    writeMemory(handle,angleadr2,angle);
}

void setWorldAngle(HANDLE handle, long address, int angle){
    writeMemory(handle,address,angle);
}

//find maximum value in an 1D array and return its  
int findMax(short arr[],int slots){
    int max,sira;

    for(int i = 0; i< slots; i++){

        if(i==0){
            if(arr[i] >= arr[i+1]){
                max = arr[i];
                sira = i;
            }
            else{
                 max = arr[i+1];
                 sira = i + 1;
            }
        }
        else if(arr[i] > max){
            max = arr[i];
            sira = i;
        }
    }
    return sira;
}
