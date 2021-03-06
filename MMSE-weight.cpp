#include "const-function-declerations.h"		
extern double CNR, channel_power[N][2];
double channel_gain[N][2], p_in[N];

void MMSE_Weight_Matrix(double(*H)[TS][BS][2], double(*W)[BS][CL][2]){
	int rx, tx, tx2, f_index, i;
	double WH[N][BS][TS][2];
	double g_i[BS][2], g_iH[BS][2], power_i[2], g[TS][2], gH[TS][2], power[2], no, no_loss;
	double HH[BS][TS][2], Rt[BS][BS][2], I[BS][BS][2], R[BS][BS][2], W_temp[N][BS][TS][2];


	/* This one looks like numerical CNR calculation, while taking into account the coding rate */
	no = pow(10.0, -CNR / 10.0) / CODING_RATE;	/* Noise power. if you assume one for signal power... */

	for (f_index = 0; f_index < N; f_index++){

		/* Noise autocorrelation matrix	--------------------------- */
		for (tx = 0; tx < BS; tx++){
			for (tx2 = 0; tx2 < BS; tx2++){
				if (tx == tx2){
					I[tx][tx2][0] = no;
					I[tx][tx2][1] = 0.0;
				}
				else{
					I[tx][tx2][0] = 0.0;
					I[tx][tx2][1] = 0.0;
				}
			}
		}
		/* -------------------------------------------- */

		/* Weight vector is calculated for each frequency index. */
		/* Calculate H^H x (H^H x H + I)^-1 x H */
		MatrixHermite_TSxBS(H[f_index], HH);	/* H^H = H^*T */
		MatrixMulMatrixToMatrix_BSxTS(HH, H[f_index], Rt);	/* Rt = H^H x H */
		MatrixAddMatrixToMatrix_BSxBS(Rt, I, Rt);	/* Rt = Rt + I */
		MatrixInverse_BSxBS(Rt);	/* Rt = (Rt + I)^-1 */
		MatrixMulMatrixToMatrix_BSxBSXBSxTS(Rt, HH, W_temp[f_index]);	/* W = (Rt + I)^-1 x H^H */

		for (tx = 0; tx < BS; tx++){
			for (rx = 0; rx < TS; rx++){
				for (i = 0; i < 2; i++){
					WH[f_index][tx][rx][i] = W_temp[f_index][tx][rx][i];	/* WH = W */
				}
			}
		}
		MatrixMulMatrixToMatrix_BSxTS(WH[f_index], H[f_index], R);	/* R = (H^H x H + I)^-1 x H^H x H = (Rt + I)^-1 x Rt */
		/* -------------------------------------------- */

#ifdef SD
		for (tx2 = 0; tx2 < M; tx2++){	/* M = 2 for curent state, R[1][1] */
			if (tx2 == 0){
				g_i[tx2][0] = 0.0;
				g_i[tx2][1] = 0.0;
			}
			else {
				g_i[tx2][0] = R[0][tx2][0];
				g_i[tx2][1] = R[0][tx2][1];
			}
		}
		VectorConjugate_M(g_i, g_iH);	/* This function defined to work only with two element vectors */
		VectorMulVectorToScalar_M(g_iH, g_i, power_i);	/* power_i = R^2? */
		/*----------------------------skip*/
		for (rx = 0; rx < TS; rx++){	/* g = WH(1,:) */
			g[rx][0] = WH[f_index][0][rx][0];
			g[rx][1] = WH[f_index][0][rx][1];
		}
		//g[0][0] = 0;
		//g[0][1] = 0;
		//g[1][0] = R[0][0][0];
		//g[1][1] = R[0][0][1];
		VectorConjugate_TS(g, gH);
		VectorMulVectorToScalar_TS(gH, g, power);
		/*---------------------------------------------skipend*/
		power[0] = WH[f_index][0][0][0] * WH[f_index][0][0][0] + WH[f_index][0][0][1] * WH[f_index][0][0][1];
		power[1] = WH[f_index][0][1][0] * WH[f_index][0][1][0] + WH[f_index][0][1][1] * WH[f_index][0][1][1];
		channel_gain[f_index][0] = R[0][0][0];
		channel_gain[f_index][1] = -R[0][0][1];	/* Signal received by first user */
		//p_in[f_index] = no * power[0] + power_i[0];	/* Incoming signal power. */
		//p_in[f_index] = sqr(no)*power[0];	/* Incoming signal power. */
		//p_in[f_index] = (sqr(no)+1)*power[0];	 		
		//p_in[f_index] = power[0] + power_i[0] + 1;	/* 0.5 = no */
		//p_in[f_index] = power[0];
		//p_in[f_index] = power[0] + power_i[0] / no;

		no = pow(10.0, -CNR / 10.0) / CODING_RATE;
		no_loss = pow(10.0, -(CNR - LOSS) / 10.0) / CODING_RATE;
		//p_in[f_index] = (channel_power[f_index][0]) * power[0] + power_i[0];
		p_in[f_index] = (no_loss)*power[0] + (no)*power[1] + power_i[0];
#endif // SD

	}
}
