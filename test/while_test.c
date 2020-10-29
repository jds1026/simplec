int main(void)
{
	char alpha = 'a';
	char charlie = 'c';

	int i = 0;

	while (alpha <= charlie) {
		alpha = 'a' + i;
		i = i + 1;
	}

	return 0;
}