#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<cstring>
#define clr(X) memset(X,0,sizeof(X))
#include<string>
#include<iomanip>
#include<vector>
#include<set>
#include<list>
#include<stack>
#include<queue>
#include<bitset>
#include<map>
#define Z(V) cout << #V << ":"<< (V) << endl
//#include<fstream>
using namespace std;

const int Ni = 210;
const int MAX = 1<<26;
struct Edge{
	int u,v,c;
	int next;
}edge[Ni*Ni];
int n,m;
int edn;//边数
int p[Ni];//父亲
int d[Ni];
int sp,tp;//原点，汇点

void addedge(int u,int v,int c)
{
	edge[edn].u=u; edge[edn].v=v; edge[edn].c=c;
	edge[edn].next=p[u]; p[u]=edn++;

	edge[edn].u=v; edge[edn].v=u; edge[edn].c=0;
	edge[edn].next=p[v]; p[v]=edn++;
}
int bfs()
{
	queue <int> q;
	memset(d,-1,sizeof(d));
	d[sp]=0;
	q.push(sp);
	while(!q.empty())
	{
		int cur=q.front();
		q.pop();
		for(int i=p[cur];i!=-1;i=edge[i].next)
		{
			int u=edge[i].v;
			if(d[u]==-1 && edge[i].c>0)
			{
				d[u]=d[cur]+1;
				q.push(u);
			}
		}
	}
	return d[tp] != -1;
}
int dfs(int a,int b)
{
	int r=0;
	if(a==tp)return b;
	for(int i=p[a];i!=-1 && r<b;i=edge[i].next)
	{
		int u=edge[i].v;
		if(edge[i].c>0 && d[u]==d[a]+1)
		{
			int x=min(edge[i].c,b-r);
			x=dfs(u,x);
			r+=x;
			edge[i].c-=x;
			edge[i^1].c+=x;
		}
	}
	if(!r)d[a]=-2;
	return r;
}

int dinic(int sp,int tp)
{
	int total=0,t;
	while(bfs())
	{
		while((t=dfs(sp,MAX)))
			total+=t;
	}
	return total;
}

int main()
{
	int np,nd,nc,ne,i,u,v,c;
	char ttt;
	stdin = fopen("in.txt","r");
	while(scanf("%d %d %d %d",&nd,&np,&nc,&ne) != -1)
	{
		sp = nd;
		tp = sp + 1;
		edn = 0;
		memset(p,-1,sizeof(p));
		for(i = 0;i < ne;i++)
		{
			scanf(" %c%d,%d%c%d",&ttt,&u,&v,&ttt,&c);
			addedge(u,v,c);
		}
		for(i = 0;i < np;i++)
		{
			scanf(" %c%d%c%d",&ttt,&v,&ttt,&c);
			addedge(sp,v,c);
		}
		for(i = 0;i < nc;i++)
		{
			scanf(" %c%d%c%d",&ttt,&u,&ttt,&c);
			addedge(u,tp,c);
		}
		printf("%d\n",dinic(sp,tp));
	}
	return 0;
}
