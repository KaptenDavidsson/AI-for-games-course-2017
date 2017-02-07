//2.2-------------------------------------------------------------
bool Logic::King_IsInCheck(char name, bool longReach, std::vector<int> move, int startingPosX, int startingPosY, int ply) {
	int x1 = startingPosX + startingPosX; int y1 = startingPosY + startingPosY;

	if (x1 < 0 || x1 > 8 || y1 < 0 || y1 > 8) return false;
	else if (mGamePos[ply].sq[x1][y1] == name) return true;
	else if (mGamePos[ply].sq[x1][y1] != '*') return false;
	else if (longReach) return King_IsInCheck(name, longReach, move, x1, y1 , ply);
	else return false;
}


bool Logic::King_IsInCheck(int ply){
    
    //--------------------------------------
    if (GAME == STRATEGO) return false;
    //--------------------------------------

	int kingPos[2];
	For(x, FILES) {
		For(y, RANKS) {
			if (mGamePos[ply].sq[x][y] == (mIsWhite ? 'K' : 'k')) {
				kingPos[0] = x;
				kingPos[1] = y;
			}
		}
	}
	
	std::vector<std::tuple<char, bool, std::vector<std::vector<int>>>> pieceMoves = {
		{ (mIsWhite ? 'p' : 'P'), false, { { 1, (mIsWhite ? -1 : 1) }, { -1, (mIsWhite ? -1 : 1) } } },
		{ (mIsWhite ? 'r' : 'R'), true,{ {1,0},{ 0,1 },{ -1,0 },{ 0,-1 } } },
		{ (mIsWhite ? 'n' : 'N'), false,{ { 1,1 },{ -1,1 },{ -1,-1 },{ 1,-1 } } },
		{ (mIsWhite ? 'b' : 'B'), true,{ { 1,2 },{ 2,1 },{ 1,-2 },{ 2,-1 },{ -1,2 },{ -2,1 },{ -1,-2 },{ -2,-1 } } },
		{ (mIsWhite ? 'q' : 'Q'), true,{ { 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 } } },
		{ (mIsWhite ? 'k' : 'K'), false,{ { 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 } } }
	};

	For (i, pieceMoves.size()) {
		For (j, std::get<2>(pieceMoves[i]).size()) {
			if (King_IsInCheck(std::get<0>(pieceMoves[i]), std::get<1>(pieceMoves[i]), std::get<2>(pieceMoves[i])[j], kingPos[0], kingPos[1], ply)) {
				printf("IsChecked");
				return true;
			}
		}
	}
	//printf("IsNotChecked");
    return false;
}



//2.3-------------------------------------------------------------------------
void Logic::OptChildOrder(int ply){
	int highest[2] = { -1, -int(mMaxValue) };
	int secondHighest[2] = { -1, -int(mMaxValue) };

	For(i, mMove[ply].children) {
		UpdateGamePos(i, ply + 1);
		int score = GoalFunc(false, RAND_OFF, ply + 1);
		if (score > highest[1]) {
			highest[0] = i;
			highest[1] = score;
		}
		else if (score > secondHighest[1]) {
			secondHighest[0] = i;
			secondHighest[1] = score;
		}
	}

	char highestChild[4];
	std::copy(std::begin(mMove[ply].child[highest[0]]), std::end(mMove[ply].child[highest[0]]), std::begin(highestChild));
	std::copy(std::begin(mMove[ply].child[0]), std::end(mMove[ply].child[0]), std::begin(mMove[ply].child[highest[0]]));
	std::copy(std::begin(highestChild), std::end(highestChild), std::begin(mMove[ply].child[0]));

	char secondHighestChild[4];
	std::copy(std::begin(mMove[ply].child[secondHighest[0]]), std::end(mMove[ply].child[secondHighest[0]]), std::begin(secondHighestChild));
	std::copy(std::begin(mMove[ply].child[1]), std::end(mMove[ply].child[1]), std::begin(mMove[ply].child[secondHighest[0]]));
	std::copy(std::begin(secondHighestChild), std::end(secondHighestChild), std::begin(mMove[ply].child[1]));
}