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
	print("la variable b vaut ", b, "et doit valoir 0");
}