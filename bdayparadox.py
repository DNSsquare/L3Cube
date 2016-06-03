from decimal import Decimal
def fact(ans,k):
	if(k==1):
		return ans
	else:
		ans=ans*fact(ans-1, k-1)
		return ans

def gcd(a,b):
	while b:
		a,b=b,a%b
	return abs(a)


k=input("Enter number of people   ")
if (k==0 or k==1):
	print "0"
	exit()
a=fact(364,k-1)
p=365**(k-1)
g=gcd(a,p)
a=a/g
p=p/g
ans=Decimal(a)/(p)
print 1-ans
