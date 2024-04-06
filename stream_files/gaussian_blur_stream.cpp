#include "ap_axi_sdata.h"
#include "hls_stream.h"

#define SIZE 3 //size of the overlapped window and the kernel size
typedef ap_axis<32,2,5,6> std_axi; //renaming as standard axi or std_axi



void gaussianBlur(
		hls::stream<std_axi>&window,
		hls::stream<std_axi>&kernel,
		hls::stream<std_axi>&return_stream)
{

#pragma HLS INTERFACE axis port=window
#pragma HLS INTERFACE axis port=kernel
#pragma HLS INTERFACE axis port=return_stream
#pragma hls interface s_axilite port=return


	std_axi window_tmp, kernel_tmp, accumulator;

	float sum = 0;

	while(1)
	{

#pragma HLS pipeline off
		window.read(window_tmp);
		kernel.read(kernel_tmp);

		sum += window_tmp.data.to_int() * kernel_tmp.data.to_float();

		if(window_tmp.last || kernel_tmp.last)
		{
			accumulator.data = sum;
			return_stream.write(accumulator);
			break;
		}


	}



}
