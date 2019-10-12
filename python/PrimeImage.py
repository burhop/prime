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
        self.bits=[False,True,True,False,True]

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
        self.maxValue= bitsInFile*5*3*2//8
        
        self.maxValue = int(2E07)

        #Start counting until we get to maxValue
        count = 6
        #start with no bitcount so that we will read a byte first time through
        bitCount=8

        #Loop until we know all the primes less than maxValue
        while 1:
            if count>6 and ( count%2==0 or count%3==0 or count%5==0 ):
                    count = count +1
                    self.bits.append(False)
            else:
                if bitCount==8:  # no more bits, get the next byte in the file
                    byte=f.read(1)
                    bitCount=0
                    #printbits(byte[0])
                bit = (byte[0]>> bitCount) & 1
                bitCount=bitCount+1
                if bit:
                    self.primeList.append(count)
                    self.bits.append(True)
                else:
                    self.bits.append(False)

                count = count + 1
            # check if any more useful data. Could be last byte where all the bits weren't used
            if count > self.maxValue:                
                break    
        return
    def iSqrt(self,num):
        x = num
        y = (x + 1) // 2
        while y < x:
            x = y
            y = (x + num // x) // 2
        return x

    def isPrime(self, val):
        #TODO: this can be much faster
        if val in self.primeList:
            return True
        else:
            return False

    def GetSlabBitmap(self,width,height):
        #Slab of Primes
        img = Image.new( 'RGB', (width,height), "black") # create a new black image
        pixels = img.load() # create the pixel map
        for p in self.primeList:
            if p> (width*height):
                break
            j = p // width 
            i = (p-1) % width  #subract one as pixels start at 0,0 and not 1,1
            pixels[i,j]= (255,0,0)
        return img
        #img.save("prime500000.png")
    def GetSpiralPath(self):
        #SPIRAL  - First create a pattern to follow
        
        rotateCCW=[]
        direction=['r','u','l','d']
        #sum of  1+1+2+2+3+3+4+4+5+5+6+6+7+7+8+8+9+9+10+10 =110 = 2*(10+1)+2*(9+2)...
        #So don't need count but something far less (SQRT of count?)
        max=self.iSqrt(self.maxValue*2)
        for i in range (1, max):
            rotateCCW.append(i)
            rotateCCW.append(i)
        path=[]
        idex=0
        for i in rotateCCW:
            d=direction[idex]
            for count in range(0,i):
                path.append(d)
            idex=idex+1
            if idex==4:
                idex=0
        return path
    def GetSpiralImage(self):
        #path now contains a list of command to go right, up, left, or down
        path=self.GetSpiralPath()
        i=(self.iSqrt(self.maxValue)+1)//2  # set it to the center of the grid
        j=i
        count=0
        img = Image.new( 'RGB', (i*2+1,j*2+1), "blue") # create a new black image
        pixels = img.load() # create the pixel map
        for k in range(0,self.maxValue):
            val=self.bits[k]
            d=path[k]
            if val :
                pixels[i,j]=(255,0,0)
            else:
                pixels[i,j]=(0,255,0)
            if d=='l':
                i=i-1
            if d=='r':
                i=i+1
            if d== 'u':
                j=j-1
            if d== 'd':
                j=j+1
        return img

#for prime in listOfPrimes:
#    j = prime // 1000 
#    i = (prime-1) % 1000  #subract one as pixels start at 0,0 and not 1,1
##    pixels[i,j]= (255,0,0)
#img.save("prime500000.png")

 #   pixels[]
#for i in range(img.size[0]):    # for every col:
###    for j in range(img.size[1]):    # For every row
#        for prime in primes:
##            
 #       pixels[i,j] = (i, j, 100) # set the colour accordingly
#img.save('pil_red.png')


# we have 4 primes for x,x+1,x+2,x+3
#41,43,53,71
def coefficient(x,y):
    x_1 = x[0]
    x_2 = x[1]
    x_3 = x[2]
    y_1 = y[0]
    y_2 = y[1]
    y_3 = y[2]

    a = y_1/((x_1-x_2)*(x_1-x_3)) + y_2/((x_2-x_1)*(x_2-x_3)) + y_3/((x_3-x_1)*(x_3-x_2))

    b = (-y_1*(x_2+x_3)/((x_1-x_2)*(x_1-x_3))
         -y_2*(x_1+x_3)/((x_2-x_1)*(x_2-x_3))
         -y_3*(x_1+x_2)/((x_3-x_1)*(x_3-x_2)))

    c = (y_1*x_2*x_3/((x_1-x_2)*(x_1-x_3))
        +y_2*x_1*x_3/((x_2-x_1)*(x_2-x_3))
        +y_3*x_1*x_2/((x_3-x_1)*(x_3-x_2)))

    return a,b,c

x = [1,2,3]
y = [4,7,12]

a,b,c = coefficient(x, y)
#4x2 − 2x + 41 with x = 0, 1, 2,
print( "a = ", a)
print("b = ", b)
print("c = ", c)

    
def main():
    #bits(open('myPrimes1.prm', 'rb'))
    prime = Prime()
    prime.LoadPrimes('myPrimes1.prm')
    img = prime.GetSlabBitmap(500,10000)
    img.save("prime5000000.png")
    img2=prime.GetSpiralImage()
    img2.save("primeSpiral.png")
    #listOfPrimes =prime.primeList

if __name__ == "__main__":
    main()
