#include<stdio.h>
#include<stdlib.h>



int qusort(double *s,int start ,int end)
{

    int  i,j;    
    i=start;    
    j = end;    
    s[0]=s[start];    
    while(i<j)
    {
        while(i<j&&s[0]<s[j])
        j--;    
        if(i<j)
        {
            s[i]=s[j];    
            i++;    
        }
        while(i<j&&s[i]<=s[0])
            i++;    
        if(i<j)
        {
            s[j]=s[i];    
            j--;    
        }
    }
    s[i]=s[0];    
    if (start<i)
        qusort(s,start,j-1);    
    if (i<end)
        qusort(s,j+1,end);
    return 0;
}


int main()
{
    int n ,i ;
    double*s ;
    scanf("%d",n);
    s=(double*)malloc(sizeof(double)*n);
    for(i=1;i<=n;i++)
        scanf("%d",&s[i]);  
    qsort(s,1,n);
    for(i=1;i<=n;i++)
        printf("%5lf\n",s[i]);
    return 0;




}