import struct
def isPrime(num):
    num
    f = open('myPrimes1.prm', 'rb')
    #assume data is from 64 bit OS. For example C++ size_t is 8 bytes (long long)  
    #Q data type is long long and 8 bytes
    data= struct.unpack('Q', f.read(8))
    #how many prime flag bits were stored in this file
    bitsInFile = data[0]
    #this file allows us to know all primes from 0 to maxValue (which we will calcuate below)
    #we need to know this as there may be some buffer bits in the last byte that should be ignored. 
    #We can't just read until the end of the file
    maxValue= bitsInFile*5*3*2//8
    if num>maxValue :
        return -1
    if num>6 and ( num%2==0 or num%3==0 or num%5==0 ):
        return 0
    if num==2 or num==3 or num==5 :
        return 1
    #bit we need to check
    bitnum = num + num // 6 + num // 10 + num // 15 - num // 2 - num // 3 - num // 5 - num // 30 - 1
    #Find the byte containng the number's prime flag.  Need to go 8 farther as first 8 bytes of file used above
    bytenum = bitnum//8 +8
    subbit=bitnum%8
    f.seek(bytenum)
    byte=f.read(1)
    bit = (byte[0]>> subbit) & 1
    f.close()
    return bit
def test(num):
    print( num, " is ", isPrime(num) )


test(2)
test(3)
test(3456)
test(4567)
test(17389)
test(104729)
test(104731)