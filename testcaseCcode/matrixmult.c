#include<stdio.h>
#include<stdlib.h>


/*
void initializeMatrix(int* a[],int *m,int *n)
{
    int i,j,x;
    scanf("%d%d",m,n);
    for(i=0;i<*m;i++)
        for(j=0;j<*n;j++)
            {
            scanf("%d",&x);
            a[i][j] = x;
            }
}

void matrixMult(int*a[],int ma,int na,int*b[],int mb,int nb,int*c[])
{
    int i,j,k;
    int mc = ma , nc = nb;
    for(i=0;i<mc;i++)
       { for(j=0;j<nc;j++)
            {for(k=0;k<na;k++)
                c[i][j] += a[i][k]*b[k][j];
            printf("%10d",c[i][j]);
            }
       printf("\n");
       }

}
*/


int main()
{
    int A[20][20],B[20][20],C[20][20];
    int ma,mb,mc,na,nb,nc;
    int i,j,k;
    scanf("%d%d",&ma,&na);
    for(i=0;i<ma;i++)
        for(j=0;j<na;j++)
            scanf("%d",&A[i][j]);
    scanf("%d%d",&mb,&nb);
    for(i=0;i<mb;i++)
        for(j=0;j<nb;j++)
            scanf("%d",&B[i][j]);
    
    if(na != mb)
        printf("Incompatible Demensions\n");
    else 
        {
            mc = ma ;
            nc = nb;
            for(i=0;i<mc;i++)
                { 
                for(j=0;j<nc;j++)
                    {
                    for(k=0;k<na;k++)
                        C[i][j] += A[i][k]*B[k][j];
                    printf("%10d",C[i][j]);
                    }
                printf("\n");
                } 
            
        }
    return 0;



}
