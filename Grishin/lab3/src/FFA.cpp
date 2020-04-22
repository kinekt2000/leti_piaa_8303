#include <iostream>
#include <ostream>
#include <fstream>

#include <limits>
#include <algorithm>

#include <vector>
#include <stack>
#include <map>

class Graph{
    // rename char as Vertex
    using Vertex = char;

    // adjacency matrix
    std::map<Vertex, std::map<Vertex, long int>> adjMatrix; // [from][to]

    // adjacency matrix of flow
    std::map<Vertex, std::map<Vertex, long int>> flow;

    // flag, which says "log process"
    bool logger;

    // log file descriptor
    std::ofstream log;

public:
    Graph(bool log_p){
        logger = log_p;
        // open file
        log.open("log", std::ios::out | std::ios::trunc);
    }

    ~Graph(){
        // close file
        log.close();
    }

    // write value to adjacency matrix
    void addPair(Vertex a, Vertex b, long int capacity){
        adjMatrix[a][b] = capacity;
        if(logger)
            log << "Added edge with capacity: " << capacity
                << " between " << a << " and " << b << std::endl;
    }

    // Ford Fulkerson algorithm
    long int iFordFulkerson(Vertex src, Vertex snk){
        if(logger)
            log << std::endl << "Ford-Fulkerson initiated" << std::endl;

        flow.clear();

        // adjacency matrix of residual flows
        auto residualFlow = adjMatrix;

        // max flow accumulator
        long int max_flow = 0;

        // lambda function of path finding
        // works as dfs but chose next vertex using residual flows
        auto findWay = [this, &residualFlow](Vertex src, Vertex dst){
            std::stack<Vertex> stack;
            std::map<Vertex, bool> visited;
            std::map<Vertex, Vertex> prev;

            stack.push(src);
            while(!stack.empty()){
                // u <- current vertex
                auto u = stack.top();
                if(u == dst) break;

                stack.pop();
                visited[u] = 1;

                // neighbours of current vertex
                std::vector<Vertex> neighbours;
                for(auto p: residualFlow[u]){
                    if(!visited[p.first] && p.second > 0)
                        neighbours.push_back(p.first);

                }

                // sort neighbours ascending residual flow 
                std::sort(neighbours.begin(), neighbours.end(),
                          [u, &residualFlow](const Vertex &a, const Vertex &b)
                          {return residualFlow[u][a] < residualFlow[u][b];});

                // put neighbours on stack (with max residual flow on top)
                for(auto n: neighbours){
                    stack.push(n);
                    prev[n] = u;
                }
            }
            return prev;
        };


        int iteration = 1;

        // get path from source to sink
        auto prev = findWay(src, snk);
        std::vector<long int> flows;

        // if snk in prev map then there is a way
        while(prev.count(snk) > 0){
            if(logger)
                log << "iteration " << iteration << ":" << std::endl;

            long int path_flow = std::numeric_limits<long int>::max();
            auto temp = snk;

            if(logger)
                log << "path: ";

            std::vector<Vertex> path;

            // find min residual flow using prev map
            while(temp != src){
                if(logger)
                    path.insert(path.begin(), temp);

                path_flow = std::min(path_flow, residualFlow[prev[temp]][temp]);
                temp = prev[temp];
            }
            if(logger)
                path.insert(path.begin(), temp);

            if(logger){
                for(auto it = path.begin(); it != path.end(); it++){
                    if(it != path.begin()){
                        auto to = *it;
                        auto from = prev[to];
                        log << "->[" <<  residualFlow[from][to]
                            << ";" << residualFlow[to][from] << "]<-";
                    }
                    log << *it;
                }
                log << std::endl;
                log << "Path flow is " << path_flow << std::endl;
                log << std::endl;
                flows.push_back(path_flow);
            }

            // add path flow to max flow
            max_flow += path_flow;
            temp = snk;
            while(temp != src){
                // reduce residual flow on the way
                auto from = prev[temp];
                residualFlow[from][temp] -= path_flow;
                residualFlow[temp][from] += path_flow;
                temp = prev[temp];
            }

            // recalculate path from source to sink
            prev = findWay(src, snk);
            iteration++;
        }

        if(logger){
            log << "Max Flow = ";
            for(auto it = flows.begin(); it != flows.end(); it++){
                if(it != flows.begin())
                    log << " + ";
                log << *it;
            }
            log << " = " << max_flow << std::endl;
            log << std::endl;
        }

        for(auto from_p: adjMatrix)
            for(auto to_p: from_p.second){
                auto iFlow = to_p.second - residualFlow[from_p.first][to_p.first];
                flow[from_p.first][to_p.first] = (iFlow < 0) ? 0 : iFlow;
            }

        return max_flow;
    }

    // get adjacency matrix of flows
    const auto& getFlow() const{
        return flow;
    }

};


int main(){
    Graph graph(1);

     // get number of edges
    int N;
    std::cin >> N;

    // get src and dst of path
    char src, snk;
    std::cin >> src >> snk;

    // get graph (two points and length between)
    char from, to;
    long int len;    
    for(int i = 0; i < N; i++){
        std::cin >> from >> to >> len;
        graph.addPair(from, to, len);
    }

    // get max flow
    auto max_flow = graph.iFordFulkerson(src, snk);
    std::cout << max_flow << std::endl;

    // get flows of graph and print them
    for(auto from_p: graph.getFlow()){
        for(auto to_p: from_p.second){
            std::cout << from_p.first << " "
                      << to_p.first << " "
                      << to_p.second << std::endl;
        }
    }

    return 0;
}


