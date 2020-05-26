bool a = true;

void main()
{
	bool b = false;
	bool c = (b != a);
	bool d = (b != false);
	print("c  = ", c, " d = ", d);
	// c = 1 ; d = 0 
}