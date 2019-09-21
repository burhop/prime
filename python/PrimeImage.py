from PIL import Image
import sys
import struct
class Prime:
    #Initializer / Instance Attributes
    def __init__(self): 
        #initalize the list with 2,3,5  
        self.primeList=[2,3,5]
        #number of primes this class contains
        self.primeCount=3
        #maximum value we can search (e.g. we know all primes up to 6)
        self.maxValue=6

    #useful for debuging
    def __printbits(self,byte):
        for i in range(0,8):
            print ((byte>>i) & 1) 
    # Load Primes from a file (likely created by a much faster C++ program
    # See https://github.com/burhop/prime 
    def LoadPrimes(self,filename):
        f = open(filename, 'rb')
        self.primeList=[2,3,5]
        #assume data is from 64 bit OS. For example C++ size_t is 8 bytes (long long)  
        #Q data type is long long and 8 bytes
        data= struct.unpack('Q', f.read(8))
        #how many prime flag bits were stored in this file
        bitsInFile = data[0]
        
        #this file allows us to know all primes from 0 to maxValue (which we will calcuate below)
        #we need to know this as there may be some buffer bits in the last byte that should be ignored. 
        #We can't just read until the end of the file
        self.maxValue= bitsInFile*5*3*2/8
        
        #Start counting until we get to maxValue
        count = 1
        #start with no bitcount so that we will read a byte first time through
        bitCount=8

        #Loop until we know all the primes less than maxValue
        while 1:
            if count%2==0 or count%3==0 or count%5==0:
                    count = count +1
            else:
                if bitCount==8:  # no more bits, get the next byte in the file
                    byte=f.read(1)
                    bitCount=0
                    #printbits(byte[0])
                bit = (byte[0]>> bitCount) & 1
                bitCount=bitCount+1
                if bit:
                    self.primeList.append(count)
                count = count + 1
            # check if any more useful data. Could be last byte where all the bits weren't used
            if count > self.maxValue:                
                break    
        return

#bits(open('myPrimes1.prm', 'rb'))
prime = Prime()
prime.LoadPrimes('myPrimes1.prm')
listOfPrimes =prime.primeList
print(listOfPrimes)

#img = Image.new('RGB', (100, 30), color = 'red')
#img.save('pil_red.png')


img = Image.new( 'RGB', (1000,600), "black") # create a new black image
pixels = img.load() # create the pixel map
for prime in listOfPrimes:
    j = prime // 1000 
    i = (prime-1) % 1000  #subract one as pixels start at 0,0 and not 1,1
    pixels[i,j]= (255,0,0)
img.save("prime500000.png")
 #   pixels[]
#for i in range(img.size[0]):    # for every col:
###    for j in range(img.size[1]):    # For every row
#        for prime in primes:
##            
 #       pixels[i,j] = (i, j, 100) # set the colour accordingly
#img.save('pil_red.png')

