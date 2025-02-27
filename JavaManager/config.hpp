#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "filemanager.hpp"
#include "config_file.h"

using namespace std;
namespace fs = filesystem;

class Config {
public:
    map<int, fs::path> versionsInstalled;

    fs::path configPath;

    int defaultVersion;

	Config(fs::path _configPath = "") {
        //get where the exe is
        fs::path appDir = abs_exe_directory();

        //Use default config path or use custom one
        if (_configPath == "") {
            configPath = appDir;
            configPath += "\\java.config";
        }
        else {
            configPath = _configPath;
        }

        //cout << appDir;

        for (const auto& entry : fs::directory_iterator(appDir)) {

            // Converting the path to const char * in the
            // subsequent lines
            fs::path outfilename = entry.path();
            //fs::path directorier = outfilename.relative_path();
            std::string outfilename_str = outfilename.string();

            // Testing whether the path points to a
            // non-directory or not If it does, displays path
            if (fs::exists(outfilename_str + "\\bin\\java.exe")) {
                std::string version = outfilename.filename().string();
                
                //std::cout << version << "\n";
                std::cout << "Found Java version " + version << "\n";
                versionsInstalled.insert({ stoi(version), outfilename });
            }
        }

        //Check if config exists
        ifstream f_in2(configPath);
        if (!f_in2) {
            cout << "No config file found, using default" << endl;
            for (const auto& pair : versionsInstalled) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }

            //if not, then create one with default values

            defaultVersion = versionsInstalled.rbegin()->first;

            std::ofstream f_out(configPath);
            if (!f_out)
                cout << "Error writing file !" << endl;
            else
            {
                //write it to disk
                CFG::WriteFile(f_out, _configKeys, _configVersion, defaultVersion);
                f_out.close();
            }
        }
        //Read config and show output
        CFG::ReadFile(f_in2, _configKeys, _configVersion, defaultVersion);
        cout << "Values obtained:" << endl;
        CFG::WriteFile(std::cout, _configKeys, _configVersion, defaultVersion);
        f_in2.close();
	}
private:
    int _configVersion = 1;
    vector<string> _configKeys = { "version", "defaultVersion" };
};