#include "myAudio.h"

#include <iostream>
#define buffNumber 2

void menu()
{
	cout << "1-----record audio"	  << endl;
	cout << "2-----stop record"		  << endl;
    cout << "3-----player audio"	  << endl;
	cout << "4-----stop player"		  << endl;
	cout << "5-----release source"	  << endl;
	cout << "0-----exit"			  << endl;
	cout << "*****************************" << endl;
	cout << "please input your choice:";

}

int main() {
    int ch;
    myAudio myaudio(buffNumber);

    menu();

    do {
        cin >> ch;
        switch (ch)
        {
        case 1:
             cout << "Begin recording..." << endl;
             myaudio.recordAudio();
             break;
        case 2:
             cout << "Stop recording..." << endl;
             myaudio.stopRecord();
             break;
        case 3:
             cout << "Playing..." << endl;
             myaudio.playerAudio();
             break;
        case 4:
             cout << "Stop playing..." << endl;
             myaudio.stopPlayer();
             break;
        default:
             break;
        }
    } while(ch != 0);

    return 0;
}
