// Quadtree_Libo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Point.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include<fstream>
#include<iomanip>
using namespace std;

#define nag_infi -1e+10  //������
#define pos_infi 1e+10  //������
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
//#define max_level 8  //����ĵȼ����ֵ(����ֵ)
#define eps 1e-5

typedef struct Elem  //��Ԫ��Ϣ
{
	int i;  //��Ԫ����,1ΪҶ��
	int j;  //��Ԫ��Ҷ�������е�λ��
	int k;  //��Ԫ�ȼ�
	int p;  //��Ԫ���ʷ����е�λ�ã�0Ϊ�߽磬1Ϊ�ⲿ��2Ϊ�ڲ�
	int mark;  //��Ԫ���
	int Neighbour[8];  //���ڵ�Ԫ���
	Node Nodes[2];  //��Ԫ�����ţ����������£�
	Node center;  //���ĵ�����
	int father;  //����Ԫ���
	int son[4];  //�ӵ�Ԫ���
}Elem;


typedef struct Region  //�ʷ�����Ϣ
{
	int n;  //�������
	int m;  //�ߵ�����
	Node *nodes;  //�������
	int **sides;  //�߶�Ӧ�Ķ�����
	int *outside;  //��߽���Ϣ
	int osn;  //��߽������
	int **inside;  //�ڱ߽���Ϣ
	int isn;  //�ڱ߽�����
	int *ise;  //�ڱ߽����Ϣ
}Region;

vector<Elem> Elems;
Elem Es;
Region Region;
int NodesNum;  //��ǰ�������
int SidesNum;  //��ǰ�ߵ�����
int ElemsNum;  //��ǰ��Ԫ����
int LeafsNum;  //��ǰҶ����
int Level=0;  //��ǰ�ȼ����ֵ
int max_level;  //��Ԫ�ȼ����ֵ
int max_level1;
vector<int> Leafs;  //Ҷ�ӵ�Ԫ���
vector<int> Elems_opr;  //��Ҫϸ�ֵĵ�Ԫ���
vector<int> constrat;  //ϸ�ֵ�Ԫ��ż�¼

void Input();  //�����ʷ�������
void LoadNodes(istream& fin);
void LoadSides(istream& fin);
void LoadDomain(istream& fin);
void Initialization();  //��ʼ��
void find_center(int a);  //��a�����ĵ�����
bool Point_Online(Node &P,Node &Q1,Node &Q2);    //�жϵ�P�Ƿ����߶�Q1Q2��
bool Line_Cross(Node &P1,Node &P2,Node &Q1,Node &Q2);    //�ж����߶��Ƿ��ཻ(P1P2����Q1Q2)
int Point_Inside(Node &P);    //�жϵ��Ƿ��ڶ�����ڲ�(0Ϊ�߽��ϣ�1Ϊ�ڲ���2Ϊ�ⲿ)
//bool Line_Inside(Node &P1,Node &P2)
void dissection();    //������ʷ�
void seperat_to_4(int a);  //��aһ��Ϊ4
void find_Neighbour(int a,int a1,int a2,int a3,int a4);  //��a���ڵ�Ԫ���
void subdivision(int a);  //ϸ�ֵ�Ԫa
int find_elems();  //�ҵ�����Ҫϸ�ֵĵ�Ԫ
int Position_judge(int a);
bool Point_InMatrix(Node &P,Node &Q1,Node &Q2);
void Output();
bool insideConstrat(int a);
void find_element1();
void dissection1();
void Cuhua(int a);

int _tmain(int argc, _TCHAR* argv[])
{
	Input();
    Initialization();
	dissection();
	//dissection1();
	Output();
	system("pause");
	return 0;
}

/*---------------------------------------------------
�������ܣ���ʼ���޶�����Ϣ 
�������룺��ʼ����������ݽṹRegion,�����ļ�Ϊ*.txt
----------------------------------------------------*/
void Input()
{
	string filename;
loop:
	cout<<"����������ļ������޺�׺����";
	cin>>filename;
	string tmp=filename+".txt";
	ifstream fin(tmp.c_str());
	if(fin.good())
	{
		LoadNodes(fin);
		LoadSides(fin);
		LoadDomain(fin);
	}
	else 
	{
		cout<<"���ļ��򲻴��ڵ��ļ���"<<endl;
		filename.clear();
		goto loop;
	}
	fin.close();
	cout<<"��������������Ԫ�ȼ�Ϊ��";
	cin>>max_level;
}

void LoadNodes(istream& fin)
{
	int i;
	fin>>Region.n;
	if(Region.n==0)
	{
		cout<<"bad node number!"<<endl;
		return;
	}
	Region.nodes=new Node[Region.n];
	string dump;
	for(i=0;i<Region.n;i++)
	{
		fin>>dump>>Region.nodes[i].x>>Region.nodes[i].y;
	}
}

void LoadSides(istream& fin)
{
	int i;
	fin>>Region.m;
	if(Region.m==0)
	{
		cout<<"bad side number!"<<endl;
		return;
	}
	Region.sides=new int*[Region.m];
	for(i=0;i<Region.m;i++)
	{
		Region.sides[i]=new int[2];
	}
	string dump;
	for(i=0;i<Region.m;i++)
	{
		fin>>dump>>Region.sides[i][0]>>Region.sides[i][1];
	}
}

void LoadDomain(istream& fin)
{
	int i,j;
	fin>>Region.osn;
    if(Region.osn==0)
	{
		cout<<"bad side number!"<<endl;
		return;
	}
	Region.outside=new int[Region.osn];
	string dump;
	for(i=0;i<Region.osn;i++)
	{
		fin>>dump>>Region.outside[i];
	}
    fin>>Region.isn;
    if(Region.isn==0)
	{
		return;
	}
	Region.ise=new int[Region.isn];
	Region.inside=new int*[Region.isn];
	for(i=0;i<Region.isn;i++)
	{
		fin>>Region.ise[i];
		Region.inside[i]=new int[Region.ise[i]];
		for(j=0;j<Region.ise[i];j++)
		{
			fin>>dump>>Region.inside[i][j];
		}	
	}
}


/*-----------------------------------------------
�������ܣ���ʼ���������ݽṹ
----------------------------------------------*/
void Initialization()
{
	int i;
	double Length=0;    //��ʼ���α߳�
	double x_min=pos_infi,x_max=nag_infi,y_min=pos_infi,y_max=nag_infi;  //�޶����е���С�����ᣬ������
    for(i=0;i<Region.m;i++)
	{
		x_min=min(Region.nodes[i].x,x_min);
		x_max=max(Region.nodes[i].x,x_max);
        y_min=min(Region.nodes[i].y,y_min);
		y_max=max(Region.nodes[i].y,y_max);
		Length=max(x_max-x_min,y_max-y_min);
	}
	Elems.push_back(Es);
	Elems[0].i=1;Elems[0].k=0;Elems[0].father=-1;
    for(i=0;i<8;i++)
	{
		Elems[0].Neighbour[i]=-1;
	}
	for(i=0;i<4;i++)
	{
		Elems[0].son[i]=-1;
	}
	/*Elems[0].Nodes[0]=0;Elems[0].Nodes[1]=1;*/
	ElemsNum=1;LeafsNum=1;
	Elems[0].Nodes[0].x=x_min;Elems[0].Nodes[0].y=y_max;
	Elems[0].Nodes[1].x=x_min+Length;Elems[0].Nodes[1].y=y_max-Length;
	find_center(0);
	Leafs.push_back(0);
	Elems[0].j=0;
	Elems[0].p=0;
}


/*----------------------------------------
�������ܣ��жϵ��Ƿ����߶���
---------------------------------------*/
bool Point_Online(Node &P,Node &Q1,Node &Q2)
{
    
	if ((min(Q1.x,Q2.x)<=P.x) && (P.x<=max(Q1.x,Q2.x)) && (min(Q1.y,Q2.y)<=P.y) && (P.y<=max(Q1.y,Q2.y)))
	{
		if (fabs((P-Q1)*(Q2-Q1))<eps)
			return true;
	}
	return false;
}


/*---------------------------------------------------
�������ܣ��ж�P1,P2�Ƿ����Q1,Q2
---------------------------------------------------*/
bool Line_Cross(Node &P1,Node &P2,Node &Q1,Node &Q2)
{
	if ((min(P1.x,P2.x)>max(Q1.x,Q2.x)) || (min(P1.y,P2.y)>max(Q1.y,Q2.y)) || (min(Q1.x,Q2.x)>max(P1.x,P2.x)) || (min(Q1.y,Q2.y)>max(P1.y,P2.y)))
	{
		return false;
	}
	else if ((Point_Online(P1,Q1,Q2)) || (Point_Online(P2,Q1,Q2))) return false;
	else if (((P1-Q1)*(Q2-Q1))*((Q2-Q1)*(P2-Q1))>=0) {return true;}
	return false;
}

/*------------------------------------------
�������ܣ��жϵ��Ƿ��ڶ������
-----------------------------------------*/
int Point_Inside(Node &P)
{	
	Node Q(nag_infi,P.y);
	int i,count=0;
	for(i=0;i<Region.m;i++)
	{
		if (Point_Online(P,Region.nodes[Region.sides[i][0]],Region.nodes[Region.sides[i][1]])) return 0;
		if (Region.nodes[Region.sides[i][0]].y!=Region.nodes[Region.sides[i][1]].y)
		{
             if ((Point_Online(Region.nodes[Region.sides[i][0]],P,Q) && Region.nodes[Region.sides[i][0]].y>Region.nodes[Region.sides[i][1]].y) ||
				  (Point_Online(Region.nodes[Region.sides[i][1]],P,Q) && Region.nodes[Region.sides[i][1]].y>Region.nodes[Region.sides[i][0]].y))
			 {count++;}
			 else if (Line_Cross(P,Q,Region.nodes[Region.sides[i][0]],Region.nodes[Region.sides[i][1]])){count++;}
		}
	}
	if (count%2==1) {return 2;}
	return 1;
}


/*------------------------------------------
�������ܣ���ʼ�ʷ������
--------------------------------------*/
void dissection()
{
	int i,k;
	seperat_to_4(0);
	while(Level<max_level)
	{
		k=find_elems();
		//k=LeafsNum;
		if (k==0){break;}
		for(i=0;i<k;i++)
		{
			//subdivision(Leafs[i]);
			subdivision(Elems_opr[i]);
		}
		Elems_opr.clear();
	}
}

/*------------------------------------------
�������ܣ�ϸ�ֵ�Ԫa
------------------------------------------*/
void subdivision(int a)
{
	int i;
	for(i=0;i<4;i++)
	{
		if (Elems[a].Neighbour[2*i]==-1){continue;}
		else if(Elems[Elems[a].Neighbour[2*i]].k<Elems[a].k)
		{subdivision(Elems[a].Neighbour[2*i]);}
	}
	if (Elems[a].i==1)
	seperat_to_4(a);
}

/*---------------------------------------------
�������ܣ���Ԫ����
------------------------------------------*/
void find_center(int a)
{
	Elems[a].center.x=(Elems[a].Nodes[0].x+Elems[a].Nodes[1].x)/2.0;
    Elems[a].center.y=(Elems[a].Nodes[0].y+Elems[a].Nodes[1].y)/2.0;
}

/*-------------------------------------------
�������ܣ������ڵ�Ԫ���
-------------------------------------------*/
void find_Neighbour(int a,int a1,int a2,int a3,int a4)
{
	Elems[a].Neighbour[0]=a1;Elems[a].Neighbour[1]=a1;
    Elems[a].Neighbour[2]=a2;Elems[a].Neighbour[3]=a2;
	Elems[a].Neighbour[4]=a3;Elems[a].Neighbour[5]=a3;
	Elems[a].Neighbour[6]=a4;Elems[a].Neighbour[7]=a4;

}

/*------------------------------------
�������ܣ��ҳ�����Ҫϸ�ֵĵ�Ԫ
------------------------------------*/
int find_elems()
{
	int count=0;
    for(int i=0;i<LeafsNum;i++)
	{
		if (Elems[Leafs[i]].p==0)
		{
			Elems_opr.push_back(Leafs[i]);count++;
		}
	}
	return count;
}

/*--------------------------------------------------
�������ܣ���������aһ��Ϊ4
-------------------------------------------------*/
void seperat_to_4(int a)
{
	int i,j,k,m;
	for(i=0;i<4;i++)
	{
		Elems.push_back(Es);
	}


	//����Ҷ����Ϣ
	Leafs[Elems[a].j]=ElemsNum;
	Elems[a].i=0;
	for(i=0;i<3;i++)
	{
		Leafs.push_back(ElemsNum+i+1);
	}

	//���µ�Ԫ��Ϣ
	Elems[ElemsNum].Nodes[0]=Elems[a].Nodes[0];Elems[ElemsNum].Nodes[1]=Elems[a].center;

	Elems[ElemsNum+1].Nodes[0].x=Elems[a].Nodes[0].x;Elems[ElemsNum+1].Nodes[0].y=Elems[a].center.y;
	Elems[ElemsNum+1].Nodes[1].x=Elems[a].center.x;Elems[ElemsNum+1].Nodes[1].y=Elems[a].Nodes[1].y;

	Elems[ElemsNum+2].Nodes[0]=Elems[a].center;Elems[ElemsNum+2].Nodes[1]=Elems[a].Nodes[1];

	Elems[ElemsNum+3].Nodes[0].x=Elems[a].center.x;Elems[ElemsNum+3].Nodes[0].y=Elems[a].Nodes[0].y;
	Elems[ElemsNum+3].Nodes[1].x=Elems[a].Nodes[1].x;Elems[ElemsNum+3].Nodes[1].y=Elems[a].center.y;

	if (Elems[a].p!=0)
	{
		for(i=0;i<4;i++)
		{
			Elems[ElemsNum+i].p=Elems[a].p;
		}
	}
	for(i=0;i<4;i++)
	{
		Elems[a].son[i]=ElemsNum+i;
		find_center(ElemsNum+i);
		Elems[ElemsNum+i].i=1;
		Elems[ElemsNum+i].k=1+Elems[a].k;
		Elems[ElemsNum+i].j=LeafsNum+i-1;
		Elems[ElemsNum+i].father=a;
		Elems[ElemsNum+i].mark=0;
		Elems[ElemsNum+i].p=Position_judge(ElemsNum+i);
		for(j=0;j<4;j++)
		{
			Elems[ElemsNum+i].son[j]=-1;
		}
	}
	Elems[ElemsNum].j=Elems[a].j;Elems[a].j=-1;
	find_Neighbour(ElemsNum,Elems[a].Neighbour[0],ElemsNum+1,ElemsNum+3,Elems[a].Neighbour[7]);
    find_Neighbour(ElemsNum+1,Elems[a].Neighbour[1],Elems[a].Neighbour[2],ElemsNum+2,ElemsNum);
    find_Neighbour(ElemsNum+2,ElemsNum+1,Elems[a].Neighbour[3],Elems[a].Neighbour[4],ElemsNum+3);
	find_Neighbour(ElemsNum+3,ElemsNum,ElemsNum+2,Elems[a].Neighbour[5],Elems[a].Neighbour[6]);

	//�������ڵ�Ԫ��Ϣ
    for(i=0;i<4;i++)
	{
		j=(2*i+4)%8;
		if (Elems[a].Neighbour[i*2]==-1){continue;}
		if (Elems[a].Neighbour[2*i]==Elems[a].Neighbour[2*i+1])
		{
			Elems[Elems[a].Neighbour[i*2]].Neighbour[j+1]=ElemsNum+i;
            Elems[Elems[a].Neighbour[i*2]].Neighbour[j]=ElemsNum+(i+1)%4;
		}
		else
		{
			for(k=0;k<2;k++)
			{
				m=(i+k)%4;
			    Elems[Elems[a].Neighbour[i*2+k]].Neighbour[j+1]=ElemsNum+m;
                Elems[Elems[a].Neighbour[i*2+k]].Neighbour[j]=ElemsNum+m;
			}
		}
	}
	
	//���µȼ����ֵ
	Level=max(1+Elems[a].k,Level);

	ElemsNum+=4;LeafsNum+=3;
}

int Position_judge(int a)
{
	if (Elems[a].p!=0){return Elems[a].p;}
	int i,j,k;
	Node a_point[4];
	a_point[0]=Elems[a].Nodes[0];a_point[2]=Elems[a].Nodes[1];
	a_point[1].x=a_point[0].x;a_point[1].y=a_point[2].y;
    a_point[3].x=a_point[2].x;a_point[3].y=a_point[0].y;
    for(i=0;i<Region.m;i++)
	{
		for(j=0;j<4;j++)
		{
			k=(j+1)%4;
			if (Point_InMatrix(Region.nodes[Region.sides[i][0]],a_point[0],a_point[2]) 
				|| Point_InMatrix(Region.nodes[Region.sides[i][1]],a_point[0],a_point[2])
				|| Point_InMatrix((Region.nodes[Region.sides[i][0]]+Region.nodes[Region.sides[i][1]])/2,a_point[0],a_point[2]))
				return 0;
			if ((Line_Cross(a_point[j],a_point[k],Region.nodes[Region.sides[i][0]],Region.nodes[Region.sides[i][1]]))
				&& (Line_Cross(Region.nodes[Region.sides[i][0]],Region.nodes[Region.sides[i][1]],a_point[j],a_point[k])))
			{
				return 0;
			}
		}
	}
	return (Point_Inside(Elems[a].center));
}

bool Point_InMatrix(Node &P,Node &Q1,Node &Q2)
{
	return ((min(Q1.x,Q2.x)<P.x) && (P.x<max(Q1.x,Q2.x)) && (min(Q1.y,Q2.y)<P.y) && (P.y<max(Q1.y,Q2.y)));
}


/*-----------------------------------------------
�������ܣ��������
-----------------------------------------------*/
void Output()
{
	int i,j,k=0;
	FILE* fout;
	fout = fopen("output.txt", "w");
	fprintf(fout, "Elememts Number:%6d\t%6d\n", LeafsNum,Level);
	for(i = 0; i < LeafsNum; i++)
	{
		fprintf(fout, "%5d:\t%5d\t%5d\n", Leafs[i], Elems[Leafs[i]].p,Elems[Leafs[i]].k);
	}

	fclose(fout);

    fout = fopen("draw.txt", "w");
	fprintf(fout, "%6d\t%6d\n", LeafsNum,Level);
	for(i = 0; i < LeafsNum; i++)
	{
		if ((Elems[Leafs[i]].p==1) || (Elems[Leafs[i]].mark==1)){continue;}
		k++;
		fprintf(fout, "%lf\t%lf\n%lf\t%lf\n%lf\t%lf\n%lf\t%lf\n",
			Elems[Leafs[i]].Nodes[0].x,Elems[Leafs[i]].Nodes[0].y,
            Elems[Leafs[i]].Nodes[0].x,Elems[Leafs[i]].Nodes[1].y,
			Elems[Leafs[i]].Nodes[1].x,Elems[Leafs[i]].Nodes[1].y,
			Elems[Leafs[i]].Nodes[1].x,Elems[Leafs[i]].Nodes[0].y);
	}
	fclose(fout);
	
	fout = fopen("Input.txt", "w");
	fprintf(fout, "%6d\t%6d\n", Region.m,Region.n);
	for(i = 0; i < Region.m; i++)
	{
		for(j=0;j<2;j++)
		{
			fprintf(fout, "%lf\t%lf\n",Region.nodes[Region.sides[i][j]].x,Region.nodes[Region.sides[i][j]].y);
		}
	}
	fclose(fout);

    cout<<"��Ԫ����Ϊ"<<k<<endl;
	cout<<"��ϸ��Ϣ�����\"output.txt\"��\"draw.txt\""<<endl;

}

void coaring()
{
	int k;
	for(k=1;k<ElemsNum;k=k+4)
	{
		if ((Elems[k].k!=max_level) || insideConstrat(k)) continue;
        Cuhua(k);
	}
}

bool insideConstrat(int a)
{
	for (int i=0;i<constrat.size();i++)
	{
		if (a==constrat[i])
			return true;
	}
	return false;
}


void Cuhua(int a)
{
	Elems[Elems[a].father].i=1;
	Elems[Elems[a].father].j=Elems[a].j;
	Leafs[Elems[a].j]=Elems[a].father;
	for(int i=0;i<4;i++)
	{
		Elems[a+i].i=0;
		Elems[a+i].mark=1;
	}
}

void dissection1()
{
	Node a(0.0,0.0),b(0.0,2.0),s;
	int i;
	int maxLeafs;
	ifstream fin("adaptive.txt",ios::in);
	maxLeafs=LeafsNum;
	while(!fin.eof())
	{
		fin>>s.x>>s.y;
        
		for(i=0;i<maxLeafs;i++)
		{
			if (Point_InMatrix(s,Elems[Leafs[i]].Nodes[0],Elems[Leafs[i]].Nodes[1]))
			{
				subdivision(Leafs[i]);
			}
		}
	}
	fin.close();
	//ifstream fina("adaptive.txt",ios::in);
	//maxLeafs=LeafsNum;
	//while(!fina.eof())
	//{
	//	fina>>s.x>>s.y;
 //       
	//	for(i=0;i<maxLeafs;i++)
	//	{
	//		if (Point_InMatrix(s,Elems[Leafs[i]].Nodes[0],Elems[Leafs[i]].Nodes[1]))
	//		{
	//			subdivision(Leafs[i]);
	//		}
	//	}
	//}
	//maxLeafs=LeafsNum;
	//fina.close();
	//ifstream finb("adaptive.txt",ios::in);
	//while(!finb.eof())
	//{
	//	finb>>s.x>>s.y;
 //       
	//	for(i=0;i<maxLeafs;i++)
	//	{
	//		if (Point_InMatrix(s,Elems[Leafs[i]].Nodes[0],Elems[Leafs[i]].Nodes[1]))
	//		{
	//			subdivision(Leafs[i]);
	//		}
	//	}
	//}
	//maxLeafs=LeafsNum;
	//finb.close();
	ifstream fin1("adaptive1.txt",ios::in);
	maxLeafs=LeafsNum;
	while(!fin1.eof())
	{
		fin1>>s.x>>s.y;
		for(i=0;i<maxLeafs;i++)
		{
			if (Point_InMatrix(s,Elems[Leafs[i]].Nodes[0],Elems[Leafs[i]].Nodes[1]))
			{
				subdivision(Leafs[i]);

			}
		}
	}
	fin1.close();
	//ifstream fin2("adaptive2.txt",ios::in);
	//maxLeafs=LeafsNum;
	//	while(!fin2.eof())
	//{
	//	fin2>>s.x>>s.y;
	//	for(i=0;i<maxLeafs;i++)
	//	{
	//		if (Point_InMatrix(s,Elems[Leafs[i]].Nodes[0],Elems[Leafs[i]].Nodes[1]))
	//		{
	//			subdivision(Leafs[i]);

	//		}
	//	}
	//}
	//	fin2.close();
	//max_level1=8;

	//for(int j=0;j<4;j++)
	//{
	//	maxLeafs=LeafsNum;
	//	for(i=0;i<maxLeafs;i++)
	//	{
	//		cout<<i<<endl;
	//		if ((Point_Online(Elems[Leafs[i]].Nodes[0],a,b)) || (Point_Online(Elems[Leafs[i]].Nodes[1],a,b)))
	//		{
	//			subdivision(Leafs[i]);
	//		}
	//	}
	//}
	//while(Level<max_level1)
	//{
	//	for(i=0;i<4;i++)
	//	{
	//		if (Point_InMatrix(a,Elems[ElemsNum-i-1].Nodes[0],Elems[ElemsNum-i-1].Nodes[1]))
	//		{
	//			subdivision(ElemsNum-i-1);break;
	//		}
	//	}
	//}
	//a.x=1.4;a.y=0.2;
	//max_level1=9;
	//for(i=0;i<LeafsNum;i++)
	//{
	//	if (Point_InMatrix(a,Elems[Leafs[i]].Nodes[0],Elems[Leafs[i]].Nodes[1]))
	//	{
	//		subdivision(Leafs[i]);break;
	//	}
	//}

	//while(Level<max_level1)
	//{
	//	for(i=0;i<4;i++)
	//	{
	//		if (Point_InMatrix(a,Elems[ElemsNum-i-1].Nodes[0],Elems[ElemsNum-i-1].Nodes[1]))
	//		{
	//			subdivision(ElemsNum-i-1);break;
	//		}
	//	}
	//}

}

void Xifen(int a)
{
	for(int i=0;i<4;i++)
	{
		constrat.push_back(a+i);
	}
	if (Elems[a].son[0]!=-1)
	{
		Elems[a].i=0;
		Leafs[Elems[a].j]=Elems[a].son[0];
		for(int i=0;i<4;i++)
		{
			Elems[Elems[a].son[i]].mark=0;
			Elems[Elems[a].son[i]].i=1;
		}
		return;
	}

    seperat_to_4(a);
}