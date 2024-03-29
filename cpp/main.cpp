/*!
    \file "main.cpp"

    Author: Matt Ervin <matt@impsoftware.org>
    Formatting: 4 spaces/tab (spaces only; no tabs), 120 columns.
    Doc-tool: Doxygen (http://www.doxygen.com/)

    https://leetcode.com/problems/course-schedule/
*/

//!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "utils.hpp"

/*
    There are a total of numCourses courses you have to take, labeled from 
    0 to numCourses - 1. You are given an array prerequisites where 
    prerequisites[i] = [ai, bi] indicates that you must take course bi first 
    if you want to take course ai.

        * For example, the pair [0, 1], indicates that to take course 0 you 
          have to first take course 1.
    
    Return true if you can finish all courses. Otherwise, return false.

    Constraints:

        * 1 <= numCourses <= 2000
        * 0 <= prerequisites.length <= 5000
        * prerequisites[i].length == 2
        * 0 <= ai, bi < numCourses
        * All the pairs prerequisites[i] are unique.
*/

class Solution1_DFS {
        using dependencies_t = vector<int>;
        using courses_t = unordered_map<int, dependencies_t>;
        using visited_t = unordered_set<int>;

        static bool canFinishDFS(
            courses_t& courses
            , int course
            , visited_t& visited
        ) noexcept {
            if (visited.contains(course)) { return false; }
            
            auto coursesIter = courses.find(course);
            if (courses.end() == coursesIter) { return true; }
            
            dependencies_t const& dependencies = coursesIter->second;
            if (!dependencies.empty()) {
                auto const courseIter = visited.insert(course).first;

                for (auto const dependency : dependencies) {
                    if (!canFinishDFS(courses, dependency, visited)) {
                        return false;
                    }
                }

                visited.erase(courseIter);
            }

            courses.erase(coursesIter);
            
            return true;
        }

public:
    /*
        The prerequisites create a dependency tree.
        Return true only when all branches in the tree contain no cycles.
        
        Populate a dictionary with all courses.  Keys are prerequisites[0],
        values are arrays composed of prerequisites[1], which handles 
        courses with more than one prerequisite.  Use a recursive depth
        first search to visit each individual branch of the prerequisite
        tree.  Remove the branch (or sub-branch) from the tree after 
        visiting it (DP).  As each node in the branch is visited, check
        to see if it was already visited.  If it was then there is a cyclic
        dependency in the branch, which results in false being returned.
        Add branch nodes to the visited collection as the branch is traversed
        from root to tip and remove the added nodes as the stack unwinds back
        from tip to root (backtracking).

        Time = O(n)
               n = number of prerequisites.  Prerequisite tree branch nodes
                   are removed from the prerequisite tree after being visited,
                   so the maximum number of nodes that can ever possibly be 
                   visited is n.

        Space = O(n)
                n = maximum call stack depth when all prerequisites reference
                    each other and create a linear chain.
    */
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        courses_t courses{};
        for (auto const& prerequisite : prerequisites) {
            auto const course = prerequisite[0];
            auto const dependency = prerequisite[1];
            auto coursesIter = courses.find(course);
            if (courses.end() != coursesIter) {
                coursesIter->second.push_back(dependency);
            } else {
                courses.insert(make_pair(course, dependencies_t{dependency}));
            }
        }

        visited_t visited{};
        while (!courses.empty()) {
            auto const course = courses.begin()->first;
            if (!canFinishDFS(courses, course, visited)) {
                return false;
            }
        }

        return true;
    }
};

class Solution2_TopologicalSort {
public:
    /*
        The prerequisites create a dependency tree.
        Return true only when all branches in the tree contain no cycles.
        
        Use a topological sort to detect cycles.
          1) Create a graph (adjacency list) from the prerequisites.
          2) Count the number of edges coming in to a node (it's in-degree).
          3) Push the nodes with an in-degree of zero (0) on to the que.
          4) While the queue is not empty:
        4.1)   Pop node ('A') off of que.
        4.2)   Decrement the in-degree of each node referenced by 'A', i.e.
               each node that can be reached by following an edge from 'A'.
        4.3)   If the in-degree of a node becomes zero (0), push the node 
               on to the que.
          5) If the number of nodes pushed to or popped from the que is equal
             to the number of nodes in the graph then there are no cycles in
             the dependency tree and the result is true.

        Time = O(V + V + E + V + V) => O(4V + E) => O(V+E)
               V = vertex count == numCourses
               E = edge count == len(prerequisites)
               Term 1: inDegrees allocation.
               Term 2: graph allocation.
               Term 3: graph population.
               Term 4: queue initialization.
               Term 5: queue processing.

        Space = O(V + V*V + V) => O(2V + V**2) => O(V**2)
                V = vertex count == numCourses
                Term 1: inDegrees capacity
                Term 2: graph capacity (all nodes could reference each other).
                Term 3: queue capacity (all nodes when none reference any other).
    */
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        // Create graph [adjacency list] and in-degree counts from prerequisites.
        int inDegrees[numCourses];
        memset(inDegrees, 0, sizeof(inDegrees));
        vector<int> graph[numCourses];
        for (auto const& edge : prerequisites) {
            graph[edge[0]].push_back(edge[1]);
            ++inDegrees[edge[1]];
        }

        // Initialize queue with nodes that have an in-degree count of zero.
        deque<int> que{};
        for (int node = 0; numCourses > node; ++node) {
            if (0 == inDegrees[node]) {
                que.push_back(node);
            }
        }

        int visitCount = static_cast<int>(que.size());

        // Remove each node from graph until all removed or cycle detected.
        while (!que.empty()) {
            auto const node = que.front();
            que.pop_front();

            // "Remove" node from graph.
            for (auto const neighbor : graph[node]) {
                if (0 == --inDegrees[neighbor]) {
                    que.push_back(neighbor);
                    ++visitCount;
                }
            }
        }

        auto const noCycleDetected = visitCount == numCourses;
        return noCycleDetected;
    }
};

// {----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------}

namespace doctest {
    const char* testName() noexcept { return doctest::detail::g_cs->currentTest->m_name; }
} // namespace doctest {

TEST_CASE("Case 1")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 2;
    auto prerequisites = vector<vector<int>>{
        {1,0}
    };
    auto const expected = true;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 2")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 2;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {0,1},
    };
    auto const expected = false;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 3")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 5;
    auto prerequisites = vector<vector<int>>{
        {1,4},
        {2,4},
        {3,1},
        {3,2}
    };
    auto const expected = true;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 4")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 8;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {2,6},
        {1,7},
        {6,4},
        {7,0},
        {0,5}
    };
    auto const expected = true;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 5")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 20;
    auto prerequisites = vector<vector<int>>{
        {0,10},
        {3,18},
        {5,5},
        {6,11},
        {11,14},
        {13,1},
        {15,1},
        {17,4}
    };
    auto const expected = false;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 6")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 3;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {2,0},
        {0,2}
    };
    auto const expected = false;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 7")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 3;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {1,2},
        {0,1}
    };
    auto const expected = false;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 8")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 100;
    auto prerequisites = vector<vector<int>>{
        {1,0},{2,0},{2,1},{3,1},{3,2},{4,2},{4,3},{5,3},{5,4},{6,4},{6,5},{7,5},{7,6},{8,6},{8,7},{9,7},{9,8},{10,8},{10,9},{11,9},{11,10},{12,10},{12,11},{13,11},{13,12},{14,12},{14,13},{15,13},{15,14},{16,14},{16,15},{17,15},{17,16},{18,16},{18,17},{19,17},{19,18},{20,18},{20,19},{21,19},{21,20},{22,20},{22,21},{23,21},{23,22},{24,22},{24,23},{25,23},{25,24},{26,24},{26,25},{27,25},{27,26},{28,26},{28,27},{29,27},{29,28},{30,28},{30,29},{31,29},{31,30},{32,30},{32,31},{33,31},{33,32},{34,32},{34,33},{35,33},{35,34},{36,34},{36,35},{37,35},{37,36},{38,36},{38,37},{39,37},{39,38},{40,38},{40,39},{41,39},{41,40},{42,40},{42,41},{43,41},{43,42},{44,42},{44,43},{45,43},{45,44},{46,44},{46,45},{47,45},{47,46},{48,46},{48,47},{49,47},{49,48},{50,48},{50,49},{51,49},{51,50},{52,50},{52,51},{53,51},{53,52},{54,52},{54,53},{55,53},{55,54},{56,54},{56,55},{57,55},{57,56},{58,56},{58,57},{59,57},{59,58},{60,58},{60,59},{61,59},{61,60},{62,60},{62,61},{63,61},{63,62},{64,62},{64,63},{65,63},{65,64},{66,64},{66,65},{67,65},{67,66},{68,66},{68,67},{69,67},{69,68},{70,68},{70,69},{71,69},{71,70},{72,70},{72,71},{73,71},{73,72},{74,72},{74,73},{75,73},{75,74},{76,74},{76,75},{77,75},{77,76},{78,76},{78,77},{79,77},{79,78},{80,78},{80,79},{81,79},{81,80},{82,80},{82,81},{83,81},{83,82},{84,82},{84,83},{85,83},{85,84},{86,84},{86,85},{87,85},{87,86},{88,86},{88,87},{89,87},{89,88},{90,88},{90,89},{91,89},{91,90},{92,90},{92,91},{93,91},{93,92},{94,92},{94,93},{95,93},{95,94},{96,94},{96,95},{97,95},{97,96},{98,96},{98,97},{99,97}
    };
    auto const expected = true;
    auto solution = Solution1_DFS{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 21")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 2;
    auto prerequisites = vector<vector<int>>{
        {1,0}
    };
    auto const expected = true;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 22")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 2;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {0,1},
    };
    auto const expected = false;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 23")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 5;
    auto prerequisites = vector<vector<int>>{
        {1,4},
        {2,4},
        {3,1},
        {3,2}
    };
    auto const expected = true;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 24")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 8;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {2,6},
        {1,7},
        {6,4},
        {7,0},
        {0,5}
    };
    auto const expected = true;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 25")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 20;
    auto prerequisites = vector<vector<int>>{
        {0,10},
        {3,18},
        {5,5},
        {6,11},
        {11,14},
        {13,1},
        {15,1},
        {17,4}
    };
    auto const expected = false;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 26")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 3;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {2,0},
        {0,2}
    };
    auto const expected = false;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 27")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 3;
    auto prerequisites = vector<vector<int>>{
        {1,0},
        {1,2},
        {0,1}
    };
    auto const expected = false;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

TEST_CASE("Case 28")
{
    cerr << doctest::testName() << '\n';
    auto const numCourses = 100;
    auto prerequisites = vector<vector<int>>{
        {1,0},{2,0},{2,1},{3,1},{3,2},{4,2},{4,3},{5,3},{5,4},{6,4},{6,5},{7,5},{7,6},{8,6},{8,7},{9,7},{9,8},{10,8},{10,9},{11,9},{11,10},{12,10},{12,11},{13,11},{13,12},{14,12},{14,13},{15,13},{15,14},{16,14},{16,15},{17,15},{17,16},{18,16},{18,17},{19,17},{19,18},{20,18},{20,19},{21,19},{21,20},{22,20},{22,21},{23,21},{23,22},{24,22},{24,23},{25,23},{25,24},{26,24},{26,25},{27,25},{27,26},{28,26},{28,27},{29,27},{29,28},{30,28},{30,29},{31,29},{31,30},{32,30},{32,31},{33,31},{33,32},{34,32},{34,33},{35,33},{35,34},{36,34},{36,35},{37,35},{37,36},{38,36},{38,37},{39,37},{39,38},{40,38},{40,39},{41,39},{41,40},{42,40},{42,41},{43,41},{43,42},{44,42},{44,43},{45,43},{45,44},{46,44},{46,45},{47,45},{47,46},{48,46},{48,47},{49,47},{49,48},{50,48},{50,49},{51,49},{51,50},{52,50},{52,51},{53,51},{53,52},{54,52},{54,53},{55,53},{55,54},{56,54},{56,55},{57,55},{57,56},{58,56},{58,57},{59,57},{59,58},{60,58},{60,59},{61,59},{61,60},{62,60},{62,61},{63,61},{63,62},{64,62},{64,63},{65,63},{65,64},{66,64},{66,65},{67,65},{67,66},{68,66},{68,67},{69,67},{69,68},{70,68},{70,69},{71,69},{71,70},{72,70},{72,71},{73,71},{73,72},{74,72},{74,73},{75,73},{75,74},{76,74},{76,75},{77,75},{77,76},{78,76},{78,77},{79,77},{79,78},{80,78},{80,79},{81,79},{81,80},{82,80},{82,81},{83,81},{83,82},{84,82},{84,83},{85,83},{85,84},{86,84},{86,85},{87,85},{87,86},{88,86},{88,87},{89,87},{89,88},{90,88},{90,89},{91,89},{91,90},{92,90},{92,91},{93,91},{93,92},{94,92},{94,93},{95,93},{95,94},{96,94},{96,95},{97,95},{97,96},{98,96},{98,97},{99,97}
    };
    auto const expected = true;
    auto solution = Solution2_TopologicalSort{};
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = solution.canFinish(numCourses, prerequisites);
        CHECK(expected == result);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << "\n";
}

/*
    End of "main.cpp"
*/
