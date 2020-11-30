int main(void)
{
	float realnum = 2.43E-9;
	int fakenum = 42;
	int humble = 27;

	int expr = 15 + 9 * 7 - (4 + 2) / 4;

	if (humble < 9000) {
		char hello = 'h';
	}

	if (fakenum == 42) {
		char rain = 'r';
		int cloud = 98;

		if (rain != 'o') {
			rain = 'o';
		}
		else {
			rain = 'f';
		}
	}
	else {
		while (realnum > 1) {
			int fred = 76;
			if (fred > 75) {
				fred = fred + humble;
			}
			else {
				fred = fred - humble;
			}
		}
	}

	return 0;
}