#include <iostream>
#include <cstdlib>
#include <queue>
#include <vector>
#include <stack>

//The URL to my code is: github.com/EdwardA10/CS170

using namespace std;

const int puzzle_size = 9;

/* These are the preset puzzles for the user to play with. 
The impossible puzzle was taken from the project briefing slides.
The remaining 5 were borrowed from the project PDF.
*/
int trivial[puzzle_size] = {1, 2, 3, 4, 5, 6, 7, 8, 0};
int very_easy[puzzle_size] = {1, 2, 3, 4, 5, 6, 7, 0, 8};
int easy[puzzle_size] = {1, 2, 0, 4, 5, 3, 7, 8, 6};
int doable[puzzle_size] = {0, 1, 2, 4, 5, 3, 7, 8, 6};
int oh_boy[puzzle_size] = {8, 7, 1, 6, 0, 2, 5, 4, 3};
int impossible[puzzle_size] = {8, 6, 7, 2, 5, 4, 3, 0, 1};

int initial_puzzle[puzzle_size];

class eightPuzzle {
    public:
        eightPuzzle();
        int puzzle[puzzle_size]; //an array that represents the puzzle at each node
        int g_cost; //depth of node
        int h_cost; //heuristic cost of node
        vector<eightPuzzle> children;
        vector<eightPuzzle> parents;
        
        int misplacedTile_Cost();
        int manhattan_Cost();
        
        void create_Child(int child[puzzle_size], int parent[puzzle_size]);
        void misplacedTile_Heuristic(string algorithm); 
        void manhattanDistance(string algorithm);
        void uniformCostSearch(string algorithm);
        void expand_node(string algorithm);
        void display_path(string algorithm);
        
        void move_up(eightPuzzle node, string algorithm);
        void move_down(eightPuzzle node, string algorithm);
        void move_left(eightPuzzle node, string algorithm);
        void move_right(eightPuzzle node, string algorithm);
        
        bool puzzleMatch();
        bool initialComparison();
		bool parentComparison();
};

/*
------------------------------------------------------------------------------------------
Structure to create our min-heap. C++ creates a max heap via priority queue by default.
However, this comparison structure will allow us to organize a min heap by comparing the
total cost (f(n)) of our nodes.
------------------------------------------------------------------------------------------
*/
struct minHeapComparison {
    bool operator()(const eightPuzzle left_node, const eightPuzzle right_node) {
        return(left_node.g_cost + left_node.h_cost) > (right_node.g_cost + right_node.h_cost);
    }
};

priority_queue<eightPuzzle, vector<eightPuzzle>, minHeapComparison> min_heap;

/*
------------------------------------------------------------------------------------------
Helper data structures
------------------------------------------------------------------------------------------
*/
stack<eightPuzzle> goal_path; //LIFO structure that holds the nodes on the path to our goal state.
queue<eightPuzzle> frontier; //Queue that holds nodes in the frontier
queue<eightPuzzle> nodes; //Queue that holds every node

int node_depth = 0; //depth of an arbitrary node (helps during traversal)
int frontier_size; //Maximum number of nodes in the frontier
int nodes_size; //A value to hold the size of our queue nodes for UCS.
int goalpathLength = 0; //Length of the goal path AKA goal state depth
int num_nodes; //Total number of nodes expanded
bool goal_state = false; //Boolean value that confirms if a node is the goal state.

void displayPuzzle(eightPuzzle node); //Helper function that displays a node.


/*
------------------------------------------------------------------------------------------
The default constructor of our puzzle. To clear any confusion, we initialize every
value to 0.
------------------------------------------------------------------------------------------
*/ //COMPLETED
eightPuzzle::eightPuzzle() {
    for(int i = 0; i < puzzle_size; i++) {
        puzzle[i] = 0;
    }
}


/*
---------------------------------------------------------------------------------------------
Function to create a child node that is equivalent to the parent node.
The array contained within the child node will be influenced by our void moveset functions.
---------------------------------------------------------------------------------------------
*/ //COMPLETED
void eightPuzzle::create_Child(int child[puzzle_size], int parent[puzzle_size]) {
    for(int i = 0; i < puzzle_size; i++) { child[i] = parent[i]; }
}


/*
---------------------------------------------------------------------------------------------
A boolean function that checks to see if our node matches with the goal puzzle.
---------------------------------------------------------------------------------------------
*/
bool eightPuzzle::puzzleMatch() {
    bool match = false;
    
    for(int i = 0; i < puzzle_size; i++) {
        if(i == 8) { 
            if(puzzle[i] == 0) {
                match = true;
            }
        }
        else if(puzzle[i] == i+1) {
            match = true;
        }
		else { match = false; }
    }
    return match;
    
    /* 
	RELEVANT COMMENT DOWN BELOW! This ruined my old code somehow...
	
    for(int i = 0; i < puzzle_size; i++) {
        cout << "Node puzzle: " << this->puzzle[i] << " vs. Goal puzzle: " << trivial[i] << endl; 
        
        /*
        NOTE: Weird error occurs in the if-statement below. The test output above CLEARLY indicates 
        differences between test puzzle and the trivial (goal) puzzle, as they should be. 
        However, down below the output indicates that they are equal, 
        but neither one of them is influenced, only compared...
        
        if(this->puzzle[i] == trivial[i]) {
            //cout << "Node puzzle: " << this->puzzle[i] << " vs. Goal puzzle: " << trivial[i] << endl; 
            match = true;
            if(match == false) { return match; }
        }
    }
    //cout << "Bool val: " << match << endl;
    return false; //return match; */
}

/*
------------------------------------------------------------------------------------------
Helper function that checks if our current state is equal to our initial state
------------------------------------------------------------------------------------------
*/ 
bool eightPuzzle::initialComparison() {
    bool match = false;
    for(int i = 0; i < puzzle_size; i++) {
        if(this->puzzle[i] == initial_puzzle[i]) { match = true; }
    }
    //cout << "TEST OUTPUT: " << match << endl;
    return match;
}

/*
------------------------------------------------------------------------------------------
Helper function that checks if our current state is equal to its parent node
Helpful to know when not to repeat states
------------------------------------------------------------------------------------------
*/
bool eightPuzzle::parentComparison() {
	bool match = false;
	for(int i = 0; i < puzzle_size; i++) {
		if(this->puzzle[i] == this->parents.front().puzzle[i]) { match = true; }
		else { match = false; }
	}
	return match;
}

/*
---------------------------------------------------------------------------------------------
This function will display the final goal path needed to reach our goal state.
---------------------------------------------------------------------------------------------
*/ //COMPLETED
void eightPuzzle::display_path(string algorithm) {
    
    if(this->initialComparison() == false) {  
        while(this->initialComparison() == false) { 
            *this = this->parents.front(); 
            goal_path.push(*this); 
            goalpathLength++;
        }
    }
    
    while(goal_path.empty() != true) {
        //These two statements allow us to 'iterate' through our path until we reach the goal node
        eightPuzzle goal = goal_path.top();
        goal_path.pop();
        
        goalpathLength++;
        displayPuzzle(goal);
        cout << endl << endl;
    }
    
    cout << "Nodes expanded: " << num_nodes << endl;
    if(algorithm == "MTH" || algorithm == "MDH") { cout << "Number of nodes in queue: " << frontier_size << endl;}
    else { cout << "Number of nodes in queue: " << nodes_size << endl; }
    
    cout << "Goal node depth: " << goalpathLength << endl;
}


/*
---------------------------------------------------------------------------------------------
This function returns the number of misplaced tiles.
In the goal state, the tile values are equivalent to 1 + the index
(E.g. index 4 (center tile) should contain a value of 5)
---------------------------------------------------------------------------------------------
*/ //COMPLETED
int eightPuzzle::misplacedTile_Cost() {
    int misplaced_tiles = 0;
    for(int i = 0; i < 8; i++) {
        if(this->puzzle[i] != i+1) {
            ++misplaced_tiles;
        }
    }
    
    return misplaced_tiles;
}


/*
---------------------------------------------------------------------------------------------
This is the main functionality of our misplaced tile heuristic.
Our initial node's g_cost (g(n) or depth) is set to 0,
while we use a helper function to calculate its h_cost (h(n) or heuristic cost).
---------------------------------------------------------------------------------------------
*/ //COMPLETED
void eightPuzzle::misplacedTile_Heuristic(string algorithm) {
    this->g_cost = 0; 
    this->h_cost = this->misplacedTile_Cost(); 
    min_heap.push(*this); 
    
    while(min_heap.size() >= 1 && goal_state == false) {
        eightPuzzle startNode = min_heap.top(); //Our starting node will be at the top of the min heap 
        frontier.push(startNode); //Push our current node to the frontier
        
        if(frontier.size() > frontier_size) { frontier_size = frontier.size(); }
        
        if(startNode.puzzleMatch() == true) {
            goal_state = true;
            goal_path.push(startNode);
            startNode.display_path(algorithm);
        }
        else {
            frontier.pop();
            //cout << "TEST OUTPUT: " << endl;
            startNode.expand_node(algorithm);
        }
    }
}


/*
---------------------------------------------------------------------------------------------
This function returns the manhattan cost of a node.
---------------------------------------------------------------------------------------------
*/
int eightPuzzle::manhattan_Cost() {
    int m_cost = 0;
    int difference = 0;
    
    /*
    -----------------------------------------------------------------------------
    In this for loop, we calculate the difference between each value's index
    and the index that contains the same value in the goal state.
    E.g.: goal state value 5 @ index 4, puzzle value 5 @ index 2 
    Difference = |2 - 4| = |-2| = 2.
    
    NOTE: Each proceeding value's index is +1 of the previous value.
    Therefore, we can extend the same if-else logic for each value so long as 
    we make sure to increase the index value for each number.
    -----------------------------------------------------------------------------
    */ //COMPLETED
    for(int i = 0; i < puzzle_size; i++) {
        if(this->puzzle[i] == 1) {
            difference = abs(i - 0);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 2) {
            difference = abs(i - 1);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 3) {
            difference = abs(i - 2);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 4) {
            difference = abs(i - 3);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 5) {
            difference = abs(i - 4);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 6) {
            difference = abs(i - 5);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 7) {
            difference = abs(i - 6);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
        else if(this->puzzle[i] == 8) {
            difference = abs(i - 7);
            if(difference == 8) { m_cost = m_cost + 4; }
            else if(difference == 7 || difference == 5) { m_cost = m_cost + 3; }
            else if(difference == 6 || difference == 4 || difference == 2) { m_cost = m_cost + 2; }
            else if(difference == 3 || difference == 1) { m_cost = m_cost + 1; }
        }
    }
    
    return m_cost;
}


/*
---------------------------------------------------------------------------------------------
This is the main functionality of our manhattan distance heuristic.
Our initial node's g_cost (g(n) or depth) is set to 0,
while we use a helper function to calculate its h_cost (h(n) or heuristic cost).
---------------------------------------------------------------------------------------------
*/ //COMPLETED
void eightPuzzle::manhattanDistance(string algorithm) {
    this->g_cost = 0;
    this->h_cost = this->manhattan_Cost();
    min_heap.push(*this);
    
    while(min_heap.size() >= 1 && goal_state == false) {
        eightPuzzle startNode = min_heap.top();
        frontier.push(startNode); //Push our current node to the frontier
        
        if(frontier.size() > frontier_size) { frontier_size = frontier.size(); }
        
        if(startNode.puzzleMatch() == true) {
            goal_state = true;
            goal_path.push(startNode);
            startNode.display_path(algorithm);
        }
        else {
            frontier.pop();
            startNode.expand_node(algorithm);
        }
    }
}


/*
---------------------------------------------------------------------------------------------
Our uniform cost search algorithm. Here we must hardcode our node's h(n) cost to 0.
---------------------------------------------------------------------------------------------
*/ //COMPLETED
void eightPuzzle::uniformCostSearch(string algorithm) {
    nodes.push(*this);
    if(nodes.size() > nodes_size) { nodes_size = nodes.size(); }
    
    while(nodes.size() >= 1 && goal_state == false) {
        eightPuzzle startNode = nodes.front();
        frontier.push(startNode);
        if(nodes.size() > nodes_size) { nodes_size = nodes.size(); }
        
        if(startNode.puzzleMatch() == true) {
            goal_state = true;
            startNode.h_cost = 0;
            goal_path.push(startNode);
            startNode.display_path(algorithm);
        }
        else {
            nodes.pop();
			startNode.h_cost = 0;
            startNode.expand_node(algorithm);
        }
    }
}


/*
This function expands our current node.
*/ //COMPLETED
void eightPuzzle::expand_node(string algorithm) {
    ++num_nodes;
    ++node_depth;
    
    //this->puzzleMatch();
    this->move_up(*this, algorithm);
    this->move_down(*this, algorithm);
    this->move_left(*this, algorithm);
    this->move_right(*this, algorithm);
}

//COMPLETED
void eightPuzzle::move_up(eightPuzzle node, string algorithm) {
    int tile_space; //Holds the index of our blank space.
    for(int i = 0; i < puzzle_size; i++) {
        if(node.puzzle[i] == 0) {
            tile_space = i;
        }
    }
    
    if(goal_state == false) {
        if(tile_space - 3 > -1) { //If blank tile y-coordinate is not in upper row of puzzle
            
            //Create a new node and assign our original node as its parent.
            eightPuzzle newNode;
            newNode.create_Child(newNode.puzzle, node.puzzle);
            newNode.parents.push_back(node);
            
            swap(newNode.puzzle[tile_space], newNode.puzzle[tile_space - 3]);
            
            if(newNode.puzzleMatch() == true) { //If we've found a matching puzzle
                goal_state = true;
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    //Note: In uniform cost search, h(n) is hardcoded to 0.
                }
                //cout << endl << "Outputting current state: " << endl;
                //displayPuzzle(newNode);
                
                //Since this is our goal state, we push it to the goal_path stack.
                goal_path.push(newNode);
                newNode.display_path(algorithm);
            }
            else { //If we DO NOT have a match
                newNode.g_cost = node_depth;
                
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    nodes.push(newNode);
                }
            }
        }
    }
}

//COMPLETED
void eightPuzzle::move_down(eightPuzzle node, string algorithm) {
    int tile_space; //Holds the index of our blank space.
    for(int i = 0; i < puzzle_size; i++) {
        if(node.puzzle[i] == 0) {
            tile_space = i;
        }
    }
    
    if(goal_state == false) {
        if(tile_space + 3 < puzzle_size) { //If blank tile's y-coordinate is not in bottom row of puzzle
            
            //Create a new node and assign our original node as its parent.
            eightPuzzle newNode;
            newNode.create_Child(newNode.puzzle, node.puzzle);
            newNode.parents.push_back(node);
            
            swap(newNode.puzzle[tile_space], newNode.puzzle[tile_space + 3]);
            
            if(newNode.puzzleMatch() == true) { //If we've found a matching puzzle
                goal_state = true;
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    //Note: In uniform cost search, h(n) is hardcoded to 0.
                }
                //cout << endl << "Outputting current state: " << endl;
                //displayPuzzle(newNode);
                
                //Since this is our goal state, we push it to the goal_path stack.
                goal_path.push(newNode);
                newNode.display_path(algorithm);
            }
            else { //If we DO NOT have a match
                newNode.g_cost = node_depth;
                
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    nodes.push(newNode);
                }
            }
        }
    }
}

//COMPLETED
void eightPuzzle::move_left(eightPuzzle node, string algorithm) {
    int tile_space; //Holds the index of our blank space.
    for(int i = 0; i < puzzle_size; i++) {
        if(node.puzzle[i] == 0) {
            tile_space = i;
        }
    }
    
    if(goal_state == false) {
        if(tile_space % 3 >= 1) { //If blank tile's x-coordinate is not in left column of puzzle
            
            //Create a new node and assign our original node as its parent.
            eightPuzzle newNode;
            newNode.create_Child(newNode.puzzle, node.puzzle);
            newNode.parents.push_back(node);
            
            swap(newNode.puzzle[tile_space], newNode.puzzle[tile_space - 1]);
            
            if(newNode.puzzleMatch() == true) { //If we've found a matching puzzle
                goal_state = true;
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    //Note: In uniform cost search, h(n) is hardcoded to 0.
                }
                //cout << endl << "Outputting current state: " << endl;
                //displayPuzzle(newNode);
                
                //Since this is our goal state, we push it to the goal_path stack.
                goal_path.push(newNode);
                newNode.display_path(algorithm);
            }
            else { //If we DO NOT have a match
                newNode.g_cost = node_depth;
                
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    nodes.push(newNode);
                }
            }
        }
    }
}

//COMPLETED
void eightPuzzle::move_right(eightPuzzle node, string algorithm) {
    int tile_space; //Holds the index of our blank space.
    for(int i = 0; i < puzzle_size; i++) {
        if(node.puzzle[i] == 0) {
            tile_space = i;
        }
    }
    
    if(goal_state == false) {
        if(tile_space % 3 < 2) { //If blank tile's x-coordinate is not in right column of puzzle
        /*
        Here is a quick demonstration proving this logic:
        The rightmost indices are 2, 5, and 8. 2 % 3 = 2, 5 % 3 = 2, 8 % 3 = 2.
        */
            
            //Create a new node and assign our original node as its parent.
            eightPuzzle newNode;
            newNode.create_Child(newNode.puzzle, node.puzzle);
            newNode.parents.push_back(node);
            
            swap(newNode.puzzle[tile_space], newNode.puzzle[tile_space - 1]);
            
            if(newNode.puzzleMatch() == true) { //If we've found a matching puzzle
                goal_state = true;
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    //Note: In uniform cost search, h(n) is hardcoded to 0.
                }
                
                //Since this is our goal state, we push it to the goal_path stack.
                goal_path.push(newNode);
                newNode.display_path(algorithm);
            }
            else { //If we DO NOT have a match
                newNode.g_cost = node_depth;
                
                if(algorithm == "MTH") {
                    newNode.h_cost = newNode.misplacedTile_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "MDH") {
                    newNode.h_cost = newNode.manhattan_Cost();
                    min_heap.push(newNode);
                }
                else if(algorithm == "UCS") {
                    newNode.h_cost = 0;
                    nodes.push(newNode);
                }
            }
        }
    }
}

//COMPLETED
void displayPuzzle(eightPuzzle node) {
    for(int i = 0; i < puzzle_size; i++) {
        if(i == 2 || i == 5 || i == 8) {
            cout << node.puzzle[i] << endl;
        }
        else {
            cout << node.puzzle[i] << " ";
        }
    }
}

//COMPLETED
void preset_puzzle() {
    int input;
    cout << "You have decided to solve a preset puzzle. Please select the difficulty level of the puzzle from 0 to 5: " << endl;
    cin >> input;
    if(input == 0) {
        for(int i = 0; i < puzzle_size; i++) {
            initial_puzzle[i] = trivial[i];
        }
    }
    else if(input == 1) {
        for(int i = 0; i < puzzle_size; i++) {
            initial_puzzle[i] = very_easy[i];
        }
    }
    else if(input == 2) {
        for(int i = 0; i < puzzle_size; i++) {
            initial_puzzle[i] = easy[i];
        }
    }
    else if(input == 3) {
        for(int i = 0; i < puzzle_size; i++) {
            initial_puzzle[i] = doable[i];
        }
    }
    else if(input == 4) {
        for(int i = 0; i < puzzle_size; i++) {
            initial_puzzle[i] = oh_boy[i];
        }
    }
    else if(input == 5) {
        for(int i = 0; i < puzzle_size; i++) {
            initial_puzzle[i] = impossible[i];
        }
    }
    else {
        cout << "ERROR: Not a valid puzzle difficulty! Terminating program..." << endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    
    int input;
    cout << "Enter 0 to use a preset puzzle. Enter 1 to enter a custom puzzle: " << endl;
    cin >> input;
    
    if(input == 0) {
        preset_puzzle();
        
        cout << "Select an algorithm to solve this puzzle: " << endl;
        cout << "1: Misplaced Tile Heuristic" << endl;
        cout << "2: Manhattan Distance Heuristic" << endl;
        cout << "3: Uniform Cost Search" << endl << endl;
        
        cin >> input;
        cout << endl;
        
        //If user wishes to use Misplaced Tile Heuristic
        if(input == 1) {
            eightPuzzle node;
            node.create_Child(node.puzzle, initial_puzzle);
            node.misplacedTile_Heuristic("MTH");
        }
        //If user wishes to use Manhattan Distance Heuristic
        else if(input == 2) {
            eightPuzzle node;
            node.create_Child(node.puzzle, initial_puzzle);
            node.manhattanDistance("MDH");
        }
        //If user wishes to use Uniform Cost Search
        else if(input == 3) {
            eightPuzzle node;
            node.create_Child(node.puzzle, initial_puzzle);
            node.uniformCostSearch("UCS");
        }
        else {
            cout << "ERROR! This is not a valid input! Don't do that again...";
            exit(EXIT_FAILURE);
        }
    }
    else if(input == 1) {
        int input_1, input_2, input_3;
        
        cout << "You have chosen to play a custom puzzle." << endl;
        cout << "Please enter the top row: " << endl;
        cin >> input_1 >> input_2 >> input_3;
        initial_puzzle[0] = input_1; 
        initial_puzzle[1] = input_2; 
        initial_puzzle[2] = input_3;
        
        cout << "Please enter the middle row: " << endl;
        cin >> input_1 >> input_2 >> input_3;
        initial_puzzle[3] = input_1; 
        initial_puzzle[4] = input_2; 
        initial_puzzle[5] = input_3;
        
        cout << "Please enter the bottom row: " << endl;
        cin >> input_1 >> input_2 >> input_3;
        initial_puzzle[6] = input_1; 
        initial_puzzle[7] = input_2; 
        initial_puzzle[8] = input_3;
        
        string algorithm;
        eightPuzzle node;
        node.create_Child(node.puzzle, initial_puzzle);
        cout << endl << "You have now created your custom puzzle!" << endl;
        cout << "Displaying created puzzle: " << endl;
        
        displayPuzzle(node);
        cout << endl;
        
        cout << "Select an algorithm to solve this puzzle: " << endl;
        cout << "1: Misplaced Tile Heuristic" << endl;
        cout << "2: Manhattan Distance Heuristic" << endl;
        cout << "3: Uniform Cost Search" << endl << endl;
        cin >> input;
        
        if(input == 1) {
            algorithm = "MTH";
            node.misplacedTile_Heuristic(algorithm);
        }
        else if(input == 2) {
            algorithm = "MDH";
            node.manhattanDistance(algorithm);
        }
        else if(input == 3) {
            algorithm = "UCS";
            node.uniformCostSearch(algorithm);
        }
        else {
            cout << "ERROR: This is not a valid input! Please don't do that." << endl;
            cout << "Terminating program..." << endl;
            exit(EXIT_FAILURE);
        }
    }
    
    //Helper loop that displays our initial puzzle
    /*for(int i = 0; i < puzzle_size; i++) {
        cout << initial_puzzle[i] << " ";
    }*/
    
    return 0;
}


