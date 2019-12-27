#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <stack>

using namespace std;

void print(string technique,long long unsigned hits, long long unsigned misses);
void printVect(vector<unsigned int> vect);
void print2DVect(vector<vector<unsigned int>> vect);

int main(int argc, char *argv[]){
	istream *input;	
	input = &cin;

	struct lineData{
		bool store;
		unsigned long long address;

	};	
	vector<lineData> data;
	//need to be bigger than int?
	unsigned int address;
	char flag;
	while(scanf("%11x %c",&address,&flag) != EOF){
		lineData dat = lineData();
		dat.address = address;
		if(flag == 'S'){
			dat.store = true;
		}else{
			dat.store = false;
		}
		//push
		data.push_back(dat);
	}
	
	//---------------------DIR------------------
	unsigned int dirCache[8192] = {};
	unsigned long long dirHits = 0;
	unsigned long long dirMisses = 0;
	unsigned int hash;
	for(int i =0; i<data.size(); i++){
		lineData dat = data[i];
		hash = dat.address%8192;
		if(dirCache[hash] == dat.address){
			dirHits++;
		}else{
			dirMisses++;
			dirCache[hash] = dat.address;
		}

	}

	print("DIR", dirHits, dirMisses);
	
	//---------------------ASS------------------

	vector<unsigned int> assCache;
	vector<unsigned int> tempAssCache;
	unsigned long long assHits;
	unsigned long long assMisses;
	bool found;
	for(int i = 0; i<data.size(); i++){
		lineData dat = data[i];
		//lets see if we have this in our cache
		found = false;
		for(int j = 0; j<assCache.size(); j++){
			if(assCache[j] == dat.address){
				//we found it
				found = true;
				assHits++;
				//move it to the front
				//by deleting it from cur pos
				assCache.erase(assCache.begin() + j);
				//and inserting at front
				assCache.insert(assCache.begin(), dat.address);
				//break out of loop
				j = assCache.size();

			}
		}
		if(!found){
			//we didnt find it
			assMisses++;
			//check off by 1
			if(assCache.size() >= 8192){
				//max size so remove least recently used
				assCache.erase(assCache.end()-1);
			}
			assCache.insert(assCache.begin(), dat.address);
		}
	}

	print("ASS", assHits, assMisses);
	
	//---------------------SET------------------
	//even though we know the length of the cache and each set
	//i am using vectors for their helpful methods such as deleting
	//and inserting items
	//vector of sets
	//sets are of length 4
	unsigned long long setHits = 0;
	unsigned long long setMisses = 0;
	vector< vector<unsigned int> > setCache(2048);
	vector<unsigned int> empty(4);
	
	//fill our vector with 2048 sets
	for(int i = 0; i<setCache.size(); i++){
		setCache[i] = empty;
	}
	
	for(int i = 0; i < data.size(); i++){
		lineData dat = data[i];
		hash = dat.address % 2048;
		//search set for our address
		found = false;
		for(int j = 0; j<4; j++){
			if(setCache[hash][j] == dat.address){
				//found address in set
				found = true;
				setHits++;
				//shift all addresses back
				for(int k = j; k>0; k--){
					setCache[hash][k] = setCache[hash][k-1];
					
				}
				//move our new element to the front
				setCache[hash][0] = dat.address;
				//break out of search loop
				j = 4;
			}
		}
		if(!found){
			setMisses++;
			//remove least recently used
			for(int j = 4; j>0; j--){
				setCache[hash][j] = setCache[hash][j-1];
			}
			//insert new address
			setCache[hash][0] = dat.address;
		}
	}
	
	
	print("SET", setHits, setMisses);
	//---------------------BLK------------------
	vector< vector<unsigned int> > blkCache(256);

	vector<unsigned int> emptySet(4);
	int offset;
	unsigned long long blkHits;
	unsigned long long blkMisses;

	for(int i =0; i<4; i++){
		emptySet[i] = 0;
	}

	for(int i =0; i<256; i++){
		blkCache[i] = emptySet;
	}

	for(int i =0; i< data.size(); i++){
		lineData dat = data[i];
		//lower 3 bits are block index (offset)
		offset = dat.address%8;
		//hash is next 8 bits
		hash = (dat.address/8)%256;
		//search through our set for this element
		found = false;
		for(int j = 0; j<4; j++){
			if(blkCache[hash][j] == dat.address-offset){
				//the address was in this set
				blkHits++;
				found = true;
				//move this block with the address to top of the set
				//shift prev blocks down the set
				for(int k = j; k>0; k--){
					blkCache[hash][k] = blkCache[hash][k-1];
				}
				blkCache[hash][0] = dat.address-offset;
				j = 4;
			}

		}
		if(!found){
			blkMisses++;
			
			//shift blocks down the set
			for(int k = 3; k>0; k--){
				blkCache[hash][k] = blkCache[hash][k-1];
			}

			//insert new block into set
			blkCache[hash][0] = dat.address-offset;

		}

	}
	
	print("BLK", blkHits, blkMisses);
	
	//---------------------NWA------------------
	vector< vector<unsigned int> > nwaCache(256);

	unsigned long long nwaHits;
	unsigned long long nwaMisses;
	
	for(int i =0; i<256; i++){
		nwaCache[i] = emptySet;
	}

	for(int i =0; i< data.size(); i++){
		lineData dat = data[i];
		//lower 3 bits are block index (offset)
		offset = dat.address%8;
		//hash is next 8 bits
		hash = (dat.address/8)%256;
		//search through our set for this element
		found = false;
		for(int j = 0; j<4; j++){
			if(nwaCache[hash][j] == dat.address-offset){
				//the address was in this set
				nwaHits++;
				found = true;
				//if not store then we update
				//move this block with the address to top of the set
				//shift prev blocks down the set
				for(int k = j; k>0; k--){
					nwaCache[hash][k] = nwaCache[hash][k-1];
				}
				nwaCache[hash][0] = dat.address-offset;
				j = 4;
			}

		}
		if(!found){
			nwaMisses++;
			if(!dat.store){	
				//shift blocks down the set
				for(int k = 3; k>0; k--){
					nwaCache[hash][k] = nwaCache[hash][k-1];
				}
				//insert new block into set
				nwaCache[hash][0] = dat.address-offset;
			}

		}

	}
	
	print("NWA", nwaHits, nwaMisses);
	
	//---------------------PRF------------------
	vector< vector<unsigned int> > prfCache(256);

	unsigned long long prfHits;
	unsigned long long prfMisses;
	
	for(int i =0; i<256; i++){
		prfCache[i] = emptySet;
	}

	for(int i =0; i< data.size(); i++){
		lineData dat = data[i];
		//lower 3 bits are block index (offset)
		offset = dat.address%8;
		//hash is next 8 bits
		hash = (dat.address/8)%256;
		//search through our set for this element
		found = false;
		for(int j = 0; j<4; j++){
			if(prfCache[hash][j] == dat.address-offset){
				//the address was in this set
				prfHits++;
				found = true;
				//if not store then we update
				//move this block with the address to top of the set
				//shift prev blocks down the set
				for(int k = j; k>0; k--){
					prfCache[hash][k] = prfCache[hash][k-1];
				}
				prfCache[hash][0] = dat.address-offset;
				j = 4;
			}

		}
		if(!found){
			prfMisses++;
			if(!dat.store){	
				//shift blocks down this set
				for(int k = 3; k>0; k--){
					prfCache[hash][k] = prfCache[hash][k-1];
				}

				//insert new block into set
				prfCache[hash][0] = dat.address-offset;
				
				//PREFETCH
				//check if next hash already contains prefetched block
				int nextHash = (hash+1)%256;
				int prefetchedBlock = dat.address-offset+8;
				found = false;
				for(int j = 0; j<4; j++){
					if(prfCache[nextHash][j] == prefetchedBlock){
						//next set already contained prefetched block
						found = true;
						//move it to front
						//shift all prev blocks down
						for(int k=j; k>0;k--){
							prfCache[nextHash][k] = prfCache[nextHash][k-1];	
						}
						//insert prefetched to most recent
						prfCache[nextHash][0] = prefetchedBlock;
						//break
						j = 4;
					}
				}

				if(!found){
					
				
					//shift blocks down subsequent set
					for(int k =3; k>0; k--){	
						prfCache[nextHash][k] = prfCache[nextHash][k-1];
					}
					//insert prefetched block into this set
					prfCache[nextHash][0] = prefetchedBlock;
				}
			}

		}

	}
	
	print("PRF", prfHits, prfMisses);

	return 0;
}

void print(string technique,long long unsigned hits, long long unsigned misses){
	printf("%s: %20llu %20llu\n",technique.c_str(),hits, misses);
}

void printVect(vector<unsigned int> vect){
	for(int i =0; i < vect.size(); i++){
		if (i != vect.size() - 1){
			cout<<vect[i]<<",";
		}else{
			cout<<vect[i]<<endl;
		}
	}
}

void print2DVect(vector<vector<unsigned int>> vect){
	cout<<"------------------"<<endl;
	for(int i =0; i<vect.size(); i++){
		printVect(vect[i]);
	}
}

