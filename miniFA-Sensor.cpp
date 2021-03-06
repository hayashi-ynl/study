﻿// miniFA-Sensor.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//2017-05-17 17:55
//本プログラムはミニFAラインVIP用

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>
#include <string.h>
#include <math.h>

#define SL1		60	//閾値1VIP-IN
#define SL2		30	//閾値2VIP-OUT
#define SL3		180	//閾値3

#define VIP_IN		0
#define VIP_OUT		1

#define SAME		0


unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

char first_row[] = "SheetNo,IN-time,OUT-time";

char masterPass[_MAX_PATH] = "C:\\work\\test\\Censor\\";
char masterCpPass[_MAX_PATH] = "C:\\work\\MiniFA-VIP\\Censor\\VIP\\";

char vip_in[] = "sensor06-punch.csv";
unsigned int vip_in_last;
char vip_out[] = "sensor13-punch.csv";
unsigned int vip_out_last;

char d_Pass[_MAX_PATH] = {};
char write_Pass[_MAX_PATH] = {};
char comp[] = "0\n";

char sheetNo[500] = {};
char dt_in_org[500][30] = {};	//元データ
char dt_out_org[500][30] = {};	//元データ
char dt_in[500][30] = {};		//時刻見直し後
char dt_out[500][30] = {};		//時刻見直し後

void getSysTime(void);
void read_csv(unsigned char sw);
unsigned int calculation(void);
void write_csv(unsigned int w_last_i);

int main(void) {

	printf("ProgramStart\n\n");

	/*現在時刻をシステムから取得*/
	getSysTime();

	/*年月日を指定（date.iniファイルが存在する場合）*/
	FILE *date_fp;
	char date_filename[] = "date.ini";
	char date_s[10];
	unsigned char date_row;
	unsigned int last_i;

	date_fp = fopen(date_filename, "r");
	if (date_fp == NULL) {
		/*ファイルが存在しない場合はなにもしない*/
	}
	else {
		/*ファイルが存在する場合は指定の年月日に書き換える*/
		for (date_row = 1; fgets(date_s, sizeof(date_s), date_fp) != NULL; date_row++) {
			switch (date_row) {
			case 1:
				n_year = atoi(date_s);
				break;
			case 2:
				n_month = atoi(date_s);
				break;
			case 3:
				n_day = atoi(date_s);
				break;
			default:
				break;
			}
		}
		fclose(date_fp);	//elseの場合fileOpenするのでclosing
	}
	sprintf(d_Pass, "%s%04d\\%02d\\%02d\\", masterPass, n_year, n_month, n_day);
	sprintf(write_Pass, "%s%04d%02d%02d-VIP.cen", masterCpPass, n_year, n_month, n_day);

	read_csv(VIP_IN);
	read_csv(VIP_OUT);

	last_i = calculation();

	write_csv(last_i);

	printf("EndTheProgram\n");
	return EXIT_SUCCESS;
}

void getSysTime(void) {
	time_t timer; struct tm *local;

	/* 現在時刻を取得 */
	timer = time(NULL);

	local = localtime(&timer); /* 地方時に変換 */

							   /* 地方時 変換後表示 */
	printf("処理時刻: ");

	n_year = local->tm_year + 1900;
	n_month = local->tm_mon + 1;
	n_day = local->tm_mday;

	printf("%04d/", n_year);
	printf("%02d/", n_month);
	printf("%02d ", n_day);
	printf("%02d:", local->tm_hour);
	printf("%02d:", local->tm_min);
	printf("%02d", local->tm_sec);
	printf("\n");
}
void read_csv(unsigned char sw)
{
	/*ファイルオープン*/
	FILE *vip_fp;
	char tmp_pass[_MAX_PATH] = {};
	char read_s[200] = {};
	char tok_s[500] = {};
	char split[] = ",";
	char *tok;
	unsigned int row, col, i;
	int result = 0;
	int year1, month1, hour1, day1, min1, sec1;
	int year2, month2, hour2, day2, min2, sec2;
	int str_length;
	struct tm cmpDate1;
	struct tm cmpDate2;
	time_t time1;
	time_t time2;
	double result_time;

	//選択処理(VIP_IN　VIP_OUT)
	if(sw == VIP_IN){
		sprintf(tmp_pass, "%s%s", d_Pass, vip_in);
	}else if(sw == VIP_OUT){
		sprintf(tmp_pass, "%s%s", d_Pass, vip_out);
	}

	vip_fp = fopen(tmp_pass, "r");
	if (vip_fp == NULL) {
		printf("%s Couldn't Open¥n", tmp_pass);
		exit(-1);
	}

	i = 0;
	for (row = 1; fgets(read_s, sizeof(read_s), vip_fp) != NULL; row++) {
		if (row == 1) {  /*最初の行*/
			//なにもしない
		}
		else {
			tok = strtok(read_s, split); /* 列読み出し */
			for (col = 1; tok != NULL; col++) {
			//	a = strcmp(tok, comp);
				if(col == 1){ //1列目
					//選択処理(VIP_IN　VIP_OUT)
					if(sw == VIP_IN){
						strcpy(&dt_in_org[i][0], tok);
					}else if(sw == VIP_OUT){
						strcpy(&dt_out_org[i][0], tok);
					}
					i++; //配列を進める
			//	} else if(col == 5 && strcmp(tok, comp) == SAME ){ //value=0の時は廃棄
				} else if(col == 5){ //5列目
					if(strcmp(tok, comp) == SAME){ //value=0 -> 廃棄処理
						i--;
						if(sw == VIP_IN){
							str_length = strlen(&dt_in_org[i][0]);
							memset(&dt_in_org[i][0], '\0', str_length);	//配列を初期化
						}else if(sw == VIP_OUT){
							str_length = strlen(&dt_out_org[i][0]);
							memset(&dt_out_org[i][0], '\0', str_length);	//配列を初期化
						}
					}else{ //value=1 -> 時差次第では廃棄
						if(i >= 2){ //比較結果次第で確定or破棄
							//選択処理(VIP_IN　VIP_OUT)
							if(sw == VIP_IN){
								//2-1の時間を算出
								sscanf(&dt_in_org[i-1][0], "%04d-%02d-%02d %02d:%02d:%02d", &year1, &month1, &day1, &hour1, &min1, &sec1);
								cmpDate1.tm_sec   = sec1;
								cmpDate1.tm_min   = min1;
								cmpDate1.tm_hour  = hour1;
								cmpDate1.tm_mday  = day1;
								cmpDate1.tm_mon   = month1 - 1;
								cmpDate1.tm_year  = year1 - 1900;
								cmpDate1.tm_isdst = -1;
								time1 = mktime(&cmpDate1);

								sscanf(&dt_in_org[i-2][0], "%04d-%02d-%02d %02d:%02d:%02d", &year2, &month2, &day2, &hour2, &min2, &sec2);
								cmpDate2.tm_sec   = sec2;
								cmpDate2.tm_min   = min2;
								cmpDate2.tm_hour  = hour2;
								cmpDate2.tm_mday  = day2;
								cmpDate2.tm_mon   = month2 - 1;
								cmpDate2.tm_year  = year2 - 1900;
								cmpDate2.tm_isdst = -1;
								time2 = mktime(&cmpDate2);

								result_time = difftime(time1, time2);
								//算出時間がSL1よりも小さいなら破棄
								if(fabs(result_time) < SL1){
									i--;	//破棄
									str_length = strlen(&dt_in_org[i][0]);
									memset(&dt_in_org[i][0], '\0', str_length);	//配列を初期化
								}
							}else if(sw == VIP_OUT){
								//2-1の時間を算出
								sscanf(&dt_out_org[i-1][0], "%04d-%02d-%02d %02d:%02d:%02d", &year1, &month1, &day1, &hour1, &min1, &sec1);
								cmpDate1.tm_sec   = sec1;
								cmpDate1.tm_min   = min1;
								cmpDate1.tm_hour  = hour1;
								cmpDate1.tm_mday  = day1;
								cmpDate1.tm_mon   = month1 - 1;
								cmpDate1.tm_year  = year1 - 1900;
								cmpDate1.tm_isdst = -1;
								time1 = mktime(&cmpDate1);

								sscanf(&dt_out_org[i-2][0], "%04d-%02d-%02d %02d:%02d:%02d", &year2, &month2, &day2, &hour2, &min2, &sec2);
								cmpDate2.tm_sec   = sec2;
								cmpDate2.tm_min   = min2;
								cmpDate2.tm_hour  = hour2;
								cmpDate2.tm_mday  = day2;
								cmpDate2.tm_mon   = month2 - 1;
								cmpDate2.tm_year  = year2 - 1900;
								cmpDate2.tm_isdst = -1;
								time2 = mktime(&cmpDate2);

								result_time = difftime(time1, time2);
								//算出時間がSL2よりも小さいなら破棄
								if(fabs(result_time) < SL2){
									i--;	//破棄
									str_length = strlen(&dt_out_org[i][0]);
									memset(&dt_out_org[i][0], '\0', str_length);	//配列を初期化
								}
							}
						}
					}
				}
				tok = strtok(NULL, split); /* 次の列を読み出し */
			}
		}
	}

	//選択処理(VIP_IN　VIP_OUT)
	if(sw == VIP_IN){
		vip_in_last = i;
	}else if(sw == VIP_OUT){
		vip_out_last = i;
	}

	fclose(vip_fp);


}
unsigned int calculation(void)
{
	int year1, month1, hour1, day1, min1, sec1;
	int year2, month2, hour2, day2, min2, sec2;
	struct tm cmpDate1;
	struct tm cmpDate2;
	time_t time1;
	time_t time2;
	double dt_time_interval_1st; //秒で格納
	double dt_time_interval_2nd; //秒で格納

	unsigned int i_in = 0;
	unsigned int i_out = 0;
	unsigned int i_in_1st, i_in_2nd;
	unsigned int i_last;
	
	unsigned int l;
	for (l = 0; l < 9999; l++) {
		if ( (i_out - vip_out_last) >= 0){ //OUTが配列最後ならば、ループを抜ける
			i_last = l - 1;
			break;
		}

		if(vip_in_last - i_in <= 1){ //INの配列がLAST
			i_in_1st = i_in;
			i_in_2nd = vip_in_last;
		}else{
			i_in_1st = i_in;
			i_in_2nd = i_in + 1;
		}

		//IN-OUTの時間差(1st)
		sscanf(&dt_out_org[i_out][0], "%04d-%02d-%02d %02d:%02d:%02d", &year1, &month1, &day1, &hour1, &min1, &sec1);
		cmpDate1.tm_sec   = sec1;
		cmpDate1.tm_min   = min1;
		cmpDate1.tm_hour  = hour1;
		cmpDate1.tm_mday  = day1;
		cmpDate1.tm_mon   = month1 - 1;
		cmpDate1.tm_year  = year1 - 1900;
		cmpDate1.tm_isdst = -1;
		time1 = mktime(&cmpDate1);

		sscanf(&dt_in_org[i_in_1st][0], "%04d-%02d-%02d %02d:%02d:%02d", &year2, &month2, &day2, &hour2, &min2, &sec2);
		cmpDate2.tm_sec   = sec2;
		cmpDate2.tm_min   = min2;
		cmpDate2.tm_hour  = hour2;
		cmpDate2.tm_mday  = day2;
		cmpDate2.tm_mon   = month2 - 1;
		cmpDate2.tm_year  = year2 - 1900;
		cmpDate2.tm_isdst = -1;
		time2 = mktime(&cmpDate2);

		dt_time_interval_1st = difftime(time1, time2);
		//IN-OUTの時間差(2nd)
		sscanf(&dt_out_org[i_out][0], "%04d-%02d-%02d %02d:%02d:%02d", &year1, &month1, &day1, &hour1, &min1, &sec1);
		cmpDate1.tm_sec   = sec1;
		cmpDate1.tm_min   = min1;
		cmpDate1.tm_hour  = hour1;
		cmpDate1.tm_mday  = day1;
		cmpDate1.tm_mon   = month1 - 1;
		cmpDate1.tm_year  = year1 - 1900;
		cmpDate1.tm_isdst = -1;
		time1 = mktime(&cmpDate1);

		sscanf(&dt_in_org[i_in_2nd][0], "%04d-%02d-%02d %02d:%02d:%02d", &year2, &month2, &day2, &hour2, &min2, &sec2);
		cmpDate2.tm_sec   = sec2;
		cmpDate2.tm_min   = min2;
		cmpDate2.tm_hour  = hour2;
		cmpDate2.tm_mday  = day2;
		cmpDate2.tm_mon   = month2 - 1;
		cmpDate2.tm_year  = year2 - 1900;
		cmpDate2.tm_isdst = -1;
		time2 = mktime(&cmpDate2);

		dt_time_interval_2nd = difftime(time1, time2);

		if(dt_time_interval_1st < SL3){
			//加工時間が規定以下・・・２部品以上の加工
			if (l == 0){
				//最初のIN時刻がOUT時刻より遅い場合
				strcpy(&dt_in[l][0], &dt_out_org[i_out][0]); //IN側にOUTのデータをコピーする
				strcpy(&dt_out[l][0], &dt_out_org[i_out][0]);
			}else{
				strcpy(&dt_in[l][0], &dt_in[l - 1][0]); //IN側にひとつ前のデータをコピーする
				strcpy(&dt_out[l][0], &dt_out_org[i_out][0]);
			}

			i_out++;	//out側のみ加算

		}else{
			if (dt_time_interval_2nd < SL3)	{
				//次のIN時刻との差が負か規定値よりも小さければ正常
				strcpy(&dt_in[l][0], &dt_in_org[i_in][0]); 
				i_in++;
				
				strcpy(&dt_out[l][0], &dt_out_org[i_out][0]);
				i_out++;
			}else{
				//次のIN時刻との差が大きい場合はINが2回反応したかOUTの検出漏れ
				//→OUTの時刻を次回の値から複写する
				strcpy(&dt_in[l][0], &dt_in_org[i_in][0]);
				i_in++;

				strcpy(&dt_out[l][0], &dt_out_org[i_out][0]);
				//i_outはカウントアップしない
			}
		}
	}
	return i_last;
}
void write_csv(unsigned int w_last_i)
{
	FILE *w_fp;
	char tmp_pass[_MAX_PATH] = {};
	unsigned int j;
	w_fp = fopen(write_Pass, "w");
	if (w_fp == NULL) {
		printf("%s Couldn't Open¥n", write_Pass);
		exit(-2);
	}

	fprintf(w_fp, "%s", first_row); //ファイルへ書込み
	for (j = 0; j < w_last_i; j++)
	{
		fprintf(w_fp, "%d,%s,%s", sheetNo, &dt_in[j][0], &dt_out[j][0]); //SheetNo,IN-time,OUT-timeの順に書込み
	}
	fclose(w_fp);
}
