#
# bch157.py
#
# Encoder/Decoder for Non-Systematic (15,7,5) Binary BCH Code
# by snovvcrash
# 05.2017
#

from random import seed, randint
from sys    import exit

n = 15 # codeword length (n = 2^m - 1, m = 4)
k =  7 # information bits
t =  2 # correcting capability

p1 = 0b10011 # irreducible polynomial p1(x) (in GF(2))
             # p1(x) = x^4 + x + 1
p2 = 0b11111 # irreducible polynomial p2(x) (in GF(2))
             # p2(x) = x^4 + x^3 + x^2 + x + 1

g    = ""    # generating polynomial g(x)
             # g(x) = p1(x) * p2(x) = x^8 + x^7 + x^6 + x^4 + 1
c    = ""    # codeword c(x)
             # c(x) = data(x) * g(x)
data = ""    # information polynomial data(x)

# ------------------------------------------------------
# ------------------- GF-ARITHMETIC --------------------
# ------------------------------------------------------

def gf_add(a, b):
	prod = ""

	if len(a) > len(b):
		b = '0'*(len(a) - len(b)) + b
	elif len(a) < len(b):
		a = '0'*(len(b) - len(a)) + a

	for i in range(len(a)):
		# XOR
		if a[i] == b[i]:
			prod += '0'
		else:
			prod += '1'

	return prod

def gf_mul(a, b):
	# Shifting
	shifted = []
	for i in range(len(b)):
		if b[i] != '0':
			shifted.append(a + '0'*(len(b)-i-1))

	# XORing
	prod = ""
	for poly in shifted:
		prod = gf_add(prod, poly)

	return prod

def gf_xor(a, b):
	prod = ""

	for i in range(1, len(b)):
		# XOR
		if a[i] == b[i]:
			prod += '0'
		else:
			prod += '1'

	return prod

def gf_div(a, b, mode=0):
	# mode 0: return remainder
	# mode 1: return quotient

	pick = len(b)
	q = ""       # quotient
	r = a[:pick] # remainder

	while pick < len(a):
		if r[0] == '1':
			q += '1'
			r = gf_xor(b, r) + a[pick]
		else:
			q += '0'
			r = gf_xor('0'*pick, r) + a[pick]
		pick += 1

	if r[0] == '1':
		q += '1'
		r = gf_xor(b, r)
	else:
		q += '0'
		r = gf_xor('0'*pick, r)

	if not mode:
		return r
	return q

# ------------------------------------------------------
# ------------------ ENCODER/DECODER -------------------
# ------------------------------------------------------

# Encoding method: c(x) = data(x) * g(x)
def bch_encode():
	global g
	global data

	return gf_mul(data, g)

# Decoding method: polynomial division (while-loop, shifting until w <= t)
def bch_decode():
	global g
	global c

	syndrome = gf_div(c, g)
	if not weight(syndrome):
		return c

	recd = c
	cnt_rot = 0
	while weight(syndrome) > t:
		if cnt_rot > 100:
			print("bch_decode: Error while decoding, try number of errors = t")
			exit(-3)
		recd = l_rotate(recd)
		syndrome = gf_div(recd, g)
		cnt_rot += 1

	recd = gf_add(recd, syndrome)
	recd = r_rotate(recd, cnt_rot)

	return recd

# ------------------------------------------------------
# --------------------- UTILITIES ----------------------
# ------------------------------------------------------

def l_rotate(poly, s=1):
	return poly[s:] + poly[:s]

def r_rotate(poly, s=1):
	return poly[-s:] + poly[:-s]

def weight(poly):
	return sum([int(coeff) for coeff in poly])

def show_poly(poly):
	wasThereOne = 0

	for coeff in range(len(poly)) :
		if (poly[coeff] != '0') :
			if (coeff != len(poly)-1):
				print("x^" + str(len(poly)-coeff-1) + " + ", end="")
			else:
				print("1", end="")
				wasThereOne = 1

	if wasThereOne:
		print(" + 0")
	else:
		print("0")

# ------------------------------------------------------
# ------------------------ MAIN ------------------------
# ------------------------------------------------------

seed()
data = ''.join([str(randint(0, 1)) for i in range(k)])

g = gf_mul(p1, p2)
# c = bch_encode()
#
# print("Generating polynomial: ", end = "")
# show_poly(g)

# print("Enter number of errors (t = 2):");
# while True:
# 	try:
# 		numerr = int(input())
# 	except ValueError:
# 		print("numerr: Invalid input type")
# 		exit(-1)
# 	if numerr <= 16:
# 		break
# 	print("Try again")
#
# print("Enter number of errors (t = 2, 0 <= err <= " + str(n+1) + "):");
# for i in range(numerr):
# 	while True:
# 		try:
# 			errpos = int(input())
# 		except ValueError:
# 			print("errpos: Invalid input type")
# 			exit(-2)
# 		if 0 <= errpos and errpos <= 15:
# 			break
# 		print("Try again")
#
# 	if c[errpos] == '1':
# 		c = c[:errpos] + '0' + c[errpos+1:]
# 	else:
# 		c = c[:errpos] + '1' + c[errpos+1:]
#
# recd = bch_decode()
# recd = gf_div(recd, g, 1)
#
# print("Original data:  ", end="")
# show_poly(data)
# print("Recovered data: ", end="")
# show_poly(recd)
#
# decerror = 0
# for i in range(k):
# 	if data[i] != recd[i]:
# 		decerror += 1
#
# if decerror:
# 	print("Decoding errors:", decerror);
# else:
# 	print("Successful decoding");
