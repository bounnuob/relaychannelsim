#include "const.h"
double hher[N][TS][2], channel_power[N][2];

void mrc_weight(double(*H)[CL][BS][2]){	/* Here, w is selected as h; a kind of matched filter. */
	double h[N][TS][2];

	h_desired(H, h);	/* Choose the channel of desired user */
	VectorHermite(h, hher);	/* hher = h^H */
	VectorMulVectorToscalar(hher, h, channel_power);	/* channel_power = h^H x h */
}


void h_desired(double(*H)[CL][BS][2], double(*h_m)[TS][2]){	
	int ts, i;
	for (ts = 0; ts < TS; ts++)
	{
		for (i = 0; i < N; i++)
		{
			h_m[i][ts][0] = H[i][ts][0][0];
			h_m[i][ts][1] = H[i][ts][0][1];
		}
	}
}

void VectorHermite(double(*h)[TS][2], double(*hher)[TS][2]){
	int i, ts;

	for (i = 0; i < N; i++)
	{
		for (ts = 0; ts < TS; ts++)
		{
			hher[i][ts][0] = h[i][ts][0];
			hher[i][ts][1] = -h[i][ts][1];
		}
	}
}


void VectorMulVectorToscalar(double(*hher)[TS][2], double(*h)[TS][2], double(*channel_power)[2]){
	int ts, n;
	double sum[2];
	for (n = 0; n < N; n++)
	{
		sum[0] = 0;
		sum[1] = 0;
		for (ts = 0; ts < TS; ts++)
		{
			sum[0] += hher[n][ts][0] * h[n][ts][0] - hher[n][ts][1] * h[n][ts][1];
			sum[1] += hher[n][ts][0] * h[n][ts][1] + hher[n][ts][1] * h[n][ts][0];
		}
		channel_power[n][0] = sum[0];
		channel_power[n][1] = sum[1];
	}
}








