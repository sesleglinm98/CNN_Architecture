#include <stdio.h>
#include <math.h>
#include <assert.h>

void padding_upg(int channel, int size, int input[channel][size][size], int pad, 
				 int out[channel][size+(pad*2)][size+(pad*2)]){
	int x, y, a;
	int pad_yeni = pad;
	int count = 0;
	
	if (pad == 0){
		for (a=0 ; a<channel; a++){
			for (x=0 ; x<size; x++){
				for (y=0 ; y<size; y++){
					out[a][x][y] = input[a][x][y];
				}
			}
		}
	}
	else {
		for(a = 0; a<channel; a++){
			while(pad_yeni > 0){
				for(x=count ; x < (size+(pad_yeni*2)+count); x++){
					out[a][count][x] = 0;
					out[a][x][count] = 0;
					out[a][size+(pad_yeni*2)-1+count][x] = 0;
					out[a][x][size+(pad_yeni*2)-1+count] = 0;
				}
			pad_yeni = pad_yeni -1 ;
			count++;
			}
			for(x=pad ; x<size+(pad*2)-pad ; x++){
				for(y=pad ; y<size+(pad*2)-pad ; y++){
					out[a][x][y] = input[a][x-pad][y-pad];
				}		
			}
			/*
			for (x=0 ; x<size+(pad*2); x++){
				for (y=0 ; y<size+(pad*2); y++){
					printf(" p[%d][%d][%d] = %d ",y+1,x+1,a+1,out[a][x][y]);
				}
				printf("\n");
			}
			*/
		pad_yeni = pad;
		count = 0;
		}	
	}
}

int relu(int deger){
	if(deger < 0){
		return 0;
	}
	else
		return deger;
}

void conv_upg2 (int input_channel, int size_input, int input[input_channel][size_input][size_input], int filtre_sayisi,
				int bias[filtre_sayisi], int size_filtre, int filtre[filtre_sayisi][input_channel][size_filtre][size_filtre], 
				 int stride, int pad, int output[filtre_sayisi][(size_input + (pad*2) - size_filtre) / stride + 1][(size_input + (pad*2)- size_filtre) / stride +1]){
	int a, b;
	int y,x,i,j;
	int count_filter = 0;
	int output_temp = 0;
	int padded_size = size_input + (pad*2);
	int padded_input[input_channel][padded_size][padded_size];
	int n, l, m, chan;
	
	padding_upg(input_channel, size_input, input, pad, padded_input);
	/*
	for(n = 0; n < 3 ; n++){
		printf("%d channel \n",n);
		for(l = 0; l <7; l++){
			for(m = 0; m <7; m++){	
				printf("padding_in[%d][%d][%d] = %d ", m,l, n, padded_input[n][l][m]);
			}
		printf("\n");
		}
	}
	*/
	for(b = 0; b < filtre_sayisi; b++){	
		int temp = 0;
		for (y=0; y < padded_size - size_filtre +1 ; y++){         		
			for (x=0; x< padded_size - size_filtre +1 ;x++){		
				for(a = 0; a < input_channel; a++){   		
					for (i=0; i<size_filtre; i++){    								   
						for (j=0; j<size_filtre; j++){ 
							//printf("yapilan islem: filtre[%d][%d][%d][%d] = %d -- input[%d][%d][%d] = %d \n", j, i, count_filter, b, filtre[b][count_filter][i][j], j+(x * stride), i+(y * stride), a, padded_input[a][i+(y * stride)][j+(x * stride)]);							
							temp += filtre[b][count_filter][i][j] * padded_input[a][i+(y * stride)][j+(x * stride)];				
						}
					} 		
				//printf(" outputa atanan deger : %d \n", output[y][x][a]);
				//output_temp[y][x] = temp;
				count_filter++;
				}
			temp = temp + bias[b];
			temp = relu(temp);
			output[b][y][x] = temp;
			//printf("outputa atanan deger output[%d][%d][%d] = %d \n",x,y,b, output[b][y][x]);
			count_filter = 0;
			temp = 0;
			}
			//output_temp += temp; 
		}
	//	output_temp = 0;
	}
}



int main(){
	int size_im = 3;
	int size_fil = 2;
	int pad = 0;
	int stride = 1;
	int inp_channel = 3;
	int fil_channel = 4 ;
	int l, m, n;
	int out[(size_im + (pad*2) - size_fil) / stride + 1][(size_im + (pad*2) - size_fil) / stride + 1];
	int padout[7][7];
	int size = 5;
	int image[5][5] = {{2, 3, 5, 7, 11}, {12, 8, 4, 6, 1}, {9, 7, 5, 0, 13}, {1, 3, 2, 6, 5}, {4, 6, 8, 8, 1}};
	int image_3d[3][3][3] = {
							{{2, 3, 5}, {12, 8, 4}, {9, 7, 5}},
							{{1, 3, 2}, {6, 5, 8}, {4, 6, 8}},
					 		{{7, 5, 1}, {8, 9, 0}, {4, 3, 2}}
							};

	int kernel_4d[4][3][2][2] = { {{{1, 4}, {2, 6}},{{5, 9}, {8,2}},{{4, 3}, {3, 1}}},
							  	{{{-2, -5}, {-3, -7}},{{-4, -8}, {-5,-9}},{{-1, 0}, {-3, -7}}},
							  	{{{6, 6}, {7, 2}},{{8, 2}, {9,1}},{{7, 8}, {0, 3}}},
							  	{{{1, 4}, {2, 6}},{{5, 9}, {8,2}},{{4, 3}, {3, 1}}}};
							  	
	int bias[4] = {3, 5, 2, 7};
							  
	int chan;
				
	for(n = 0; n < 4 ; n++){
		printf("%d katman \n",n);
		for(chan = 0; chan <3 ; chan++){
			for(l = 0; l <2; l++){
				for(m = 0; m <2; m++){	
					printf("filtre[%d][%d][%d][%d] = %d ", m,l,chan, n,kernel_4d[n][chan][l][m]);
				}
			printf("\n");
			}
		}
	}

	int output_3d[4][(size_im + (pad*2) - size_fil) / stride + 1][(size_im + (pad*2) - size_fil) / stride + 1];

	conv_upg2(inp_channel, size_im, image_3d, fil_channel, bias, size_fil, kernel_4d, stride, pad, output_3d);

	for(n = 0; n < 4 ; n++){
		printf("%d katman \n",n);
		for(l = 0; l <(size_im + (pad*2) - size_fil) / stride +1; l++){
			for(m = 0; m <(size_im + (pad*2) - size_fil) / stride +1; m++){	
				printf("conv_out[%d][%d][%d] = %d  ", m,l,n,output_3d[n][l][m]);
			}
			printf("\n");
		}
	}
}
