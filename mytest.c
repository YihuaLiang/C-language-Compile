int sum(int a, int b)
{
	return a+b;
}

int main()
{
	int a = 0;
	int b = 1, c = 2;
	while( a < 10 )
	{
		if( a < 5 ) 
		{
			a = a + b;
		}
		else
		{
			a = a + c;
		}
	}
	return 0;
}
