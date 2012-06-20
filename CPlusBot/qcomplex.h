#pragma once

class qcomplex
{
private:
	double RealPart;
	double ImagPart;
public:
	qcomplex();
	qcomplex(double RealValue, double ImagValue);
	~qcomplex(void);

	double RPart(void);
	double IPart(void);
	double RIPart(void);

	qcomplex Sqrt(void);
	qcomplex Pow(double Power);

	qcomplex operator+(const qcomplex &rhs);
	qcomplex operator-(const qcomplex &rhs);
	qcomplex operator*(const qcomplex &rhs);
	qcomplex operator/(const qcomplex &rhs);
	qcomplex operator-();
	void operator+=(const qcomplex &rhs);
	void operator/=(const qcomplex &rhs);
	void operator=(const qcomplex &rhs);
	void operator=(const double &rhs);
	bool operator==(const qcomplex &rhs);
};