//
// Created by zhs007 on 2016/12/26.
//

#ifndef EXSLOTS_SLOTSLOGIC_H
#define EXSLOTS_SLOTSLOGIC_H

#include <vector>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <string>
// #include "csv.h"

struct PayTables {
    int** lst;
    int width, height;
    std::vector<std::string> lstSymbolCode;

    PayTables() : lst(NULL), width(0), height(0) {}

    void init(int w, int h);

    void output() {
        printf("paytables\n");
        for (int ii = 0; ii < height; ++ii) {
            printf("%d", ii);

            for (int jj = 0; jj < width; ++jj) {
                printf(",%d", lst[ii][jj]);
            }

            printf("\n");
        }
    }

    int getSymbol(const char* symbolcode) {
        for (int i = 0; i < (int)lstSymbolCode.size(); ++i) {
            if (lstSymbolCode[i] == symbolcode) {
                return i;
            }
        }

        return -1;
    }

};

void loadPayTables5(PayTables& pt, const char* filename);
void loadPayTables7(PayTables& pt, const char* filename);

struct Wheel {
    int** lstArr;
    int* lstLength;

    int width, height;

    Wheel() : lstArr(NULL), lstLength(NULL), width(0), height(0) {}

    void init(int w, int h);

    int getSymbol(int x, int y);

    void output() {
        printf("wheel\n");
        for (int ii = 0; ii < width; ++ii) {
            printf("%d %d\n", ii, lstLength[ii]);
        }
    }
};

void loadWheel5(Wheel& wheel, const char* filename);

struct LineInfo {
    int** lst;
    int width, height;

    LineInfo() : lst(NULL), width(0), height(0) {}

    void init(int w, int h);

    void output() {
        printf("paytables\n");
        for (int ii = 0; ii < height; ++ii) {
            printf("%d", ii);

            for (int jj = 0; jj < width; ++jj) {
                printf(",%d", lst[ii][jj]);
            }

            printf("\n");
        }
    }
};

void loadLineInfo5(LineInfo& li, const char* filename);

struct PayoutNode{
    int payout;
    int64_t nums;

    bool operator == (int val) {
        return payout == val;
    }

    bool operator < (const PayoutNode& right) const {
        return payout < right.payout;
    }
};

struct PayoutInfo{
    int64_t maxnums;
    std::vector<PayoutNode> lst;

    PayoutInfo() {
        clear();
    }

    void clear() {
        maxnums = 0;
        lst.clear();
    }

    void mulPayout(int m) {
        for (std::vector<PayoutNode>::iterator it = lst.begin(); it != lst.end(); ++it) {
            it->payout *= m;
        }
    }

    void add(int payout) {
        ++maxnums;

        std::vector<PayoutNode>::iterator it = std::find(lst.begin(), lst.end(), payout);
        if (it == lst.end()) {
            PayoutNode pn;
            pn.payout = payout;
            pn.nums = 1;

            lst.push_back(pn);
        }
        else {
            it->nums++;
        }
    }

    void addval(int payout, int val) {
        maxnums += val;

        std::vector<PayoutNode>::iterator it = std::find(lst.begin(), lst.end(), payout);
        if (it == lst.end()) {
            PayoutNode pn;
            pn.payout = payout;
            pn.nums = val;

            lst.push_back(pn);
        }
        else {
            it->nums += val;
        }
    }

    int64_t getval(int payout) {
        std::vector<PayoutNode>::iterator it = std::find(lst.begin(), lst.end(), payout);
        if (it != lst.end()) {
            return it->nums;
        }

        return 0;
    }

    void compress(PayoutInfo& src, int base, std::vector<int>& mularr) {
        clear();

        // 完全没有回报的
        addval(-2, src.getval(0));

        // 回报在投入以下的
        int64_t val_1 = 0;
        for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
            if (it->payout > 0) {
                if (it->payout < base) {
                    val_1 += it->nums;
                }
            }
        }

        addval(-1, val_1);

        // 平的
        int64_t val0 = 0;
        for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
            if (it->payout > 0) {
                if (it->payout == base) {
                    val0 += it->nums;
                }
            }
        }

        addval(0, val0);

        // 根据倍数来算
        int bvm = 0;
        for (std::vector<int>::iterator it1 = mularr.begin(); it1 != mularr.end(); ++it1) {
            if (*it1 == 0) {
                continue ;
            }

            int bv = bvm * base;
            int ev = *it1 * base;
            bvm = *it1;

            int val = 0;
            for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
                if (it->payout > base) {
                    if (it->payout > bv + base && it->payout <= ev + base) {
                        val += it->nums;
                    }
                }
            }

            addval(bvm, val);
        }

        // 超过的给99999
        int val = 0;
        int bv = bvm * base;
        for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
            if (it->payout > base) {
                if (it->payout > bv + base) {
                    val += it->nums;
                }
            }
        }

        addval(99999, val);
    }

    // 统计总回报
    void compressTotalPayout(PayoutInfo& src, int base, std::vector<int>& mularr) {
        clear();

        // 回报在投入以下的
        int64_t val_1 = 0;
        for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
            if (it->payout > 0) {
                if (it->payout < base) {
                    val_1 += it->nums;// * it->payout;
                }
            }
        }

        addval(-1, val_1);

        // 平的
        int64_t val0 = 0;
        for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
            if (it->payout > 0) {
                if (it->payout == base) {
                    val0 += it->nums;// * it->payout;
                }
            }
        }

        addval(0, val0);

        // 根据倍数来算
        int bvm = 0;
        for (std::vector<int>::iterator it1 = mularr.begin(); it1 != mularr.end(); ++it1) {
            if (*it1 == 0) {
                continue ;
            }

            int bv = bvm * base;
            int ev = *it1 * base;
            bvm = *it1;

            int val = 0;
            for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
                if (it->payout > base) {
                    if (it->payout > bv + base && it->payout <= ev + base) {
                        val += it->nums;// * it->payout;
                    }
                }
            }

            addval(bvm, val);
        }

        // 超过的给99999
        int val = 0;
        int bv = bvm * base;
        for (std::vector<PayoutNode>::iterator it = src.lst.begin(); it != src.lst.end(); ++it) {
            if (it->payout > base) {
                if (it->payout > bv + base) {
                    val += it->nums;// * it->payout;
                }
            }
        }

        addval(99999, val);
    }

    void sort() {
        std::sort(lst.begin(), lst.end());
    }

    void output() {
        printf("payout totalnums is %lld\n", maxnums);
        for (int ii = 0; ii < (int)lst.size(); ++ii) {
            printf("%d\t%lld\n", lst[ii].payout, lst[ii].nums);
        }
    }

    void add(PayoutInfo& right) {
        maxnums += right.maxnums;

        for (std::vector<PayoutNode>::iterator it = right.lst.begin(); it != right.lst.end(); ++it) {
            std::vector<PayoutNode>::iterator it1 = std::find(lst.begin(), lst.end(), it->payout);
            if (it1 == lst.end()) {
                PayoutNode pn;
                pn.payout = it->payout;
                pn.nums = it->nums;

                lst.push_back(pn);
            }
            else {
                it1->nums += it->nums;
            }
        }
    }

    void save(const char* filename) {
        FILE* fp = fopen(filename, "w");
        if (fp != NULL) {
            fprintf(fp, "payout,nums\n");

            for (std::vector<PayoutNode>::iterator it = lst.begin(); it != lst.end(); ++it) {
                fprintf(fp, "%d,%lld\n", it->payout, it->nums);
            }
        }
        fclose(fp);
    }

    void load(const char* filename) {
        // clear();
        // CSVData csv;
        // csv.load(filename);

        // for (int y = 0; y < csv.m_height; ++y) {
        //     int p = atoi(csv.m_lst[y][0]);
        //     int n = atoi(csv.m_lst[y][1]);

        //     addval(p, n);
        // }
    }
};

struct SlotsResultList {
    struct _Node{
        int symbol;
        int nums;
    };

    typedef std::vector<_Node> _List;

    _List lst;

    void init(SlotsResultList& lstsr) {
        lst = lstsr.lst;
    }

    int countBlockNums() {
        int nums = 0;
        for (_List::iterator it = lst.begin(); it != lst.end(); ++it) {
            nums += it->nums;
        }

        return nums;
    }

    void clear() {
        lst.clear();
    }

    void addSymbol(int symbol, int nums) {
        _Node n;

        n.symbol = symbol;
        n.nums = nums;

        lst.push_back(n);
    }

    void makeString(std::string& str, std::string strsrc) {
        str.clear();

        char str0[1024];
        int nums = 0;

        str.append("[");

        for (_List::iterator it = lst.begin(); it != lst.end(); ++it) {
            sprintf(str0, "\"\"%c\"\":%d", strsrc.c_str()[it->symbol], it->nums);
            if (nums != 0) {
                str.append(", ");
            }

            str.append(str0);

            ++nums;
        }

        str.append("]");
    }

    void addList(SlotsResultList& lstsr) {
        for (_List::iterator it = lstsr.lst.begin(); it != lstsr.lst.end(); ++it) {
            lst.push_back(*it);
        }
    }
};

template<int MSN, int RN>
struct SlotsResult {
    typedef SlotsResult<MSN, RN>    _TYPE;

    int64_t totalnums;
    int64_t totalwin;

    int64_t symbolwin[MSN];
    int64_t symbolnums[MSN];

    int64_t symbolwinex[MSN][RN];
    int64_t symbolnumsex[MSN][RN];

    int64_t symbolwinnow[MSN][RN];
    int64_t symbolnumsnow[MSN][RN];

    std::vector<int64_t> lstdown;

    std::vector<int64_t> lstjackpot;

    SlotsResult() {
        clear();
    }

    void clear() {
        lstjackpot.clear();
        lstdown.clear();

        totalnums = 0;
        totalwin = 0;

        for (int ii = 0; ii < MSN; ++ii) {
            symbolwin[ii] = 0;
            symbolnums[ii] = 0;
        }

        for (int ii = 0; ii < MSN; ++ii) {
            for (int jj = 0; jj < RN; ++jj) {
                symbolwinex[ii][jj] = 0;
                symbolnumsex[ii][jj] = 0;

                symbolwinnow[ii][jj] = 0;
                symbolnumsnow[ii][jj] = 0;
            }
        }
    }

    void output() {
        printf("total nums is %lld\n", totalnums);
        printf("total wins is %lld\n", totalwin);

        for (int ii = 0; ii < MSN; ++ii) {
            printf("%d nums is %lld\n", ii, symbolnums[ii]);
            printf("%d wins is %lld\n", ii, symbolwin[ii]);

            for (int jj = 0; jj < RN; ++jj) {
                printf("%d(%d) nums is %lld\n", ii, jj, symbolnumsex[ii][jj]);
                printf("%d(%d) wins is %lld\n", ii, jj, symbolwinex[ii][jj]);

                printf("%d(%d)now nums is %lld\n", ii, jj, symbolnumsnow[ii][jj]);
                printf("%d(%d)now wins is %lld\n", ii, jj, symbolwinnow[ii][jj]);
            }
        }

        if (!lstdown.empty()) {
            for (int ii = 0; ii < lstdown.size(); ++ii) {
                printf("%d nums is %lld\n", ii, lstdown[ii]);
            }
        }

        if (!lstjackpot.empty()) {
            for (int ii = 0; ii < lstjackpot.size(); ++ii) {
                printf("%d jackpot is %lld\n", ii, lstjackpot[ii]);
            }
        }
    }

    void saveCSV(const char* filename) {
        FILE* fp = fopen(filename, "w");

        fprintf(fp, "symbol,X,nums,payout\n");

        for (int ii = 0; ii < MSN; ++ii) {
            for (int jj = 0; jj < RN; ++jj) {
                fprintf(fp, "%d,%d,%lld,%lld\n", ii, jj + 1, symbolnumsex[ii][jj], symbolwinex[ii][jj]);
            }
        }

        fclose(fp);
    }

    void add(_TYPE& right) {
        totalnums += right.totalnums;
        totalwin += right.totalwin;

        for (int ii = 0; ii < MSN; ++ii) {
            symbolwin[ii] += right.symbolwin[ii];
            symbolnums[ii] += right.symbolnums[ii];
        }

        for (int ii = 0; ii < MSN; ++ii) {
            for (int jj = 0; jj < RN; ++jj) {
                symbolwinex[ii][jj] += right.symbolwinex[ii][jj];
                symbolnumsex[ii][jj] += right.symbolnumsex[ii][jj];

                symbolwinnow[ii][jj] += right.symbolwinnow[ii][jj];
                symbolnumsnow[ii][jj] += right.symbolnumsnow[ii][jj];
            }
        }

        if (!right.lstdown.empty()) {
            for (int ii = lstdown.size(); ii < right.lstdown.size(); ++ii) {
                lstdown.push_back(0);
            }

            for (int ii = 0; ii < right.lstdown.size(); ++ii) {
                lstdown[ii] += right.lstdown[ii];
            }
        }

        if (!right.lstjackpot.empty()) {
            for (int ii = lstjackpot.size(); ii < right.lstjackpot.size(); ++ii) {
                lstjackpot.push_back(0);
            }

            for (int ii = 0; ii < right.lstjackpot.size(); ++ii) {
                lstjackpot[ii] += right.lstjackpot[ii];
            }
        }
    }

    void addDown(int downnums) {
//        if (downnums <= 0) {
//            return ;
//        }

        if (downnums >= lstdown.size()) {
            for (int i = lstdown.size(); i <= downnums; ++i) {
                lstdown.push_back(0);
            }
        }

        lstdown[downnums]++;
    }

    void addJackpot(int jackpot) {
        if (jackpot <= 0) {
            return ;
        }

        if (jackpot > lstjackpot.size()) {
            for (int i = lstjackpot.size(); i <= jackpot; ++i) {
                lstjackpot.push_back(0);
            }
        }

        lstjackpot[jackpot - 1]++;
    }
};

template <int WIDTH>
struct PosList{
    int x[WIDTH];
    int y[WIDTH];

    int nums;

    PosList() : nums(0) {}

    bool addXY(int xx, int yy) {
        if (hasXY(xx, yy)) {
            return false;
        }

        x[nums] = xx;
        y[nums] = yy;

        nums++;

        return true;
    }

    bool hasXY(int xx, int yy) {
        for (int i = 0; i < nums; ++i) {
            if (x[i] == xx && y[i] == yy) {
                return true;
            }
        }

        return false;
    }

    void clear() { nums = 0; }
};

template <int LEN0, int LEN1>
void addPosList(PosList<LEN0>& dest, PosList<LEN1>& src) {
    for (int i = 0; i < src.nums; ++i) {
        if (!dest.hasXY(src.x[i], src.y[i])) {
            dest.addXY(src.x[i], src.y[i]);
        }
    }
};

template <int LEN>
void addPosListEx(PosList<LEN>& dest, const PosList<LEN>& src) {
    for (int i = 0; i < src.nums; ++i) {
        if (!dest.hasXY(src.x[i], src.y[i])) {
            dest.addXY(src.x[i], src.y[i]);
        }
    }
};

struct SymbolList{
    typedef std::map<int, int> _MAP;
    typedef std::vector<int> _LIST;

    _LIST lst;
    _MAP map;

    void clear() {
        lst.clear();
        map.clear();
    }

    // 添加一个symbol
    void addSymbol(int symbol) {
        lst.push_back(symbol);
        map[symbol]++;
    }

    // 是否均衡，不要有占比超过per的块
    bool isBalance(float per) {
        int mlen = lst.size();
        for (_MAP::iterator it = map.begin(); it != map.end(); ++it) {
            if (it->second >= per * mlen) {
                return false;
            }
        }

        return true;
    }

    void makeString(std::string& str, std::string& src) {
        str.clear();
        for (_LIST::iterator it = lst.begin(); it < lst.end(); ++it) {
            str.push_back(src.c_str()[*it]);
        }
    }
};

template <int WIDTH, int HEIGHT>
struct GameScene{
    int arr[HEIGHT][WIDTH];

    void init(GameScene<WIDTH, HEIGHT>& gs) {
        memcpy(&(arr[0][0]), &(gs.arr[0][0]), sizeof(int) * WIDTH * HEIGHT);
    }

    void clear() {
        memset(&(arr[0][0]), 0, sizeof(int) * WIDTH * HEIGHT);
    }

    // 用轮子来初始化
    void initWithWheel(Wheel& wheel, int* wi) {
        for (int xx = 0;  xx < WIDTH; ++xx) {
            for (int yy = 0; yy < HEIGHT; ++yy) {
                arr[yy][xx] = wheel.getSymbol(xx, wi[xx] + yy);
            }
        }
    }

    // 用于三消
    // 用轮子下落补齐
    void reinitWithWheel(Wheel& wheel, int* wi) {
        for (int xx = 0;  xx < WIDTH; ++xx) {
            for (int yy = HEIGHT - 1; yy >= 0; --yy) {
                if (arr[yy][xx] == -1) {
                    wi[xx]--;
                    arr[yy][xx] = wheel.getSymbol(xx, wi[xx]);
                }
            }
        }
    }

    void setList(PosList<WIDTH>& pl, int val) {
        for (int i = 0; i < pl.nums; ++i) {
            arr[pl.y[i]][pl.x[i]] = val;
        }
    }

    template<int LEN>
    void setListEx(PosList<LEN>& pl, int val) {
        for (int i = 0; i < pl.nums; ++i) {
            if (pl.y[i] >= 0 && pl.y[i] < HEIGHT && pl.x[i] >= 0 && pl.x[i] < WIDTH) {
                arr[pl.y[i]][pl.x[i]] = val;
            }
            else {
                printf("bug!!!\n");
            }
        }
    }

    void setGameSceneWithVal(GameScene<WIDTH, HEIGHT>& dest, int val, int destval) {
        for (int yy = 0; yy < HEIGHT; ++yy) {
            for (int xx = 0; xx < WIDTH; ++xx) {
                if (arr[yy][xx] == val) {
                    dest.arr[yy][xx] = destval;
                }
            }
        }
    }

    int countSymbolNums(int val) {
        int nums = 0;
        for (int yy = 0; yy < HEIGHT; ++yy) {
            for (int xx = 0; xx < WIDTH; ++xx) {
                if (arr[yy][xx] == val) {
                    nums++;
                }
            }
        }

        return nums;
    }

    int getVal(int xx, int yy) {
        if (xx >= 0 && xx < WIDTH) {
            if (yy >= 0 && yy < HEIGHT) {
                return arr[yy][xx];
            }
        }

        return -1;
    }

    bool __procDown(int x, int by, int nums) {
        if (by == 0) {
            return false;
        }

        bool hasnode = false;
        for (int yy = by; yy >= 0; --yy) {
            if (yy >= 0 && yy < HEIGHT) {
                int sy = yy - nums;
                arr[yy][x] = getVal(x, sy);

                if (arr[yy][x] != -1) {
                    hasnode = true;
                }
            }
        }

        return hasnode;
    }

    void procDown() {
        for (int xx = 0; xx < WIDTH; ++xx) {
            for (int by = HEIGHT - 1; by >= 0; ) {
                if (arr[by][xx] == -1) {
                    int nums = 1;
                    for (int yy = by - 1; yy >= 0; --yy) {
                        if (arr[yy][xx] == -1) {
                            ++nums;
                        }
                        else {
                            break;
                        }
                    }

                    if (!__procDown(xx, by, nums)) {
                        break;
                    }
                }
                else {
                    --by;
                }
            }
        }
    }

    // 如果gs里某项为-1，则填充
    void add2GameScene(GameScene<WIDTH, HEIGHT>& gs) {
        for (int yy = 0; yy < HEIGHT; ++yy) {
            for (int xx = 0; xx < WIDTH; ++xx) {
                if (gs.arr[yy][xx] == -1) {
                    gs.arr[yy][xx] = arr[yy][xx];
                }
            }
        }
    }

    // 计数
    int countNums(int val) {
        int nums = 0;
        for (int yy = 0; yy < HEIGHT; ++yy) {
            for (int xx = 0; xx < WIDTH; ++xx) {
                if (arr[yy][xx] == val) {
                    ++nums;
                }
            }
        }

        return nums;
    }

    // 如果gs里某项为-1，则填充
    void add2GameSceneEx(GameScene<WIDTH, HEIGHT>& gs, SymbolList& lst) {
        for (int xx = 0; xx < WIDTH; ++xx) {
            for (int yy = HEIGHT - 1; yy >= 0; --yy) {
                if (gs.arr[yy][xx] == -1) {
                    gs.arr[yy][xx] = arr[yy][xx];

                    lst.addSymbol(arr[yy][xx]);
                }
            }
        }
    }

    // 如果gs里某项为-1，则填充
    void makeSymbolList(GameScene<WIDTH, HEIGHT>& gs, SymbolList& lst) {
        for (int xx = 0; xx < WIDTH; ++xx) {
            for (int yy = HEIGHT - 1; yy >= 0; --yy) {
                if (gs.arr[yy][xx] == -1) {
                    lst.addSymbol(arr[yy][xx]);
                }
            }
        }
    }

    void output(const char* name) {
        printf("%s=============\n", name);
        for (int yy = 0; yy < HEIGHT; ++yy) {
            for (int xx = 0; xx < WIDTH; ++xx) {
                printf("%3d ", arr[yy][xx]);
            }

            printf("\n");
        }
        printf("=============\n");
    }

    void makeString(std::string& str, const char* src, char o, bool mulline) {
        str.clear();
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                int cv = arr[y][x];
                if (cv < 0) {
                    str.push_back(o);
                }
                else {
                    str.push_back(src[cv]);
                }
            }

            if (mulline) {
                str.append("\n");
            }
        }
    }
};

template <int WIDTH, int HEIGHT, int ARRLEN>
struct GameSceneArr{
    GameScene<WIDTH, HEIGHT> arr[ARRLEN];
    GameScene<WIDTH, HEIGHT> arrend[ARRLEN];
    GameScene<WIDTH, HEIGHT> arrend2[ARRLEN];
    SlotsResultList lstsr[ARRLEN];
    bool isend[ARRLEN];
    int nums;

    void pushGameScene(GameScene<WIDTH, HEIGHT>& gs) {
        arr[nums].init(gs);
        isend[nums] = false;
        ++nums;
    }

    void pushGameSceneEnd(GameScene<WIDTH, HEIGHT>& gs, SlotsResultList& lst) {
        if (nums > 0) {
            arrend[nums - 1].init(gs);
            lstsr[nums - 1].init(lst);

            isend[nums - 1] = true;
        }
    }

    void pushGameSceneEnd2(GameScene<WIDTH, HEIGHT>& gs) {
        if (nums > 0) {
            arrend2[nums - 1].init(gs);
            isend[nums - 1] = true;
        }
    }

    void popGameScene() {
        if (nums > 0) {
            nums--;
        }
    }

    void clear() {
        nums = 0;
    }

    void makeSymbolList(SymbolList& lst) {
        lst.clear();

        for (int k = 0; k < nums; ++k) {
            if (isend[k]) {
                //arrend2[k].makeSymbolList(arr[k + 1], lst);
                arr[k + 1].makeSymbolList(arrend2[k], lst);
            }
        }
    }
};

template <int WIDTH, int HEIGHT>
struct WheelScene {
    int** lstArr;
    int length;
    int width, height;

    WheelScene() : lstArr(NULL), width(WIDTH), height(HEIGHT), length(0) {}

    void init(int len) {
        int bl = WIDTH * HEIGHT;

        lstArr = (int**)malloc(len * bl * sizeof(int) + len * sizeof(int*));
        char* bp = (char*)lstArr + len * sizeof(int*);

        for (int i = 0; i < len; ++i) {
            lstArr[i] = (int*)(bp + bl * sizeof(int) * i);
        }

        length = len;
    }

    void buildGameScene(GameScene<WIDTH, HEIGHT>& gs, int index) {
        if (index >= 0 && index < length) {
            memcpy(&(gs.arr[0][0]), lstArr[index], sizeof(int) * WIDTH * HEIGHT);
        }
    }

    // 如果gs里某项为-1，则填充
    void add2GameScene(GameScene<WIDTH, HEIGHT>& gs, int index) {
        if (index >= 0 && index < length) {
            for (int yy = 0; yy < HEIGHT; ++yy) {
                for (int xx = 0; xx < WIDTH; ++xx) {
                    if (gs.arr[yy][xx] == -1) {
                        gs.arr[yy][xx] = lstArr[index][yy * WIDTH + xx];
                    }
                }
            }
        }
    }
};

template <int WIDTH, int HEIGHT>
void loadWheelScene(WheelScene<WIDTH, HEIGHT>& ws, const char* filename) {
    // CSVData csv;
    // csv.load(filename);

    // ws.init(csv.m_height);

    // for (int y = 0; y < csv.m_height; ++y) {

    //     for (int x = 0; x < WIDTH * HEIGHT; ++x) {
    //         int pv = atoi(csv.m_lst[y][x + 1]);
    //         ws.lstArr[y][x] = pv;
    //     }
    // }
};

template <int MAX_SYMBOL_NUMS>
struct SymbolGenerator {
    int symbolRight[MAX_SYMBOL_NUMS];
    int totalRight;

    void cloneWith(SymbolGenerator<MAX_SYMBOL_NUMS>& src) {
        totalRight = src.totalRight;
        memcpy(symbolRight, src.symbolRight, sizeof(int) * MAX_SYMBOL_NUMS);
    }

    void mul(int mul) {
        for (int i = 0; i < MAX_SYMBOL_NUMS; ++i) {
            setSymbolRight(i, symbolRight[i] * mul);
        }
    }

    void setSymbolRight(int symbol, int right) {
        totalRight -= symbolRight[symbol];
        symbolRight[symbol] = right;
        totalRight += symbolRight[symbol];
    }

    void clear() {
        memset(symbolRight, 0, sizeof(int) * MAX_SYMBOL_NUMS);

        totalRight = 0;
    }

    int randomSymbol() {
        if (totalRight <= 0) {
            return -1;
        }

        int r = rand() % totalRight;
        for (int i = 0; i < MAX_SYMBOL_NUMS; ++i) {
            if (r < symbolRight[i]) {
                return i;
            }
            else {
                r -= symbolRight[i];
            }
        }

        return -1;
    }

    int countSymbolNums() {
        int nums = 0;
        for (int i = 0; i < MAX_SYMBOL_NUMS; ++i) {
            if (symbolRight[i] > 0) {
                ++nums;
            }
        }

        return nums;
    }

    bool hasSymbol(int s) {
        return symbolRight[s] > 0;
    }

    bool isValid() {
        for (int i = 0; i < MAX_SYMBOL_NUMS; ++i) {
            if (symbolRight[i] > 0) {
                return true;
            }
        }

        return false;
    }
};

#endif //EXSLOTS_SLOTSLOGIC_H
