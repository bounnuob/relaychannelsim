#include "const.h"
extern double channel_gain[N][2], p_in[N], her[N][TS][2], channel_power[N][2];
extern int loop;

void CoherentDetection4SD(int data_number, double(*DFT_signal)[N][2], double(*WH)[Tx][Rx][2], double(*diLLR)[N_cbps]){
	int rx, n, index, i, offset;
	double received_vector[FILTER_ORDER + 1][2], temp[2], temp2[2], detected_signal[N_cbps], coef, e[2], e_freqav[2];
	e_freqav[0] = 0; e_freqav[1] = 0;

	index = 0;
	for (n = 0; n < N; n++) {
		temp[0] = temp[1] = 0.0;
		temp2[0] = temp2[1] = 0.0;

		for (rx = 0; rx < Rx; rx++) {

			temp[0] += WH[n][0][rx][0] * DFT_signal[rx][n][0] - WH[n][0][rx][1] * DFT_signal[rx][n][1];
			temp[1] += WH[n][0][rx][0] * DFT_signal[rx][n][1] + WH[n][0][rx][1] * DFT_signal[rx][n][0];
		}


		if (MODULATION == QPSK){
			/************************ QPSK ****************************/
			temp2[0] = (channel_gain[n][0] * temp[0] - channel_gain[n][1] * temp[1])/p_in[n];
			temp2[1] = (channel_gain[n][0] * temp[1] + channel_gain[n][1] * temp[0])/p_in[n];

			detected_signal[index] = temp2[0];
			detected_signal[index + 1] = temp2[1];
			/***********************************************************/
		}
		else if (MODULATION == QAM16){
			coef = 2.0*OneBySqrt10 / (1.0 - channel_gain[n][0]);
			if (fabs(temp[0]) <= 2.0 * OneBySqrt10 * channel_gain[n][0]){
				detected_signal[index] = coef * OneBySqrt10 * temp[0];
			}
			else{
				if (temp[0] >= 0.0){
					detected_signal[index] = coef * OneBySqrt10 * (2.0 * temp[0] - 2.0 * OneBySqrt10 * channel_gain[n][0]);
				}
				else {
					detected_signal[index] = coef * OneBySqrt10 * (2.0 * temp[0] + 2.0 * OneBySqrt10 * channel_gain[n][0]);
				}
			}
			detected_signal[index + 1] = coef * OneBySqrt10 * (2.0 * OneBySqrt10 * channel_gain[n][0] - fabs(temp[0]));

			if (fabs(temp[1]) <= 2.0 * OneBySqrt10 * channel_gain[n][0]){
				detected_signal[index + 2] = coef * OneBySqrt10 * temp[1];
			}
			else{
				if (temp[1] >= 0.0){
					detected_signal[index + 2] = coef * OneBySqrt10 * (2.0 * temp[1] - 2.0 * OneBySqrt10 * channel_gain[n][0]);
				}
				else {
					detected_signal[index + 2] = coef * OneBySqrt10 * (2.0 * temp[1] + 2.0 * OneBySqrt10 * channel_gain[n][0]);
				}
			}
			detected_signal[index + 3] = coef * OneBySqrt10 * (2.0 * OneBySqrt10 * channel_gain[n][0] - fabs(temp[1]));


		}
		else if (MODULATION == QAM64){
			coef = 2.0*OneBySqrt42 / (1.0 - channel_gain[n][0]);
			for (i = 0; i<2; i++){
				offset = i * 3;
				if (fabs(temp[i]) > 6.0 * OneBySqrt42 * channel_gain[n][0]){
					if (temp[i] >= 0.0){
						detected_signal[index + offset] = coef * (4.0 * temp[i] - 12.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset] = coef * (4.0 * temp[i] + 12.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
				else if (fabs(temp[i]) > 4.0 * OneBySqrt42 * channel_gain[n][0]){
					if (temp[i] >= 0.0){
						detected_signal[index + offset] = coef * (3.0 * temp[i] - 6.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset] = coef * (3.0 * temp[i] + 6.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
				else if (fabs(temp[i]) > 2.0 * OneBySqrt42 * channel_gain[n][0]){
					if (temp[i] >= 0.0){
						detected_signal[index + offset] = coef * (2.0 * temp[i] - 2.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset] = coef * (2.0 * temp[i] + 2.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
				else{
					detected_signal[index + offset] = coef * temp[i];
				}

				if (fabs(temp[i]) > 6.0 * OneBySqrt42 * channel_gain[n][0]){
					if (temp[i] >= 0.0){
						detected_signal[index + offset + 1] = coef * (-2.0 * temp[i] + 10.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset + 1] = coef * (2.0 * temp[i] + 10.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
				else if (fabs(temp[i]) > 2.0 * OneBySqrt42 * channel_gain[n][0]){
					if (temp[i] >= 0.0){
						detected_signal[index + offset + 1] = coef * (-temp[i] + 4.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else {
						detected_signal[index + offset + 1] = coef * (temp[i] + 4.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
				else{
					if (temp[i] >= 0.0){
						detected_signal[index + offset + 1] = coef * (-2.0 * temp[i] + 6.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset + 1] = coef * (2.0 * temp[i] + 6.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}

				if (fabs(temp[i]) > 4.0 * OneBySqrt42 * channel_gain[n][0]){
					if (temp[i] >= 0.0){
						detected_signal[index + offset + 2] = coef * (-temp[i] + 6.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset + 2] = coef * (temp[i] + 6.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
				else{
					if (temp[i] >= 0.0){
						detected_signal[index + offset + 2] = coef * (temp[i] - 2.0 * OneBySqrt42 * channel_gain[n][0]);
					}
					else{
						detected_signal[index + offset + 2] = coef * (-temp[i] - 2.0 * OneBySqrt42 * channel_gain[n][0]);
					}
				}
			}
		}
		index += MODULATION;

	}
	deinterleaver4SD(detected_signal, diLLR[data_number - Np]);
}

void CoherentDetection4HD(
	int data_number,
	double(*DFT_signal)[N][2],
	double(*W)[Tx][Rx][2],
	int(*diLLR)[N_cbps]){

	int rx, n, index, detected_signal[N_cbps];

	double received_vector[FILTER_ORDER + 1][2], temp[2], e[2], e_freqav[2];
	e_freqav[0] = 0; e_freqav[1] = 0;


	index = 0;

	for (n = 0; n < N; n++) {
		temp[0] = temp[1] = 0.0;

#ifdef  MMSE	/* MMSE and MRC use different coefficients in their respective functions, therefore here they need algorithm specific modifications. */
		for (rx = 0; rx < Rx; rx++) {
			temp[0] += W[n][0][rx][0] * DFT_signal[rx][n][0] - W[n][0][rx][1] * DFT_signal[rx][n][1];
			temp[1] += W[n][0][rx][0] * DFT_signal[rx][n][1] + W[n][0][rx][1] * DFT_signal[rx][n][0];
		}
#endif
#ifdef  RLS	
		for (rx = 0; rx < Rx; rx++) {
			temp[0] += W[n][0][rx][0] * DFT_signal[rx][n][0] - W[n][0][rx][1] * DFT_signal[rx][n][1];
			temp[1] += W[n][0][rx][0] * DFT_signal[rx][n][1] + W[n][0][rx][1] * DFT_signal[rx][n][0];
		}
		temp[0] = temp[0] / channel_power[index][0];
		temp[1] = temp[1]/ channel_power[index][0];
#endif
#ifdef  MRC 
		for (rx = 0; rx < Rx; rx++) {	/* Recall that for MRC, optimal combining is the channel itself. :: her = h^H */
			temp[0] += her[n][rx][0] * DFT_signal[rx][n][0] - her[n][rx][1] * DFT_signal[rx][n][1];
			temp[1] += her[n][rx][0] * DFT_signal[rx][n][1] + her[n][rx][1] * DFT_signal[rx][n][0];

		}
		temp[0] = temp[0] / channel_power[index][0];
		temp[1] = temp[1]/ channel_power[index][0];
#endif

		if (MODULATION == QPSK){
			/************************ Hard QPSK decoding *************/
			detected_signal[index] = (temp[0] >= 0.0) ? 1 : 0;
			detected_signal[index + 1] = (temp[1] >= 0.0) ? 1 : 0;

			/************************ Hard QAM16 decoding *************/
		}
		else if (MODULATION == QAM16){
			detected_signal[index] = (temp[0] >= 0.0) ? 1 : 0;
			detected_signal[index + 1] = (-2.0*OneBySqrt10 <= temp[0] && temp[0] <= 2.0*OneBySqrt10) ? 1 : 0;
			detected_signal[index + 2] = (temp[1] >= 0.0) ? 1 : 0;
			detected_signal[index + 3] = (-2.0*OneBySqrt10 <= temp[1] && temp[1] <= 2.0*OneBySqrt10) ? 1 : 0;
		}

		/************************ Hard QAM64 Mod decoding *************/

		else if (MODULATION == QAM64){
			if (temp[0] > 0.0)detected_signal[index] = 1;
			else detected_signal[index] = 0;


			/*.....................*/
			if (fabs(temp[0]) > 4.0 / sqrt(42.0)){
				detected_signal[index + 1] = 0;
				if (fabs(temp[0]) > 6.0 / sqrt(42.0)) detected_signal[index + 2] = 0;
				else detected_signal[index + 2] = 1;
			}
			/*.....................*/
			else{
				detected_signal[index + 1] = 1;
				if (fabs(temp[0]) > 2.0 / sqrt(42.0)) detected_signal[index + 2] = 1;
				else detected_signal[index + 2] = 0;
			}

			/************************ ********************** *************/

			if (temp[1] > 0.0) detected_signal[index + 3] = 1;
			else detected_signal[index + 3] = 0;

			/*.....................*/
			if (fabs(temp[1]) > 4.0 / sqrt(42.0)){
				detected_signal[index + 4] = 0;
				if (fabs(temp[1]) > 6.0 / sqrt(42.0)) detected_signal[index + 5] = 0;
				else detected_signal[index + 5] = 1;

			}
			/*.....................*/
			else{
				detected_signal[index + 4] = 1;
				if (fabs(temp[1]) > 2.0 / sqrt(42.0)) detected_signal[index + 5] = 1;
				else detected_signal[index + 5] = 0;
			}
		}
		index += MODULATION;
	}
	deinterleaver4HD(detected_signal, diLLR[data_number - Np]);
}
