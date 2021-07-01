#include <iostream>
#include <vector>
#include <windows.h>
#include <unistd.h>
#include <fstream>

#define MAX_MSG 1024

using namespace std;

string getUserName() {
	char szUserName[1024];
	unsigned long dwLength = 1024;
	GetUserName(szUserName, &dwLength);
	string result(szUserName);
	return result;
}

void printIntro() {
	cout << "Android-Connector [Version 0.0.2]" << "\n";
	cout << "This is an open source project. You can access it from the link https://github.com/Park-SM/android-connector." << "\n\n";
}

string checkAndGetAdbPath() {
	string pathAdb;
	string pathTxt = "adb-path.txt";
	if (access(pathTxt.c_str(), F_OK) != 0) {
		pathAdb = "C:\\Users\\" + getUserName() + "\\AppData\\Local\\Android\\Sdk\\platform-tools\\adb.exe";
	} else {
		ifstream in("adb-path.txt");
		getline(in, pathAdb);
	}
	
	bool isChangedPath = false;
	while(access(pathAdb.c_str(), F_OK) != 0) {
		isChangedPath = true;
        cout << "There is no adb.exe file in the \"" + pathAdb + "\". Please enter the path of the adb.exe file." << "\n";
        cout << "$ ";
        
        cin >> pathAdb;
    }
    if (isChangedPath) {
    	ofstream out("adb-path.txt");
	    out << pathAdb;
	    out.close();
	}
	
	return pathAdb;
}

bool _checkIp(string ip) {
	if (ip.length() > 15 || ip[0] == '.' || ip[ip.size() - 1] == '.') return false;
	
	int dotCnt = 0;
	int blockCnt = 0;
	for (int i = 0; i < ip.length(); i++) {
		if (ip[i] == '.') {
			dotCnt++;
			blockCnt = 0;
		} else if (ip[i] < '0' || ip[i] > '9' || (++blockCnt > 3)) {
			return false;
		}
	}
	return (dotCnt == 3);
}

string getIpAddr() {
	string ip;
	cout << "Please enter your Wi-Fi IP address." << "\n$ ";
	cin >> ip;
	
	while (!_checkIp(ip)) {
		cout << "You entered an incorrect Wi-Fi IP address. Please enter the correct address." << "\n$ ";
		cin >> ip;
	}
	return ip;
	
}

string excuteAndReadFromAdb(string cmd) {
	FILE *adb = _popen(cmd.c_str(), "r");
	string* output;
	if (adb != NULL) {
		char msg[MAX_MSG];
		fgets(msg, MAX_MSG, adb);
		output = new string(msg);
	} else {
		output = new string();
	}
	pclose(adb);
	return *output;
}


int main() {
	
	printIntro();
	string path = checkAndGetAdbPath();
	string ip = getIpAddr();
	
	vector<string> cmds;
	cmds.push_back(path + " tcpip 12055");
	cmds.push_back(path + " connect " + ip + ":12055");

	cout << "Connecting to device..." << "\n";
	for (int i = 0; i < cmds.size(); i++) {
		string output = excuteAndReadFromAdb(cmds[i]);
		if (output.length() > 14 && output.substr(0, 14) == "cannot connect")  {
			cout << "Failed to connect to \"" + ip + "\"." << "\n";
			cout << "Please Check the IP, connect the device to the PC with a USB cable and try again." << "\n";
			system("PAUSE");
			return 0;
		} 
		if (output.length() > 14 && output.substr(0, 14) == "cannot resolve")  {
			cout << "Failed to connect to \"" + ip + "\"." << "\n";
			cout << "Please check the IP and try again." << "\n";
			system("PAUSE");
			return 0;
		}
		//cout << output;
	}
	
	cout << "Connected with the device." << "\n\n";
	system("PAUSE");
	return 0;
}
