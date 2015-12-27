#include <iostream>
#include <stdio.h>
#include "parse_manager.h"
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../recordmanager/RecordManager.h"
#include "../recordmanager/rm_filescan.h"
#include "../systemmanager/sm_manager.h"

using namespace std;

int main(int argc, char **argv){
	FileManager* fm = new FileManager();
	BufPageManager* bpm = new BufPageManager(fm);
	RM_Manager* rm_m = new RM_Manager(fm, bpm);
	SM_Manager* sm_m = new SM_Manager(*rm_m);
	if(argc < 2) printf("Please Input Sql instruction filename\n");
	string filename = string(argv[1]);
	Parse_Manager PM(filename);
	PM.MainLoop(sm_m, rm_m);
	return 0;
}