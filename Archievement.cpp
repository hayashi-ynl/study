// Archievement.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//2017-05-16 15:44

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>
#include <string.h>
#include <windows.h>
//#include <dirent.h> //VisualStudioは利用不可

unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

char masterPass[_MAX_PATH] = "C:\\work\\test\\Achievement\\";
//char masterCpPass[_MAX_PATH] = "C:\\work\\JAMB-Line\\Achievement\\20170405\\";
char masterCpPass[_MAX_PATH] = "C:\\work\\JAMB-Line\\Achievement\\";

//char base_filename[_MAX_PATH] = "test.txt";
//char cp_filename[_MAX_PATH] = "C:\\work\\JAMB-Line\\Achievement\\20170405\\";
char base_filename[_MAX_PATH] = {};
char cp_filename[_MAX_PATH] = {};

void getSysTime(void);
int fileCopy(void);

int main(void) {
	printf("JAMB-ArchievementPasserProgram\n\n");

	/*現在時刻をシステムから取得*/
	getSysTime();

	/*パスを用意*/
	char pass1[_MAX_PATH] = "C:\\work\\test\\Achievement\\";
	char yearPath[5];
	char pass2[] = "\\";
	char monthPath[3];
	char pass3[] = "\\";
	char dayPath[3];
	char pass4[] = ".csv";

	/*年月日を指定（date.iniファイルが存在する場合）*/
	FILE *date_fp;
	char date_filename[] = "date.ini";
	char date_s[10];
	unsigned char date_row;
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
	

	/*年月日を文字列に変換*/
	sprintf(yearPath, "%04d", n_year);
	sprintf(monthPath, "%02d", n_month);
	sprintf(dayPath, "%02d", n_day);

	/*完全パスになるよう結合*/
	//strcat(pass1, yearPath);  /* 文字列連結(pass$ = pass$ + pass2$) */
	//strcat(pass1, pass2);  /* 文字列連結(pass$ = pass$ + pass2$) */
	//strcat(pass1, monthPath);  /* 文字列連結(pass$ = pass$ + pass2$) */
	//strcat(pass1, pass3);  /* 文字列連結(pass$ = pass$ + pass2$) */
	//strcat(pass1, dayPath);  /* 文字列連結(pass$ = pass$ + pass2$) */
	//strcat(pass1, pass4);  /* 文字列連結(pass$ = pass$ + pass2$) */

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	FILETIME ft;
	SYSTEMTIME st;

	char findPass[_MAX_PATH] = {};

	/* 最初のファイル検索 */
	//hFind = FindFirstFile("\\my documents\\*.*", &fd);
	//hFind = FindFirstFile("C:\\*.*", &fd);
	//hFind = FindFirstFile("C:\\work\\test\\Achievement\\*.*", &fd);
	//hFind = FindFirstFile("C:\\work\\test\\Achievement\\", &fd); //NG
	//hFind = FindFirstFile("C:\\work\\test\\Achievement\\201703*.srd", &fd); //OK
	printf("検索する日付：%04d年%02d月%02d日\n", n_year, n_month, n_day);
	sprintf(findPass, "C:\\work\\test\\Achievement\\%04d%02d%02d*.srd", n_year, n_month, n_day);

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
		//コピー元のパスをbase_filename[]へ格納
		sprintf(base_filename, "%s%s", masterPass, fd.cFileName);
		//コピー先のパスをcp_filename[]へ格納
		sprintf(cp_filename, "%s%s", masterCpPass, fd.cFileName);
		//strcat(cp_filename, fd.cFileName); //NG
		//fileCopy()を実行
		fileCopy();

		//if (fileCopy() == EXIT_FAILURE) {
		//	printf(" ファイルクローズに失敗しました。\n");
		//	return EXIT_FAILURE;
		//}

		/* ディレクトリの場合は(ディレクトリ)と表示 */
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			printf("(ディレクトリ)");
		}

		//printf("\n短いファイル名: %s\n", fd.cAlternateFileName);
		//printf("ファイルサイズ: %d\n", fd.nFileSizeLow);
		//printf("更新日: %04d/%02d/%02d %02d:%02d:%02d\n\n",
		//	st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

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
	//	fputs( "ファイルクローズに失敗しました。\n", stderr );
		printf(" ファイルクローズに失敗しました。\n");
		exit( EXIT_FAILURE );
	}
	if( fclose( fpSrc ) == EOF ){
	//	fputs( "ファイルクローズに失敗しました。\n", stderr );
		printf(" ファイルクローズに失敗しました。\n");
		exit( EXIT_FAILURE );
	}

	return EXIT_SUCCESS;
}
