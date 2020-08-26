rect: {[w;h]
    res: w # 1
    do[h-2; {res,: 1, (x-2)}[w]]
    res, w # 1
    :res
}
