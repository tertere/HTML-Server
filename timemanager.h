#pragma once
#include <ctime>
#include <string>

//Файл для получения и отслеживания времени, TODO полноценный тайм-менеджер

void UpdateTime(time_t& crtime, tm& timestruct, char* buf) {
	crtime = time(NULL);
	gmtime_s(&timestruct, &crtime);
	strftime(buf, 26, "%a, %d %b %Y %H:%M:%S", &timestruct);
}

string NowTime() {
	time_t CurrTime;
	CurrTime = time(NULL);
	tm tm_gmt;
	char buf[26];
	UpdateTime(CurrTime, tm_gmt, buf);
	string res = "[";
	res += buf;
	res += "]";
	return res;
}


