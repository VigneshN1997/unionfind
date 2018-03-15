#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <cmath>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;


vector<long int>* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping);
void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess);
