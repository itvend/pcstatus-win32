// CPUUsage.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;


void init(){
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
}


double getCurrentValue(){
    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
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
		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
		DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

		char buffer[40];
		sprintf_s(buffer, "C %1.0f%% M %I64luMb\n", getCurrentValue(), memInfo.ullAvailPhys/(1024*1024));
		serial.SendData(buffer, strlen(buffer));

		printf("COM %d, BOUD %d, %s", comPort, comBoud, buffer);

		Sleep(comUpdate);

	}

	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	serial.~CSerial();
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	return 0;
}