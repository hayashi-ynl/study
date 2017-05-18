// Archievement2.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
// 2017-05-18 20:20

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>
#include <string.h>
#include <windows.h>

unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

char PathName[_MAX_PATH] = {};	//カレントパス

char masterPass[_MAX_PATH] = "M:\\Ｌ・実績フォルダ\\";	//生データディレクトリ
char masterCpPass[_MAX_PATH] = "C:\\iot_work\\JAMB-Line\\Achievement\\";	//処理結果ディレクトリ

char base_filename[_MAX_PATH] = {};
char cp_filename[_MAX_PATH] = {};

char ini_filename[] = "date.ini";	//iniファイル名
char ini_filepath[_MAX_PATH] = {};	//iniファイルパス

void getSysTime(void);
int fileCopy(void);

int main(void) {
	printf("Program\n\n");

	/*現在時刻をシステムから取得*/
	getSysTime();

	GetCurrentDirectory(_MAX_PATH, PathName);
	printf("%s\n", PathName);	//カレントディレクトリ
	sprintf(ini_filepath, "%s\\%s", PathName, ini_filename);	//日付指定iniファイル用
	
	/*年月日を指定（date.iniファイルが存在する場合）*/
	FILE *date_fp;
	char date_s[10];
	unsigned char date_row;
	date_fp = fopen(ini_filepath, "r");
	if (date_fp == NULL) {
		/*ファイルが存在しない場合はなにもしない*/
		printf("iniファイル無し %s \n", ini_filepath);
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
		printf("iniファイルを利用します\n");
	}

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	FILETIME ft;
	SYSTEMTIME st;

	char findPass[_MAX_PATH] = {};

	/* 最初のファイル検索 */
	printf("検索する日付：%04d年%02d月%02d日\n", n_year, n_month, n_day);
	sprintf(findPass, "%s%04d%02d%02d*.srd", masterPass, n_year, n_month, n_day);
	printf("%sを検索します\n", findPass);

	hFind = FindFirstFile(findPass, &fd);
	
	/* 検索ファイルが見つからない場合 */
	if(hFind == INVALID_HANDLE_VALUE) {
		printf("該当ファイル無し\n\n");
		return EXIT_FAILURE; /******** エラー終了 ********/
	}

	/* 検索結果の表示とファイルの移動(コピー) */
	unsigned char num = 1;	//ファイル数カウンタ
	printf("検索結果：\n");
	//コピー先のディレクトリを年月で作成
	sprintf(masterCpPass, "%s%04d%02d%02d\\", masterCpPass, n_year, n_month, n_day);
	CreateDirectory(masterCpPass, NULL);
	do {
		/* 更新時間取得 */
		FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ft);
		FileTimeToSystemTime(&ft, &st);

		/* 結果の表示 */
		printf(" %d.ファイル名: %s\n", num, fd.cFileName);
		/* ファイルの移動(コピー) */
		sprintf(base_filename, "%s%s", masterPass, fd.cFileName); //コピー元のパス
		sprintf(cp_filename, "%s%s", masterCpPass, fd.cFileName); //コピー先のパス
		fileCopy();

		/* ディレクトリの場合は(ディレクトリ)と表示 */
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			printf("(ディレクトリ)");
		}

		num++;
	} while(FindNextFile(hFind, &fd)); //次のファイルを検索

	/* 検索終了 */
	FindClose(hFind);

	printf("\nEndTheProgram\n");
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

int fileCopy(void) {
	FILE* fpSrc;
	FILE* fpDest;
	char c;


	fpSrc = fopen( base_filename, "rb" );
	if( fpSrc == NULL ){
		printf(" コピー元のファイルオープン失敗。\n");
		exit( EXIT_FAILURE );
	}

	fpDest = fopen( cp_filename, "wb" );
	if( fpDest == NULL ){
		printf(" コピー先のファイルオープンに失敗。\n");
		exit( EXIT_FAILURE );
	}


	while( 1 ){
		/* バイナリデータとして 1Byteずつ読み込み、1Byteずつ書き込む */
		fread( &c, sizeof(c), 1, fpSrc );
		if( feof( fpSrc ) ){
			break;
		}
		if( ferror( fpSrc ) ){
			printf(" バイナリデータとしての読み込みに失敗。\n");
			exit( EXIT_FAILURE );
		}

		fwrite( &c, sizeof(c), 1, fpDest );
	}


	if( fclose( fpDest ) == EOF ){
		printf(" ファイルクローズに失敗しました。\n");
		exit( EXIT_FAILURE );
	}
	if( fclose( fpSrc ) == EOF ){
		printf(" ファイルクローズに失敗しました。\n");
		exit( EXIT_FAILURE );
	}

	return EXIT_SUCCESS;
}
