//Emir Erbasan May 18' - June 18' 

// I'm not good at reverse engineering things but I am getting used to...
//(so i may be coded a little bit shitty)

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include "hexagoner.h"

#define TEST 0       
//I used to check some variables and adresses
//They're still available in the code but, not going to be...
//compiled if you set TEST to 0

using namespace std;

int main(){
    
start:
    //get process id and window handle, get all access in game's memory (read + write)
    DWORD pid=0;                        
    HWND Gamewindow = FindWindow(0, ("Super Hexagon"));
    GetWindowThreadProcessId(Gamewindow,&pid);
    HANDLE superHexagon = OpenProcess(PROCESS_ALL_ACCESS,0,pid);

    SetConsoleTitleA("Hexagoner Bot");

    if(pid == 0){
        cout<<"Couldn't find Super Hexagon.exe"<<endl;       //check if process is open or not.
        cout<<"Click enter to try again."<<endl;
        cin.get();
        goto start;
    }
    else{
        system("cls");
        cout<<"hexagoner by Emir Erbasan"<<endl;
        cout<<"github.com/humanova"<<endl<<endl;
        cout<<"Found Super Hexagon.exe..."<<endl;
        cout<<"pid : "<<pid<<endl;
    }
    
    
    DWORD BaseAddress = dwGetModuleBaseAddress(pid, _T("superhexagon.exe"));
    BaseAddress  += 0x2857F0;
    BaseAddress = readMemory(superHexagon,BaseAddress);

    long ButtonAddress = BaseAddress + offset::button;  //never used, address for left and right buttons
    long slotsAddress = BaseAddress + offset::num_slots;
    long wallsAddress = BaseAddress + offset::num_walls;
    long firstWallAddress = BaseAddress + offset::first_wall;
    long playerAngleAddress = BaseAddress + offset::player_angle;
    long playerAngle2Address = BaseAddress + offset::player_angle_2;
    long worldAngleAddress = BaseAddress + offset::world_angle;
    long isMenuOnAddress = BaseAddress + offset::is_menu_on;   
    //never used isMenuOnAddress, value changes to 0 when you start to play

    //ButtonAddress stuff, never used
    //int left = 1, right = 16777216, stop=0   
    
    int slots, currSlot, walls, firstWall, playerAngle, playerAngle2, worldAngle, isMenuOn;
    int algo,decision=0,isThereWall[6],wayFoundFlag, count[6],isThereWallCount;
    short int wall[50][3];
    short int slotWalls[6][10];
    short int closestWalls[6];


    #if TEST    //to check addresses with Cheat Engine
    cout<<"pid : " <<pid<<endl;
    printf("Base address = %x\n",BaseAddress);
    printf("button address : %x\n",ButtonAddress); 
    printf("first wall address = %x\n",firstWallAddress);
    printf("isMenuOnAddress address = %x\n",isMenuOnAddress);
    #endif

    while(1){         
        wayFoundFlag=0;
        isThereWallCount=0;

        //Read addresses to get necessary values from memory
        slots = readMemory(superHexagon, slotsAddress);
        walls = readMemory(superHexagon, wallsAddress);
        playerAngle = readMemory(superHexagon, playerAngleAddress);
        playerAngle2 = readMemory(superHexagon, playerAngle2Address);
        worldAngle = readMemory(superHexagon, worldAngleAddress);
        currSlot = getPlayerSlot(playerAngle,slots);        //never used

        //playerangle and playerangle2 both has the same degrees but when I changed only one of thems value...
        //cursor started flicking out to 2 locations at the same time, so it was necessary to set both...
        //of them to the correct degree

    
        //Get walls' slots, distances and isWallAlive values, when wall hits to center, they visually get 
        //deleted, but their slot values and distance values stays still, I can check this with isWallAlive 
        //value
    
        for(int i = 0; i<walls; i++){
            wall[i][0] = readMemory(superHexagon, firstWallAddress + (i * 0x14));   //get wall slot
            wall[i][1] = readMemory(superHexagon, firstWallAddress + (i * 0x14) + 0x4);  //get wall distance         
            wall[i][2] = readMemory(superHexagon, firstWallAddress + (i * 0x14) + 0x10); //get isWallAlive  
        }
        
        //set isThereWall and closestWalls values of every slot to 0
        for(int i = 0; i<slots; i++){           
            isThereWall[i]=0;
            closestWalls[i] = 6500; //this is for slots that has no walls, their dist becomes 6500...
                                    //so they get the priority (see the code below)
        }

        //check every slot, if any wall is alive, set its slots' isThereWall value to 1
        for(int i = 0; i< walls; i++){
            for(int a = 0; a < slots; a++){
                if(wall[i][0]==a && wall[i][2]==1){
                    isThereWall[a]=1;
                    isThereWallCount++;
                    break;
                }
            }
        }

        //sets the decision to "slot that has no walls", useless after first level..
        //waste of time at higher levels (because they're fast and they %99 
        //have walls at their each slot all the time.
        /*
        if(isThereWallCount != 0){
            if(isThereWall[decision] == 0){
                algo = 1;
                wayFoundFlag = 1;
            }
            else{
                algo = 2;
                for(int i = 0; i < slots; i++){
                    if(isThereWall[i]!=1){
                        decision = i;
                        wayFoundFlag = 1;
                        break;
                    }
            
                }
             }
        }*/

        //if couldn't find any slot that has no walls, use this...
        //organize walls by their slots + find the closest wall of each slot
        if(wayFoundFlag != 1){
            algo =3;   //to check which way is used to find the correct slot
            for(int i = 0; i < slots; i++){     
                count[i] = 0;
                for(int a = 0; a < walls; a++){
                    if(wall[a][0] == i && wall[a][2]==1){
                        slotWalls[i][count[i]] = wall[a][1];
                        count[i]++;
                    }
                }
            }


            //find closest wall of each slot, if there is only one
            //count it as that slots closest
            //!!! wall must be alive, that fkn mistake took my hour !!!

            for(int i = 0; i < slots; i++){     
                if(count[i] == 1){              
                    closestWalls[i] = slotWalls[i][0];
                }
                else{
                    for(int a = 0; a < count[i]; a++){
                        if(a == 0){
                            if(slotWalls[i][a] < slotWalls[i][a+1]){
                                closestWalls[i] = slotWalls[i][a];
                            }

                            else{
                                closestWalls[i] = slotWalls[i][a+1];
                            }
                        }
                        else if(slotWalls[i][a] < closestWalls[i]){
                            closestWalls[i] = slotWalls[i][a];
                        }
                    }
                }
            }

            decision = findMax(closestWalls, slots);  
            //find the most far wall among closestWalls (most far wall AMONG each slots' closestWall)
            //(fk my english)
        }

        //set playerslot to correct slot 
        setPlayerSlot(superHexagon, playerAngleAddress, playerAngle2Address, decision, slots);  
        
        //make wallcount 1  (to test some functions)                                                                             
        //writeMemory(superHexagon, wallsAddress, 1);   

        //setWorldAngle to 0 to see clear because screen spins!
        //setWorldAngle(superHexagon, worldAngleAddress, 0); 

        Sleep(25); 
        //sleep for 25 milliseconds, otherwise CPU usage goes wild!!

        #if TEST  //debugging my shit
        if(GetAsyncKeyState(VK_HOME)&1) {
            system("cls");
            cout<<"Slot sayisi :" <<slots<<endl;
            cout<<"Duvar sayisi :" <<walls<<endl;
            cout<<"Ilk duvar : "<<firstWall<<endl;
            cout<<"Oyuncu acisi : "<<playerAngle<<endl;
            cout<<"Oyuncu acisi2 :"<<playerAngle2<<endl;
            cout<<"Oyunun acisi :" <<worldAngle<<endl;
            cout<<"Suanki slot : " <<currSlot <<endl;
            cout<<"Ilk duvarin slot :"<<wall[0][0] <<endl;
            cout<<"Ilk duvar uzaklik : "<<wall[0][1] <<endl;

            for(int i = 0; i<walls; i++){
                cout << i <<". duvar || slot : "<< wall[i][0] <<" uzaklik : "<<wall[i][1]<<endl;
            }

            for(int i = 0; i < slots; i++){
                cout<< "slot "<<i<<" duvar var mi = "<<isThereWall[i]<<endl;
            }

            for(int i = 0; i < slots; i++){
                for(int a = 0; a < count[i]; a++){
                 cout<< "slot "<<i<<" slotWalls = "<<slotWalls[i][a]<<endl;
                }
               
            }

            for(int i = 0; i < slots; i++){
                cout<<"slot " << i <<" en kisa duvar uzakligi = " <<closestWalls[i]<<endl;
               
            }

            cout<<"ALGO : " <<algo<<endl;
            cout<<"karar :"<<decision<<endl;
        }
        #endif
    }
    return 0;
}
