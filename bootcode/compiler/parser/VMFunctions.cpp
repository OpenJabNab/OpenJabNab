#include "VMFunctions.h"

void VMFunctions::addVMFunctionsToProgram() {
	Program *p = Program::getInstance();

	TypeTable *t = TypeTable::getInstance();

	DEBUG(cout << "Adding VM functions..");

	p->addDeclarationVMFunction("hd", 1,NULL);
	p->addDeclarationVMFunction("tl", 1,NULL);
	p->addDeclarationVMFunction("tabnew", 2, NULL);
	p->addDeclarationVMFunction("tablen", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("abs", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("min", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("max", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("rand", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("srand", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("strnew", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("strset", 3, t->getScalar("string"));
	p->addDeclarationVMFunction("strcpy", 5, t->getScalar("string"));
	p->addDeclarationVMFunction("vstrcmp", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("strfind", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("strfindrev", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("strlen", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("strget", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("strsub", 3, t->getScalar("string"));
	p->addDeclarationVMFunction("strcat", 2, t->getScalar("string"));
	p->addDeclarationVMFunction("strcatlist", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("atoi", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("htoi", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("itoa", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("ctoa", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("itoh", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("ctoh", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("itobin2", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("strcmp", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("crypt", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("uncrypt", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("listswitch", 2, NULL); //To check
	p->addDeclarationVMFunction("listswitchstr", 2, NULL);
	p->addDeclarationVMFunction("led", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("motorset", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("motorget", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("button2", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("button3", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("load", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("save", 5, t->getScalar("int"));
	p->addDeclarationVMFunction("loopcb", 1, NULL); //to check
	p->addDeclarationVMFunction("rfidGet", 0, t->getScalar("string"));
	p->addDeclarationVMFunction("reboot", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("flashFirmware", 3, t->getScalar("int"));
	p->addDeclarationVMFunction("gc", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("corePP", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("corePush", 1, NULL);//To check
	p->addDeclarationVMFunction("corePull", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("coreBit0", 2, NULL); //To check
	p->addDeclarationVMFunction("netCb", 1, NULL);//To check
	p->addDeclarationVMFunction("netSend", 6, t->getScalar("int"));
	p->addDeclarationVMFunction("netState", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("netMac", 0, t->getScalar("string"));
	p->addDeclarationVMFunction("netChk", 4, t->getScalar("int"));
	p->addDeclarationVMFunction("netSetmode", 3, t->getScalar("int"));
	p->addDeclarationVMFunction("netScan", 1, NULL); //To check
	p->addDeclarationVMFunction("netAuth", 5, NULL); //To check
	p->addDeclarationVMFunction("netPmk", 2, t->getScalar("string"));
	p->addDeclarationVMFunction("netRssi", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("netSeqAdd", 2, t->getScalar("string"));
	p->addDeclarationVMFunction("strgetword", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("strputword", 3, t->getScalar("string"));
	p->addDeclarationVMFunction("udpStart", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("udpCb", 2, NULL); //To check
	p->addDeclarationVMFunction("udpStop", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("udpSend", 6, t->getScalar("int"));
	p->addDeclarationVMFunction("tcpOpen", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("tcpClose", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("tcpSend", 4, t->getScalar("int"));
	p->addDeclarationVMFunction("tcpCb", 2, NULL); //??
	p->addDeclarationVMFunction("tcpListen", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("tcpEnable", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("playStart", 3, t->getScalar("int")); //To check
	p->addDeclarationVMFunction("playFeed", 3, t->getScalar("int"));
	p->addDeclarationVMFunction("playStop", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("recStart", 4, t->getScalar("int")); //To check
	p->addDeclarationVMFunction("recStop", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("recVol", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("sndVol", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("playTime", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("sndRefresh", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("sndWrite", 2, t->getScalar("int"));
	p->addDeclarationVMFunction("sndRead", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("sndFeed", 3, t->getScalar("int"));
	p->addDeclarationVMFunction("sndAmpli", 1, t->getScalar("int"));
	p->addDeclarationVMFunction("adp2wav", 5, t->getScalar("string"));
	p->addDeclarationVMFunction("wav2adp", 5, t->getScalar("string"));
	p->addDeclarationVMFunction("alaw2wav", 6, t->getScalar("string"));
	p->addDeclarationVMFunction("wav2alaw", 6, t->getScalar("string"));
	p->addDeclarationVMFunction("Secholn", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("Secho", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("Iecholn", 1,NULL); //type list
	p->addDeclarationVMFunction("Iecho", 1,NULL);
	p->addDeclarationVMFunction("time", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("time_ms", 0, t->getScalar("int"));
	p->addDeclarationVMFunction("bytecode", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("envget", 0, t->getScalar("string"));
	p->addDeclarationVMFunction("envset", 1, t->getScalar("string"));
	p->addDeclarationVMFunction("rfidGetList", 0,NULL); //list
	p->addDeclarationVMFunction("rfidRead", 2, t->getScalar("string"));
	p->addDeclarationVMFunction("rfidWrite", 3, t->getScalar("int"));
	p->addDeclarationVMFunction("fixarg2", 2, NULL);
	p->addDeclarationVMFunction("fixarg3", 2, NULL);
	p->addDeclarationVMFunction("fixarg4", 2, NULL);
	DEBUG(cout << "Done.");
}
