#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <vector>

using namespace std;

string exec(const string& cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

void split(const string& s, char delim, vector<string>* elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems->push_back(item);
    }
}

void list(const string& dir, vector<string>* files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) return;
    while ((dirp = readdir(dp)) != NULL) {
        string file = string(dirp->d_name);
        if (file != "." && file != "..") files->push_back(file);
    }
    closedir(dp);
}

int main(int argc, char** argv) {
  if (argc < 3 || argc > 4) return -1;
  string train = argv[1];
  if (train[train.length() - 1] != '/') train += '/';
  string test = argv[2];
  string compression_level = "5";
  if (argc == 4) compression_level = argv[3];
  vector<string> labels;
  list(train, &labels);
  map<string, vector<double> > results;
  for (int i = 0; i < labels.size(); ++i) {
    string command = "./PAQclass -" + compression_level + " temp ";
    command += train + labels[i] + " " + test;
    string output = exec(command);
    vector<string> lines;
    split(output, '\n', &lines);
    for (int j = 0; j < lines.size(); ++j) {
      vector<string> cols;
      split(lines[j], '\t', &cols);
      results[cols[0]].push_back(atof(cols[1].c_str()));
    }
  }

  cout << "name\tclass";
  for (int i = 0; i < labels.size(); ++i) {
    cout << "\t" << labels[i];
  }
  cout << endl;

  map<string, vector<double> >::iterator it;
  for (it = results.begin(); it != results.end(); it++) {
    cout << it->first;
    double best = 100;
    int best_index = 0;
    for (int i = 0; i < it->second.size(); ++i) {
      if (it->second[i] < best) {
        best = it->second[i];
        best_index = i;
      }
    }
    cout << "\t" << labels[best_index];
    for (int i = 0; i < it->second.size(); ++i) {
      cout << "\t" << it->second[i];
    }
    cout << endl;
  }
  return 0;
}
