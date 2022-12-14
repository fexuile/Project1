#include<bits/stdc++.h>
#include"jsoncpp/json.h"
using namespace std;
typedef pair<int,int> pii;
const int N=10;
int board[N][N],dx[4]={0,1,0,-1},dy[4]={1,0,-1,0};
int vis_air[N][N];
void read(){
    string str;
	int x,y;
	getline(cin,str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input); 
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++) 
	{
		x=input["requests"][i]["x"].asInt(), y=input["requests"][i]["y"].asInt();
		if (x!=-1) board[x][y]=1;
		x=input["responses"][i]["x"].asInt(), y=input["responses"][i]["y"].asInt();
		if (x!=-1) board[x][y]=-1;
	}
	x=input["requests"][turnID]["x"].asInt(), y=input["requests"][turnID]["y"].asInt();
	if (x!=-1) board[x][y]=1;
}
namespace MCTS{
    int _N,num,node;
    int b[2000001][N][N],put[2000010];
    int val[2000010],_n[2000010],fa[2000010];
    double ucb[2000010];
    vector<int>son[2000010];
    bool in(int x,int y){return x>=0&&x<9&&y>=0&&y<9;}
    int air(int id,int x,int y){
        vis_air[x][y]=1;
        int flag=0;
        for(int i=0;i<4;i++){
            int X=x+dx[i],Y=y+dy[i];
            if(!in(X,Y))continue;
            if(b[id][X][Y]==0)flag|=1;
            else if(b[id][X][Y]==b[id][x][y]&&!vis_air[X][Y])flag|=air(id,X,Y);
        }
        return flag;
    }
    bool ok(int id,int x,int y,int col){
        if(b[id][x][y])return false;
        b[id][x][y]=col;memset(vis_air,0,sizeof(vis_air));
        if(!air(id,x,y)){b[id][x][y]=0;return false;}
        for(int i=0;i<4;i++){
            int X=x+dx[i],Y=y+dy[i];
            if(!in(X,Y))continue;
            if(b[id][X][Y]&&!vis_air[X][Y]){
                if(!air(id,X,Y)){b[id][x][y]=0;return false;}
            }
        }
        b[id][x][y]=0;return true;
    }
    int h(int id){
        int s=0;
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,1))s++;
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,-1))s--;
        return s;
    }
    void update(){
        int flag=0,del=h(node);
        while(node){
            val[node]+=flag?del:-del;_n[node]++;
            node=fa[node];flag^=1;
        }
    }
    bool end(int id){
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,1))return false;
        return true;
    }
    void expand(int id){
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,1)){
                    num++;
                    _n[num]=val[num]=0;
                    put[num]=i*9+j;
                    for(int k=0;k<9;k++)
                        for(int l=0;l<9;l++)b[num][k][l]=-b[id][k][l];
                    b[num][i][j]=-1;fa[num]=id;
                    son[id].push_back(num);
                }
    }
    void mcts(int id){
        if(end(id)){node=id;return;}
        if(!son[id].size())expand(id);
        for(int s:son[id])if(!_n[s]){node=s;return;}
        double mx=0;int Max=0;
        for(int s:son[id])ucb[s]=val[s]*1./_n[s]+2*sqrt((double)log((double)_N)*1./_n[s]);
        for(int s:son[id])
            if(ucb[s]>mx)mx=ucb[s],Max=s;
        mcts(Max);
    }
    vector<int>work(){
        int Time=clock();
        vector<int>ans;ans.clear();

        num=1;_N=0;
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)b[1][i][j]=-board[i][j];
        
        expand(1);
        int timeout = (int)(0.85 * (double)CLOCKS_PER_SEC);
        while(clock()-Time<=timeout){
            _N++;
            node=1;mcts(1);
            update();
        }

        int mx=-1e9-10;
        for(int s:son[1])if(val[s]>mx)mx=val[s];
        for(int s:son[1])if(val[s]==mx)ans.push_back(put[s]);
        printf("%d\n",ans.size());
        return ans;
    }
}
void output(vector<int>ans){
    int result=ans[rand()%ans.size()];
	Json::Value ret;
	Json::Value action;
    action["x"]=result/9; action["y"]=result%9;
	ret["response"]=action;
	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;
}
int main(){
    srand((unsigned)time(0));
    read();
    vector<int>ans = MCTS::work();
    output(ans);
    return 0;
}