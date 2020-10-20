#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <math.h>
#include <cstdlib>
#include <utility>

using namespace std;

struct Result
{
	int correct;
	int total;
	Result()
	{
		correct = 0;
		total = 0;
	}
};

Result alwaysTaken(vector <string> state){
	Result result;
	for(int i = 0; i < state.size(); i++){
		if(state[i] == "T"){
			result.correct++;
		}
		result.total++;
	}
	return result;
}

Result alwaysNotTaken(vector <string> state){
	Result result;
	for(int i = 0; i < state.size(); i++){
		if(state[i] == "NT"){
			result.correct++;
		}
		result.total++;
	}
	return result;
}

Result singleBitBimodalPredictor(vector <unsigned long long> addr, vector<string> state, int size){
	Result result;
	vector<string> bitSize(size, "T");
	
	for (int i = 0; i < addr.size(); i++){
		int indexBit = addr[i]%size;
		
		if (bitSize[indexBit] == state[i]){
			result.correct++;
		}
		else {
			if (state[i] == "T"){
				bitSize[indexBit] = "T";
			}
			else{
				bitSize[indexBit] = "NT";
			}
		}
		result.total++;
	}
	
	return result;
}

Result twoBitBimodalPredictor(vector <unsigned long long> addr, vector<string> state, int size){
	Result result;
	vector<int> bitSize(size, 3);
	for (int i = 0; i < addr.size(); i++){
		int indexBit = addr[i]%size;
		if(state[i] == "T"){
			if(bitSize[indexBit] == 2 || bitSize[indexBit] == 3){
				result.correct++;
			}
			if(bitSize[indexBit] < 3){
				bitSize[indexBit]++;
			}
		}
		else{
			if(bitSize[indexBit] == 0 || bitSize[indexBit] == 1){
				result.correct++;
			}
			if(bitSize[indexBit] >0){
				bitSize[indexBit]--;
			}
		}
		result.total++;
	}
	return result;
}

Result gshare(vector <unsigned long long> addr, vector<string> state, int bitSize){
	Result result;
	unsigned long size = (int) pow(2,bitSize)-1;
	unsigned int ghr = 0;
	
	vector<int> table(2048 ,3);
	
	for (int i = 0; i < addr.size(); i++){
		bool shift = false;
		int index = (addr[i] ^ (ghr & size))%2048;
		if(state[i] == "T"){
			if(table[index] == 2 || table[index] == 3){
				result.correct++;
			}
			if(table[index] < 3){
				table[index]++;
			}
			shift = true;
		}
		else{
			if(table[index] == 0 || table[index] ==1){
				result.correct++;
			}
			if(table[index] > 0){
				table[index]--;
			}
		}
		ghr = ghr << 1;
		if(shift){
			ghr = ghr | 1;
		}
	}
	result.total = addr.size();
	return result;
}

Result tournamentPredictor(vector <unsigned long long> addr, vector<string> state){
	Result result;
	vector<int> tableGshare(2048, 3);
	vector<int> tableBimodal(2048, 3);
	vector<int> selectorTable(2048 ,0);
	unsigned long size = (int) pow(2,11)-1;
	unsigned int ghr = 0;
	
	
	for (int i = 0; i < addr.size(); i++){
		bool bimodalPrediction = false;
		int bimodalIndex = addr[i]%2048;
		
		if(state[i] == "T"){
			if(tableBimodal[bimodalIndex] == 2 || tableBimodal[bimodalIndex] == 3){
				bimodalPrediction = true;
			}
			if(tableBimodal[bimodalIndex] != 3){
				tableBimodal[bimodalIndex]++;
			}
		}
		else{
			if(tableBimodal[bimodalIndex] == 0 || tableBimodal[bimodalIndex] == 1){
				bimodalPrediction = true;
			}
			if(tableBimodal[bimodalIndex] != 0){
				tableBimodal[bimodalIndex]--;
			}
		}
		
		
		bool shift = false;
		
		bool gSharePrediction = false;
		int gShareIndex = (addr[i] ^ (ghr & size))%2048;
		
		if(state[i] == "T"){
			if(tableGshare[gShareIndex] == 2 || tableGshare[gShareIndex] ==3){
				gSharePrediction = true;
				
			}
			if(tableGshare[gShareIndex] != 3){
				tableGshare[gShareIndex]++;
			}
			shift = true;
		}
		else{
			if(tableGshare[gShareIndex] == 0 || tableGshare[gShareIndex] ==1){
				gSharePrediction = true;
				
			}
			if(tableGshare[gShareIndex] != 0){
				tableGshare[gShareIndex]--;
			}
		}
		
		ghr = ghr << 1;
		if(shift == true){
			ghr = ghr | 1;
		}
		
		int selectorState = selectorTable[bimodalIndex];
		
		if(gSharePrediction == bimodalPrediction){
			if(gSharePrediction == true){
				result.correct++;
			}
		}
		else{
			if(selectorState == 2 || selectorState == 3){
				if(bimodalPrediction == true){
					result.correct++;
					if (selectorState == 2){
						selectorTable[bimodalIndex]++;
					}
				}
				else{
					if (selectorState > 0) {
						selectorTable[bimodalIndex]--;
					}
				}
			}
			else{
				if(gSharePrediction == true){
					result.correct++;
					if(selectorState == 1){
						selectorTable[bimodalIndex]--;
					}
				}
				else{
					if (selectorState < 3) {
						selectorTable[bimodalIndex]++;
					}
				}
			}
		}
	}
	result.total = addr.size();
	return result;
}

Result btb(vector<unsigned long long> addr, vector<string> state, vector<unsigned long long> tgt){
	Result result;
	vector<string> bimodal(512, "T");
	vector<unsigned long long> targets(512, 0);
	for(int i = 0; i < addr.size(); i++){
		int index = addr[i]%512;
		//branch
		if(bimodal[index] == "T"){
			result.total++;
			//hit
			if(targets[index] == tgt[i]){
				result.correct++;
			}
			
		}
		if(state[i] == "T"){
			targets[index] = tgt[i];
		}
		if(bimodal[index] != state[i]){
			bimodal[index] = state[i];
		}
	}
	
	return result;
	
	
}


int main(int argc, char *argv[]){
	unsigned long long addr;
	vector<unsigned long long> address;
  string behavior;
  vector<string> behaviors;
  unsigned long long target;
  vector<unsigned long long> targets;
  
  ofstream output;
  output.open(argv[2]);
  
  ifstream infile(argv[1]);
  
  while(infile >> std::hex >> addr >> behavior >> std::hex >> target){
  	address.push_back(addr);
  	behaviors.push_back(behavior);
  	targets.push_back(target);
  }
  
  Result res = alwaysTaken(behaviors);
  output << res.correct << "," << res.total << ";" << endl;
  res = alwaysNotTaken(behaviors);
  output << res.correct << "," << res.total << ";" << endl;
 	vector<int> tableSizes{16, 32, 128, 256, 512, 1024, 2048};
 	for(int i = 0; i< tableSizes.size()-1; i++){
 		res = singleBitBimodalPredictor(address, behaviors, tableSizes[i]);
 		output << res.correct << "," << res.total << "; ";
 	}
 	res = singleBitBimodalPredictor(address, behaviors, tableSizes[tableSizes.size()-1]);
 	
 	output << res.correct << "," << res.total << ";" << endl;
 	
 	
 	for(int i = 0; i < tableSizes.size()-1; i++){
 		res = twoBitBimodalPredictor(address, behaviors, tableSizes[i]);
 		output << res.correct << "," << res.total << "; ";
 	}
 	
 	res = twoBitBimodalPredictor(address, behaviors, tableSizes[tableSizes.size()-1]);
 	
 	output << res.correct << "," << res.total << ";" << endl;
 	
 	
 	for(int i = 3; i < 11; i++){
 		res = gshare(address, behaviors, i);
 		output << res.correct << "," << res.total << "; ";
 	}
 	
 	res = gshare(address, behaviors, 11);
 	
 	output << res.correct << "," << res.total << ";" << endl;
  
  
  res = tournamentPredictor(address, behaviors);
  output << res.correct << "," << res.total << ";" << endl;
  
  res = btb(address, behaviors, targets);
  output << res.correct << "," << res.total << ";" << endl;
  
  infile.close();
  output.close();

}

