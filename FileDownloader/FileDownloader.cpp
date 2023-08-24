
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include "include/httplib.h"

using namespace std;
mutex mtx;
vector<pair<int, int>> Errors;
void MakeURLs(vector<string>& URLs) {
    int k = URLs.size();
    cout << "Write " << k << " URL links.\n";
    for (int i = 0; i < k; i++) {
        string s;
        cin >> s;
        URLs[i] = s;
    }
}
void MakeNames(vector<string>& Names, int Rand) {
    int k = Names.size();
    if (Rand) {
        for (int i = 0; i < k; i++) {
            Names[i] = to_string(rand());
        }
    }
    else {
        cout << "Write " << k << " names.\n";
        for (int i = 0; i < k; i++) {
            string s;
            cin >> s;
            Names[i] = s;
        }
    }
}
void DownloadJPG(vector<string> URLs,vector<string> Names,string Path, int threadNum) {
    for (int i = threadNum; i < URLs.size(); i+=4) {
        int pos = URLs[i].find(".com"); // we are looking for .org or .com
        if (pos != -1) {
            string Website = URLs[i].substr(0,pos+4), WebPath = URLs[i].substr(pos + 4);
            httplib::Client client(Website);
            auto res = client.Get(WebPath);
            if (res->status == 302) {
                if (res->has_header("Location")) {
                    string newWebPath = res->get_header_value("Location");
                    res = client.Get(newWebPath);
                }
            }
            if (res->status == 200) {
                ofstream ofs(Path + "/" + Names[i] + ".jpg", ios::out | ios::binary);
                ofs.write(res->body.data(), res->body.size());
                ofs.close();
            }
            else {
                lock_guard<mutex> lg(mtx);
                Errors.push_back({ i,res->status });
            }
            
        }
    }
}
int main()
{
    cout << "Hallo! Write the required number of images.\n";
    int k, Rand;
    cin >> k;
    cout << "Do you want the pictures to be given random names?(0/1)\n";
    cin >> Rand;
    string Path;
    cout << "Specify the download path for the files.\n";
    cin >> Path;
    vector<string> URLs(k);
    vector<string> Names(k);
    MakeURLs(URLs);
    MakeNames(Names,Rand);
    vector<thread> threads(4);
    for (int i = 0; i < 4; i++) {
        threads[i] = thread(DownloadJPG, URLs,Names,Path,i);
    }
    for (int i = 0; i < 4; i++) {
        threads[i].join();
    }
    cout << "Loading is complete!\n";
    for (int i = 0; i < Errors.size(); i++) {
        cout << "Failed to upload " << Errors[i].first << " file. Error: " << Errors[i].second << "\n";
    }
    return 0;
}

