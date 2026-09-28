#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DICE_COUNT 5
#define TOTAL_ROUNDS 13

// Function prototypes
void initializeScores(int scores[]);
void rollDice(int dice[], int size);
void displayDice(int dice[], int size);
void rerollDice(int dice[], int size, int rerollFlags[]);
void displayCategories(int usedCategories[]);
int selectCategory(int usedCategories[], const char *playerName);
int calculateScore(int dice[], int category);
void updateScores(int scores[], int category, int score);
void humanTurn(int dice[], int size, int usedCategories[], int *totalScore, int scores[], const char *playerName);
void computerTurn(int dice[], int size, int usedCategories[], int *totalScore, int scores[], const char *aiName);
void playRound(int round, const char *humanName, const char *aiName, int *humanScore, int *computerScore, int humanUsedCategories[], int computerUsedCategories[], int humanScores[], int computerScores[]);
void printRoundSummary(const char *playerName, int score, int totalScore);
void printFinalResults(const char *humanName, const char *aiName, int humanTotalScore, int computerTotalScore, int humanScores[], int computerScores[]);

int main() {
    srand(time(0));

    char humanName[50];
    const char *aiName = "Computer";  // Fixed AI name as a computer

    // Get human player name
    printf("Enter your name: ");
    fgets(humanName, sizeof(humanName), stdin);
    humanName[strcspn(humanName, "\n")] = '\0'; // Remove newline character

    //tracking the scores and categories 
    int humanScore = 0, computerScore = 0;
    int humanCategories[13] = {0}, computerCategories[13] = {0};
    int humanScores[13] = {0}, computerScores[13] = {0};

    printf("\nWelcome to Yahtzee, %s vs %s!\n", humanName, aiName);

    for (int round = 1; round <= TOTAL_ROUNDS; ++round) {
        printf("\n--- Round %d ---\n", round);
        playRound(round, humanName, aiName, &humanScore, &computerScore, humanCategories, computerCategories, humanScores, computerScores);
    }

    printFinalResults(humanName, aiName, humanScore, computerScore, humanScores, computerScores);

    return 0;
}

// Start at zero points
void initializeScores(int scores[]) {
    for (int i = 0; i < 13; i++) {
        scores[i] = 0;
    }
}

// Roll dice values
void rollDice(int dice[], int size) {
    for (int i = 0; i < size; i++) {
        dice[i] = (rand() % 6) + 1;
    }
}

// Display dice values
void displayDice(int dice[], int size) {
    printf("Dice: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", dice[i]);
    }
    printf("\n");
}

// Re-roll dice based on user choice
void rerollDice(int dice[], int size, int rerollFlags[]) {
    for (int i = 0; i < size; i++) {
        if (rerollFlags[i] == 1) {
            dice[i] = (rand() % 6) + 1;
        }
    }
}

// Show available categories for player to choose
void displayCategories(int usedCategories[]) {
    printf("Available Categories:\n");

    if (usedCategories[0] == 0) printf("0 - Ones             : Score the sum of all 1s\n");
    if (usedCategories[1] == 0) printf("1 - Twos             : Score the sum of all 2s\n");
    if (usedCategories[2] == 0) printf("2 - Threes           : Score the sum of all 3s\n");
    if (usedCategories[3] == 0) printf("3 - Fours            : Score the sum of all 4s\n");
    if (usedCategories[4] == 0) printf("4 - Fives            : Score the sum of all 5s\n");
    if (usedCategories[5] == 0) printf("5 - Sixes            : Score the sum of all 6s\n");
    if (usedCategories[6] == 0) printf("6 - Three of a Kind  : Sum of all dice if at least three match\n");
    if (usedCategories[7] == 0) printf("7 - Four of a Kind   : Sum of all dice if at least four match\n");
    if (usedCategories[8] == 0) printf("8 - Full House       : 25 points for a pair and three of a kind\n");
    if (usedCategories[9] == 0) printf("9 - Small Straight   : 30 points for four consecutive numbers\n");
    if (usedCategories[10] == 0) printf("10 - Large Straight  : 40 points for five consecutive numbers\n");
    if (usedCategories[11] == 0) printf("11 - Yahtzee         : 50 points for five of a kind\n");
    if (usedCategories[12] == 0) printf("12 - Chance          : Score the sum of all dice\n");
}

// Select a category for scoring
int selectCategory(int usedCategories[], const char *playerName) {
    int category;
    displayCategories(usedCategories);
    printf("%s, choose a category (0-12): ", playerName);
    scanf("%d", &category);

    while (category < 0 || category > 12 || usedCategories[category] == 1) {
        printf("Invalid or used category. Choose a different one: ");
        displayCategories(usedCategories);
        scanf("%d", &category);
    }

    usedCategories[category] = 1;
    return category;
}

// Calculate score based on chosen category using switch cases
int calculateScore(int dice[], int category) {
    int score = 0;
    int frequency[7] = {0};
    for (int i = 0; i < DICE_COUNT; i++) {
        frequency[dice[i]]++;
    }

    switch (category) {
        case 0: case 1: case 2: case 3: case 4: case 5:  // Ones to Sixes
            score = frequency[category + 1] * (category + 1);
            break;
        case 6:  // Three of a Kind
            for (int i = 1; i <= 6; i++) {
                if (frequency[i] >= 3) {
                    score = dice[0] + dice[1] + dice[2] + dice[3] + dice[4];
                    break;
                }
            }
            break;
        case 7:  // Four of a Kind
            for (int i = 1; i <= 6; i++) {
                if (frequency[i] >= 4) {
                    score = dice[0] + dice[1] + dice[2] + dice[3] + dice[4];
                    break;
                }
            }
            break;
        case 8:  // Full House
            {
                int ThreeNum = 0, TwoNum = 0;
                for (int i = 1; i <= 6; i++) {
                    if (frequency[i] == 3) ThreeNum = 1;
                    if (frequency[i] == 2) TwoNum = 1;
                }
                if (ThreeNum && TwoNum) score = 25;
            }
            break;
        case 9:  // Small Straight
            if ((frequency[1] && frequency[2] && frequency[3] && frequency[4]) ||
                (frequency[2] && frequency[3] && frequency[4] && frequency[5]) ||
                (frequency[3] && frequency[4] && frequency[5] && frequency[6])) {
                score = 30;
            }
            break;
        case 10:  // Large Straight
            if ((frequency[1] && frequency[2] && frequency[3] && frequency[4] && frequency[5]) ||
                (frequency[2] && frequency[3] && frequency[4] && frequency[5] && frequency[6])) {
                score = 40;
            }
            break;
        case 11:  // Yahtzee
            for (int i = 1; i <= 6; i++) {
                if (frequency[i] == 5) score = 50;
            }
            break;
        case 12:  // Chance
            score = dice[0] + dice[1] + dice[2] + dice[3] + dice[4];
            break;
        default:
            score = 0;
    }
    return score;
}

// Update score for selected category
void updateScores(int scores[], int category, int score) {
    scores[category] = score;
}

// Function for human turn
void humanTurn(int dice[], int size, int usedCategories[], int *totalScore, int scores[], const char *playerName) {
    int rerollFlags[5] = {0};
    rollDice(dice, size);
    printf("%s's initial roll:\n", playerName);
    displayDice(dice, size);

    // Only up to two rerolls can be made
    int rerollCount = 0;
    while (rerollCount < 2) {
        printf("Do you want to re-roll any dice? (y/n): ");
        char choice;
        if (scanf(" %c", &choice) != 1) {
            // Clear invalid input
            while (getchar() != '\n');
            printf("Invalid input. Please enter 'y' or 'n'.\n");
            continue;
        }

        if (choice == 'y' || choice == 'Y') {
            printf("Enter 1 to re-roll and 0 to keep for each die (e.g., 1 0 1 1 0):\n");

            
            int validInput = 1;
            for (int j = 0; j < size; j++) {
                if (scanf("%d", &rerollFlags[j]) != 1 || (rerollFlags[j] != 0 && rerollFlags[j] != 1)) {
                    
                    while (getchar() != '\n');
                    printf("Invalid input. Please enter only 0 or 1.\n");
                    validInput = 0;
                    break;
                }
            }

            if (validInput) {
                rerollDice(dice, size, rerollFlags);
                printf("Your new roll:\n");
                displayDice(dice, size);
                rerollCount++; 
                
            } else {
                // Retry the reroll
                continue;
            }
        } else if (choice == 'n' || choice == 'N') {
            break; 
        } else {
            printf("Invalid input. Please enter 'y' or 'n'.\n");
        }
    }

    int category = selectCategory(usedCategories, playerName);
    int score = calculateScore(dice, category);
    *totalScore += score;
    updateScores(scores, category, score);
    printRoundSummary(playerName, score, *totalScore);
}



// Function for computer turn
void computerTurn(int dice[], int size, int usedCategories[], int *totalScore, int scores[], const char *aiName) {
    rollDice(dice, size);
    printf("%s's initial roll:\n", aiName);
    displayDice(dice, size);

    int frequency[7] = {0};
    for (int i = 0; i < size; i++) frequency[dice[i]]++;

    // AI strategy: prioritize categories based on current dice roll
    int bestCategory = -1;
    int bestScore = 0;

    // First, check for higher-scoring categories that are not used
    for (int category = 0; category < 13; category++) {
        if (usedCategories[category] == 0) {
            int score = calculateScore(dice, category);

            // Improve decision making by considering specific category strategies:
            if (category == 11) {  
                if (score == 50) { 
                    bestCategory = category;
                    bestScore = score;
                    break;
                }
            } else if (category == 8 && score == 25) {  
                bestCategory = category;
                bestScore = score;
                break;
            } else if (category == 9 && score == 30) {  
                bestCategory = category;
                bestScore = score;
                break;
            } else if (category == 10 && score == 40) {  
                bestCategory = category;
                bestScore = score;
                break;
            } else if (score > bestScore) {  // Maximize score
                bestCategory = category;
                bestScore = score;
            }
        }
    }

    // If neither category offers an advantage, return to Chance
    if (bestCategory == -1) {
        for (int category = 12; category < 13; category++) {
            if (usedCategories[category] == 0) {
                bestCategory = category;
                bestScore = calculateScore(dice, category);
            }
        }
    }

    // Apply the best selected category
    if (bestCategory != -1) {
        usedCategories[bestCategory] = 1;
        *totalScore += bestScore;
        updateScores(scores, bestCategory, bestScore);
        printRoundSummary(aiName, bestScore, *totalScore);
    }
}

// Print the summary of round
void printRoundSummary(const char *playerName, int score, int totalScore) {
    printf("%s scored %d points this round! Total Score: %d\n", playerName, score, totalScore);
}

// Play one round of the game
void playRound(int round, const char *humanName, const char *aiName, int *humanScore, int *computerScore, int humanUsedCategories[], int computerUsedCategories[], int humanScores[], int computerScores[]) {
    int dice[5];
    printf("\n%s's Turn:\n", humanName);
    humanTurn(dice, 5, humanUsedCategories, humanScore, humanScores, humanName);
    printf("\n%s's Turn:\n", aiName);
    computerTurn(dice, 5, computerUsedCategories, computerScore, computerScores, aiName);
}

// Print final results
void printFinalResults(const char *humanName, const char *aiName, int humanTotalScore, int computerTotalScore, int humanScores[], int computerScores[]) {
    printf("\nFinal Scores:\n");
    printf("%s's Total Score: %d\n", humanName, humanTotalScore);
    printf("%s's Total Score: %d\n", aiName, computerTotalScore);

    printf("\nBreakdown of Scores:\n");
    for (int i = 0; i < 13; i++) {
        printf("Category %d: %s scored %d, %s scored %d\n", i, humanName, humanScores[i], aiName, computerScores[i]);
    }

    printf("\n");
    if (humanTotalScore > computerTotalScore) {
    printf("***************************************\n");
    printf("*                                     *\n");
    printf("*    Congratulations %s, you win!     *\n", humanName);
    printf("*                                     *\n");
    printf("***************************************\n");
} else if (computerTotalScore > humanTotalScore) {
    printf(" The winner is %s!\n", aiName);
} else {
    printf("It's a tie!\n");
}
    printf(" Thank you for playing!\n");
}  