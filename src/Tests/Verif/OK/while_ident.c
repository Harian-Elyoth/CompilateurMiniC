bool b = true;

void main()
{
	int sum = 0;
	while(b)
	{
		sum = sum + 1;
		if (sum == 10)
		{
			b = false;
		}
	}
}