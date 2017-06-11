#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <cmath>

using namespace std;

float Average(float* heightmap[],int* num[],int x,int z,int offset){
	float sum = 0.0f;
	int count = 0; 
	int i,j;
	for(int i = x-offset; i <= x+offset; i++){
		j = z - offset;
		if(i>=0 && i < 1000 && j >=0 && j < 1000){
			if(num[i][j] != 0){
				sum += heightmap[i][j];
				count++;
			}
		}
	}

	for(int i = x-offset; i <= x+offset; i++){
		j = z + offset;
		if(i>=0 && i < 1000 && j >=0 && j < 1000){
			if(num[i][j] != 0){
				sum += heightmap[i][j];
				count++;
			}
		}
	}

	for(int j = z-offset+1; j <= z+offset-1; j++){
		i = x + offset;
		if(i>=0 && i < 1000 && j >=0 && j < 1000){
			if(num[i][j] != 0){
				sum += heightmap[i][j];
				count++;
			}
		}
	}

	for(int j = z-offset+1; j <= z+offset-1; j++){
		i = x - offset;
		if(i>=0 && i < 1000 && j >=0 && j < 1000){
			if(num[i][j] != 0){
				sum += heightmap[i][j];
				count++;
			}
		}
	}

	if(count == 0){
		return Average(heightmap,num,x,z,offset+1);
	}
	else{
		return sum/count;
	}
}

int main(int argc,char** argv){
	string buf;
	float x,y,z;
	int i,j;
	float * heightmap[1000];
	int * num[1000];
	for(i = 0; i < 1000; i++){
		heightmap[i] = new float[1000];
	}
	for(i = 0; i < 1000; i++){
		num[i] = new int[1000];
		memset(num[i], 0 , sizeof(int)*1000);
	}
	ifstream fin(argv[1]);
	while(fin >> buf){
		if(buf == string("v")){
			fin >> x;
			fin >> y;
			fin >> z;
			cout << x << " " << z << endl;
			i = (int)floor(x/2);
			j = (int)floor(z/2);
			if(i < 0) i = 0;
			if(j < 0) j = 0;
			if(i >= 1000) i = 999;
			if(j >= 1000) j = 999;
			heightmap[i][j] = (num[i][j] * heightmap[i][j] + y) / (num[i][j] + 1);
			num[i][j]++;
		}
		else{
			getline(fin,buf);
		}
	}
	fin.close();
	ofstream fout("mountain.hmp");
	for(i = 0; i < 1000; i++){
		for(j = 0; j < 1000; j++){
			if(num[i][j] != 0)
				fout << heightmap[i][j] << endl;
			else
				fout << Average(heightmap, num, i, j, 0) << endl;
		}
	}
	fout.close();
	for(i = 0; i < 1000; i++){
		delete [] heightmap[i];
	}
	for(i = 0; i < 1000; i++){
		delete [] num[i];
	}
	return 0;
}