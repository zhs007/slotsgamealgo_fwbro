#include "fwbro.h"

const int R_NUMS = 5;
const int R_HEIGHT = 5;
const int MAX_SYMBOL_NUMS = 11;

typedef SlotsResult<MAX_SYMBOL_NUMS, R_NUMS>    SlotsResultEx;
typedef PosList<R_NUMS>                         PosListEx;
typedef PosList<R_NUMS * R_HEIGHT>              PosListEx2;
typedef GameScene<R_NUMS, R_HEIGHT>             GameSceneEx;

struct TotalResult{
    SlotsResultEx   sr;
    PayoutInfo      pi;
};

typedef std::vector<int> LineArray;

int countLine(TotalResult& tr, LineArray& li, PayTables& pt, int bx, int lasts, int& symbolnums, int& win, bool& hasw) {
    symbolnums = 0;
    win = 0;

    if (li.size() - bx < 3) {
        return -1;
    }

    hasw = false;
    int s = li[bx];
    int nums = 1;
    if (s < 0) {
        return -1;
    }

    if (s == 0) {
        hasw = true;
        int nnums = 0;
        int ns = -1;
        for (int ii = bx + 1; ii < (int)li.size(); ++ii) {
            if (li[ii] == 0 && ns == -1) {
                ++nums;
            }
            else if (ns == -1) {
                if (li[ii] < 0) {
                    break;
                }

                ns = li[ii];
                nnums = nums + 1;
            }
            else if (li[ii] == ns || li[ii] == 0) {
                ++nnums;
            }
            else {
                break;
            }
        }

        if (ns > 0) {
            int wval = pt.lst[0][nums - 1];
            int val = pt.lst[ns][nnums - 1];

            if (val == wval) {
                if (nums != 3) {
                    s = ns;
                    nums = nnums;
                }
            }
            else if (val > wval) {
                s = ns;
                nums = nnums;
            }
        }
    }
    else {
        for (int ii = bx + 1; ii < (int)li.size(); ++ii) {
            if (li[ii] == 0 || li[ii] == s) {
                ++nums;
            }
            else {
                break;
            }
        }
    }

    if (pt.lst[s][nums - 1] == 0) {
        return -1;
    }

    if (lasts == s) {
        return -1;
    }

    symbolnums = nums;
    win = pt.lst[s][nums - 1];

    return s;
}

int countResultWithX(GameSceneEx& gse, int y, TotalResult& tr, PayTables& pt, GameSceneEx& gse_remove, int mul) {
    int totalwin = 0;
    LineArray li;

    for (int ii = 0; ii < R_HEIGHT; ++ii) {
        li.push_back(gse.getVal(ii, y));
    }

    int lasts = -1;
    for (int ii = 0; ii < 3; ++ii) {
        int symbolnums = 0;
        int win = 0;
        bool hasw = false;
        int ret = countLine(tr, li, pt, ii, lasts, symbolnums, win, hasw);
        if (ret >= 0) {
            win *= mul;

            lasts = ret;
            totalwin += win;

            for (int jj = 0; jj < symbolnums; ++jj) {
                if (symbolnums == 3 && jj == 1) {
                    gse_remove.arr[y][ii + jj] = 1;
                }
                else if (gse_remove.arr[y][ii + jj] != 1) {
                    gse_remove.arr[y][ii + jj] = -1;
                }
            }

            tr.sr.totalwin += win;
            ++tr.sr.symbolnums[ret];
            tr.sr.symbolwin[ret] += win;

            ++tr.sr.symbolnumsex[ret][symbolnums - 1];
            tr.sr.symbolwinex[ret][symbolnums - 1] += win;

            if (!hasw) {
                ++tr.sr.symbolnumsnow[ret][symbolnums - 1];
                tr.sr.symbolwinnow[ret][symbolnums - 1] += win;
            }
        }
    }

    return totalwin;
}

int countResultWithY(GameSceneEx& gse, int x, TotalResult& tr, PayTables& pt, GameSceneEx& gse_remove, int mul) {
    int totalwin = 0;
    LineArray li;

    for (int ii = 0; ii < R_NUMS; ++ii) {
        li.push_back(gse.getVal(x, ii));
    }

    int lasts = -1;
    for (int ii = 0; ii < 3; ++ii) {
        int symbolnums = 0;
        int win = 0;
        bool hasw = false;
        int ret = countLine(tr, li, pt, ii, lasts, symbolnums, win, hasw);
        if (ret >= 0) {
            win *= mul;

            lasts = ret;
            totalwin += win;

            for (int jj = 0; jj < symbolnums; ++jj) {
                if (symbolnums == 3 && jj == 1) {
                    gse_remove.arr[ii + jj][x] = 1;
                }
                else if (gse_remove.arr[ii + jj][x] != 1) {
                    gse_remove.arr[ii + jj][x] = -1;
                }
            }

            tr.sr.totalwin += win;
            ++tr.sr.symbolnums[ret];
            tr.sr.symbolwin[ret] += win;

            ++tr.sr.symbolnumsex[ret][symbolnums - 1];
            tr.sr.symbolwinex[ret][symbolnums - 1] += win;

            if (!hasw) {
                ++tr.sr.symbolnumsnow[ret][symbolnums - 1];
                tr.sr.symbolwinnow[ret][symbolnums - 1] += win;
            }
        }
    }

    return totalwin;
}

int countResultWithXY(GameSceneEx& gse, int x, int y, TotalResult& tr, PayTables& pt, GameSceneEx& gse_remove, int mul) {
    int totalwin = 0;
    LineArray li;

    if (x >= 3) {
        return totalwin;
    }

    if (y >= 3) {
        return totalwin;
    }

    for (int ii = 0; x + ii < R_NUMS && y + ii < R_HEIGHT; ++ii) {
        li.push_back(gse.getVal(x + ii, y + ii));
    }

    int lasts = -1;
    for (int ii = 0; ii < 3; ++ii) {
        int symbolnums = 0;
        int win = 0;
        bool hasw = false;
        int ret = countLine(tr, li, pt, ii, lasts, symbolnums, win, hasw);
        if (ret >= 0) {
            win *= mul;

            lasts = ret;
            totalwin += win;

            for (int jj = 0; jj < symbolnums; ++jj) {
                if (symbolnums == 3 && jj == 1) {
                    gse_remove.arr[y + ii + jj][x + ii + jj] = 1;
                }
                else if (gse_remove.arr[y + ii + jj][x + ii + jj] != 1) {
                    gse_remove.arr[y + ii + jj][x + ii + jj] = -1;
                }
            }

            tr.sr.totalwin += win;
            ++tr.sr.symbolnums[ret];
            tr.sr.symbolwin[ret] += win;

            ++tr.sr.symbolnumsex[ret][symbolnums - 1];
            tr.sr.symbolwinex[ret][symbolnums - 1] += win;

            if (!hasw) {
                ++tr.sr.symbolnumsnow[ret][symbolnums - 1];
                tr.sr.symbolwinnow[ret][symbolnums - 1] += win;
            }
        }
    }

    return totalwin;
}

int countResultWithYX(GameSceneEx& gse, int x, int y, TotalResult& tr, PayTables& pt, GameSceneEx& gse_remove, int mul) {
    int totalwin = 0;
    LineArray li;

    if (x >= 3) {
        return totalwin;
    }

    if (y >= 3) {
        return totalwin;
    }

    for (int ii = 0; x + ii < R_NUMS && y - ii >= 0; ++ii) {
        li.push_back(gse.getVal(x + ii, y - ii));
    }

    int lasts = -1;
    for (int ii = 0; ii < 3; ++ii) {
        int symbolnums = 0;
        int win = 0;
        bool hasw = false;
        int ret = countLine(tr, li, pt, ii, lasts, symbolnums, win, hasw);
        if (ret >= 0) {
            win *= mul;

            lasts = ret;
            totalwin += win;

            for (int jj = 0; jj < symbolnums; ++jj) {
                if (symbolnums == 3 && jj == 1) {
                    gse_remove.arr[y - (ii + jj)][x + ii + jj] = 1;
                }
                else if (gse_remove.arr[y - (ii + jj)][x + ii + jj] != 1) {
                    gse_remove.arr[y - (ii + jj)][x + ii + jj] = -1;
                }
            }

            tr.sr.totalwin += win;
            ++tr.sr.symbolnums[ret];
            tr.sr.symbolwin[ret] += win;

            ++tr.sr.symbolnumsex[ret][symbolnums - 1];
            tr.sr.symbolwinex[ret][symbolnums - 1] += win;

            if (!hasw) {
                ++tr.sr.symbolnumsnow[ret][symbolnums - 1];
                tr.sr.symbolwinnow[ret][symbolnums - 1] += win;
            }
        }
    }

    return totalwin;
}

int countResult(GameSceneEx& gse, TotalResult& tr, PayTables& pt, GameSceneEx& gse_remove, int mul) {
    int totalwin = 0;
    for (int ii = 0; ii < R_HEIGHT; ++ii) {
        totalwin += countResultWithX(gse, ii, tr, pt, gse_remove, mul);
    }

    for (int ii = 0; ii < R_NUMS; ++ii) {
        totalwin += countResultWithY(gse, ii, tr, pt, gse_remove, mul);
    }


    totalwin += countResultWithXY(gse, 0, 0, tr, pt, gse_remove, mul);
    totalwin += countResultWithXY(gse, 0, 1, tr, pt, gse_remove, mul);
    totalwin += countResultWithXY(gse, 0, 2, tr, pt, gse_remove, mul);
    totalwin += countResultWithXY(gse, 1, 0, tr, pt, gse_remove, mul);
    totalwin += countResultWithXY(gse, 2, 0, tr, pt, gse_remove, mul);

    totalwin += countResultWithYX(gse, 0, 4, tr, pt, gse_remove, mul);
    totalwin += countResultWithYX(gse, 0, 3, tr, pt, gse_remove, mul);
    totalwin += countResultWithYX(gse, 0, 2, tr, pt, gse_remove, mul);
    totalwin += countResultWithYX(gse, 1, 4, tr, pt, gse_remove, mul);
    totalwin += countResultWithYX(gse, 2, 4, tr, pt, gse_remove, mul);

    return totalwin;
}