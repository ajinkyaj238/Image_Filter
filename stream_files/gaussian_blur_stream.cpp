#include "ap_axi_sdata.h"
#include "hls_stream.h"

#define WINDOW_SIZE 5 //size of the overlapped window and the kernel size
typedef ap_axis<32,2,5,6> std_axi; //renaming as standard axi or std_axi



void gaussianBlur(
		hls::stream<std_axi>&window,
		hls::stream<std_axi>&out_pix
)
{

#pragma HLS INTERFACE axis port=window
#pragma HLS INTERFACE axis port=out_pix
#pragma hls interface s_axilite port=return

	std_axi window_tmp;
	float sum = 0;
	float kernel[WINDOW_SIZE][WINDOW_SIZE] = {
			{0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497},
			{0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651},
			{0.021938231279714643, 0.09832033134884577, 0.16210282163712664, 0.09832033134884577, 0.021938231279714643},
			{0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651},
			{0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497}
	}; //5x5 kernel




	for(int i = 0; i < WINDOW_SIZE; i++)
	{
		for(int j = 0; j < WINDOW_SIZE; j++)
		{

			window.read(window_tmp);

			sum += window_tmp.data.to_float() * kernel[i][j]; //both to float to prevent any type errors

			if(window_tmp.last)
			{
				window_tmp.data = sum;
				out_pix.write(window_tmp);
				break;
			}


		}

	}






}
