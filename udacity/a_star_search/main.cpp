#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <exception>
#include <sstream>

namespace AStar
{

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};
const int delta[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

struct Node
{
public:
    Node() = default;

    Node(const Node& node) : mPos(node.mPos), mG(node.mG), mH(node.mH)
    {    }

    Node(std::pair<int, int> pos, int g, int h) : mPos(pos), mG(g), mH(h)
    {    }

    bool operator<(const Node& node) const
    {
        return (mG + mH) < (node.mG + node.mH);
    }

    bool operator>(const Node& node) const
    {
        return (mG + mH) > (node.mG + node.mH);
    }

public:
    std::pair<int, int> mPos;
#define X mPos.first
#define Y mPos.second

    int mG, mH;
};

int
get_heuristic_val(std::pair<int, int> pos1, std::pair<int, int> pos2)
{
    return abs(pos1.first - pos2.first) + abs(pos1.second - pos2.second);
}

void 
addNodeToOpenList(std::vector<Node>& openList, const Node& node)
{
    openList.push_back(node);
}

bool
compareNodes(const Node& node1, const Node& node2)
{
    return node1 > node2;
}

std::vector<State>
parseLine(const std::string& line)
{
    std::istringstream lineStream(line);
    std::vector<State> row;
    int c;
    char comma;
    while (lineStream >> c >> comma && comma == ',')
    {
        (c == 0) ? row.push_back(State::kEmpty) \
                    : row.push_back(State::kObstacle);
    }
    return row;
}

std::vector<std::vector<State>> 
readMapFile(const std::string& path)
{
    std::vector<std::vector<State>> map;
    std::ifstream mapFile(path);
    if (!mapFile)
    {
        throw std::ios_base::failure("Failed to open file: " + path);
    }

    std::string line;
    while (getline(mapFile, line))
    {
        map.push_back(parseLine(line));
    }
    return map;
}

void 
searchAndAddNeighbors(const Node& node, \
                std::pair<int, int> end, \
                std::vector<Node>& openList, \
                std::vector<std::vector<State>> map)
{
    for (int i = 0; i < 4; i++)
    {
        int x = delta[i][0] + node.X;
        int y = delta[i][1] + node.Y;

        if (x < 0 || y < 0 || x >= map.size() || y >= map[0].size())
            continue;

        if (map[x][y] == State::kEmpty) 
        {
            Node neighborNode({x, y}, node.mG + 1, get_heuristic_val({x, y}, end));
            addNodeToOpenList(openList, neighborNode);
            map[x][y] = State::kClosed;
        }
    }
}

void
search(std::vector<std::vector<State>>& map, \
        std::pair<int, int> start, \
        std::pair<int, int> end)
{
    std::vector<Node> openList;

    Node startNode(start, 0, get_heuristic_val(start, end));
    addNodeToOpenList(openList, startNode);

    while (!openList.empty())
    {
        // sort the openList by f value
        std::sort(openList.begin(), openList.end(), compareNodes);

        // get the node which has the smallest f value and mark as closed
        Node currentNode = openList.back();

        if (currentNode.X == end.first && currentNode.Y == end.second)
        {
            map[start.first][start.second] = State::kStart;
            map[currentNode.X][currentNode.Y] = State::kFinish;
            return;
        }
        else
        {
            map[currentNode.X][currentNode.Y] = State::kPath;
        }

        // remove this node from openList
        openList.pop_back();

        // search and add the valid neighbors of current node to openList
        searchAndAddNeighbors(currentNode, end, openList, map);
    }

    std::cout << "No path found" << std::endl;
    return;    
}

std::string 
stateToString(State state) 
{
    switch(state) 
    {
        case State::kObstacle: return "⛰   ";
        case State::kPath: return "🚗   ";
        case State::kStart: return "🚦   ";
        case State::kFinish: return "🏁   ";
        default: return "0   "; 
    }
}

void
printMap(const std::vector<std::vector<State>>& map) 
{
    for (int i = 0; i < map.size(); i++) 
    {
        for (int j = 0; j < map[i].size(); j++) 
        {
            std::cout << stateToString(map[i][j]);
        }
        std::cout << "\n";
    }
}

};


int 
main(int argc, char *argv[])
{       
    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {14, 19};

    try
    {
        if (argc != 2)
            throw std::ios_base::failure("Missing argument");

        std::string path = argv[1];
        std::vector<std::vector<AStar::State>> map = AStar::readMapFile(path);
        AStar::search(map, start, end);
        AStar::printMap(map);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}