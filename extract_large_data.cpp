#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *in_file_name = "large_data_test.txt";
char *out_file_name[8] = {"Top_k_large.txt", "Dipole_k_large.txt",
                         "Uniform_k_large.txt"};

int main(){
	std::ifstream fin(in_file_name);
    FILE *out_file[8];
    for(int i = 0; i < 3; i++)
    	out_file[i] = fopen(out_file_name[i], "a");
    std::string one_line;
    int file_indx, t_value;
    while(fin.good()){
    	while(std::getline(fin, one_line)){
    		if(one_line.length() < 1)
    			continue;
    		int cmp_v = one_line.compare("Top-k");
    		if(cmp_v == 0)
    			file_indx = 0;
    		else if(cmp_v < 0)
    			file_indx = 1;
    		else
    			file_indx = 2;
    		std::getline(fin, one_line);
    		sscanf(one_line.c_str(), "Input size: %*d %d", &t_value);
    		fprintf(out_file[file_indx], "%d\n", t_value);
    		for(int i = 0; i < 12; i++){
    			std::getline(fin, one_line);
                sscanf(one_line.c_str(), "%d", &t_value);
                fprintf(out_file[file_indx], "%d\n", t_value);
    		}
    	}
    }
    for(int i = 0; i < 3; i++)
    	fclose(out_file[i]);
}