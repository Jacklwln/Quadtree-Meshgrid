#ifndef POINT_H_
#define POINT_H_



class Node  //�����Ϣ
{
public:
	double x,y;  //�������
public:
	Node();
	Node(double p1,double p2);
	Node(const Node &p);
	Node operator+(const Node &P)const;
	Node operator-(const Node &P)const;
	bool operator==(const Node &P)const;
	double operator*(const Node &P)const;
	Node operator/(const double a)const;
	void show();
};

#endif