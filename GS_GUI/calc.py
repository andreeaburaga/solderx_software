def calc_temp(a, b):
    temp = 0.0
    temp = a << 8 + b
    print(temp)
    temp *= 165
    print(temp)
    temp /= (1 << 16) - 1
    print(temp)
    temp -= 40
    return temp

print(calc_temp(132, 203))
