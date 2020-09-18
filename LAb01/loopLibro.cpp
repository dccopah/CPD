//multiplicacion por bloques 6 for 
//estudiar implementar y evaluar el desempe�o
//
//
//valgraim
//cache grein 


#include<bits/stdc++.h>
#define MAX 4
using namespace std;
double D[MAX][MAX], x[MAX], y[MAX];

void FirstLoop(){
	for (int i = 0; i < MAX; i++){
		for (int j = 0; j < MAX; j++){
			y[i] += D[i][j]*x[j];
		}
	}
		
}
void SecondLoop(){
	for (int j = 0; j < MAX; j++)
	{
		for (int i = 0; i < MAX; i++){
			y[i] += D[i][j]*x[j];
		}
	}
}


int main(){
	for(int i=0;i<MAX;i++)
	{
		y[i]=0;
	}
	cout<<"Vector X"<<endl;
	for(int i=0;i<MAX;i++){
		cin>>x[i];
	}
	cout<<"Matriz D"<<endl;
	for(int i=0;i<MAX;i++)
	{
		for(int j=0;j<MAX;j++)
		{
			cin>>D[i][j];
		}
	}
	FirstLoop();
	for(int i=0;i<MAX;i++)
	{
			cout<<y[i]<<" ";
	}
	cout<<endl;
	
	for(int i=0;i<MAX;i++)
	{
		y[i]=0;
	}
	SecondLoop();
	
	for(int i=0;i<MAX;i++)
	{
		cout<<y[i]<<" ";
	}

//----------------------------------------------------------
/*	int A[20][20], B[20][20], C[20][20];
    int k, m, n;
	cout<<"filas";cin>>k;
    cout<<"columnas";cin>>m;
	for(int i=0; i<k; ++i){
        for(int j=0; j<m; ++j)
        {
            cin>>A[i][j];
        }
    }
    
    
    cout<<"columnas";cin>>n;
    for(int i=0; i<m; ++i){
        for(int j=0; j<n; ++j)
        {
            cin>>B[i][j];
        }
    }
    
    for(int i=0; i<k; ++i){
        for(int j=0; j<n; ++j){
            C[i][j] = 0;
        }
    }
    //Multiplicacion
    for(int i=0; i<k; ++i)
        for(int j=0; j<n; ++j)
            for(int z=0; z<m; ++z)
                C[i][j] += A[i][z] * B[z][j];
    
	cout<<"Matriz Resultante"<<endl;      
    for(int i=0; i<k; ++i){
        for(int j=0; j<n; ++j){
            cout<<C[i][j]<<" ";
        }
        cout<<endl;
    }*/
}

