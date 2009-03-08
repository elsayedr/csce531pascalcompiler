char *p;

int main()
{
    p = (char *)malloc(17);
    free(p);
    printf("long = %d bytes\n",sizeof(long));
    printf("int = %d bytes\n",sizeof(int));
}
