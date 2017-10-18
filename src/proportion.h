//
// Created by zhs007 on 2017/2/28.
//

#ifndef EXSLOTS_PROPORTION_H
#define EXSLOTS_PROPORTION_H

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include "slotslogic.h"

// 返奖的等比缩放
void proportionPayout(PayoutInfo& pi, PayoutInfo& dest, int min, int newnums);

// 查找赔率下，最小的块数
bool findMinNums(PayTables& pt, int payout, int& symbol, int& nums);
// 查找赔率下，大一档的块数
bool findNextUp(PayTables& pt, int payout, int srcnums, int& symbol, int& nums);

struct SymbolResultInfo{
    int symbol;
    int nums;
    int payout;

    int right;
};

struct SymbolResultInfoList{
    typedef std::vector<SymbolResultInfo> _List;

    _List lst;
    int totalright;

    void clear() {
        totalright = 0;
        lst.clear();
    }

    void reinitRight() {
        totalright = 0;
        for (int i = 0; i < (int)lst.size(); ++i) {
            totalright += lst[i].right;
        }
    }

    int random() {
        if (totalright > 0) {
            int cr = rand() % totalright;

            for (int i = 0; i < (int)lst.size(); ++i) {
                if (lst[i].right > 0) {
                    if (cr < lst[i].right) {
                        return i;
                    }

                    cr -= lst[i].right;
                }
            }
        }

        return -1;
    }

    void add(int symbol, int nums, int payout, int right) {
        SymbolResultInfo sri;

        sri.symbol = symbol;
        sri.nums = nums;
        sri.payout = payout;
        sri.right = right;

        lst.push_back(sri);

        totalright += right;
    }

    int countIndex(int minnums) {
        int nums = 0;
        for (int i = 0; i < (int)lst.size(); ++i) {
            nums += lst[i].nums;
            if (nums >= minnums) {
                return i + 1;
            }
        }

        return -1;
    }

    int countNums() {
        int nums = 0;
        for (int i = 0; i < (int)lst.size(); ++i) {
            nums += lst[i].nums;
        }

        return nums;
    }

    int countNumsEx(int maxindex) {
        if (maxindex <= 0) {
            return 0;
        }

        int nums = 0;
        for (int i = 0; i < (int)lst.size() && i < maxindex; ++i) {
            nums += lst[i].nums;
        }

        return nums;
    }

    void buildDownList(std::vector<int>& dl, int beginindex, int maxindex, int maxdn) {
//        dl.clear();

        int curbn = 0;
        for (int i = beginindex; i < (int)lst.size() && i < maxindex; ++i) {
            if (curbn == 0) {
                dl.push_back(i);
            }

            curbn += lst[i].nums;
            if (curbn >= maxdn) {
                curbn = 0;
            }
        }
    }

    void output(const char* name) {
        printf("SymbolResultInfoList %s-->\n", name);
        for (int i = 0; i < (int)lst.size(); ++i) {
            printf("s:%d n:%d p:%d r:%d\n", lst[i].symbol, lst[i].nums, lst[i].payout, lst[i].right);
        }
        printf("SymbolResultInfoList %s<--\n", name);
    }

    int countDownNums(int maxbn) {
        int dn = 0;
        int nums = 0;
        for (int i = 0; i < (int)lst.size(); ++i) {
            nums += lst[i].nums;
            if (nums >= maxbn) {
                ++dn;
                nums = 0;
            }
        }

        return dn;
    }

    // 查找小于curindex的最大块，如果-1表示找最大块
    int findMaxNums(int curindex) {
        int curnums = 999;
        if (curindex >= 0) {
            curnums = lst[curindex].nums;
        }

        int nextindex = -1;
        int nextnums = -1;
        for (int i = 0; i < (int)lst.size(); ++i) {
            if (curnums == lst[i].nums) {
                if (i < curindex) {
                    nextindex = i;
                    nextnums = curnums;
                }
            }
            else if (curnums > lst[i].nums) {
                if (lst[i].nums > nextnums) {
                    nextindex = i;
                    nextnums = curnums;
                }
            }
        }

        return nextindex;
    }

    // 不改变赔付的情况下，降低消除个数
    bool makeDown(PayTables& pt, int maxnums) {
        // int off = 0;
        int curnums = countNums();
        if (curnums < maxnums) {
            return true;
        }

        int curindex = -1;
        do {
            curindex = findMaxNums(curindex);
            if (curindex >= 0) {
                int ms, mn;
                if (findMinNums(pt, lst[curindex].payout, ms, mn)) {
                    if (lst[curindex].nums > mn) {
                        lst[curindex].symbol = ms;
                        lst[curindex].nums = mn;

                        curnums = countNums();
                        if (curnums < maxnums) {
                            return true;
                        }
                    }
                }
            }
        } while(curindex >= 0);

        return false;
    }

    // 不改变赔付的情况下，提升消除块数
    // 从第一个开始，每次提升一档
    bool makeUp(PayTables& pt, int maxindex, int minnums) {
        if (maxindex < 0) {
            maxindex = lst.size();
        }

        int curnums = countNumsEx(maxindex);
        if (curnums >= minnums) {
            return true;
        }

        do {
            int thisturnisok = false;
            for (int i = 0; i < maxindex; ++i) {
                int ms, mn;
                if (findNextUp(pt, lst[i].payout, lst[i].nums, ms, mn)) {
                    lst[i].symbol = ms;
                    lst[i].nums = mn;

                    thisturnisok = true;
                }
            }

            int curnums = countNumsEx(maxindex);
            if (curnums >= minnums) {
                return true;
            }

            if (!thisturnisok) {
                return false;
            }
        } while (true);
    }
};

void __procMinMaxPayout(int& minp, int& maxp, int payout);

// 生成该赔付下，所有可能产生的赔付情况
// 必须大于minv
template <int MAX_SYMBOL_NUMS>
void makeTotalResultList7(SymbolResultInfoList& lst, SymbolGenerator<MAX_SYMBOL_NUMS>& geex, PayTables& pt,
                          int totalpayout, int& minp, int& maxp, int minv, int maxbn) {

    lst.clear();
    minp = -1;
    maxp = -1;

    for (int y = 0; y < pt.height; ++y) {
        int cp = pt.lst[y][0];
        if (cp <= 0) {
            continue ;
        }

        cp = pt.lst[y][0];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 5, cp, 1);
        }

        cp = pt.lst[y][1];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 6, cp, 1);
        }

        cp = pt.lst[y][2];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 7, cp, 1);
        }

        cp = pt.lst[y][3];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 8, cp, 1);
        }

        cp = pt.lst[y][4];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 9, cp, 1);
        }

        cp = pt.lst[y][5];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 10, cp, 1);
            lst.add(y, 11, cp, 1);
        }

        cp = pt.lst[y][6];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 12, cp, 1);
            lst.add(y, 13, cp, 1);
            lst.add(y, 14, cp, 1);
        }

        cp = pt.lst[y][7];
        if (cp <= totalpayout && cp >= minv) {
            __procMinMaxPayout(minp, maxp, cp);

            lst.add(y, 15, cp, 1);
            lst.add(y, 16, cp, 1);
            lst.add(y, 17, cp, 1);
            lst.add(y, 18, cp, 1);
        }
    }

    // int ls = -1;
    std::vector<int> lstsi;
    for (int i = 0; i < MAX_SYMBOL_NUMS; ++i) {
        lstsi.push_back(0);
    }

    for (int i = 0; i < (int)lst.lst.size(); ++i) {
        int tn = 0;
        if (lst.lst[i].nums == 5) {
            tn += 6;
        }
        else if (lst.lst[i].nums == 6) {
            tn += 6;
        }
        else if (lst.lst[i].nums == 7) {
            tn += 6;
        }
        else if (lst.lst[i].nums == 8) {
            tn += 4;
        }
        else if (lst.lst[i].nums == 9) {
            tn += 4;
        }
        else if (lst.lst[i].nums >= 10 && lst.lst[i].nums < 12) {
            tn += 2;
        }
        else if (lst.lst[i].nums >= 12 && lst.lst[i].nums < 15) {
            tn += 1;
        }
        else if (lst.lst[i].nums >= 15) {
            tn += 1;
        }

        lstsi[lst.lst[i].symbol] += tn;
    }

    for (int i = 0; i < (int)lst.lst.size(); ++i) {
        int tn = 0;
        if (lst.lst[i].nums == 5) {
            tn += 6;
        }
        else if (lst.lst[i].nums == 6) {
            tn += 6;
        }
        else if (lst.lst[i].nums == 7) {
            tn += 6;
        }
        else if (lst.lst[i].nums == 8) {
            tn += 4;
        }
        else if (lst.lst[i].nums == 9) {
            tn += 4;
        }
        else if (lst.lst[i].nums >= 10 && lst.lst[i].nums < 12) {
            tn += 2;
        }
        else if (lst.lst[i].nums >= 12 && lst.lst[i].nums < 15) {
            tn += 1;
        }
        else if (lst.lst[i].nums >= 15) {
            tn += 1;
        }

        int ttn = lstsi[lst.lst[i].symbol];
        if (ttn > 0) {
            float cc = geex.symbolRight[lst.lst[i].symbol];
            lst.lst[i].right = cc * tn / ttn;
        }
        else {
            float cc = geex.symbolRight[lst.lst[i].symbol];
            lst.lst[i].right = cc;
        }
    }

//    for (int i = 0; i < lst.lst.size(); ++i) {
//        if (ls != lst.lst[i].symbol) {
//            if (!lstsi.empty()) {
//                int tn = 0;
//                for (int j = 0; j < lstsi.size(); ++j) {
//                    if (lst.lst[lstsi[j]].nums == 5) {
//                        tn += 6;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 6) {
//                        tn += 6;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 7) {
//                        tn += 6;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 8) {
//                        tn += 4;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 9) {
//                        tn += 4;
//                    }
//                    else if (lst.lst[lstsi[j]].nums >= 10 && lst.lst[lstsi[j]].nums < 12) {
//                        tn += 2;
//                    }
//                    else if (lst.lst[lstsi[j]].nums >= 12 && lst.lst[lstsi[j]].nums < 15) {
//                        tn += 1;
//                    }
//                    else if (lst.lst[lstsi[j]].nums >= 15) {
//                        tn += 1;
//                    }
//                }
//
//                int cr = 1000 * geex.symbolRight[lst.lst[lstsi[0]].symbol] / 100;
//                float sr = 1000 / (float)tn;
//
//                for (int j = 0; j < lstsi.size(); ++j) {
//                    if (lst.lst[lstsi[j]].nums == 5) {
//                        lst.lst[lstsi[j]].right = 6 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 6) {
//                        lst.lst[lstsi[j]].right = 6 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 7) {
//                        lst.lst[lstsi[j]].right = 6 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 8) {
//                        lst.lst[lstsi[j]].right = 4 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums == 9) {
//                        lst.lst[lstsi[j]].right = 4 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums >= 10 && lst.lst[lstsi[j]].nums < 12) {
//                        lst.lst[lstsi[j]].right = 2 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums >= 12 && lst.lst[lstsi[j]].nums < 15) {
//                        lst.lst[lstsi[j]].right = 1 * sr;
//                    }
//                    else if (lst.lst[lstsi[j]].nums >= 15) {
//                        lst.lst[lstsi[j]].right = 1 * sr;
//                    }
//
//                    if (lst.lst[lstsi[j]].right <= 0) {
//                        lst.lst[lstsi[j]].right = 1;
//                    }
//                }
//            }
//
//            ls = lst.lst[i].symbol;
//            lstsi.clear();
//            lstsi.push_back(i);
//        }
//        else {
//            lstsi.push_back(i);
//        }
//    }
//
//    if (!lstsi.empty()) {
//        int tn = 0;
//        for (int j = 0; j < lstsi.size(); ++j) {
//            if (lst.lst[lstsi[j]].nums == 5) {
//                tn += 6;
//            }
//            else if (lst.lst[lstsi[j]].nums == 6) {
//                tn += 6;
//            }
//            else if (lst.lst[lstsi[j]].nums == 7) {
//                tn += 6;
//            }
//            else if (lst.lst[lstsi[j]].nums == 8) {
//                tn += 4;
//            }
//            else if (lst.lst[lstsi[j]].nums == 9) {
//                tn += 4;
//            }
//            else if (lst.lst[lstsi[j]].nums >= 10 && lst.lst[lstsi[j]].nums < 12) {
//                tn += 2;
//            }
//            else if (lst.lst[lstsi[j]].nums >= 12 && lst.lst[lstsi[j]].nums < 15) {
//                tn += 1;
//            }
//            else if (lst.lst[lstsi[j]].nums >= 15) {
//                tn += 1;
//            }
//        }
//
//        int cr = 1000 * geex.symbolRight[lst.lst[lstsi[0]].symbol] / 100;
//        float sr = 1000 / (float)tn;
//
//        for (int j = 0; j < lstsi.size(); ++j) {
//            if (lst.lst[lstsi[j]].nums == 5) {
//                lst.lst[lstsi[j]].right = 6 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums == 6) {
//                lst.lst[lstsi[j]].right = 6 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums == 7) {
//                lst.lst[lstsi[j]].right = 6 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums == 8) {
//                lst.lst[lstsi[j]].right = 4 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums == 9) {
//                lst.lst[lstsi[j]].right = 4 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums >= 10 && lst.lst[lstsi[j]].nums < 12) {
//                lst.lst[lstsi[j]].right = 2 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums >= 12 && lst.lst[lstsi[j]].nums < 15) {
//                lst.lst[lstsi[j]].right = 1 * sr;
//            }
//            else if (lst.lst[lstsi[j]].nums >= 15) {
//                lst.lst[lstsi[j]].right = 1 * sr;
//            }
//
//            if (lst.lst[lstsi[j]].right <= 0) {
//                lst.lst[lstsi[j]].right = 1;
//            }
//        }
//    }

    lst.reinitRight();
};

// 判断在这个paytables下，消除nums次，能否形成解，不要用这个接口处理3次及以上的解
bool _isValidResult7(PayTables& pt, int payout, int nums);

// 给定赔付表，返回一种满足条件的解
template <int MAX_SYMBOL_NUMS>
bool makeResult7(SymbolResultInfoList& lst, SymbolGenerator<MAX_SYMBOL_NUMS>& geexsrc, PayTables& pt,
                 int totalpayout, int paynums, int maxbn) {
    SymbolGenerator<MAX_SYMBOL_NUMS> geex;

    geex.cloneWith(geexsrc);

    lst.clear();

    if (totalpayout == 0) {
        return true;
    }

    SymbolResultInfoList lsttotal;

    int minp, maxp;
    int lastpaynums = paynums;
    int lastpayout = totalpayout;

    int i = 0;
    while (lastpaynums > 0 && lastpayout > 0) {
        // bool thisturnok = false;
        bool lastturnok = true;

        int minval = lastpayout / lastpaynums;

        geex.cloneWith(geexsrc);
        geex.mul(2);
        std::vector<int> lstsymbol;
        for (int j = 0; j < (int)lst.lst.size(); ++j) {
            std::vector<int>::iterator curit  = std::find(lstsymbol.begin(), lstsymbol.end(), lst.lst[j].symbol);
            if (curit != lstsymbol.end()) {
                lstsymbol.push_back(lst.lst[j].symbol);
            }
        }
        for (int j = 0; j < (int)lstsymbol.size(); ++j) {
            int cr = geex.symbolRight[lstsymbol[j]];
            geex.setSymbolRight(lstsymbol[j], cr / 2);
        }

        makeTotalResultList7(lsttotal, geex, pt, lastpayout, minp, maxp, minval, maxbn);
        //lsttotal.output("lsttotal");

        if (minp < 0 || maxp < 0) {
            if (i == 0) {
                return false;
            }
            else {
                lastturnok = false;
            }
        }

        if (maxp * lastpaynums < lastpayout) {
            if (i == 0) {
                return false;
            }
            else {
                lastturnok = false;
            }
        }

        if (lastturnok) {
            bool isok = false;
            do{
                int curlsti = lsttotal.random();//rand() % lsttotal.lst.size();
                if (lastpaynums <= 3) {
                    if (_isValidResult7(pt, lastpayout - lsttotal.lst[curlsti].payout, lastpaynums - 1)) {
                        isok = true;
                    }
                }
                else {
                    isok = true;
                }

                if (isok) {
                    lastpaynums--;
                    lastpayout -= lsttotal.lst[curlsti].payout;

                    lst.add(lsttotal.lst[curlsti].symbol, lsttotal.lst[curlsti].nums, lsttotal.lst[curlsti].payout, 0);

                    ++i;
                }
            }while(!isok);
        }
        else {
            if (lst.lst.size() == 0) {
                return false;
            }

            int li = lst.lst.size();

            lastpaynums++;
            lastpayout += lst.lst[li - 1].payout;

            lst.lst.pop_back();

            --i;
        }
    }

    int curnums = lst.countNums();
    int curlastnums = lst.countNumsEx(lst.lst.size() - 1);
    if (curnums >= 25 && curlastnums < 25) {
        if (lst.makeUp(pt, lst.lst.size() - 1, 25)) {
            return true;
        }
        else {
            if (lst.makeDown(pt, 24)) {
                return true;
            }
        }

        lst.clear();

        return false;
    }

    return true;
};

#endif //EXSLOTS_PROPORTION_H
