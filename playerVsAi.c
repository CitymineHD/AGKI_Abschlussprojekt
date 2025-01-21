
//playerColor -> true = Weiß || false = Schwarz
int playerVsBadAi(bool playerColor){
    //starting color is the color of the player
    //the color switches each move and determen if the player or the Ai is moving
    bool moves[4096];
    char board[8][8];
    initializeBoard(board);
    
    //information for the determineLegalMoves()
    int badAi = playerColor ? 0 : 1;
    int player = playerColor ? 1 : 0;
    bool hittingMove = false; // draw after 50 moves without hitting 
    int drawTime = 0;

    int anz = -1; //anz possible moves
    int move = -1;
    int winner = 0; // 0 = draw || 1 = player won || -1 = AI won
    bool passt = false;
    
    while (anz != 0 && drawTime <= 50){
        printBoardToTerminal(board);
        if (playerColor){ //player
            anz = determineLegalMoves(moves, player, board, &hittingMove);
            hittingMove ? drawTime = 0 : drawTime++;
            if (anz != 0){
                while (!passt){
                    move = moveFromPlayer();
                    if (move >= 0 && move <= 4096 && checkMove(moves, move)){
                        passt = true;
                    } else {
                        printf("Nicht gültiger Zug!\n");
                        printAllPossibleMoves(moves); //prints all Legal Moves as humanReadable Input
                    }
                }

                passt = false;
            } else {
                winner = 1;
                break;
            }
            
            updateBoard(board,move);
        } else { //BadAi
            //TODO
            //THE GOOD AI need to be implemented
            int possibleMovesCount = determineLegalMoves(moves, badAi, board, &hittingMove);
            hittingMove ? drawTime = 0 : drawTime++;
            printAllPossibleMoves(moves);
            if (possibleMovesCount == 0){
                winner = -1;
                break;
            }

            for (int i = 0; i < 4096; i++){
                if (moves[i]){
                    updateBoard(board,i);
                    break;
                }
            }                
        }

        playerColor = !playerColor;
    }

    return winner;
}
