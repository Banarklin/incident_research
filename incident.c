/*this code is for testing stock shock AR and CAR for incidents*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


// 這裡的值需要-260~-11天的數據
#define RISK_FREE_RATE_MSFT 0.00425
#define RISK_FREE_RATE_SNY 0.0057
#define RISK_FREE_RATE_ATVI 0.00425
#define RISK_FREE_RATE_UBI_FY 0.00425
#define RISK_FREE_RATE_US 0.00689
#define BETA_SNY 0.81
#define BETA_ATVI 0.43
#define BETA_UBI_FY 0.08
#define BETA_MSFT 0.94

#define N 7
#define estimate_time 250
#define test_time 21


// this function is used to calculate beta of every company relative to market
float expected_value_beta (float* firm_return_rate, float* market_return_rate, int num, 
                            float market_avg, float firm_avg) {
    float beta = 0;
    // calculate beta
    //float relativity = 0;
    float XY = 0;
    float XX = 0;
    for (int i = 0; i < num; i++) {
        XY += (market_return_rate[i] - market_avg) * (firm_return_rate[i] - firm_avg);
        XX += (market_return_rate[i] - market_avg) * (market_return_rate[i] - market_avg);
    }
    beta = XY / XX;
    
    return beta;
} 

// this function is used to calculate alpha of every company relative to market
float expected_value_alpha (float beta, float market_avg, float firm_avg) {
    float alpha = firm_avg - beta * market_avg;
    return alpha;
}

char *stock[] = {"Blizzard", "Microsoft",  "Ubisoft", "Sony",  "Tencent", "Nitendo", "Apple"};
char *indexName[] = {"S&P500", "S&P500", "S&P500", "S&P500"};
int date[] = {-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//float actual_return_rate[7][21];
float abnormalReturnArr[N][test_time];
float CAR[test_time] = {0};
float AAR[test_time] = {0};
int main(){
    FILE* fp = fopen("s&p500_1.txt", "r");
    FILE* out = fopen("output_1.txt", "w");
    if(fp == NULL) printf("Error opening file.\n");
    //float openStockPrice = 0, closeStockPrice = 0;
    float beta[N] = {0};
    float riskFreeRate[N] = {0};
    float Avg_market_return = 0;
    float expected_market_return_rate [estimate_time] = {0};
    float total_epson_sqrt[N] = {0};
    // scan s&p500 data
    while(!feof(fp)){
        char company[30];
        float temp;
        //float oPrice = 0, cPrice = 0;
        fscanf(fp, "%[^\n]", company); 
        if(strcmp(company, "S&P500") == 0){
            printf("Now investigating %s:\n", company);
            for(int i = 0; i < estimate_time; i++){
                fscanf(fp, "%f", &temp);
                expected_market_return_rate[estimate_time-1-i] = temp;
                Avg_market_return += expected_market_return_rate[i];
                //fprintf(out, "expected return of market is %f\n", expected_market_return_rate[estimate_time-1-i]);
            }
            Avg_market_return /= estimate_time;
            printf("AVG_market_return is   %f \n", Avg_market_return);
            break;
        }
        break;
    }
    //char ch;
    //fscanf(fp, "%c", &ch);

    // scan company rate 1~7
    for(int i = 0; i < N; i++){
        char company[30];
        float temp;
        float firm_return_rate[estimate_time] = {0};
        float actual_return_rate[test_time] = {0};
        float Avg_firm_return = 0;
        fscanf(fp, "%s", company);
        if (strcmp(company, "S&P500") == 0) {
            printf("there no company!!\n");
            break;
        }
        stock[i] = company;
        printf("Now investigating %s\n", company);


        for(int j = 0; j < test_time; j++) {
            fscanf(fp, "%f", &temp);
            actual_return_rate[20-j] = temp;
        }            
        for(int j = 0; j < estimate_time; j++) {
            fscanf(fp, "%f", &temp);
            firm_return_rate[estimate_time-1-j] = temp;
            Avg_firm_return += temp;
        }

        Avg_firm_return /= estimate_time;
        // calculate beta and alpha 1~ 7
        beta[i] = expected_value_beta(firm_return_rate, expected_market_return_rate, estimate_time, Avg_market_return, Avg_firm_return);
        riskFreeRate[i] = expected_value_alpha(beta[i], Avg_market_return, Avg_firm_return);
        
        for (int k = 0; k < estimate_time; k++) {
            total_epson_sqrt[k] += (firm_return_rate[k] - riskFreeRate[i] - beta[i] * expected_market_return_rate[k]) 
                                    * (firm_return_rate[k] - riskFreeRate[i] - beta[i] * expected_market_return_rate[k]);
        }


        //calculate AR of 1~7
        for (int j = 0; j < test_time; j++) {
            abnormalReturnArr[i][j] = actual_return_rate[j] - (riskFreeRate[i] + beta[i] * Avg_market_return);
            printf("%s 's %d days of AR is %f\n", stock[i], date[j], abnormalReturnArr[i][j]);
        }
    }

    // calculate avg AAR
    for (int i = 0; i < test_time; i++) {
        for (int j = 0; j < N; j++) {
            AAR[i] += abnormalReturnArr[j][i];
        }
        AAR[i] /= N;
    }


    // calculate 
    for(int i = 0; i < test_time; i++){
        if (i!=0)
            CAR[i] = CAR[i-1] + AAR[i];
        else CAR[i] = AAR[i];
        fprintf(out, "%d %f %f\n", date[i], AAR[i], CAR[i]);

        //fprintf(out, "CAR for %s stock is %f%%.\n", stock[i], CAR[i][test_time-1] * 100);
        /*float avgCAR = CAR_temp / test_time;
        float ARSquareSum = 0;
        for(int j = 0; j < test_time; j++){
            ARSquareSum += (abnormalReturnArr[i][j] - avgCAR) * (abnormalReturnArr[i][j] - avgCAR);
        }
        float SD = sqrt(ARSquareSum / 13);
        float tValue = avgCAR / (SD * sqrt(14));*/
        //fprintf(out, "t-value for %s stock is %f.\n", stock[i], tValue);
    }

    // T-crossection test
    float T_cross[test_time] = {0};
    float RM_AR = 0;
    for (int i = 0; i < test_time; i++) {
        for (int j = 0; j < N; j++) {
            RM_AR += (abnormalReturnArr[j][i] - AAR[i]) * (abnormalReturnArr[j][i] - AAR[i]);
        }
        T_cross[i] = AAR[i] / sqrt(RM_AR) * sqrt(N*(N-1));
        printf("%f of data %d\n", T_cross[i], i);
    }


    // T_sign_test
    float T_sign[test_time] = {0};
    for (int i = 0; i < test_time; i++) {
        int count = 0; 
        for (int j = 0; j < N; j++) {
            if (abnormalReturnArr[j][i] > 0) count++;
            //printf("%f , %d\n", abnormalReturnArr[j][i], count);
        }
        float temp = count/(float)N;
        float temp1 = temp - 0.5;
        T_sign[i] = temp1 / sqrt(0.25/(float)N);
        printf("%f of data %d  %d   %f\n", T_sign[i], i,count,  temp);
    }

    // T_tranditional 
    float denom = 0;
    for (int j = 0; j < N; j++) {
        denom += total_epson_sqrt[j];
    }
    denom = denom / (estimate_time-2);

    float T_tradition[test_time] = {0};
    for (int i = 0; i < test_time; i++) {
        T_tradition[i] = AAR[i] * (float)N / sqrt(denom);
        printf("%f of data %d\n", T_tradition[i], i);
    }

    for (int i = 0; i < test_time; i++) 
        fprintf(out, "%d %f %f %f %f %f\n", date[i], AAR[i], CAR[i], T_tradition[i], T_cross[i], T_sign[i]);
    return 0;
}
