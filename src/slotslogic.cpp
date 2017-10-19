//
// Created by zhs007 on 2016/12/26.
//

#include "slotslogic.h"

void PayTables::init(std::vector<PaytablesNode>& lstpaytables) {
    int w = 5;
    int h = (int)lstpaytables.size();
    init(w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            lst[y][x] = lstpaytables[y].payout[x];
        }
    }
}

void PayTables::init(int w, int h) {
    lst = (int**)malloc(w * h * sizeof(int) + h * sizeof(int*));
    char* bp = (char*)lst + h * sizeof(int*);

    for (int i = 0; i < h; ++i) {
        lst[i] = (int*)(bp + w * sizeof(int) * i);
    }

    width = w;
    height = h;
}

void loadPayTables5(PayTables& payout, const char* filename) {
    // CSVData csv;
    // csv.load(filename);

    // payout.init(5, csv.m_height);

    // for (int y = 0; y < csv.m_height; ++y) {
    //     int s = atoi(csv.m_lst[y][0]);

    //     payout.lstSymbolCode.push_back(csv.m_lst[y][1]);

    //     int p1 = atoi(csv.m_lst[y][2]);
    //     int p2 = atoi(csv.m_lst[y][3]);
    //     int p3 = atoi(csv.m_lst[y][4]);
    //     int p4 = atoi(csv.m_lst[y][5]);
    //     int p5 = atoi(csv.m_lst[y][6]);

    //     payout.lst[s][0] = p1;
    //     payout.lst[s][1] = p2;
    //     payout.lst[s][2] = p3;
    //     payout.lst[s][3] = p4;
    //     payout.lst[s][4] = p5;
    // }
}

void loadPayTables7(PayTables& payout, const char* filename) {
    // CSVData csv;
    // csv.load(filename);

    // payout.init(8, csv.m_height);

    // for (int y = 0; y < csv.m_height; ++y) {
    //     int s = atoi(csv.m_lst[y][0]);

    //     int p1 = atoi(csv.m_lst[y][2]);
    //     int p2 = atoi(csv.m_lst[y][3]);
    //     int p3 = atoi(csv.m_lst[y][4]);
    //     int p4 = atoi(csv.m_lst[y][5]);
    //     int p5 = atoi(csv.m_lst[y][6]);
    //     int p6 = atoi(csv.m_lst[y][7]);
    //     int p7 = atoi(csv.m_lst[y][8]);
    //     int p8 = atoi(csv.m_lst[y][9]);

    //     payout.lst[s][0] = p1;
    //     payout.lst[s][1] = p2;
    //     payout.lst[s][2] = p3;
    //     payout.lst[s][3] = p4;
    //     payout.lst[s][4] = p5;
    //     payout.lst[s][5] = p6;
    //     payout.lst[s][6] = p7;
    //     payout.lst[s][7] = p8;
    // }
}

void Wheel::init(int w, int h) {
    lstArr = (int**)malloc(w * h * sizeof(int) + h * sizeof(int*));
    char* bp = (char*)lstArr + h * sizeof(int*);

    for (int i = 0; i < h; ++i) {
        lstArr[i] = (int*)(bp + w * sizeof(int) * i);
    }

    lstLength = (int*)malloc(w * sizeof(int*));
    for (int i = 0; i < w; ++i) {
        lstLength[i] = 0;
    }

    width = w;
    height = h;
}

int Wheel::getSymbol(int x, int y) {
    if (x >= 0) {
        if (x < width) {
            while (y < 0) {
                y += lstLength[x];
            }

            while (y >= lstLength[x]) {
                y -= lstLength[x];
            }

            return lstArr[y][x];
        }
    }

    return -1;
}

void loadWheel5(Wheel& wheel, const char* filename) {
    // CSVData csv;
    // csv.load(filename);

    // wheel.init(5, csv.m_height);

    // for (int y = 0; y < csv.m_height; ++y) {

    //     for (int x = 0; x < 5; ++x) {
    //         int pv = atoi(csv.m_lst[y][x + 1]);

    //         if (pv != -1) {
    //             wheel.lstArr[wheel.lstLength[x]][x] = pv;
    //             wheel.lstLength[x]++;
    //         }
    //     }
    // }
}

void LineInfo::init(int w, int h) {
    lst = (int**)malloc(w * h * sizeof(int) + h * sizeof(int*));
    char* bp = (char*)lst + h * sizeof(int*);

    for (int i = 0; i < h; ++i) {
        lst[i] = (int*)(bp + w * sizeof(int) * i);
    }

    width = w;
    height = h;
}

void loadLineInfo5(LineInfo& li, const char* filename) {
    // CSVData csv;
    // csv.load(filename);

    // li.init(5, csv.m_height);

    // for (int y = 0; y < csv.m_height; ++y) {
    //     //int s = atoi(csv.m_lst[y][0]);

    //     int p1 = atoi(csv.m_lst[y][1]);
    //     int p2 = atoi(csv.m_lst[y][2]);
    //     int p3 = atoi(csv.m_lst[y][3]);
    //     int p4 = atoi(csv.m_lst[y][4]);
    //     int p5 = atoi(csv.m_lst[y][5]);

    //     li.lst[y][0] = p1;
    //     li.lst[y][1] = p2;
    //     li.lst[y][2] = p3;
    //     li.lst[y][3] = p4;
    //     li.lst[y][4] = p5;
    // }
}
