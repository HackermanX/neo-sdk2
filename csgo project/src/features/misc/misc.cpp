#include "misc.h"
#include <algorithm>
#include <random>
#include <iterator>
#include <iostream>
#include <vector>

const wchar_t* misc::rand_hint() {
	const wchar_t* shit[] = {
		L"neo-sdk2", L"What’s wrong with being gay",
		L"You are now manually breathing", L"If you step on a fish, you are now a watermelon",
		L"The government tried to ban me from the dark web", L"I don’t like dick -sweats-",
		L"So what if I like dick", L"Sometimes your enemy has better cheats",
		L"killing two players on the first round unlocks free reports!", L"Henrijs gejs",
		L"Cry about it", L"Guacamole",
		L"Please die nigger - Az1o", L"I sucked dick once and im not gay becuse he wear'd skirt",
		L"Free driver source - Null", L"Fuck you faggot - Rake",
		L"CIA niggers glow in the dark. You can see them if you're driving. You just run them over. That's what you do. - Terry A Davis",
		L"I report to God. You report to me. - Terry A Davis", L"Start Running"
	};

	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(&shit[0], &shit[18], g);
	//it repeated itself too often so i randomized everything i thought of
	srand(time(NULL));
	return shit[rand() % 19];
}
