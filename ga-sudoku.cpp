#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <valarray> //USE TO MAKE SLICES FOR CROSSOVER AND COMBINATION

//SORT CHILDREN BASED ON FITNESS
//SPLIT and COMBINE, apply mutation

using namespace std;

/************************************************
    STRUCTS
************************************************/
struct line
{
   int vals[9];
};

struct puzzle
{
    line row[9];
    line col[9];
    line block[9];
    int fitness;
};

/************************************************
    GLOBAL
************************************************/
puzzle STARTPUZZLE;
deque<puzzle> POPULATION;
deque<puzzle> CHILDREN;

int BESTFITNESS = 999;

const int POPULATIONCOUNT = 80;

/************************************************
    PROTOTYPES
************************************************/
bool openPuzzle(int argc, char* argv[], ifstream &fin);
void readPuzzle(ifstream &fin);
void printPuzzle( puzzle puzzleToPrint );
void generatePopulation();
void generateColsBlocks(puzzle &puzWithRows);
void generateChildren();
void applyFitness();
void fitnessFunction(puzzle puzToCheck);
int errorsInArray(int arrayIn[]);
bool sortPuzzles(puzzle puzzle1, puzzle puzzle2);

int main(int argc, char* argv[])
{
    srand(time(NULL));

    ifstream fin;
    
    if( !openPuzzle( argc, argv, fin ) )
        return 0;

    readPuzzle( fin );

    printPuzzle( STARTPUZZLE );

    generatePopulation();

    while(BESTFITNESS > 0)
    {
        generateChildren();
        //cout << "back from generate children" << endl;
    }
}

bool openPuzzle(int argc, char* argv[], ifstream &fin)
{
    if( argc != 2 )
    {
        cout << "INCORRECT USAGE. CORRECT USAGE: ga-sudoku <puzzlename>" << endl;
        return false;
    }

    fin.open(argv[1]);
    
    cout << argv[1] << endl;

    if( !fin )
    {
        cout << "FILE NOT OPENED." << endl;
        return false;
    }

    return true;
}

void readPuzzle(ifstream &fin)
{
    for( int i = 0; i < 9; i++ )
    {
        for( int j = 0; j < 9; j++ )
        {
            fin >> STARTPUZZLE.row[i].vals[j];
        }
    }

    fin.close();
}

void printPuzzle( puzzle puzzleToPrint )
{
    for( int i = 0; i < 9; i++ )
    {
        for( int j = 0; j < 9; j++ )
        {
            cout << puzzleToPrint.row[i].vals[j] << " ";
        }
        cout << endl;
    }
}

void generatePopulation()
{
    for( int i = 0; i < POPULATIONCOUNT; i++ )
    {
        puzzle newPuz;
        newPuz.fitness = -1;

        //generate rows
        for( int j = 0; j < 9; j++ )
        {
            for( int k = 0; k < 9; k++ )
            {
                //make sure it's an originally "unknown" value"
                if( STARTPUZZLE.row[j].vals[k] )
                    newPuz.row[j].vals[k] = STARTPUZZLE.row[j].vals[k];
                else
                    newPuz.row[j].vals[k] = rand() % 9 + 1;
            }
        }

        //generate columns and blocks based on rows
        generateColsBlocks(newPuz);

        POPULATION.push_back(newPuz);
    }
}

void generateColsBlocks(puzzle &puzWithRows)
{
    for( int i = 0; i < 9; i++ )
    {
        for( int j = 0; j < 9; j++ )
        {
            puzWithRows.col[i].vals[j] = puzWithRows.row[j].vals[i];
        }
    }

    puzWithRows.block[0].vals[0] = puzWithRows.row[0].vals[0];
    puzWithRows.block[0].vals[1] = puzWithRows.row[0].vals[1];
    puzWithRows.block[0].vals[2] = puzWithRows.row[0].vals[2];
    puzWithRows.block[0].vals[3] = puzWithRows.row[1].vals[0];
    puzWithRows.block[0].vals[4] = puzWithRows.row[1].vals[1];
    puzWithRows.block[0].vals[5] = puzWithRows.row[1].vals[2];
    puzWithRows.block[0].vals[6] = puzWithRows.row[2].vals[0];
    puzWithRows.block[0].vals[7] = puzWithRows.row[2].vals[1];
    puzWithRows.block[0].vals[8] = puzWithRows.row[2].vals[2];
    
    puzWithRows.block[1].vals[0] = puzWithRows.row[0].vals[3];
    puzWithRows.block[1].vals[1] = puzWithRows.row[0].vals[4];
    puzWithRows.block[1].vals[2] = puzWithRows.row[0].vals[5];
    puzWithRows.block[1].vals[3] = puzWithRows.row[1].vals[3];
    puzWithRows.block[1].vals[4] = puzWithRows.row[1].vals[4];
    puzWithRows.block[1].vals[5] = puzWithRows.row[1].vals[5];
    puzWithRows.block[1].vals[6] = puzWithRows.row[2].vals[3];
    puzWithRows.block[1].vals[7] = puzWithRows.row[2].vals[4];
    puzWithRows.block[1].vals[8] = puzWithRows.row[2].vals[5];

    puzWithRows.block[2].vals[0] = puzWithRows.row[0].vals[6];
    puzWithRows.block[2].vals[1] = puzWithRows.row[0].vals[7];
    puzWithRows.block[2].vals[2] = puzWithRows.row[0].vals[8];
    puzWithRows.block[2].vals[3] = puzWithRows.row[1].vals[6];
    puzWithRows.block[2].vals[4] = puzWithRows.row[1].vals[7];
    puzWithRows.block[2].vals[5] = puzWithRows.row[1].vals[8];
    puzWithRows.block[2].vals[6] = puzWithRows.row[2].vals[6];
    puzWithRows.block[2].vals[7] = puzWithRows.row[2].vals[7];
    puzWithRows.block[2].vals[8] = puzWithRows.row[2].vals[8];


    puzWithRows.block[3].vals[0] = puzWithRows.row[3].vals[0];
    puzWithRows.block[3].vals[1] = puzWithRows.row[3].vals[1];
    puzWithRows.block[3].vals[2] = puzWithRows.row[3].vals[2];
    puzWithRows.block[3].vals[3] = puzWithRows.row[4].vals[0];
    puzWithRows.block[3].vals[4] = puzWithRows.row[4].vals[1];
    puzWithRows.block[3].vals[5] = puzWithRows.row[4].vals[2];
    puzWithRows.block[3].vals[6] = puzWithRows.row[5].vals[0];
    puzWithRows.block[3].vals[7] = puzWithRows.row[5].vals[1];
    puzWithRows.block[3].vals[8] = puzWithRows.row[5].vals[2];

    puzWithRows.block[4].vals[0] = puzWithRows.row[3].vals[3];
    puzWithRows.block[4].vals[1] = puzWithRows.row[3].vals[4];
    puzWithRows.block[4].vals[2] = puzWithRows.row[3].vals[5];
    puzWithRows.block[4].vals[3] = puzWithRows.row[4].vals[3];
    puzWithRows.block[4].vals[4] = puzWithRows.row[4].vals[4];
    puzWithRows.block[4].vals[5] = puzWithRows.row[4].vals[5];
    puzWithRows.block[4].vals[6] = puzWithRows.row[5].vals[3];
    puzWithRows.block[4].vals[7] = puzWithRows.row[5].vals[4];
    puzWithRows.block[4].vals[8] = puzWithRows.row[5].vals[5];

    puzWithRows.block[5].vals[0] = puzWithRows.row[3].vals[6];
    puzWithRows.block[5].vals[1] = puzWithRows.row[3].vals[7];
    puzWithRows.block[5].vals[2] = puzWithRows.row[3].vals[8];
    puzWithRows.block[5].vals[3] = puzWithRows.row[4].vals[6];
    puzWithRows.block[5].vals[4] = puzWithRows.row[4].vals[7];
    puzWithRows.block[5].vals[5] = puzWithRows.row[4].vals[8];
    puzWithRows.block[5].vals[6] = puzWithRows.row[5].vals[6];
    puzWithRows.block[5].vals[7] = puzWithRows.row[5].vals[7];
    puzWithRows.block[5].vals[8] = puzWithRows.row[5].vals[8];



    puzWithRows.block[6].vals[0] = puzWithRows.row[6].vals[0];
    puzWithRows.block[6].vals[1] = puzWithRows.row[6].vals[1];
    puzWithRows.block[6].vals[2] = puzWithRows.row[6].vals[2];
    puzWithRows.block[6].vals[3] = puzWithRows.row[7].vals[0];
    puzWithRows.block[6].vals[4] = puzWithRows.row[7].vals[1];
    puzWithRows.block[6].vals[5] = puzWithRows.row[7].vals[2];
    puzWithRows.block[6].vals[6] = puzWithRows.row[8].vals[0];
    puzWithRows.block[6].vals[7] = puzWithRows.row[8].vals[1];
    puzWithRows.block[6].vals[8] = puzWithRows.row[8].vals[2];

    puzWithRows.block[7].vals[0] = puzWithRows.row[6].vals[3];
    puzWithRows.block[7].vals[1] = puzWithRows.row[6].vals[4];
    puzWithRows.block[7].vals[2] = puzWithRows.row[6].vals[5];
    puzWithRows.block[7].vals[3] = puzWithRows.row[7].vals[3];
    puzWithRows.block[7].vals[4] = puzWithRows.row[7].vals[4];
    puzWithRows.block[7].vals[5] = puzWithRows.row[7].vals[5];
    puzWithRows.block[7].vals[6] = puzWithRows.row[8].vals[3];
    puzWithRows.block[7].vals[7] = puzWithRows.row[8].vals[4];
    puzWithRows.block[7].vals[8] = puzWithRows.row[8].vals[5];

    puzWithRows.block[8].vals[0] = puzWithRows.row[6].vals[6];
    puzWithRows.block[8].vals[1] = puzWithRows.row[6].vals[7];
    puzWithRows.block[8].vals[2] = puzWithRows.row[6].vals[8];
    puzWithRows.block[8].vals[3] = puzWithRows.row[7].vals[6];
    puzWithRows.block[8].vals[4] = puzWithRows.row[7].vals[7];
    puzWithRows.block[8].vals[5] = puzWithRows.row[7].vals[8];
    puzWithRows.block[8].vals[6] = puzWithRows.row[8].vals[6];
    puzWithRows.block[8].vals[7] = puzWithRows.row[8].vals[7];
    puzWithRows.block[8].vals[8] = puzWithRows.row[8].vals[8];
}

void applyFitness()
{

   //cout << "start of applyfitness" << endl;
   for( int i = 0; i < POPULATIONCOUNT; i++ )
   {
       puzzle puzToCheck = POPULATION.front();
       POPULATION.pop_front();

       //this line is redundant
       fitnessFunction(puzToCheck);

       POPULATION.push_back(puzToCheck);
   }

   //cout << "End of ApplyFitness" << endl;
}

void fitnessFunction(puzzle puzToCheck)
{
    int totalFitness = 0;
    
    for( int i = 0; i < 9; i++ )
    {
        totalFitness += errorsInArray(puzToCheck.row[i].vals);
        totalFitness += errorsInArray(puzToCheck.col[i].vals);
        totalFitness += errorsInArray(puzToCheck.block[i].vals);
    }

    puzToCheck.fitness = totalFitness;

    /**************CHECK FOR SOLUTION HERE**************/
    if( totalFitness == 0 )
    {
        cout << "SOLUTION FOUND." << endl;
        printPuzzle(puzToCheck);
        exit(0);
    }
    else if( totalFitness < BESTFITNESS )
    {
        cout << "New best: " << totalFitness << endl;
        BESTFITNESS = totalFitness;
    }
}

int errorsInArray(int arrayIn[])
{
    int nums[9];
    int errors = 0;

    for( int i = 0; i< 9; i++ )
        nums[i] = arrayIn[i];

    sort(nums, nums + 9);

    for( int i = 1; i < 10; i++ )
    {
        if( nums[i] != i )
            errors++;
    }

    return errors;
}

void generateChildren()
{
    //cout << "start of generatechildren" << endl;
    for( int i = 0; i < POPULATIONCOUNT/2; i++ )
    {
        puzzle parent1, parent2, child1, child2;

        parent1 = POPULATION.front();
        POPULATION.pop_front();
        parent2 = POPULATION.front();
        POPULATION.pop_front();

        for( int j = 0; j < 9; j++ )
        {
            const int splitPoint = rand() % 8 + 1;

            for( int k = 0; k < splitPoint; k++ )
            {
                child1.row[i].vals[k] = parent1.row[i].vals[k];
                child2.row[i].vals[k] = parent2.row[i].vals[k];
            }
            for( int k = splitPoint; k < 9; k++ )
            {
                child1.row[i].vals[k] = parent2.row[i].vals[k];
                child2.row[i].vals[k] = parent1.row[i].vals[k];
            }
        }

        //PERFORM MUTATION, FORCE STARTING VALS
        for( int j = 0; j < 9; j++ )
        {
            int mutationLocation = rand() % 9 + 1;
            child1.row[j].vals[mutationLocation] = rand() % 9 + 1;
            child2.row[j].vals[mutationLocation] = rand() % 9 + 1;
        }

        for( int j = 0; j < 9; j++ )
            for( int k = 0; k < 9; k++ )
            {
                if( STARTPUZZLE.row[j].vals[k] )
                {
                    child1.row[j].vals[k] = STARTPUZZLE.row[j].vals[k];
                    child2.row[j].vals[k] = STARTPUZZLE.row[j].vals[k];
                }
            }

        generateColsBlocks(child1);
        generateColsBlocks(child2);

        CHILDREN.push_back(child1);
        CHILDREN.push_back(child2);

    }

    POPULATION = deque<puzzle>();
    POPULATION = CHILDREN;
    applyFitness();
    sort(POPULATION.begin(), POPULATION.end(), sortPuzzles);
    CHILDREN = deque<puzzle>();

    //cout << "End of generatechildren" << endl;

    return;
}

bool sortPuzzles(puzzle puzzle1, puzzle puzzle2)
{
    return puzzle1.fitness < puzzle2.fitness;
}
