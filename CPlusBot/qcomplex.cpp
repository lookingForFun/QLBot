#include "qcomplex.h"
#include "math.h"

qcomplex::~qcomplex(void)
{
}

qcomplex::qcomplex() : RealPart(0.0), ImagPart(0.0)
{
}

qcomplex::qcomplex(double RealValue, double ImagValue) : RealPart(RealValue), ImagPart(ImagValue)
{
}

qcomplex qcomplex::operator +(const qcomplex &rhs)
{
	return qcomplex(RealPart + rhs.RealPart, ImagPart + rhs.ImagPart);
}

qcomplex qcomplex::operator -(const qcomplex &rhs)
{
	return qcomplex(RealPart - rhs.RealPart, ImagPart - rhs.ImagPart);
}

qcomplex qcomplex::operator*(const qcomplex &rhs)
{
	return qcomplex(RealPart * rhs.RealPart - ImagPart * rhs.ImagPart, ImagPart * rhs.RealPart + RealPart * rhs.ImagPart);
}

qcomplex qcomplex::operator/(const qcomplex &rhs)
{
	double denom = rhs.ImagPart*rhs.ImagPart + rhs.RealPart*rhs.RealPart;
	return qcomplex((ImagPart*rhs.ImagPart + RealPart*rhs.RealPart)/denom, (ImagPart*rhs.RealPart - RealPart*rhs.ImagPart)/denom);
}

qcomplex qcomplex::operator-()
{
	return qcomplex(-RealPart, -ImagPart);
}

void qcomplex::operator+=(const qcomplex &rhs)
{
	RealPart += rhs.RealPart;
	ImagPart += rhs.ImagPart;
}

/*void qcomplex::operator+=(const double &rhs)
{
	RealPart += rhs;
}*/

void qcomplex::operator/=(const qcomplex &rhs)
{
	double denom = rhs.ImagPart*rhs.ImagPart + rhs.RealPart*rhs.RealPart;
	RealPart = (ImagPart*rhs.ImagPart + RealPart*rhs.RealPart)/denom;
	ImagPart = (ImagPart*rhs.RealPart - RealPart*rhs.ImagPart)/denom;
}

/*void qcomplex::operator/=(const double &rhs)
{
	RealPart /= rhs;
	ImagPart /= rhs;
}*/

void qcomplex::operator=(const qcomplex &rhs)
{
	RealPart = rhs.RealPart;
	ImagPart = rhs.ImagPart;
}

void qcomplex::operator=(const double &rhs)
{
	RealPart = rhs;
	ImagPart = 0.0;
}

bool qcomplex::operator==(const qcomplex &rhs)
{
	return (RealPart == rhs.RealPart) && (ImagPart == rhs.ImagPart);
}

double qcomplex::RIPart()
{
	// we want to make sure that this gives real results for very-nearly real numbers.
	// so, if we have 10,000 + -1.8e-15 it should return 
	double epsilon = 0.00000000001;
	if( abs(ImagPart) < abs(RealPart)/epsilon )
		return RealPart;
	else
		return RealPart + ImagPart * sqrt(-1.0);
}

qcomplex qcomplex::Sqrt()
{
	return Pow(0.5);
}

qcomplex qcomplex::Pow(double Power)
{
	// (r cis O)^n == (r^n cis O*n)
	double r = sqrt(RealPart*RealPart + ImagPart*ImagPart);
	double theta = atan2(ImagPart, RealPart);
	r = pow(r, Power);
	theta *= Power;
	return qcomplex(r*cos(theta), r*sin(theta));
}