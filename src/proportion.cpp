//
// Created by zhs007 on 2017/2/28.
//

#include "proportion.h"

// 返奖的等比缩放
void proportionPayout(PayoutInfo& pi, PayoutInfo& dest, int min, int newnums) {
    dest.clear();

    int totalnums = pi.maxnums;
    int lastpayout = 0;
    int maxlen = pi.lst.size();

    int minnums = totalnums / newnums;
    if (totalnums % newnums > 0) {
        minnums++;
    }

    int curnums = 0;
    int curpayout = 0;
    for (int i = maxlen - 1; i >= 0; --i) {
        curnums += pi.lst[i].nums;
        curpayout += pi.lst[i].payout * pi.lst[i].nums;
        if (curnums >= minnums) {
            int cn = curnums / minnums;
            int cno = cn * minnums;
            int cp = curpayout / cno;
            int lcp = cp % min;
            if (lcp > 0) {
                cp = cp - lcp;
            }
            dest.addval(cp, cn);
            lastpayout += curpayout % cno;
            lastpayout += lcp * cno;

            curnums = 0;
            curpayout = 0;
        }
    }

    printf("lastpayout %d\n", lastpayout);

    if (lastpayout > 0) {
        int cp = lastpayout / minnums;
        int lcp = cp % min;
        if (lcp > 0) {
            cp = cp - lcp;
        }

        dest.addval(cp, 1);
    }
}

void __procMinMaxPayout(int& minp, int& maxp, int payout) {
    if (minp < 0) {
        minp = payout;
    }
    else if (minp > payout) {
        minp = payout;
    }

    if (maxp < 0) {
        maxp = payout;
    }
    else if (maxp < payout) {
        maxp = payout;
    }
}

// 判断在这个paytables下，消除nums次，能否形成解，不要用这个接口处理3次及以上的解
bool _isValidResult7(PayTables& pt, int payout, int nums) {
    if (payout < 0) {
        return false;
    }

    if (payout == 0) {
        return true;
    }

    if (nums <= 0) {
        return false;
    }

    for (int y = 0; y < pt.height; ++y) {
        int cp = pt.lst[y][0];
        if (cp <= 0) {
            continue ;
        }

        for (int j = 0; j < 8; ++j) {
            int cp = pt.lst[y][j];
            if (cp == payout) {
                return true;
            }

            if (cp < payout) {
                if (_isValidResult7(pt, payout - cp, nums - 1)) {
                    return true;
                }
            }
        }
    }

    return false;
}

int __countNums(int pti) {
    switch(pti) {
        case 0:
            pti = 5;
            break;
        case 1:
            pti = 6;
            break;
        case 2:
            pti = 7;
            break;
        case 3:
            pti = 8;
            break;
        case 4:
            pti = 9;
            break;
        case 5:
            pti = 10;
            break;
        case 6:
            pti = 12;
            break;
        case 7:
            pti = 15;
            break;
    }

    return pti;
}

int __countMaxNums(int pti) {
    switch(pti) {
        case 0:
            pti = 5;
            break;
        case 1:
            pti = 6;
            break;
        case 2:
            pti = 7;
            break;
        case 3:
            pti = 8;
            break;
        case 4:
            pti = 9;
            break;
        case 5:
            pti = 11;
            break;
        case 6:
            pti = 14;
            break;
        case 7:
            pti = 18;
            break;
    }

    return pti;
}

// 查找赔率下，最小的块数
bool findMinNums(PayTables& pt, int payout, int& symbol, int& nums) {
    symbol = -1;
    nums = -1;

    for (int y = 0; y < pt.height; ++y) {
        int cp = pt.lst[y][0];
        if (cp <= 0) {
            continue ;
        }

        for (int j = 0; j < 8; ++j) {
            int cp = pt.lst[y][j];
            if (cp == payout) {
                if (nums < 0 || j < nums) {
                    symbol = y;
                    nums = j;
                }
            }
        }
    }

    if (symbol >= 0) {
        switch(nums) {
            case 0:
                nums = 5;
                break;
            case 1:
                nums = 6;
                break;
            case 2:
                nums = 7;
                break;
            case 3:
                nums = 8;
                break;
            case 4:
                nums = 9;
                break;
            case 5:
                nums = 10;
                break;
            case 6:
                nums = 12;
                break;
            case 7:
                nums = 15;
                break;
        }

        return true;
    }

    return false;
}

// 查找赔率下，大一档的块数
bool findNextUp(PayTables& pt, int payout, int srcnums, int& symbol, int& nums) {
    symbol = -1;
    nums = -1;

    for (int y = 0; y < pt.height; ++y) {
        int cp = pt.lst[y][0];
        if (cp <= 0) {
            continue ;
        }

        for (int j = 0; j < 8; ++j) {
            int cp = pt.lst[y][j];
            if (cp == payout) {
                int curjnums = __countNums(j);
                int curmjnums = __countMaxNums(j);

                if (nums < 0 || curmjnums > srcnums) {
                    if (srcnums == curjnums && srcnums == curmjnums) {
                        continue ;
                    }
                    else if (srcnums >= curjnums && srcnums < curmjnums) {
                        symbol = y;
                        nums = srcnums + 1;
                    }
                    else if (curjnums - srcnums < nums - srcnums) {
                        symbol = y;
                        nums = curjnums;
                    }
                }
            }
        }
    }

    if (symbol >= 0) {
        return true;
    }

    return false;
}

//// 生成该赔付下，所有可能产生的赔付情况
//void makeTotalResultList7(SymbolResultInfoList& lst, PayTables& pt, int totalpayout, int& minp, int& maxp, int minv) {
//    lst.clear();
//    minp = -1;
//    maxp = -1;
//
//    for (int y = 0; y < pt.height; ++y) {
//        int cp = pt.lst[y][0];
//        if (cp <= 0) {
//            continue ;
//        }
//
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 5, cp);
//        }
//
//        cp = pt.lst[y][1];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 6, cp);
//        }
//
//        cp = pt.lst[y][2];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 7, cp);
//        }
//
//        cp = pt.lst[y][3];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 8, cp);
//        }
//
//        cp = pt.lst[y][4];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 9, cp);
//        }
//
//        cp = pt.lst[y][5];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 10, cp);
//            lst.add(y, 11, cp);
//        }
//
//        cp = pt.lst[y][6];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 12, cp);
//            lst.add(y, 13, cp);
//            lst.add(y, 14, cp);
//        }
//
//        cp = pt.lst[y][7];
//        if (cp <= totalpayout && cp >= minv) {
//            __procMinMaxPayout(minp, maxp, cp);
//
//            lst.add(y, 15, cp);
//            lst.add(y, 16, cp);
//            lst.add(y, 17, cp);
//            lst.add(y, 18, cp);
////            lst.add(y, 19, pt.lst[y][7]);
////            lst.add(y, 20, pt.lst[y][7]);
////            lst.add(y, 21, pt.lst[y][7]);
//        }
//    }
//}
//
//// 给定赔付表，返回一种满足条件的解
//bool makeResult7(SymbolResultInfoList& lst, PayTables& pt, int totalpayout, int paynums) {
//    lst.clear();
//
//    SymbolResultInfoList lsttotal;
//
//    int minp, maxp;
//    int lastpaynums = paynums;
//    int lastpayout = totalpayout;
//
//    int i = 0;
//    while (lastpaynums > 0 || lastpayout > 0) {
//        bool thisturnok = false;
//        bool lastturnok = true;
//
//        int minval = lastpayout / lastpaynums;
//        makeTotalResultList7(lsttotal, pt, lastpayout, minp, maxp, minval);
//
//        if (minp < 0 || maxp < 0) {
//            if (i == 0) {
//                return false;
//            }
//            else {
//                lastturnok = false;
//            }
//        }
//
////        if (minp * lastpaynums > lastpayout) {
////            if (i == 0) {
////                return false;
////            }
////            else {
////                lastturnok = false;
////            }
////        }
//
//        if (maxp * lastpaynums < lastpayout) {
//            if (i == 0) {
//                return false;
//            }
//            else {
//                lastturnok = false;
//            }
//        }
//
//        if (lastturnok) {
//            int curlsti = rand() % lsttotal.lst.size();
//
//            lastpaynums--;
//            lastpayout -= lsttotal.lst[curlsti].payout;
//
//            lst.add(lsttotal.lst[curlsti].symbol, lsttotal.lst[curlsti].nums, lsttotal.lst[curlsti].payout);
//
//            ++i;
//        }
//        else {
//            if (lst.lst.size() == 0) {
//                return false;
//            }
//
//            int li = lst.lst.size();
//
//            lastpaynums++;
//            lastpayout += lst.lst[li - 1].payout;
//
//            lst.lst.pop_back();
//
//            --i;
//        }
//    }
//
//    return true;
//}