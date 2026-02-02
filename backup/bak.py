
EXP = [0] * 512 # Galois fields # -- exponents (anti-logarithms)
LOG = [0] * 256 # -- logarithms

class gf256 :
	# INITIALISATION CONSTRUCTOR
	def __init__(self):
		# prepare the exponential and logarithmic fields
		EXP[0] = 1
		byteValu = 1
		for bytePos in range(1,255):
			byteValu <<= 1
			if (byteValu & 0x100):
				byteValu ^= 0x11d
			# update the field elements
			EXP[bytePos] = byteValu
			LOG[byteValu] = bytePos
		# finalise the exponential field
		for bytePos in range(255,512):
			EXP[bytePos] = EXP[bytePos - 255]
	## GALOIS PRIMITIVE OPERATIONS
	# -----
	# Galois multiplication
	# argX, argY: multiplicand, multiplier
	# byteValu: product
	def Mult(argX, argY):
		# parametre checks
		if ((argX == 0) or (argY == 0)):
			byteValu = 0
		else:
			# perform the operation
			byteValu = LOG[argX]
			byteValu += LOG[argY]
			byteValu = EXP[byteValu]
		# return the product result
		return (byteValu)

	# Galois division
	# argX, argY: dividend, divisor
	# byteValu: quotient
	def Divi(argX, argY):
		# validate the divisor
		if (argY == 0) : raise ZeroDivisionError()
		# validate the dividend
		if (argX == 0):
			byteValu = 0
		else:
			# perform the division
			byteValu = LOG[argX] - LOG[argY]
			byteValu += 255
			byteValu = EXP[byteValu]
		# return the division result
		return (byteValu)

	## GALOIS POLYNOMIAL OPERATIONS
	# -----
	# Polynomial addition
	# polyA, polyB: polynomial addends
	# polySum: polynomial sum
	def PolyAdd(polyA, polyB):
		# initialise the polynomial sum
		polySum = [0] * max(len(polyA), len(polyB))
		# process the first addend
		for i in range(0, len(polyA)):
			polySum[i + len(polySum) - len(polyA)] = polyA[i]
		# add the second addend
		for i in range(0, len(polyB)):
			polySum[i + len(polySum) - len(polyB)] ^= polyB[i]
		# return the sum
		return (polySum)

	# Polynomial multiplication
	# polyA, polyB: polynomial factors
	# polyProd: polynomial product
	def PolyMult(polyA, polyB):
		# initialise the product
		polyProd = len(polyA) + len(polyB) - 1
		polyProd = [0] * polyProd

		# start multiplying
		for posB in range(0, len(polyB)):
			for posA in range(0, len(polyA)):
				polyProd[posA + posB] ^= gf256.Mult(polyA[posA], polyB[posB])
		# return the product result
		return (polyProd)

	# Polynomial scaling
	# argPoly: polynomial argument
	# argX: scaling factor
	# polyVal: scaled polynomial
	def PolyScale(argPoly, argX):
		# initialise the scaled polynomial
		polyVal = [0] * len(argPoly)
		# start scaling
		for i in range(0, len(argPoly)):
			polyVal[i] = gf256.Mult(argPoly[i], argX)
		# return the scaled polynomial
		return (polyVal)

	# Polynomial evaluation
	# argPoly: polynomial argument
	# argX: independent variable
	# byteValu: dependent variable
	def PolyEval(argPoly, argX):
		# initialise the polynomial result
		byteValu = argPoly[0]
		# evaluate the polynomial argument
		for i in range(1, len(argPoly)):
			tempValu = gf256.Mult(byteValu, argX)
			tempValu = tempValu ^ argPoly[i]
			byteValu = tempValu
		# return the evaluated result
		return (byteValu)


## REED-SOLOMON SUPPORT ROUTINES
class RS :

	## REED-SOLOMON DECODING
	# Generate the syndrome polynomial
	# argCode: the code block
	# errSize: number of error symbols
	# polyValu: the syndrome polynomial
	def SyndPoly(argCode, errSize):
		# initialise the polynomial
		polyValu = [0] * errSize
		# compute the polynomial terms
		for errPos in range(0, errSize):
			byteValu = EXP[errPos]
			polyValu[errPos] = gf256.PolyEval(argCode, byteValu)
		# return the polynomial
		return (polyValu)

	def Forney(polySynd, eraseLoci, errSize):
		# make a copy of the syndrome polynomial
		polyValu = list(polySynd)

		# compute the polynomial terms
		for posI in range(0, len(eraseLoci)):
			termX = errSize - 1 - eraseLoci[posI]
			termX = EXP[termX]
			for posJ in range(0, len(polyValu) - 1):
				termY = gf256.Mult(polyValu[posJ], termX)
				termY ^= polyValu[posJ + 1]
				polyValu[posJ] = termY
			polyValu.pop()
		# return the polynomial result
		return (polyValu)
	# Locate the message errors
	# errLoci: error locator polynomial
	# errSize: number of error symbols
	def FindErr(errLoci, errSize):
		# initialise the polynomial locals
		errPoly = [1]
		tempPoly = [1]

		# generate the error locator polynomial
		# - Berklekamp-Massey algorithm
		for posSynd in range(0, len(errLoci)):
			tempPoly.append(0)
			termSynd = errLoci[posSynd]

			for posErr in range(1, len(errPoly)):
				termPoly = errPoly[len(errPoly) - posErr - 1]
				termPoly = gf256.Mult(termPoly, errLoci[posSynd - posErr])
				termSynd ^= termPoly

			if (termSynd != 0):
				if (len(tempPoly) > len(errPoly)):
					tNewP = gf256.PolyScale(tempPoly, termSynd)
					tempPoly = gf256.PolyScale(errPoly, gf256.Divi(1, termSynd))
					errPoly = tNewP

				tempValu = gf256.PolyScale(tempPoly, termSynd)
				errPoly = gf256.PolyAdd(errPoly, tempValu)

		# count the number of errors
		errCount = len(errPoly) - 1
		if ((errCount * 2) > len(errLoci)):
			print ("Too many errors to correct")
			return (None)
		else:
			print ("Error count: ", errCount, len(errLoci))

		# calculate the polynomial zeroes
		errList = []
		for errPos in range(0, errSize):
			errZed = gf256.PolyEval(errPoly, EXP[255 - errPos])
			if (errZed == 0):
				errZed = errSize - errPos - 1
				errList.append(errZed)

		if (len(errList) != errCount):
			print ("Could not locate the errors")
			return (None)
		else:
			return (errList)

    def Correct(argCode, polySynd, errList) :
	    # prepare the locator polynomial
	    polyLoci = [1]
	    # print(polyLoci)
	    for errPos in range(0, len(errList)):
	        errTerm = len(argCode) - errList[errPos] - 1
	        errTerm = EXP[errTerm]
	        polyLoci = gf256.PolyMult(polyLoci, [errTerm, 1])

	    # prepare the error evaluator polynomial
	    errEval = polySynd[0:len(errList)]
	    errEval.reverse()
	    errEval = gf256.PolyMult(errEval, polyLoci)

	    tMark = len(errEval) - len(errList)
	    errEval = errEval[tMark:len(errEval)]

	    # the error locator polynomial, minus even terms
	    errLoci = polyLoci[len(polyLoci) % 1 : len(polyLoci) : 2]

	    # start correcting
	    for errPos in range(0, len(errList)):
	        errByte = errList[errPos] - len(argCode) + 256
	        errByte = EXP[errByte]

	        errValu = gf256.PolyEval(errEval, errByte)

	        errAdj = gf256.Mult(errByte, errByte)
	        errAdj = gf256.PolyEval(errLoci, errAdj)

	        mesgByte = gf256.Mult(errByte, errAdj)
	        mesgByte = gf256.Divi(errValu, mesgByte)
	        argCode[errList[errPos]] ^= mesgByte
        return argCode

    def generator(size) :
        # size: number of error symbols
        # val: generator polynomial
        val = [1]
        for i in range(0, size):
            tmp = [1, EXP[i]]
            val = gf256.PolyMult(val, tmp)
        # return the polynomial result
        return (val)
    def Encode(msg, size) :
        gen = RS.generator(size)
        mcc = [0] * (len(msg) + size)
        # print(gen)
        # initialise the output buffer
        for i in range(0, len(msg)):
            mcc[i] = ord(msg[i])
        # print(mcc)
        # begin encoding
        for i in range(0, len(msg)):
            ch = mcc[i]
            # if (ch != 0) :
            for j in range(0, len(gen)):
                mcc[i + j] ^= gf256.Mult(gen[j], ch)

        # print(mcc)
        # finalise the output buffer
        for i in range(0, len(msg)):
            mcc[i] = ord(msg[i])
        # return the output buffer
        return (mcc)

    def Decode(argCode, errSize) :
        # initialise the code buffer
        codeBuffer = list(argCode)
        # count the number of erasures
        eraseCount = []
        for codePos in range(0, len(codeBuffer)):
            if (codeBuffer[codePos] < 0):
                codeBuffer[codePos] = 0
                eraseCount.append(codePos)

        if (len(eraseCount) > errSize):
            print ("Too many erasures")
            return (None)
        # prepare the syndrome polynomial
        polySynd = RS.SyndPoly(codeBuffer, errSize)

        print(polySynd)
        if (max(polySynd) == 0):
            print ("The message has no errors")
            return (codeBuffer)

        # prepare the error locator polynomial
        errLoci = RS.Forney(polySynd, eraseCount, len(codeBuffer))
        # locate the message errors
        errList = RS.FindErr(errLoci, len(codeBuffer))
        if (errList == None):
            print ("Could not find any errors")
            return (None)
        else:
            print ("Located errors: ", errList)
        # start correcting errors and erasures
        outMesg = RS.Correct(codeBuffer, polySynd, (eraseCount + errList))
        return (outMesg)
gf256()
ec, dc = 17,9
msg = "Hi"

mcc = [64,36,134,144,236,17,236,17,236]
mcc = [chr(it) for it in mcc]
# encode the message
edc = RS.Encode(mcc, ec)
print ("Message codeword: ", edc)
edc[3] = 9
edc[7] = -1
edc[10] = 50

# # decode the message
ecm = RS.Decode(edc, ec)
print ("Decoded message : ", ecm)
# print ("\r\r")
