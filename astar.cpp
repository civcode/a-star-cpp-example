#include <iomanip> // for std::setw
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

using namespace std;

class Point {
public:
    int x, y;

    bool operator==(const Point& o) const {
        return x == o.x && y == o.y;
    }

    bool operator!=(const Point& o) const {
        return !(*this == o);
    }

    bool operator<(const Point& o) const {
        return x < o.x || (x == o.x && y < o.y);
    }
};

class AStar {
    struct Node {
        Point coordinates;
        Node* parent;
        int gCost; // cost from start to current node
        int hCost; // heuristic cost estimate to end
        int fCost() const { return gCost + hCost; } // total cost

        Node(Point coord_, Node* parent_ = nullptr) : coordinates(coord_), parent(parent_), gCost(0), hCost(0) {}
    };

    Point start, end;
    vector<vector<bool>> grid;
    int gridWidth, gridHeight;

public:
    AStar(int width, int height, const Point& start_, const Point& end_) : gridWidth(width), gridHeight(height), start(start_), end(end_) {
        grid.resize(gridWidth, vector<bool>(gridHeight, true));
    }

    void setWall(int x, int y) {
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
            grid[x][y] = false;
        }
    }

    struct CompareNode {
        bool operator()(const Node* lhs, const Node* rhs) const {
            return lhs->fCost() > rhs->fCost();
        }
    };

    struct PointHash {
        size_t operator()(const Point& p) const {
            return hash<long long>()(((long long)p.x << 32) | p.y);
        }
    };

    vector<Point> findPath() {
        vector<Point> path;
        priority_queue<Node*, vector<Node*>, CompareNode> openSet;
        //priority_queue<Node*, vector<Node*>, [](const Node* lhs, const Node* rhs) { return lhs->fCost() > rhs->fCost(); }> openSet;
        //unordered_map<Point, Node*, [](const Point& p) { return hash<long long>()(((long long)p.x << 32) | p.y); }> allNodes;
        unordered_map<Point, Node*, PointHash> allNodes;


        Node* startNode = new Node(start);
        startNode->gCost = 0;
        startNode->hCost = heuristic(start, end);
        openSet.push(startNode);
        allNodes[start] = startNode;

        while (!openSet.empty()) {
            Node* currentNode = openSet.top();
            openSet.pop();

            if (currentNode->coordinates == end) {
                while (currentNode != nullptr) {
                    path.push_back(currentNode->coordinates);
                    currentNode = currentNode->parent;
                }
                break;
            }



            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (x == 0 && y == 0) {
                        continue; // Skip the current node itself
                    }
            
                    // Coordinates of neighbout
                    Point newCoordinates(currentNode->coordinates.x + x, currentNode->coordinates.y + y);
            
                    // Check if the new coordinates are within the grid and not a wall
                    if (!isInBounds(newCoordinates) || !grid[newCoordinates.x][newCoordinates.y]) {
                        continue;
                    }
            
                    // Straight: |x|+|y|  = 1
                    // Diagonal: |x|+|y| != 1
                    //int newGCost = currentNode->gCost + ((x != 0 && y != 0) ? diagonalCost : straightCost);
                    int newGCost = currentNode->gCost + ((abs(x)+abs(y) == 1) ? straightCost : diagonalCost);

                    Node* successor = new Node(newCoordinates, currentNode);
                    successor->gCost = newGCost;
                    successor->hCost = heuristic(successor->coordinates, end);
            

                    if (Node* existingNode = allNodes[successor->coordinates]; existingNode != nullptr) {
                        if (existingNode->gCost <= successor->gCost) {
                            delete successor;
                            continue;
                        }
                        allNodes.erase(successor->coordinates);
                    }

                    openSet.push(successor);
                    allNodes[successor->coordinates] = successor;
                }
            }
        }

        // Cleanup
        for (auto& pair : allNodes) {
            delete pair.second;
        }

        reverse(path.begin(), path.end());
        return path;
    }
    
    bool isWalkable(int x, int y) const {
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
            return grid[x][y];
        }
        return false; // Out of bounds is considered not walkable
    }

private:
    int diagonalCost = sqrt(2); //1.42; 
    int straightCost = 1; 
    
    static int heuristic(const Point& a, const Point& b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }

    bool isInBounds(const Point& p) const {
        return p.x >= 0 && p.x < gridWidth && p.y >= 0 && p.y < gridHeight;
    }
};


int main() {
    int width = 10, height = 10;
    Point start(5, 0), end(9, 9);
    AStar astar(width, height, start, end);
    
    //AStar astar(10, 10, {0, 0}, {9, 9});

    // Setting walls in the grid
    astar.setWall(1, 1);
    astar.setWall(1, 2);
    astar.setWall(1, 3);
    astar.setWall(2, 3);
    astar.setWall(3, 3);
    astar.setWall(4, 3);
    astar.setWall(5, 3);
    astar.setWall(6, 3);
    astar.setWall(6, 2);
    
    astar.setWall(6, 1);
    astar.setWall(6, 0);
    astar.setWall(0, 6);
    astar.setWall(1, 6);
    astar.setWall(2, 6);
    //astar.setWall(3, 6);
    astar.setWall(5, 9);
    astar.setWall(5, 8);
    
    astar.setWall(9, 8);
    astar.setWall(8, 8);
    astar.setWall(7, 8);

    // Finding the path
    vector<Point> path = astar.findPath();

    // Create a 2D vector to represent the grid
    vector<vector<char>> displayGrid(width, vector<char>(height, ' '));

    // Mark walls on the grid
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (!astar.isWalkable(x, y)) {
                displayGrid[x][y] = '#';
            }
        }
    }

    // Mark the path on the grid
    for (const auto& p : path) {
        if (p != start && p != end) {
            displayGrid[p.x][p.y] = '.';
        }
    }

    // Mark start and end points
    displayGrid[start.x][start.y] = 'S';
    displayGrid[end.x][end.y] = 'E';

    // Printing the grid
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cout << std::setw(2) << displayGrid[x][y];
        }
        cout << endl;
    }
    
    cout << endl;
    cout << "path length: " << path.size() << endl;

    return 0;
}


