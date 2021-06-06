#include<stdio.h>
#include<stdlib.h>


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
                c[i][j] += a[i][k]*b[i][k];
            printf("%10d",c[i][j]);
            }
       printf("\n");
       }

}

int main()
{
    int A[20][20],B[20][20],C[20][20];
    int ma,mb,na,nb;
    int *m,*n;
    int (*a)[20] = A ,(*b)[20] = B ,(*c)[20] = C ;
    m = &ma;
    n = &na;
    initializeMatrix(A,m,n);
    m = &mb;
    n = &nb;
    initializeMatrix(B,m,n);
    if(na != mb)
        printf("Incompatible Demensions\n");
    else 
        matrixMult(a,ma,mb,b,mb,nb,c);
    return 0;



}
