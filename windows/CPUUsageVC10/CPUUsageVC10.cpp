// CPUUsage.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>

using namespace std;

int memUsageLSB( int n ){
	int res = 0;

	int len = 8;

	for(int i=0; i < len; i++){
		if(1 & (n >> i)){
			res |= 1 << i;
		}else{
			res |= 0 << i;
		}
	}

	return res;
}
int memUsageMSB( int n ){
	int res = 0;

	int len = 15;

	for(int i=8; i < len; i++){
		if(1 & (n >> i)){
			res |= 1 << (i-8);
		}else{
			res |= 0 << (i-8);
		}
	}

	return res;
}

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;


void init(){
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
}


int getCurrentValue(){
    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_LONG, NULL, &counterVal);
    return counterVal.longValue;
}

int main(int argc, char *argv[]){

	int		comPort		= 0;
	int		comBoud		= 0;
	int		comUpdate	= 1000;
	
	if(!(argc > 2)){
		cout << "Viga yhtegi argumenti pole [-c,-b,-t,...]" << endl;
		return 1;
	}

	for(int i = 0; i < argc; i++){
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		if(strcmp(argv[i], "-c") == 0){
			if(atoi(argv[i+1]) > 0 && atoi(argv[i+1]) < 100){
				comPort = atoi(argv[i+1]);
			}else{
				cout << "Viga port vale/puudulik" << endl;
				return 1;
			}
		}
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		if(strcmp(argv[i], "-b") == 0){
			if(atoi(argv[i+1]) > 110 && atoi(argv[i+1]) < 230400){
				comBoud = atoi(argv[i+1]);
			}else{
				cout << "Viga boud vale/puudulik" << endl;
				return 1;
			}
		}
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		if(strcmp(argv[i], "-t") == 0){
			if(atoi(argv[i+1]) > 500 && atoi(argv[i+1]) < 10000){
				comUpdate = atoi(argv[i+1]);
			}else{
				cout << "Viga update rate puudu" << endl;
				return 1;
			}
		}
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	}
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	init();
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	CSerial serial;
	serial.Open(comPort, comBoud);
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

	while(1){

		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);

		byte cpuUsage	= (int)getCurrentValue();

		int mPU		= memInfo.ullAvailPhys/(1024*1024);

		byte msb		= memUsageMSB(mPU);
		byte lsb		= memUsageLSB(mPU);

		// 0
		serial.WriteCommByte(0xfa);
		// 1
		serial.WriteCommByte(cpuUsage);
		// 2
		serial.WriteCommByte(msb);
		// 3
		serial.WriteCommByte(lsb);
		// 4
		serial.WriteCommByte(0x6c);

		Sleep(comUpdate);

		printf("Avaible RAM: %i | CPU Usage %i \n", mPU, cpuUsage);

	}

	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	serial.~CSerial();
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	return 0;
}